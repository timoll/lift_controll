/*
 * lift.c
 *
 *  Created on: Mar 30, 2016
 *      Author: Swen
 */
//include
#include "lift.h"

//Data
int endschalterA;
int endschalterB;
int AstillMoveUP=0,AstillMoveDOWN=0;
int BstillMoveUP=0,BstillMoveDOWN=0;
int levelB;//represents the current floor
int levelA;//represents the current floor
int almost_A;
int almost_B;
int liftName;
int acceptedA=0, acceptedB=0;
int directionA=STILL;//Variable to store the actual direction
int directionB=STILL;//Variable to store the actual direction

int posLifty=10;
int posLiftx=10;

int StoppLevelA[5];//indicates the floors where the elevator needs to stopp
int StoppLevelB[5];//indicates the floors where the elevator needs to stopp

char liftString[40];

//Settings
int queueTime=10;
int messageLength=4;
//#define LIFT_B //Both elevators activatet

//array to save the jobs that where accepted in order to inform controller about finished jobs
Job acceptedJobA[5][5];
Job acceptedJobB[5][5];
Job noJob={0,0,0};

//transform CanMessage
void getInformation(void)
{
	CARME_CAN_MESSAGE receivedMessage;
	//Set Endschalter!!!
	xQueueReceive(_canToLift,&receivedMessage, queueTime); //write the contant into receivedMessage
	liftName = receivedMessage.id %2; //find out which elevator is meant
	switch (liftName)
	{
		case 0: //Lift A
			endschalterA = receivedMessage.data[0];
			break;
		case 1:	//Lift B
			endschalterB = receivedMessage.data[0];
			break;
	}
	//Set LEVEL!!!
	
	/* Level 1 =>1
	 * Level 2 =>2
	 * Level 3 =>3
	 * Level 4 =>4
	 * Level 5 =>5
	 * Level 6 =>6
	 * same for levelAalmost*/
	//set LevelA
	if(((endschalterA & 0x01)) &&(receivedMessage.id%2==0))//reached floor and ID is For Elevator A
	{
		levelA=receivedMessage.id/2;
	}

	if(((endschalterA & 0x02)) &&(receivedMessage.id%2==0))//close to floor and ID is for ElevatorA
	{
		almost_A=receivedMessage.id/2;
	}
	//set LevelB
	
	if(((endschalterB & 0x01)) && (receivedMessage.id%2!=0))//reached floor and ID is not for Elevator A
	{
		levelB=(receivedMessage.id-1)/2;
	}

	if(((endschalterB & 0x02)) && (receivedMessage.id%2!=0))//close to floor and ID is not for Elevator A
	{
		almost_B=(receivedMessage.id-1)/2;
	}
}

//functions to controll elevator-door
void openDoor(int address)//Choose which floor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg;
	msg.id  = address;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = messageLength;
	int i;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = 0x00;
	}
	msg.data[4]=0x5;//open & fast
	xQueueSend(_toCan,&msg,queueTime);
}
void closeDoor(int address)//Choose which floor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg;
	msg.id  = address;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = messageLength;
	int i;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = 0x00;
	}
	msg.data[4]=0x02;// close & slow
	xQueueSend(_toCan,&msg,queueTime);
}
void stoppDoor(int address)//Choose which floor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg;
	msg.id  = address;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = messageLength;
	int i;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = 0x00;// stopp Door
	}
	xQueueSend(_toCan,&msg,queueTime);
}

//functions to controll the motors
void reachedDestination(int address)//Choose which motor
{
	/*
	 * as address put stock1A, stock1B....
	 */

	CARME_CAN_MESSAGE msg;
	msg.id  = address;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = messageLength;
	int i;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = 0x00;//stop the motor!!
	}
	xQueueSend(_toCan,&msg,queueTime);
}
void allmostReachedUp(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg;
		msg.id  = address;
		msg.ext = 0;
		msg.rtr = 0;
		msg.dlc = messageLength;
		int i;
		for (i = 0; i < 8; i++)
		{
			msg.data[i] = 0x00;
		}
		msg.data[2]=0x02;//up & slow
		xQueueSend(_toCan,&msg,queueTime);
}
void allmostReachedDown(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg;
		msg.id  = address;
		msg.ext = 0;
		msg.rtr = 0;
		msg.dlc = messageLength;
		int i;
		for (i = 0; i < 8; i++)
		{
			msg.data[i] = 0x00;
		}
		msg.data[2]=0x05;//down & slow
		xQueueSend(_toCan,&msg,queueTime);
}
void moveUp(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg;
	msg.id  = address;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = messageLength;
	int i;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = 0x00;
	}
	msg.data[2]=0x04;//up & fast
	xQueueSend(_toCan,&msg,queueTime);
}
void moveDown(int address)//Choose which motor
{
	/*
	 * as address put motorA or motorB
	 */

	CARME_CAN_MESSAGE msg;
	msg.id  = address;
	msg.ext = 0;
	msg.rtr = 0;
	msg.dlc = messageLength;
	int i;
	for (i = 0; i < 8; i++)
	{
		msg.data[i] = 0x00;
	}
	msg.data[3]=0x05;//down & fast
	xQueueSend(_toCan,&msg,queueTime);
}

//functions to manage jobs
void addJobA(Job newJob, int level)
{
	acceptedJobA[level-1][acceptedA]=newJob;
	acceptedA++;
}
void addJobB(Job newJob, int level)
{
	acceptedJobB[level-1][acceptedB]=newJob;
	acceptedB++;
}
void sendJobA(int level)
{
	int i;
	for(i=0;i<5;i++)
	{
		acceptedJobA[level-1][i].success=1; //set Succes bit
		xQueueSend(_liftAToController, &acceptedJobA[level-1][i],queueTime);//inform the Controller
		acceptedJobA[level-1][i]=noJob;//erase all Jobs done
	}
	acceptedA=0;//set counter to Zero
}
void sendJobB(int level)
{
	int i;
	for(i=0;i<5;i++)
	{
		acceptedJobB[level-1][i].success=1; //set Succes bit
		xQueueSend(_liftBToController, &acceptedJobB[level-1][i],queueTime);//inform the Controller
		acceptedJobB[level-1][i]=noJob;//erase all Jobs done
	}
	acceptedB=0;//set counter to Zero
}

//
int initLiftA(void)
{
	int finished=0;
	moveDown(motorA);

	while(finished!=1)
	{
		getInformation();

		if((levelA==1)&&(endschalterA == 0x01))
		{
			reachedDestination(motorA);//turn off motorA
			finished=1;
		}
	}
	return finished;


}
int initLiftB(void)
{
	int finished=0;
	moveDown(motorB);

	while(finished!=1)
	{
		getInformation();

		if((levelB==1)&&(endschalterB == 0x01))
		{
			reachedDestination(motorB);//turn off motorB
			finished=1;
		}
	}
	return finished;
}

//Task-Code
void lift(void *pvargs)
{

	//used for saving the current job received from controller
	Job RXJobA;
	Job RXJobB;
	Job TXJobA;
	Job TXJobB;

	initLiftA();
	initLiftB();

	for(;;){
		getInformation();
		sprintf(liftString, "endA:%x, endB:%x levelA:%d  LevelB:%d", endschalterA, endschalterB, levelA, levelB);
		LCD_DisplayStringXY(posLiftx, posLifty,liftstring);


		//Procedure
		//1)check for job + set the Levels where I need to stop + set Direction + answer which job has NOT been accepted
		//2)check if almost reached
		//3)check if  reached + erase the StoppLevel + open and Close door + inform succesfull job

		//1a)-------------------------------------------------------------------------------------------------------------------------

		//writes the Message of the queue into RXJobA
		if( xQueueReceive( _controllerToLiftA, &RXJobA , 10 )) {
			if((directionA == UP) && (RXJobA.targetFloor > levelA)) {//Moving Upwards and the level is above

				StoppLevelA[RXJobA.targetFloor-1]=1; //set a mark where I need to stopp the elevator
				addJobA(RXJobA,RXJobA.targetFloor);//strore the Job

			} else if((directionA == DOWN) && (RXJobA.targetFloor < levelA)) {//Moving Downwards and the level is underneath
				StoppLevelA[RXJobA.targetFloor-1]=1;
				addJobA(RXJobA,RXJobA.targetFloor);//store the job
					//say to strategie the job is accepted
			} else {
				TXJobA=RXJobA;
				TXJobA.success=0;	//tell contorller Job is not accepted
				xQueueSend(_liftAToController,&TXJobA,10);
				//StoppLevelA[RXJobA.targetFloor-1]=0;
				//Say to Strategie the job is not accepted
			}

			//Is there to start the whole elevator first Job defines the direction!!!
			//and if there is no Job for long time, the elevator will have direction=STILL
			if(directionA == STILL){//Not moving
				if(levelA < RXJobA.targetFloor ){//the Level is above
					directionA=UP;
					StoppLevelA[RXJobA.targetFloor-1]=1;
					addJobA(RXJobA,RXJobA.targetFloor);//store the job
					moveUp(motorA);
				}
				if(levelA > RXJobA.targetFloor ){//the Level is underneath
					directionA=DOWN;
					StoppLevelA[RXJobA.targetFloor-1]=1;
					addJobA(RXJobA,RXJobA.targetFloor);//store the job
					moveDown(motorA);
				}
			}
			sprintf(liftString, "DirectionA:%d", directionA);
			LCD_DisplayStringXY(posLiftx, posLifty,liftstring);
		 }

#ifndef LIFT_B
		//1b)-------------------------------------------------------------------------------------------------------------------------
		 if( xQueueReceive( _controllerToLiftB, &RXJobB ,  10 )) //writes the Message of the queue into RXJobB
		 {
			if((directionB==UP) && (RXJobB.targetFloor > levelB)){//Moving Upwards and the level is above
				StoppLevelB[RXJobB.targetFloor-1]=1; //set a mark where I need to stop
				addJobB(RXJobB,RXJobB.targetFloor);
				//say to strategie the job is accepted
			}
			else if	((directionB==DOWN) && (RXJobB.targetFloor < levelB)){//Moving Downwards and the level is underneath

				StoppLevelB[RXJobB.targetFloor-1]=1;
				addJobB(RXJobB,RXJobB.targetFloor);
				//say to strategie the job is accepted
			}
			else{
				TXJobB=RXJobB;
				TXJobB.success=0;	//tell controller Job is not accepted
				xQueueSend(_liftBToController, &TXJobB,10);
				//StoppLevelB[RXJobB.targetFloor-1]=0;
				//Say to Strategie the job is not accepted
			}

			if(directionB == STILL)	{//Not moving
				if(levelB < RXJobB.targetFloor ){//the Level is above
					directionB=UP;
					StoppLevelB[RXJobB.targetFloor-1]=1;
					addJobB(RXJobB,RXJobB.targetFloor);//store the job
					moveUp(motorB);
				}
				if(levelB > RXJobB.targetFloor ){//the Level is underneath
					directionB=DOWN;
					StoppLevelB[RXJobB.targetFloor-1]=1;
					addJobB(RXJobB,RXJobB.targetFloor);//store the job
					moveDown(motorB);
				}
			}
			sprintf(liftString, "DirectionA:%d, DirectionB:%d", directionA, directionB);
			LCD_DisplayStringXY(posLiftx, posLifty,liftstring);
		 }
#endif //LIFT_B

		 //2a)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterA == 0x02) && (StoppLevelA[almost_A-1] == 1) && (directionA == UP)){//almost reached and near-switch pressed
			allmostReachedUp(motorA);
		}

		if((endschalterA == 0x02) && (StoppLevelA[almost_A-1] == 1) && (directionA == DOWN)){//almost reached and near-switch pressed
			allmostReachedDown(motorA);
		}
#ifndef LIFT_B
		 //2b)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterB == 0x02) && (StoppLevelB[almost_B-1] == 1) && (directionB == UP)){//almost reached and near-switch pressed
			allmostReachedUp(motorB);
		}

		if((endschalterB == 0x02) && (StoppLevelB[almost_B-1] == 1) && (directionB == DOWN)){//almost reached and near-switch pressed
			allmostReachedDown(motorB);
		}
#endif //LIFT_B

		//3a)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterA == 0x01) && (StoppLevelA[levelA-1] == 1)){// reached and StoppLevelA Bit is (set LevelA-1) is necessary because the first level is 1, but I need an Index of 0
			reachedDestination(motorA);//Stops the motor
			openDoor(levelA*2);//(levelA*2) is necessary to translate for example LevelA=1 to the define stock1A=0x02 which is the address for the Can-Message
			if((endschalterA & 0x08)){//door open
				stoppDoor(levelA*2);
			}
			closeDoor(levelA*2);
			if((endschalterA & 0x04) ){//door closed
				stoppDoor(levelA*2);
			}

			//Check if there are still some jobs I need to do
			if(directionA==UP){
				int i;
				for(i=levelA;i<5;i++){
					if(StoppLevelA[i]==1){
						AstillMoveUP=1;
					}
					else{
						AstillMoveUP=0;
					}
				}
				if(AstillMoveUP==1){
					directionA=UP;//set Direction
					moveUp(motorA);//move elavator
					LCD_DisplayStringXY(posLiftx, posLifty,"                                   ");//erase all characters left
					LCD_DisplayStringXY(posLiftx, posLifty,"A still move up ");
				}
				if(AstillMoveUP==0){
					directionA=STILL;
					LCD_DisplayStringXY(posLiftx, posLifty,"still           ");
				}
			}
			if(directionA==DOWN){
				int i;
				for(i=levelA;i!=0;i--){
					if(StoppLevelA[i]==1){
						AstillMoveDOWN=1;
					}
					else{
						AstillMoveDOWN=0;
					}
				}
				if(AstillMoveDOWN==1){
					directionA=DOWN;//set Direction
					moveDown(motorA);//move elavator
					LCD_DisplayStringXY(posLiftx, posLifty,"                                   ");//erase all characters left
					LCD_DisplayStringXY(posLiftx, posLifty,"still move down");
				}
				if(AstillMoveDOWN==0){
					directionA=STILL;
					LCD_DisplayStringXY(posLiftx, posLifty,"still             ");
				}
			}
			//Job is done
			StoppLevelA[levelA-1]=0; //erase the Bit because the Job is done
			sendJobA(levelA-1);//Inform of the finished Jobs
			LCD_DisplayStringXY(posLiftx, posLifty,"A reached destination");
		}
#ifndef LIFT_B
		//3b)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterB == 0x01) && (StoppLevelB[levelB-1] == 1)){// reached destination
			reachedDestination(motorB);//Stops the motor
			openDoor((levelB*2)+1);//(levelB*2)+1 is necessary to translat for example LevelB=1 to the define stock1B=0x03 which is the address for the Can-Message
			if((endschalterB & 0x08)== 1){//door open
				stoppDoor((levelB*2)+1);
			}
			closeDoor((levelB*2)+1);
			if((endschalterB & 0x04)== 1 ){//door closed
				stoppDoor((levelB*2)+1);
			}

			//Check if there are still some jobs I need to do
			if(directionB==UP){
				int i;
				for(i=levelB;i<5;i++){
					if(StoppLevelB[i]==1){
						BstillMoveUP=1;
					}
					else{
						BstillMoveUP=0;
					}
				}
				if(BstillMoveUP==1){
					directionB=UP;//set Direction
					moveUp(motorB);//move elavator
					LCD_DisplayStringXY(posLiftx, posLifty,"                                   ");//erase all characters left
					LCD_DisplayStringXY(posLiftx, posLifty,"B still move up ");
				}
				if(BstillMoveUP==0){
					directionB=STILL;
					LCD_DisplayStringXY(posLiftx, posLifty,"still           ");
				}
			}
			if(directionB==DOWN){
				int i;
				for(i=levelB;i!=0;i--){
					if(StoppLevelB[i]==1)
					{
						BstillMoveDOWN=1;
					}
					else{
						BstillMoveDOWN=0;
					}
				}
				if(BstillMoveDOWN==1){
					directionB=DOWN;//set Direction
					moveDown(motorB);//move elavator
					LCD_DisplayStringXY(posLiftx, posLifty,"                                   ");//erase all characters left
					LCD_DisplayStringXY(posLiftx, posLifty,"B still move down ");
				}
				if(BstillMoveDOWN==0){
					directionB=STILL;
					LCD_DisplayStringXY(posLiftx, posLifty,"still           ");
				}
			}
			//Job is done
			StoppLevelB[levelB-1]=0; //erase the Bit because the Job is done
			sendJobB(levelB-1);
			LCD_DisplayStringXY(posLiftx, posLifty,"B reached destination");
		}
#endif //LIFT_B
		vTaskDelay(35);
	}
}
