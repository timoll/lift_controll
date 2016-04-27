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
int messageLength=4;
int levelB;
int levelA;
int almost_A;
int almost_B;
int liftName;
CARME_CAN_MESSAGE msg;

//Defines

#define DOWN 	0
#define UP 		1
#define STILL 	2




#define motorA 		0x00
#define motorB 		0x01
#define stock1A 	0x02
#define stock1B 	0x03
#define stock2A 	0x04
#define stock2B 	0x05
#define stock3A 	0x06
#define stock3B 	0x07
#define stock4A 	0x08
#define stock4B 	0x09
#define stock5A 	0x0A
#define stock5B 	0x0B


//Function Prototype
 void openDoor(int address);
 void closeDoor(int address);
 void stoppDoor(int address);
 void reachedDestination(int address);
 void allmostReachedUp(int address);
 void allmostReachedDown(int address);
 void moveUp(int address);
 void moveDown(int address);
 void getInformation(void);

extern void lift(void *pvargs);


#endif /* LIFT_H_ */
