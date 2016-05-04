/*
 * can_controll.c
 *
 *  Created on: Apr 27, 2016
 *      Author: xorfish
 */

#include "can_controll.h"
//_canToLift;
//xQueueHandle _canToController;
//xQueueHandle _liftToCan;
//xQueueHandle _controllerToCan;


void readCanMessageIRQ () {
	CARME_CAN_MESSAGE msg;
	ERROR_CODES error;
	for(;;){
		error = CARME_CAN_Read(&msg);

		if (error == CARME_NO_ERROR) {
			if(msg.id<=0x01) {
				xQueueSendFromISR(_canToLift, &msg, portMAX_DELAY);
			} else if (msg.id <= 0x0B) {
				xQueueSendFromISR(_canToLift, &msg, portMAX_DELAY);
				xQueueSendFromISR(_canToController, &msg, portMAX_DELAY);
			} else if(msg.id <= 0x0D){
				xQueueSendFromISR(_canToController, &msg, portMAX_DELAY);
			}
		}
	}
}
void sendCanMessage(){
	CARME_CAN_MESSAGE msg;
	for(;;){
		xQueueReceive(_toCan, &msg, portMAX_DELAY);
		CARME_CAN_Write(&msg);
		vTaskDelay(100/portTICK_RATE_MS);
	}
}
