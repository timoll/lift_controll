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
const Order noOrder={0,0,0,0};
int currentId=0;
int possible_floors_lift_1[2];
int possible_floors_lift_2[2];

int last_position_lift_1=1;
int last_position_lift_2=1;
int direction_lift_1=2;
int direction_lift_2=2;
int order=0;
int inprogress_lift_1=0;
int inprogress_lift_2=0;

int state=0;

Job sendJob = {0,0,0};
Job recJob = {0,0,0};





/*----- Function prototypes ------------------------------------------------*/


int Find_direction (Order p[],int last_position);
int Array_arrange_4 (Order p[]);
int checkValidOrder(Order order);




void controller(void)
{
	int i;
	//wichtige informationen updaten
	//while(1);
//	CARME_CAN_MESSAGE msg;
//	for(i=1;i<5;i++)
//	{
//		msg.data[3]=i;//turn off lampe inside
//		msg.id=0xC;
//		msg.data[3]=i;//turn off lampe inside
//		msg.id=0xD;
//		xQueueSend(_toCan, &msg, 0);
//		msg.data[3]=0xF1;						//turn off lampe outside up
//		msg.id=i*2;
//		xQueueSend(_toCan, &msg, 0);
//		msg.data[3]=0xF1;
//		msg.id=i*2+1;
//		xQueueSend(_toCan, &msg, 0);
//		msg.data[3]=0xF0;					//turn off lampe outside down
//		msg.id=i*2;
//		xQueueSend(_toCan, &msg, 0);
//		msg.data[3]=0xF0;
//		msg.id=i*2+1;
//		xQueueSend(_toCan, &msg, 0);
//	}
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
			char Lift_12;
			for(i=0;i<order;i++)
			{

				if(Pending_orders[i].Floor!=0)
				{
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
								sendJob.id=Jobs_inprogress_lift_1[i].Id;
								sendJob.targetFloor=Jobs_inprogress_lift_1[i].Floor;
								sendJob.success=0;
								xQueueSend(_controllerToLiftA, &sendJob, 0);


								Pending_orders[i]=noOrder;

							}else if(direction_lift_1==Up)
							{
								if(last_position_lift_1>=Pending_orders[i].Floor)
								{
									Jobs_inprogress_lift_1[inprogress_lift_1+i]=Pending_orders[i];
									sendJob.id=Jobs_inprogress_lift_1[i].Id;
									sendJob.targetFloor=Jobs_inprogress_lift_1[i].Floor;
									sendJob.success=0;
									xQueueSend(_controllerToLiftA, &sendJob, 0);



									Pending_orders[i]=noOrder;
								}
							}else if(direction_lift_1==Down)
							{
								if(last_position_lift_1<=Pending_orders[i].Floor)
								{
									Jobs_inprogress_lift_1[inprogress_lift_1+i]=Pending_orders[i];
									sendJob.id=Jobs_inprogress_lift_1[i].Id;
									sendJob.targetFloor=Jobs_inprogress_lift_1[i].Floor;
									sendJob.success=0;
									xQueueSend(_controllerToLiftA, &sendJob, 0);


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
								sendJob.id=Jobs_inprogress_lift_2[i].Id;
								sendJob.targetFloor=Jobs_inprogress_lift_2[i].Floor;
								sendJob.success=0;
								xQueueSend(_controllerToLiftB, &sendJob, 0);

								Pending_orders[i]=noOrder;

							}else if(direction_lift_2==Up)
							{
								if(last_position_lift_2>=Pending_orders[i].Floor)
								{
									Jobs_inprogress_lift_2[inprogress_lift_2+i]=Pending_orders[i];
									sendJob.id=Jobs_inprogress_lift_2[i].Id;
									sendJob.targetFloor=Jobs_inprogress_lift_2[i].Floor;
									sendJob.success=0;
									xQueueSend(_controllerToLiftB, &sendJob, 0);

									Pending_orders[i]=noOrder;
								}
							}else if(direction_lift_2==Down)
							{
								if(last_position_lift_2<=Pending_orders[i].Floor)
								{
									Jobs_inprogress_lift_2[inprogress_lift_2+i]=Pending_orders[i];
									sendJob.id=Jobs_inprogress_lift_2[i].Id;
									sendJob.targetFloor=Jobs_inprogress_lift_2[i].Floor;
									sendJob.success=0;
									xQueueSend(_controllerToLiftB, &sendJob, 0);

									Pending_orders[i]=noOrder;
								}
							}
							break;
					}
				}
			}
			inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
			inprogress_lift_2=Array_arrange_4(Jobs_inprogress_lift_2);
			state=Inquiry;
			break;

		case Inquiry:
			inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
			inprogress_lift_2=Array_arrange_4(Jobs_inprogress_lift_2);
			order=Array_arrange_4(Pending_orders);
			state=Inform;
			break;
	}
	CARME_CAN_MESSAGE msg;
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
					msg.data[3]=Jobs_inprogress_lift_1[i].Floor;//turn off lampe inside
					msg.id=0xC;
					xQueueSend(_toCan, &msg, 0);
					if(direction_lift_1==Up)
					{
						msg.data[3]=0xF1;						//turn off lampe outside up
						msg.id=Jobs_inprogress_lift_1[i].Floor*2;
						xQueueSend(_toCan, &msg, 0);
						msg.data[3]=0xF1;
						msg.id=Jobs_inprogress_lift_1[i].Floor*2+1;
						xQueueSend(_toCan, &msg, 0);
					}else if(direction_lift_1==Down)
					{
							msg.data[3]=0xF0;					//turn off lampe outside down
							msg.id=Jobs_inprogress_lift_1[i].Floor*2;
							xQueueSend(_toCan, &msg, 0);
							msg.data[3]=0xF0;
							msg.id=Jobs_inprogress_lift_1[i].Floor*2+1;
							xQueueSend(_toCan, &msg, 0);
					}
					last_position_lift_1=Jobs_inprogress_lift_1[i].Floor;
					Jobs_inprogress_lift_1[i]=noOrder;
					inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
				}
			}
			i++;
		}while(recJob.id!=Jobs_inprogress_lift_1[i].Id);
	}
	i=0;
	while(xQueueReceive(_liftBToController,&recJob, TIME)!=0)
	{
		do
		{
			if(recJob.id==Jobs_inprogress_lift_2[i].Id)
			{
				if(recJob.success==0)
				{
					Pending_orders[order]=Jobs_inprogress_lift_2[i];
					order++;
				}else if(recJob.success==1)
				{
					msg.data[3]=Jobs_inprogress_lift_2[i].Floor;//turn off lampe inside
					msg.id=0xD;
					xQueueSend(_toCan, &msg, 0);
					if(direction_lift_1==Up)
					{
						msg.data[3]=0xF1;						//turn off lampe outside up
						msg.id=Jobs_inprogress_lift_2[i].Floor*2;
						xQueueSend(_toCan, &msg, 0);
						msg.data[3]=0xF1;
						msg.id=Jobs_inprogress_lift_2[i].Floor*2+1;
						xQueueSend(_toCan, &msg, 0);
					}else if(direction_lift_1==Down)
					{
						msg.data[3]=0xF0;					//turn off lampe outside down
						msg.id=Jobs_inprogress_lift_2[i].Floor*2;
						xQueueSend(_toCan, &msg, 0);
						msg.data[3]=0xF0;
						msg.id=Jobs_inprogress_lift_2[i].Floor*2+1;
						xQueueSend(_toCan, &msg, 0);
					}
					last_position_lift_2=Jobs_inprogress_lift_2[i].Floor;
					Jobs_inprogress_lift_2[i]=noOrder;
					inprogress_lift_2=Array_arrange_4(Jobs_inprogress_lift_2);
				}
			}
			i++;
		}while(recJob.id!=Jobs_inprogress_lift_1[i].Id);
	}
	i=0;
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
			if(Pending_orders[order].Lift==1)
			{
				msg.data[3]=Pending_orders[order].Floor+0x80;//turn on lampe inside
				msg.id=0xC;
				xQueueSend(_toCan, &msg, 0);

			}else if(Pending_orders[order].Lift==2)
			{
				msg.data[3]=Pending_orders[order].Floor+0x80;//turn on lampe inside
				msg.id=0xD;
				xQueueSend(_toCan, &msg, 0);
			}else
			{
				if(Pending_orders[order].Direction==Up)
				{
					msg.data[3]=0xFB;						//turn on lampe outside
					msg.id=Pending_orders[order].Floor*2;
					xQueueSend(_toCan, &msg, 0);

					msg.data[3]=0xFB;
					msg.id=Pending_orders[order].Floor*2+1;
					xQueueSend(_toCan, &msg, 0);
				}else if(Pending_orders[order].Direction==Down)
				{
					msg.data[3]=0xFC;					//turn on lampe outside
					msg.id=Pending_orders[order].Floor*2;
					xQueueSend(_toCan, &msg, 0);

					msg.data[3]=0xFC;
					msg.id=Pending_orders[order].Floor*2+1;
					xQueueSend(_toCan, &msg, 0);
				}
			}
			order++;
			currentId++;
		}
	}

	vTaskDelay(50);
	}

}

int Find_direction (Order p[],int last_position)
{
	if(p[0].Floor!=0)
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
	}else {return Stay;}
	// h�chste oder tiefste position
}
int Array_arrange_4 (Order p[]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<Max;i++)
	{
		if(p[i].Floor==0)
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


