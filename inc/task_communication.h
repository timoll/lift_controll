
#ifndef TASK_COMMUNICATION_H_
#define TASK_COMMUNICATION_H_
#include <includes.h>


xQueueHandle _canToLift;
xQueueHandle _canToController;
xQueueHandle _toCan;

xQueueHandle _controllerToLiftA;
xQueueHandle _controllerToLiftB;
xQueueHandle _liftAToController;
xQueueHandle _liftBToController;

typedef struct _Job{
	int id;
	int targetFloor;
	int success;
}Job;
CARME_CAN_MESSAGE _zeroCanMessage;//= {0,0,0,4,{0,0,0,0,0,0,0,0}}; //default Message

int initQueues();

#endif /* TASK_COMMUNICATION_H_ */
