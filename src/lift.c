/*
 * lift.c
 *
 *  Created on: Mar 30, 2016
 *      Author: Swen
 */
//include
#include "lift.h"

//Data
int endschalter[2];
int stillMoveUP[2]={0},stillMoveDOWN[2]={0};
int level[2];
int almost[2];
int accepted[2][5]={{0},{0}};
int direction[2]={STILL,STILL};//Variable to store the actual direction


int StoppLevel[2][5];//indicates the floors where the elevator needs to stopp

char liftString[2][40];

//Settings
int queueTime=10;
int messageLength=4;
//#define LIFT_B //Both elevators activatet

//array to save the jobs that where accepted in order to inform controller about finished jobs
Job acceptedJob[2][5][5];
Job noJob= {0,0,0};

//transform CanMessage

/**
 * @getInformation process can Message and consumes it if the id matches
 * @return 1 when message was consumed, 0 when no message available or id didn't matches
 */
int getInformation(int id)
{
	CARME_CAN_MESSAGE receivedMessage;
	//Set Endschalter!!!
	if(xQueuePeek(_canToLift, &receivedMessage, queueTime)){//write the contant into receivedMessage
		int liftName = receivedMessage.id %2; //find out which elevator is meant
		if(liftName == id){
			endschalter[id] = receivedMessage.data[0];


			if(endschalter[id] & 0x01) {//reached floor and ID is For Elevator A
				level[id]=(receivedMessage.id)/2;
			}

			if(endschalter[id] & 0x02) { //close to floor and ID is for ElevatorA
				almost[id]=(receivedMessage.id)/2;
			}
			xQueueReceive(_canToLift, &receivedMessage, queueTime);
			return 1;
		}
	}
	return 0;
}

//functions to controll elevator-door
void openDoor(int address)//Choose which floor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
	msg.id  = address;
	msg.data[2]=0x5;//open & fast
	xQueueSend(_toCan,&msg,queueTime);
}
void closeDoor(int address)//Choose which floor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
	msg.id=address;
	msg.data[2]=0x02;// close & slow
	xQueueSend(_toCan,&msg,queueTime);
}
void stoppDoor(int address)//Choose which floor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
	msg.id=address;
	xQueueSend(_toCan,&msg,queueTime);
}

//functions to controll the motors
void reachedDestination(int address)//Choose which motor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
	msg.id  = address;
	char string[20];
	sprintf(string,"reached by %d",address);
	LCD_DisplayStringXY(10, 10, string);
	xQueueSend(_toCan,&msg,queueTime);
}
void allmostReachedUp(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
	msg.id  = address;
	msg.data[2]=0x02;//up & slow
	xQueueSend(_toCan,&msg,queueTime);
}
void allmostReachedDown(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
		msg.id  = address;
		msg.data[2]=0x05;//down & slow
		xQueueSend(_toCan,&msg,queueTime);
}
void moveUp(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg=_zeroCanMessage;
	msg.id  = address;
	msg.data[2]=0x04;//up & fast
	xQueueSend(_toCan,&msg,queueTime);
}
void moveDown(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg = _zeroCanMessage;
	msg.id  = address;
	msg.data[2]=0x05;//down & fast
	xQueueSend(_toCan,&msg,queueTime);
}

//functions to manage jobs
void addJob(Job newJob, int level, int id)
{
	acceptedJob[id][level-1][accepted[id][level-1]]=newJob;
	accepted[id][level-1]++;
}
void finishedJob(int level, int id)
{
	int i;
	for(i=0;i<5;i++)
	{
		if(acceptedJob[id][level-1][i].targetFloor!=0){
			acceptedJob[id][level-1][i].success=1; //set Succes bit
			if(id){
				xQueueSend(_liftBToController, &acceptedJob[id][level-1][i],queueTime);//inform the Controller
			} else {
				xQueueSend(_liftAToController, &acceptedJob[id][level-1][i],queueTime);//inform the Controller
			}
			acceptedJob[id][level-1][i]=noJob;//erase all Jobs done
			//set counter to Zero
		}

	}
	accepted[id][level-1]=0;

}
//
int initLift(int id)
{
	while(!xSemaphoreTake(_initLift, 10)){
		getInformation(id);
	}
	int finished=0;
	moveUp(id);
	vTaskDelay(1000);
	moveDown(id);

	while(finished!=1)
	{
		getInformation(id);

		if((level[id]==1)&&(endschalter[id] & 0x01))
		{
			reachedDestination(id);//turn off motorA
			finished=1;
		}
	}
	xSemaphoreGive(_initLift);
	vTaskDelay(100);
	while(!xSemaphoreTake(_initLift, 10)){
		getInformation(id);
	}
	xSemaphoreGive(_initLift);
	return finished;
}


//Task-Code
void lift(void *pvargs)
{

	int id=(int)(pvargs);
	//used for saving the current job received from controller
	Job RXJob;
	Job TXJob;
//	if(id==1){
//		while(1){
//			getInformation(id);
//		}
//	}
	initLift(id);
	for(;;){
		while(getInformation(id));
		//doorControll();
		//sprintf(liftString, "endA:%x, endB:%x levelA:%d  LevelB:%d", endschalterA, endschalterB, levelA, levelB);
		//LCD_DisplayStringXY(posLiftx, posLifty,liftString);


		//Procedure
		//1)check for job + set the Levels where I need to stop + set Direction + answer which job has NOT been accepted
		//2)check if almost reached
		//3)check if  reached + erase the StoppLevel + open and Close door + inform succesfull job

		//1a)-------------------------------------------------------------------------------------------------------------------------

		//writes the Message of the queue into RXJobA
		int messageRecived;
		if(id == 0){
			messageRecived = xQueueReceive( _controllerToLiftA, &RXJob , 10 );
		} else {
			messageRecived = xQueueReceive( _controllerToLiftB, &RXJob , 10 );
		}
		if(messageRecived) {
			if((direction[id] == UP) && (RXJob.targetFloor > level[id])) {//Moving Upwards and the level is above

				StoppLevel[id][RXJob.targetFloor-1] = 1; //set a mark where I need to stopp the elevator
				addJob(RXJob,RXJob.targetFloor,id);//strore the Job
				moveUp(id);

			} else if((direction[id] == DOWN) && (RXJob.targetFloor < level[id])) {//Moving Downwards and the level is underneath
				StoppLevel[id][RXJob.targetFloor-1]=1;
				addJob(RXJob,RXJob.targetFloor,id);//store the job
				moveDown(id);
			} else if(direction[id] == STILL){//Not moving
				if(level[id] < RXJob.targetFloor ){//the Level is above
					direction[id]=UP;
					StoppLevel[id][RXJob.targetFloor-1]=1;
					addJob(RXJob,RXJob.targetFloor,id);//store the job
					moveUp(id);
				}
				if(level[id] > RXJob.targetFloor){//the Level is underneath
					direction[id]=DOWN;
					StoppLevel[id][RXJob.targetFloor-1]=1;
					addJob(RXJob,RXJob.targetFloor,id);//store the job
					moveDown(id);
				}
				if(level[id] == RXJob.targetFloor){//the Job is for the floor needed
					StoppLevel[id][RXJob.targetFloor-1]=1;
					addJob(RXJob,RXJob.targetFloor,id);//store the job
				}
			} else {
				TXJob=RXJob;
				TXJob.success=0;	//tell contorller Job is not accepted
				if(id){
					xQueueSend(_liftBToController,&TXJob,10);
				} else {
					xQueueSend(_liftAToController,&TXJob,10);
				}
				//StoppLevelA[RXJobA.targetFloor-1]=0;
				//Say to Strategie the job is not accepted
			}

			//Is there to start the whole elevator first Job defines the direction!!!
			//and if there is no Job for long time, the elevator will have direction=STILL

		 }

		 //2a)-------------------------------------------------------------------------------------------------------------------------
		if((endschalter[id] & 0x02) && (StoppLevel[id][almost[id]-1] == 1) && (direction[id] == UP)){//almost reached and near-switch pressed
			allmostReachedUp(id);
		}

		if((endschalter[id] & 0x02) && (StoppLevel[id][almost[id]-1] == 1) && (direction[id] == DOWN)){//almost reached and near-switch pressed
			allmostReachedDown(id);
		}

		//3a)-------------------------------------------------------------------------------------------------------------------------
		if(((endschalter[id] & 0x01) && (StoppLevel[id][level[id]-1] == 1))||((direction[id]==STILL) && (StoppLevel[id][level[id]-1] == 1 ))) {// reached and StoppLevelA Bit is set or Lift is already in the level needed
			reachedDestination(id);//Stops the motor

			openDoor(level[id]*2+id);//(levelA*2) is necessary to translate for example LevelA=1 to the define stock1A=0x02 which is the address for the Can-Message

			while(!(endschalter[id] & 0x08)){//door open
				vTaskDelay(10);
				getInformation(id);
			}
			stoppDoor(level[id]*2+id);
			StoppLevel[id][level[id]-1]=0; //erase the Bit because the Job is done
			closeDoor(level[id]*2+id);
			while(!(endschalter[id] & 0x04) ){//door closed
				vTaskDelay(10);
				getInformation(id);
			}
			stoppDoor(level[id]*2);
			//Check if there are still some jobs I need to do
			if(direction[id]==UP){
				int i;
				stillMoveUP[id]=0;
				for(i=level[id];i<5;i++){
					if(StoppLevel[id][i]==1){
						stillMoveUP[id]=1;
					}
				}
				if(stillMoveUP[id]==1){
					direction[id]=UP;//set Direction
					moveUp(id);//move elavator
				}
				if(stillMoveUP[id]==0){
					direction[id]=STILL;
				}
			}
			if(direction[id]==DOWN){
				int i;
				stillMoveDOWN[id]=0;
				for(i=0;i<level[id];i++){
					if(StoppLevel[id][i]==1){
						stillMoveDOWN[id]=1;
					}
				}
				if(stillMoveDOWN[id]==1){
					direction[id]=DOWN;//set Direction
					moveDown(id);//move elavator
				}
				if(stillMoveDOWN[id]==0){
					direction[id]=STILL;
				}
			}
			//Job is done

			finishedJob(level[id],id);//Inform of the finished Jobs
		}
		vTaskDelay(35);
	}
}

