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

#define Max_orders 			20
#define Max_inprogress		20

#define Up					1
#define Down				2
#define Stay				0

#define Floor				0
#define Direction			1
#define Lift				2

#define Lift1				1
#define Lift2				2
#define Outside				0
/*----- Data ---------------------------------------------------------------*/
char Pending_orders [Max_orders][3];
char Jobs_inprogress_lift_1 [Max_inprogress][3];
char Jobs_inprogress_lift_2 [Max_inprogress][3];

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




/*----- Function prototypes ------------------------------------------------*/

char maxmin_floor (char direction,char position);
char Find_direction (char p[][2],char last_position);
char Array_arrange_2 (char p[][3]);
char Array_arrange_3 (char p[][3]);



void controller(void)
{
	int i;
	//wichtige informationen updaten
	switch (state)
	{
		case Inform:

			direction_lift_1=Find_direction(Jobs_inprogress_lift_1[0][0],last_position_lift_1);
			direction_lift_2=Find_direction(Jobs_inprogress_lift_2[0][0],last_position_lift_2);
/*			possible_floors_lift_1[0]=last_position_lift_1;
			possible_floors_lift_2[0]=last_position_lift_2;
			possible_floors_lift_1[1]=maxmin_floor(direction_lift_1,last_position_lift_1);
			possible_floors_lift_2[1]=maxmin_floor(direction_lift_2,last_position_lift_2);*/


			break;

		case Order_distribution:

			order=Array_arrange_3(Pending_orders);
			for(i=0;i<order;i++)
			{
				char Lift_12;
				Lift_12=Pending_orders[i][Lift];
				switch (Lift_12)
				{
					case Outside:
						if(abs(last_position_lift_1-Pending_orders[i][Floor])>abs(last_position_lift_2-Pending_orders[i][Floor]))
						{
							Lift_12=Lift2;
						}
						else
						{
							Lift_12=Lift1;
						}
						break;

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

							Pending_orders[i][0]=0;			//Auftrag l�schen
							Pending_orders[i][1]=0;
							Pending_orders[i][2]=0;

						}else if(direction_lift_1==Up)
						{
							if(last_position_lift_1<=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_1[i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_1[i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_1[i][Lift]=Pending_orders[i][Lift];

								Pending_orders[i][0]=0;			//Auftrag l�schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
							}else if(Pending_orders[i][Lift]==Outside)
							{
								Lift_12=Lift2;
							}
						}else if(direction_lift_1==Down)
						{
							if(last_position_lift_1>=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_1[i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_1[i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_1[i][Lift]=Pending_orders[i][Lift];

								Pending_orders[i][0]=0;			//Auftrag l�schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
							}else if(Pending_orders[i][Lift]==Outside)
							{
								Lift_12=Lift2;
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

							Pending_orders[i][0]=0;			//Auftrag l�schen
							Pending_orders[i][1]=0;
							Pending_orders[i][2]=0;

						}else if(direction_lift_2==Up)
						{
							if(last_position_lift_2<=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_2[i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_2[i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_2[i][Lift]=Pending_orders[i][Lift];

								Pending_orders[i][0]=0;			//Auftrag l�schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
							}else if(Pending_orders[i][Lift]==Outside)
							{
								Lift_12=Lift1;
							}
						}else if(direction_lift_2==Down)
						{
							if(last_position_lift_2>=Pending_orders[i][Floor])
							{
								Jobs_inprogress_lift_2[i][Floor]=Pending_orders[i][Floor];
								Jobs_inprogress_lift_2[i][Direction]=Pending_orders[i][Direction];
								Jobs_inprogress_lift_2[i][Lift]=Pending_orders[i][Lift];

								Pending_orders[i][0]=0;			//Auftrag l�schen
								Pending_orders[i][1]=0;
								Pending_orders[i][2]=0;
							}else if(Pending_orders[i][Lift]==Outside)
							{
								Lift_12=Lift1;
							}
						}
						break;
				}
			}
			inprogress_lift_1=Array_arrange_2(Jobs_inprogress_lift_1);
			inprogress_lift_2=Array_arrange_2(Jobs_inprogress_lift_2);

			break;

		case Inquiry:
			if(Jobs_inprogress_lift_1[0][0]!=0)
			{
				//in queue schreiben
			}
			if(Jobs_inprogress_lift_2[0][0]!=0)
			{
				//in queue schreiben
			}
			break;

		case Wait:
			/* if(xQueueReceive(...,...,...)
			 * {
			 *
			 * }
			 */

			break;

		case New_information:

			break;
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
	}

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
char Array_arrange_2 (char p[][3]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<Max_inprogress;i++)
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
			if(j!=0)
			{
				p[i][0]=0;
				p[i][1]=0;
				p[i][21]=0;
			}
		}
	}
	return (Max_inprogress-j);
}
char Array_arrange_3 (char p[][3]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<Max_orders;i++)
	{
		if(p[i][0]==0)
		{
			j++;
		}
		else
		{
			p[i-j][0]=p[i][0];
			p[i-j][1]=p[i][1];
			p[i-j][2]=p[i][2];
			if(j!=0)
			{
				p[i][0]=0;
				p[i][1]=0;
				p[i][2]=0;
			}
		}
	}
	return (Max_orders-j);
}
char maxmin_floor (char direction,char position)
{
	if(direction==0)
	{
		return position;
	}else if(direction==1)
	{
		return 5;									//Stock 5
	}else
	{
		return 1;									//Stock 1
	}
}