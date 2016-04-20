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

int finalDestinationA;
int finalDestinationB;

int AstillMoveUP=0,AstillMoveDOWN=0;
int BstillMoveUP=0,BstillMoveDOWN=0;

int directionA=STILL;//Variable to store the actual direction
int directionB=STILL;//Variable to store the actual direction

_Bool StoppLevelA[5]={0};
_Bool StoppLevelB[5]={0};

int *rxA, *rxB;
int rxLevelA, rxLevelB;

//transform CanMessage
void getEndSchalterByte(void)
{
	CARME_CAN_MESSAGE receivedMessage;

	liftName = msg.id %2; //find out which elevator is meant
	switch (liftName)
	{
		case 0: //Lift a
			endschalterA = msg.data[0];
			break;
		case 1:	//Lift b
			endschalterB = msg.data[0];
			break;
	}
	//erstelle EndSchalter

}

//set the Level where the elevator last was/is
void setLevelA(void)
{
	//durch diese Funktion wird falls eine msg informationen über den Stock enthält direkt das Level gesetzt
	/*
	 * Level 1 =>1
	 * Level 2 =>2
	 * Level 3 =>3
	 * Level 4 =>4
	 * Level 5 =>5
	 * Level 6 =>6
	 * same for levelAalmost
	 */
	if(((endschalterA && 0x01)==1) &&(msg.id%2==0))//reached floor and ID is For Elevator A
	{
		levelA=msg.id/2;
	}

	if(((endschalterA && 0x02)==1) &&(msg.id%2==0))//close to floor and ID is for ElevatorA
	{
		almost_A=msg.id/2;
	}
}
void setLevelB(void)
{
	//durch diese Funktion wird falls eine msg informationen über den Stock enthält direkt das Level gesetzt
	/*
	 * Level 1 =>1
	 * Level 2 =>2
	 * Level 3 =>3
	 * Level 4 =>4
	 * Level 5 =>5
	 * Level 6 =>6
	 * same for levelAalmost
	 */
	if(((endschalterB && 0x01)==1) && (msg.id%2!=0))//reached floor and ID is not for Elevator A
	{
		levelB=(msg.id-1)/2;
	}

	if(((endschalterB && 0x02)==1) && (msg.id%2!=0))//close to floor and ID is not for Elevator A
	{
		almost_B=(msg.id-1)/2;
	}
}

//functions for controlling the elevator
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
}
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
	//Write to Senn that I reached floor with ADDRESS in his QUEU
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
}

//Task-Code
void lift(void *pvargs)
{

	rxA=&rxLevelA;//Set the address
	rxB=&rxLevelB;//Set the address

	for(;;)
	{

		//process message

		//xQueueReceive(Queue_Can_Rx, &pMsg, portMAX_DELAY); //Queue which is sheard with the Can-Task
		getEndSchalterByte();
		setLevelA();
		setLevelB();
		//free(pMsg);

		//generate actions for Elevator-A
		//1)check for job + set the Levels where I need to stop + set Direction + answer which one was chosen
		//2)check if almost reached
		//3)check if  reached + erase the StoppLevel

		//1A)-------------------------------------------------------------------------------------------------------------------------

		if( xQueueReceive( _controllerToLiftA,  rxA , ( TickType_t ) 10 )) //writes the Message of the queue into rxLevelA
		 {
			if((directionA==UP) && (rxLevelA > levelA))//Moving Upwards and the level is above
			{
				StoppLevelA[rxLevelA-1]=1; //set a mark where I need to stopp
				//say to strategie the job is accepted
			}
			else
			{
				StoppLevelA[rxLevelA-1]=0;
				//Say to Strategie the job is not accepted
			}

			if((directionA==DOWN) && (rxLevelA < levelA))//Moving Downwards and the level is underneath
			{
				StoppLevelA[rxLevelA-1]=1;
				//say to strategie the job is accepted
			}
			else
			{
				StoppLevelA[rxLevelA-1]=0;
				//Say to Strategie the job is not accepted
			}

	/*		if(directionA == STILL)//Not moving
			{
				if(levelA < rxLevelA )//the Level is above
				{
					moveUp(motorA);
					StoppLevelA[rxLevelA-1]=1;
				}
				if(levelA > rxLevelA )//the Level is underneath
				{
					moveDown(motorA);
					StoppLevelA[rxLevelA-1]=1;
				}
				//say to strategie the job is accepted
			}
			else
			{
				//Say to Strategie the job is not accepted
			}*/
		 }

		//1b)-------------------------------------------------------------------------------------------------------------------------
		 if( xQueueReceive( _controllerToLiftB,  rxB , ( TickType_t ) 10 ) ) //writes the Message of the queue into rxLevelA
		 {
			if((directionB==UP) && (rxLevelB > levelB))//Moving Upwards and the level is above
			{
				StoppLevelB[rxLevelB-1]=1; //set a mark where I need to stopp
				//say to strategie the job is accepted
			}
			else
			{
				StoppLevelB[rxLevelB-1]=0;
				//Say to Strategie the job is not accepted
			}

			if((directionB==DOWN) && (rxLevelB < levelB))//Moving Downwards and the level is underneath
			{
				StoppLevelB[rxLevelB-1]=1;
				//say to strategie the job is accepted
			}
			else
			{
				StoppLevelB[rxLevelB-1]=0;
				//Say to Strategie the job is not accepted
			}

		/*	if(directionB == STILL)//Not moving
			{
				if(levelB < rxLevelB )//the Level is above
				{
					moveUp(motorB);
					StoppLevelB[rxLevelB-1]=1;
				}
				if(levelB > rxLevelB )//the Level is underneath
				{
					moveDown(motorB);
					StoppLevelB[rxLevelB-1]=1;
				}

				//say to strategie the job is accepted
			}
			else
			{
				//Say to Strategie the job is not accepted
			}*/
		 }

		 //2a)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterA == 0x02) && (StoppLevelA[almost_A-1] == 1) && (directionA == UP))//almost reached and near-switch pressed
		{
			allmostReachedUp(motorA);
		}

		if((endschalterA == 0x02) && (StoppLevelA[almost_A-1] == 1) && (directionA == DOWN))//almost reached and near-switch pressed
		{
			allmostReachedDown(motorA);
		}

		 //2b)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterB == 0x02) && (StoppLevelB[almost_B-1] == 1) && (directionB == UP))//almost reached and near-switch pressed
		{
			allmostReachedUp(motorB);
		}

		if((endschalterB == 0x02) && (StoppLevelB[almost_B-1] == 1) && (directionB == DOWN))//almost reached and near-switch pressed
		{
			allmostReachedDown(motorB);
		}
		//3a)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterA == 0x01) && (StoppLevelA[levelA-1] == 1))// reached and StoppLevelA Bit is (set LevelA-1) is necessary because the first level is 1, but I need an Index of 0
		{

			reachedDestination(motorA);
			StoppLevelA[levelA-1]=0; //erase the Bit because the Job is done
			openDoor(levelA*2);//(levela*2) is necessary to translat for example LevelA=1 to the define stock1A=0x02 which is the address for the Can-Message
			if((endschalterA & 0x08)== 1)//door open
			{
				stoppDoor(levelA*2);
			}
			closeDoor(levelA*2);
			if((endschalterA & 0x04)== 1 )//door closed
			{
				stoppDoor(levelA*2);
			}

			//Check if there are still some jobs I need to do
			if(directionA==UP)
			{
				int i;
				for(i=levelA;i<5;i++)
				{
					if(StoppLevelA[i]==1)
					{
						AstillMoveUP=1;
					}
					else
					{
						AstillMoveUP=0;
					}
				}
				if(AstillMoveUP==0)
				{
					//Send I am not moving waiting for new job!!
				}
			}
			if(directionA==DOWN)
			{
				int i;
				for(i=levelA;i!=0;i--)
				{
					if(StoppLevelA[i]==1)
					{
						AstillMoveDOWN=1;
					}
					else
					{
						AstillMoveDOWN=0;
					}
				}
				if(AstillMoveDOWN==0)
				{
					//Send I am not moving waiting for new job!!
				}
			}
		}

		//3b)-------------------------------------------------------------------------------------------------------------------------
		if((endschalterB == 0x01) && (StoppLevelB[levelB-1] == 1))//almost reached and near-switch pressed
		{
			reachedDestination(motorB);
			StoppLevelB[levelB-1]=0; //erase the Bit because the Job is done
			openDoor((levelB*2)+1);//(levelB*2)+1 is necessary to translat for example LevelB=1 to the define stock1B=0x03 which is the address for the Can-Message
			if((endschalterB & 0x08)== 1)//door open
			{
				stoppDoor((levelB*2)+1);
			}
			closeDoor((levelB*2)+1);
			if((endschalterB & 0x04)== 1 )//door closed
			{
				stoppDoor((levelB*2)+1);
			}
			//Check if there are still some jobs I need to do
			if(directionB==UP)
			{
				int i;
				for(i=levelB;i<5;i++)
				{
					if(StoppLevelB[i]==1)
					{
						BstillMoveUP=1;
					}
					else
					{
						BstillMoveUP=0;
					}
				}
				if(BstillMoveUP==0)
				{
					//Send I am not moving waiting for new job!!
				}
			}
			if(directionB==DOWN)
			{
				int i;
				for(i=levelB;i!=0;i--)
				{
					if(StoppLevelB[i]==1)
					{
						BstillMoveDOWN=1;
					}
					else
					{
						BstillMoveDOWN=0;
					}
				}
				if(BstillMoveDOWN==0)
				{
					//Send I am not moving waiting for new job!!
				}
			}
		}
		vTaskDelay(35);
	}
}
