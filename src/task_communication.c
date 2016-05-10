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
	_zeroCanMessage.dlc=4;
	_zeroCanMessage.ext=0;
	_zeroCanMessage.id=0;
	_zeroCanMessage.rtr=0;
	int i=0;
	for(i=0;i<8;i++){
		_zeroCanMessage.data[i]=0;
	}
	_canToLift = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );
	_canToController = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );
	_toCan = xQueueCreate( 20, sizeof(CARME_CAN_MESSAGE) );

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
	if( _toCan == 0 ) {
		error|=0x04;
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

