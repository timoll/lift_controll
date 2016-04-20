/*
 * task_communication.c
 *
 *  Created on: Apr 20, 2016
 *      Author: Swen
 */
#include "task_communication.h"
int initQueues()
{
	int error=0;

	_canToLift = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );
	_canToController = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );
	_liftToCan = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );
	_controllerToCan = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );

	_controllerToLiftA = xQueueCreate( 20, sizeof(Job) );
	_controllerToLiftB = xQueueCreate( 20, sizeof(Job) );
	_liftAToController = xQueueCreate( 20, sizeof(Job) );
	_liftBToController = xQueueCreate( 20, sizeof(Job) );

	if( _canToLift == 0 ) {
		error|=0x01;
	}
	if( _canToController == 0 ) {
		error|=0x02;
	}
	if( _liftToCan == 0 ) {
		error|=0x04;
	}
	if( _controllerToCan == 0 ) {
		error|=0x08;
	}
	if( _controllerToLiftA == 0 ) {
		error|=0x10;
	}
	if( _controllerToLiftB == 0 ) {
		error|=0x20;
	}
	if( _liftAToController == 0 ) {
		error|=0x40;
	}
	if( _liftBToController == 0 ) {
		error|=0x80;
	}

	return error;
}

