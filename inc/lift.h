/*
 * lift.h
 *
 *  Created on: Mar 30, 2016
 *      Author: Swen
 */

#ifndef LIFT_H_
#define LIFT_H_

//includes
#include <can.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stdbool.h>
#include <lcd.h>
#include <task_communication.h>

//Data

CARME_CAN_MESSAGE msg;

//Defines
#define DOWN 	0
#define UP 		1
#define STILL 	2

//Function Prototype
 void openDoor(int address);
 void closeDoor(int address);
 void stoppDoor(int address);
 void reachedDestination(int address);
 void allmostReachedUp(int address);
 void allmostReachedDown(int address);
 void moveUp(int address);
 void moveDown(int address);
int getInformation(int);

extern void lift(void *pvargs);
extern int initLift(int);



#endif /* LIFT_H_ */
