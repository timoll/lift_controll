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
const Order noOrder={-1,-1,-1,-1};
char currentId=0;
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


char Find_direction (Order p[],char last_position);
char Array_arrange_4 (Order p[]);
int checkValidOrder(Order order);




void controller(void)
{
	int i;
	//wichtige informationen updaten
	//while(1);
	while(1){
	switch (state)
	{
		case Inform:

			direction_lift_1=Find_direction(Jobs_inprogress_lift_1,last_position_lift_1);
			direction_lift_2=Find_direction(Jobs_inprogress_lift_2,last_position_lift_2);


			state=Order_distribution;
			break;

		case Order_distribution:

			order=Array_arrange_4(Pending_orders);
			for(i=0;i<order;i++)
			{
				char Lift_12;
				Lift_12=Pending_orders[i].Lift;
				if(Lift_12==Outside)
				{
					if(abs(last_position_lift_1-Pending_orders[i].Floor)>abs(last_position_lift_2-Pending_orders[i].Floor))
					{
						Lift_12=Lift2;
					}
					else
					{
						Lift_12=Lift1;
					}
				}
				switch (Lift_12)
				{
						case Lift1:
						if(direction_lift_1==Stay)
						{
							//Direkte aufgaben verteilung
							if(last_position_lift_1>Pending_orders[i].Floor)
							{
								direction_lift_1=Down;
							}else if(last_position_lift_1<Pending_orders[i].Floor)
							{
								direction_lift_1=Up;
							}else
							{
								direction_lift_1=Stay;
							}
							Jobs_inprogress_lift_1[inprogress_lift_1+i]=Pending_orders[i];

							Pending_orders[i]=noOrder;

						}else if(direction_lift_1==Up)
						{
							if(last_position_lift_1<=Pending_orders[i].Floor)
							{
								Jobs_inprogress_lift_1[inprogress_lift_1+i]=Pending_orders[i];


								Pending_orders[i]=noOrder;
							}
						}else if(direction_lift_1==Down)
						{
							if(last_position_lift_1>=Pending_orders[i].Floor)
							{
								Jobs_inprogress_lift_1[inprogress_lift_1+i]=Pending_orders[i];

								Pending_orders[i]=noOrder;
							}
						}
						break;

					case Lift2:
						if(direction_lift_2==Stay)
						{
							//Direkte aufgaben verteilung
							if(last_position_lift_2>Pending_orders[i].Floor)
							{
								direction_lift_2=Down;
							}else if(last_position_lift_2<Pending_orders[i].Floor)
							{
								direction_lift_2=Up;
							}else
							{
								direction_lift_2=Stay;
							}
							Jobs_inprogress_lift_2[inprogress_lift_2+i]=Pending_orders[i];

							Pending_orders[i]=noOrder;

						}else if(direction_lift_2==Up)
						{
							if(last_position_lift_2<=Pending_orders[i].Floor)
							{
								Jobs_inprogress_lift_2[inprogress_lift_2+i]=Pending_orders[i];

								Pending_orders[i]=noOrder;
							}
						}else if(direction_lift_2==Down)
						{
							if(last_position_lift_2>=Pending_orders[i].Floor)
							{
								Jobs_inprogress_lift_2[inprogress_lift_2+i]=Pending_orders[i];

								Pending_orders[i]=noOrder;
							}
						}
						break;
				}
			}
			inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
			inprogress_lift_2=Array_arrange_4(Jobs_inprogress_lift_2);
			state=Inquiry;
			break;

		case Inquiry:
			if(Jobs_inprogress_lift_1[0].Floor!=0)
			{
				i=0;
				while(Jobs_inprogress_lift_1[i].Floor!=0)
				{
					sendJob.id=Jobs_inprogress_lift_1[i].Id;
					sendJob.targetFloor=Jobs_inprogress_lift_1[i].Floor;
					sendJob.success=0;

					xQueueSend(_controllerToLiftA, &sendJob, 0);

					i++;
				}
				//in queue schreiben
			}
			if(Jobs_inprogress_lift_2[0].Floor!=0)
			{
				i=0;
				while(Jobs_inprogress_lift_1[i].Floor!=0)
				{
					sendJob.id=Jobs_inprogress_lift_2[i].Id;
					sendJob.targetFloor=Jobs_inprogress_lift_2[i].Floor;
					sendJob.success=0;

					xQueueSend(_controllerToLiftB, &sendJob, 0);

					i++;
				}
				//in queue schreiben
			}
			inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
			inprogress_lift_2=Array_arrange_4(Jobs_inprogress_lift_2);
			order=Array_arrange_4(Pending_orders);
			state=Inform;
			break;
	}
	i=0;
	while(xQueueReceive(_liftAToController,&recJob, TIME)!=0)
	{
		do
		{
			if(recJob.id==Jobs_inprogress_lift_1[i].Id)
			{
				if(recJob.success==0)
				{
					Pending_orders[order]=Jobs_inprogress_lift_1[i];
					order++;
				}else if(recJob.success==1)
				{
					Jobs_inprogress_lift_1[i]=noOrder;
					inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
//////////////////////////////////////////Licht l�schen
				}
			}
			i++;
		}while(recJob.id!=Jobs_inprogress_lift_1[i].Id);
	}
	while(xQueueReceive(_liftBToController,&recJob, TIME)!=0)
	{

	}
	CARME_CAN_MESSAGE msg;
	while(xQueueReceive(_canToController,&msg,TIME)!=0){

		Order newOrder;
		if(msg.id>=0xC){
			newOrder.Floor=(msg.data[1]&0xEF)%6;
			newOrder.Direction=Stay;
			newOrder.Id=currentId;
			if(msg.id==0xC){
				newOrder.Lift=Lift1;
			} else {
				newOrder.Lift=Lift2;
			}
		} else {
			newOrder.Floor = msg.id/2;
			newOrder.Direction = (msg.data[1] - 0xFB);
			newOrder.Id = currentId;
			newOrder.Lift = Outside;
		}
		if(checkValidOrder(newOrder)){
			Pending_orders[order]=newOrder;
			order++;
			currentId++;
		}
	}

	vTaskDelay(50);
	}

}

char Find_direction (Order p[],char last_position)
{
	if(last_position==p[0].Floor)
	{
		return Stay;
	}else if(last_position>p[0].Floor)
	{
		return Down;
	}else if(last_position<p[0].Floor)
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
	if(1>=order.Floor||order.Floor>=5){
		return 0;
	}
	if(order.Direction>2||order.Direction<0){
		return 0;
	}
	if(order.Lift>2|| order.Lift<0){
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


