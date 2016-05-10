/*
 * controller.c
 *
 *  Created on: Mar 30, 2016
 *      Author: Adrian Senn
 */

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
#include "task_communication.h"

#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <memPoolService.h>			/* Memory pool manager service			*/
/*----- Definition ----------------------------------------------------------*/
#define Inform				0
#define Order_distribution	1
#define Inquiry				2
#define Wait				3
#define New_information		4

#define Max 				20


#define Up					0
#define Down				1
#define Stay				2

//#define Floor				0
//#define Direction			1
//#define Lift				2
//#define ID					3

#define TIME				10
/*----- Data ---------------------------------------------------------------*/

typedef enum _lift { Lift1, Lift2, Outside  } TaskSource;
typedef struct _orders{
	int Floor;
	int Direction;
	int Lift;
	int Id;
}Order;

Order Pending_orders [Max];
Order Jobs_inprogress_lift_1 [Max];
Order Jobs_inprogress_lift_2 [Max];
const Order noOrder;
int currentId=0;
char possible_floors_lift_1[2];
char possible_floors_lift_2[2];

char last_position_lift_1;
char last_position_lift_2;
char direction_lift_1;
char direction_lift_2;
char order;
char inprogress_lift_1;
char inprogress_lift_2;

char state=0;

Job sendJob = {0,0,0};
Job recJob = {0,0,0};





/*----- Function prototypes ------------------------------------------------*/


char Find_direction (char p[][2],char last_position);
char Array_arrange_4 (Order p[]);
int checkValidOrder(Order order);




void controller(void)
{
	int i;
	//wichtige informationen updaten
	Job t;
	t.id=0;
	t.success=0;
	t.targetFloor=4;
	xQueueSend(_controllerToLiftA,&t,0);
	xQueueSend(_controllerToLiftB,&t,0);
	t.targetFloor=2;
	xQueueSend(_controllerToLiftA,&t,0);
	t.targetFloor=3;
	xQueueSend(_controllerToLiftB,&t,0);
	while(1){

	}

	vTaskDelay(100);

}

char Find_direction (char p[][2],char last_position)
{
	if(last_position==p[0][0])
	{
		return Stay;
	}else if(last_position>p[0][0])
	{
		return Down;
	}else if(last_position<p[0][0])
	{
		return Up;
	}else
	{
		return Stay;
	}
	// h�chste oder tiefste position
}
char Array_arrange_4 (Order p[]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<Max;i++)
	{
		if(p[i].Floor==-1)
		{
			j++;
		}
		else
		{
			p[i-j]=p[i];
			if(j!=0)
			{
				p[i]=noOrder;
			}
		}
	}
	return (Max-j);
}

int checkValidOrder(Order order){
	if(1<=order.Floor&&order.Floor<=5){
		return 0;
	}
	if(order.Direction>2){
		return 0;
	}
	if(order.Lift>1){
		return 0;
	}
	int i;
	for(i=0;i<Max;i++){
		if(Pending_orders[i].Direction==order.Direction &&
				Pending_orders[i].Floor==order.Floor &&
				Pending_orders[i].Lift==order.Lift){
			return 0;
		}
		if(Jobs_inprogress_lift_1[i].Direction==order.Direction &&
				Jobs_inprogress_lift_1[i].Floor==order.Floor &&
				Jobs_inprogress_lift_1[i].Lift==order.Lift){
			return 0;
		}
		if(Jobs_inprogress_lift_2[i].Direction==order.Direction &&
				Jobs_inprogress_lift_2[i].Floor==order.Floor &&
				Jobs_inprogress_lift_2[i].Lift==order.Lift){
			return 0;
		}
	}
	return 1;
}


