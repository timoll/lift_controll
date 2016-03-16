/*
 * com.h
 *
 *  Created on: Nov 26, 2013
 *      Author: fabianzobrist
 */

#ifndef COM_H_
#define COM_H_

CARME_CAN_MESSAGE com_SendAndGetAnswer(CARME_CAN_MESSAGE * msg);
void com_Send(CARME_CAN_MESSAGE * msg);

#endif /* COM_H_ */
