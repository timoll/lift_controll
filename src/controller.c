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


#define Max 				20


#define Up					1
#define Down				2
#define Stay				0

#define Floor				0
#define Direction			1
#define Lift				2
#define ID					3

#define Lift1				1
#define Lift2				2
#define Outside				0
#define TIME				1000
/*----- Data ---------------------------------------------------------------*/
char Pending_orders [Max][4];
char Jobs_inprogress_lift_1 [Max][4];
char Jobs_inprogress_lift_2 [Max][4];

char possible_floors_lift_1[2];
char possible_floors_lift_2[2];

char last_position_lift_1=1;
char last_position_lift_2=1;
char direction_lift_1;
char direction_lift_2;
char order;
char inprogress_lift_1;
char inprogress_lift_2;

char state=0;

Job sendJob = {0,0,0};
Job recJob = {0,0,0};





/*----- Function prototypes ------------------------------------------------*/


char Find_direction (char p[][4],char last_position);
char Array_arrange_4 (char p[][4]);




void controller(void)
{
	int i;
	//wichtige informationen updaten
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
				Lift_12=Pending_orders[i][Lift];
				if(Lift_12==Outside)
				{
					if(abs(last_position_lift_1-Pending_orders[i][Floor])>abs(last_position_lift_2-Pending_orders[i][Floor]))
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
							if(last_position_lift_1>Pending_orders[i][Floor])
							{
								direction_lift_1=Down;
							}else if(last_position_lift_1<Pending_orders[i][Floor])
							{
								direction_lift_1=Up;
							}else
							{
								direction_lift_1=Stay;
							}
							Jobs_inprogress_lift_1[inprogress_lift_1+i][Floor]=Pending_orders[i][Floor];
							Jobs_inprogress_lift_1[inprogress_lift_1+i][Direction]=Pending_orders[i][Direction];
							Jobs_inprogress_lift_1[inprogress_lift_1+i][Lift]=Pending_orders[i][Lift];
							Jobs_inprogress_lift_1[inprogress_lift_1+i][ID]=Pending_orders[i][ID];

							Pending_orders[i][0]=0;			//Auftrag lï¿½schen
							Pending_orders[i][1]=0;
							Pending_orders[i][2]=0;
							Pending_orders[i][3]=0;

						}else if(direction_lift_1==Up)
						{
							if(last_position_lift_1<=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_1[inprogress_lift_1+i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_1[inprogress_lift_1+i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_1[inprogress_lift_1+i][Lift]=Pending_orders[i][Lift];
								Jobs_inprogress_lift_1[inprogress_lift_1+i][ID]=Pending_orders[i][ID];

								Pending_orders[i][0]=0;			//Auftrag lï¿½schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
								Pending_orders[i][3]=0;
							}
						}else if(direction_lift_1==Down)
						{
							if(last_position_lift_1>=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_1[inprogress_lift_1+i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_1[inprogress_lift_1+i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_1[inprogress_lift_1+i][Lift]=Pending_orders[i][Lift];
								Jobs_inprogress_lift_1[inprogress_lift_1+i][ID]=Pending_orders[i][ID];

								Pending_orders[i][0]=0;			//Auftrag lï¿½schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
								Pending_orders[i][3]=0;
							}
						}
						break;

					case Lift2:
						if(direction_lift_2==Stay)
						{
							//Direkte aufgaben verteilung
							if(last_position_lift_2>Pending_orders[i][Floor])
							{
								direction_lift_2=Down;
							}else if(last_position_lift_2<Pending_orders[i][Floor])
							{
								direction_lift_2=Up;
							}else
							{
								direction_lift_2=Stay;
							}
							Jobs_inprogress_lift_2[inprogress_lift_2+i][Floor]=Pending_orders[i][Floor];
							Jobs_inprogress_lift_2[inprogress_lift_2+i][Direction]=Pending_orders[i][Direction];
							Jobs_inprogress_lift_2[inprogress_lift_2+i][Lift]=Pending_orders[i][Lift];
							Jobs_inprogress_lift_2[inprogress_lift_2+i][ID]=Pending_orders[i][ID];

							Pending_orders[i][0]=0;			//Auftrag lï¿½schen
							Pending_orders[i][1]=0;
							Pending_orders[i][2]=0;
							Pending_orders[i][3]=0;

						}else if(direction_lift_2==Up)
						{
							if(last_position_lift_2<=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_2[inprogress_lift_2+i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_2[inprogress_lift_2+i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_2[inprogress_lift_2+i][Lift]=Pending_orders[i][Lift];
								Jobs_inprogress_lift_2[inprogress_lift_2+i][ID]=Pending_orders[i][ID];

								Pending_orders[i][0]=0;			//Auftrag lï¿½schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
								Pending_orders[i][3]=0;
							}
						}else if(direction_lift_2==Down)
						{
							if(last_position_lift_2>=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_2[inprogress_lift_2+i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_2[inprogress_lift_2+i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_2[inprogress_lift_2+i][Lift]=Pending_orders[i][Lift];
								Jobs_inprogress_lift_2[inprogress_lift_2+i][ID]=Pending_orders[i][ID];

								Pending_orders[i][0]=0;			//Auftrag lï¿½schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
								Pending_orders[i][3]=0;
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
			if(Jobs_inprogress_lift_1[0][0]!=0)
			{
				i=0;
				while(Jobs_inprogress_lift_1[i][0]!=0)
				{
					sendJob.id=Jobs_inprogress_lift_1[i][ID];
					sendJob.targetFloor=Jobs_inprogress_lift_1[i][Floor];
					sendJob.success=0;

					xQueueSend(_controllerToLiftA, &sendJob, 0);

					i++;
				}
				//in queue schreiben
			}
			if(Jobs_inprogress_lift_2[0][0]!=0)
			{
				i=0;
				while(Jobs_inprogress_lift_1[i][0]!=0)
				{
					sendJob.id=Jobs_inprogress_lift_2[i][ID];
					sendJob.targetFloor=Jobs_inprogress_lift_2[i][Floor];
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
	while(xQeueReceive(_liftAToController,&recJob, TIME)!=0)
	{
		do
		{
			if(recJob.id==Jobs_inprogress_lift_1[i][ID])
			{
				if(recJob.success==0)
				{
					Pending_orders[order][Floor]=Jobs_inprogress_lift_1[i][Floor];
					Pending_orders[order][Direction]=Jobs_inprogress_lift_1[i][Direction];
					Pending_orders[order][Lift]=Jobs_inprogress_lift_1[i][Lift];
					Pending_orders[order][ID]=Jobs_inprogress_lift_1[i][ID];
					order++;
				}else if(recJob.success==1)
				{
					Jobs_inprogress_lift_1[i][Floor]=0;			//Auftrag lï¿½schen
					Jobs_inprogress_lift_1[i][Direction]=0;
					Jobs_inprogress_lift_1[i][Lift]=0;
					Jobs_inprogress_lift_1[i][ID]=0;
					inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_1);
//////////////////////////////////////////Licht löschen
				}
			}
			i++;
		}while(recJob.id!=Jobs_inprogress_lift_1[i][ID]);
	}
	while(xQeueReceive(_liftBToController,&recJob, TIME)!=0)
	{
		do
		{
			if(recJob.id==Jobs_inprogress_lift_2[i][ID])
			{
				if(recJob.success==0)
				{
					Pending_orders[order][Floor]=Jobs_inprogress_lift_2[i][Floor];
					Pending_orders[order][Direction]=Jobs_inprogress_lift_2[i][Direction];
					Pending_orders[order][Lift]=Jobs_inprogress_lift_2[i][Lift];
					Pending_orders[order][ID]=Jobs_inprogress_lift_2[i][ID];
					order++;
				}else if(recJob.success==1)
				{
					Jobs_inprogress_lift_2[i][Floor]=0;			//Auftrag lï¿½schen
					Jobs_inprogress_lift_2[i][Direction]=0;
					Jobs_inprogress_lift_2[i][Lift]=0;
					Jobs_inprogress_lift_2[i][ID]=0;
					inprogress_lift_1=Array_arrange_4(Jobs_inprogress_lift_2);
//////////////////////////////////////////Licht löschen
				}
			}
			i++;
		}while(recJob.id!=Jobs_inprogress_lift_2[i][ID]);
	}

	/*
	 * Molllllleeeeeee
	 *
	 * Hier Can Message empfangen
	 *
	 *
	 *
	 *
	 *
	 */
	vTaskDelay(100);

}

char Find_direction (char p[][4],char last_position)
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
	// hï¿½chste oder tiefste position
}
char Array_arrange_4 (char p[][4]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<Max;i++)
	{
		if(p[i][0]==0)
		{
			j++;
		}
		else
		{
			p[i-j][0]=p[i][0];
			p[i-j][1]=p[i][1];
			p[i-j][1]=p[i][2];
			p[i-j][1]=p[i][3];
			if(j!=0)
			{
				p[i][0]=0;
				p[i][1]=0;
				p[i][2]=0;
				p[i][3]=0;
			}
		}
	}
	return (Max-j);
}




