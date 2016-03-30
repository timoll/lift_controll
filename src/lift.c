/*
 * lift.c
 *
 *  Created on: Mar 30, 2016
 *      Author: Swen
 */


//include
#include <lift.h>
#include <can.h>

//code


TaskHandle_t xHandle = NULL;
xTaskCreate(lift,  (const signed char * const)"lift",  1024, NULL, 4, NULL);

QueueHandle_t xQueueDirection;
QueueHandle_t xQueuePosition;

xQueueDirection = xQueueCreate( 10, sizeof( unsigned int ) );
xQueuePosition = xQueueCreate( 10, sizeof( unsigned int ) );


void lift(void *pvargs)
{
	CARME_CAN_MESSAGE msg;

	for(;;)
	{

	}
}


void openDoor(void)
{
	msg.id  = 0;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = 0;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = i * 0x11;
	}

}
void closeDoor(void)
{
	msg.id  = 0;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = 0;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = i * 0x11;
	}
}
void reachedDestination(void)
{
	msg.id  = 0;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = 0;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = i * 0x11;
	}
}
void moveUp(void)
{
	msg.id  = 0;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = 0;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = i * 0x11;
	}
}
void moveDown(void)
{
	msg.id  = 0;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = 0;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = i * 0x11;
	}
}


