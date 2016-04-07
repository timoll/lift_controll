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
/*----- Data ---------------------------------------------------------------*/
char Pending_orders [20][3];
char Jobs_inprogress_lift_1 [10][2];
char Jobs_inprogress_lift_2 [10][2];

char possible_floors_lift_1[2];
char possible_floors_lift_2[2];

char last_position_lift_1;
char last_position_lift_2;
char direction_lift_1;
char direction_lift_2;

char state=0;



/*----- Definition ----------------------------------------------------------*/
#define Inform				0
#define Order_distribution	1
#define Inquiry				2
#define Wait				3
#define New_information		4
/*----- Function prototypes ------------------------------------------------*/

char maxmin_floor (char direction,char position);
char Direction (char p[][2],char last_position);
void Array_arrange_2 (char p[][2]);
void Array_arrange_3 (char p[][3]);



void controller(void)
{
	//wichtige informationen updaten
	switch (state)
	{
		case Inform:

			direction_lift_1=Direction(Jobs_inprogress_lift_1,last_position_lift_1);
			direction_lift_2=Direction(Jobs_inprogress_lift_2,last_position_lift_2);
			possible_floors_lift_1[0]=last_position_lift_1;
			possible_floors_lift_2[0]=last_position_lift_2;
			possible_floors_lift_1[1]=maxmin_floor(direction_lift_1,last_position_lift_1);
			possible_floors_lift_2[1]=maxmin_floor(direction_lift_2,last_position_lift_2);


			break;

		case Order_distribution:

			break;

		case Inquiry:

			break;

		case Wait:

			break;

		case New_information:

			break;

	}

}

char Direction (char p[][2],char last_position)
{
	if(last_position==p[0][0])
	{
		return 0;
	}else if(last_position>p[0][0])
	{
		return 2;
	}else if(last_position<p[0][0])
	{
		return 1;
	}else
	{
		return 0;
	}
	// höchste oder tiefste position
}
void Array_arrange_2 (char p[][2]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<10;i++)
	{
		if(p[i][0]==0)
		{
			j++;
		}

		p[i-j][0]=p[i][0];
		p[i-j][1]=p[i][1];

	}
	for(i=0;i<j;i++)
	{
		p[10-i][0]=0;
		p[10-i][1]=0;
	}
}
void Array_arrange_3 (char p[][3]) //Array sortieren
{
	int i,j;
	j=0;
	for(i=0;i<10;i++)
	{
		if(p[i][0]==0)
		{
			j++;
		}

		p[i-j][0]=p[i][0];
		p[i-j][1]=p[i][1];
		p[i-j][2]=p[i][2];

	}
	for(i=0;i<j;i++)
	{
		p[10-i][0]=0;
		p[10-i][1]=0;
		p[10-i][2]=0;
	}
}
char maxmin_floor (char direction,char position)
{
	if(direction==0)
	{
		return position;
	}else if(direction==1)
	{
		return 5;
	}else
	{
		return 1;
	}
}
