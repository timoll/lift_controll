/*****************************************************************************
 * \brief		Startup file. In this file is the main function, which is
 *				called at the startup of the controller.
 *
 * \file		main.c
 * \version		1.0
 * \date		2012-12-12
 * \author		fue1
 ****************************************************************************/
/*
 * functions global:
 *				main
 * functions local:
 *				GreenLEDtask
 *				IOtask
 *
 ****************************************************************************/

/*----- Header-Files --------------------- ---------------------------------*/
#include <stdio.h>					/* Standard input and output			*/
#include <stdlib.h>					/* Standard library        		        */

#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <uart.h>					/* CARME BSP UART port					*/
#include <can.h>					/* CARME BSP CAN  port					*/
#include <lcd.h>
#include <carme_io1.h>
#include <carme_io2.h>

#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <memPoolService.h>			/* Memory pool manager service			*/
#include "task_communication.h"

#include "lift.h"
#include "can_controll.h"

#include "controller.h"

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
//void sendCanMessage    (void *);
//void writeCanMessage   (void *);
//void readCanMessageIRQ (void);

/*----- Data ---------------------------------------------------------------*/
xSemaphoreHandle Muxtex_Can_Tx;
xSemaphoreHandle Muxtex_Display;
xQueueHandle     Queue_Can_Rx;

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		main
 * @return		0 if success
 */
int main(void)
{
	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* initialize the mutex variables */

	Muxtex_Can_Tx  = xSemaphoreCreateMutex();
	Muxtex_Display = xSemaphoreCreateMutex();
	Queue_Can_Rx   = xQueueCreate(20, sizeof (CARME_CAN_MESSAGE *));

	/* initialize the LCD display */
	LCD_Init();
	LCD_SetFont(&font_8x16);
	LCD_DisplayStringXY(10, 10, "Can Template CARME-M4");
	int queueError = initQueues();
		if(queueError != 0) {
			char errorMessage[35];
			sprintf(errorMessage,"Error initializing queue: %x", queueError);
			fprintf(stderr, errorMessage);
			LCD_DisplayStringXY(10, 30, errorMessage);
		}
	/* initialize the CAN interface */
	CARME_CAN_InitI(CARME_CAN_BAUD_125K, CARME_CAN_DF_RESET, CARME_CAN_INT_RX);
	CARME_CAN_RegisterIRQCallback(CARME_CAN_IRQID_RX_INTERRUPT, readCanMessageIRQ);
	CARME_CAN_SetMode(CARME_CAN_DF_NORMAL);



	/* create tasks */
	xTaskCreate(sendCanMessage,  (const signed char * const)"Send Can Message",  1024, NULL, 4, NULL);
	//xTaskCreate(writeCanMessage, (const signed char * const)"Write Can Message", 1024, NULL, 4, NULL);
	xTaskCreate(lift,(const signed char * const)"Lift", 1024, NULL, 4, NULL);
	xTaskCreate(controller,  (const signed char * const)"controller",  1024, NULL, 4, NULL);
	initQueues();

	vTaskStartScheduler();

	for(;;){}

	return 0;
}

/**
 * @brief		task sendCanMessage
 */
//void sendCanMessage (void *pvargs) {
//	int i;
//	CARME_CAN_MESSAGE msg;
//
//	for(;;) {
//		msg.id  = 0x100;
//		msg.ext = 0;
//		msg.rtr = 0;
//		msg.dlc = 8;
//
//		for (i = 0; i < 8; i++) {
//			msg.data[i] = i * 0x11;
//		}
//
//		xSemaphoreTake(Muxtex_Can_Tx, portMAX_DELAY);
//		CARME_CAN_Write(&msg);
//		xSemaphoreGive(Muxtex_Can_Tx);
//
//		vTaskDelay(1000/portTICK_RATE_MS);
//	}
//}

/**
 * @brief		task writeCanMessage
 */
//void writeCanMessage (void *pvargs) {
//	CARME_CAN_MESSAGE *pMsg;
//	char text [60];
//	int i;
//
//	for(;;) {
//		xQueueReceive(Queue_Can_Rx, &pMsg, portMAX_DELAY);
//		// set the unused transmitted data bytes to 0
//		for (i = pMsg->dlc; i < 8; i++) {
//			pMsg->data[i] = 0;
//		}
//		sprintf(text, "ID=%xh,DLC=%d,Data=%xh,%xh,%xh,%xh,%xh,%xh,%xh,%xh",
//				       (unsigned)pMsg->id, pMsg->dlc,
//				       pMsg->data[0], pMsg->data[1], pMsg->data[2], pMsg->data[3],
//				       pMsg->data[4], pMsg->data[5], pMsg->data[6], pMsg->data[7]);
//		xSemaphoreTake(Muxtex_Display, portMAX_DELAY);
//		LCD_SetFont(&font_8x13);
//		LCD_DisplayStringXY(10, 30, "New Can Message received");
//		LCD_SetFont(&font_5x8);
//		LCD_DisplayStringXY(10, 50, text);
//		xSemaphoreGive(Muxtex_Display);
//		free(pMsg);
//	}
//}

/**
 * @brief		IRQ readCanMessageIRQ
 */
//void readCanMessageIRQ () {
//	CARME_CAN_MESSAGE *pMsg;
//	ERROR_CODES error;
//
//	pMsg = malloc (sizeof (CARME_CAN_MESSAGE));
//
//	if (pMsg != NULL) {
//        // read the new message
//		error = CARME_CAN_Read(pMsg);
//
//		if (error == CARME_NO_ERROR) {
//			xQueueSend(Queue_Can_Rx, &pMsg, portMAX_DELAY);
//		}
//		else {
//			free (pMsg);
//		}
//	}
//}
