/*
 * main.h
 *
 *  Created on: Nov 26, 2013
 *      Author: fabianzobrist
 */

#ifndef MAIN_H_
#define MAIN_H_


/**
 * Defines um tasks für die verschiedenen Geräte zu erstellen
 * @{
 */
#define FB_LINKS   1
#define FB_RECHTS  2
#define FB_MITTE   3
#define ARM_LINKS  4
#define ARM_RECHTS 5
/**@}*/

extern xSemaphoreHandle Sema_Mux_Can;

extern xSemaphoreHandle Sema_Fb_Links_Finish;
extern xSemaphoreHandle Sema_Fb_Rechts_Finish;
extern xSemaphoreHandle Sema_Fb_Mitte_Finish;
extern xSemaphoreHandle Sema_Arm_Links_Finish;
extern xSemaphoreHandle Sema_Arm_Rechts_Finish;
extern xSemaphoreHandle Sema_Split_Finish;

extern xSemaphoreHandle Sema_Fb_Links_Start;
extern xSemaphoreHandle Sema_Fb_Rechts_Start;
extern xSemaphoreHandle Sema_Fb_Mitte_Start;
extern xSemaphoreHandle Sema_Arm_Links_Start;
extern xSemaphoreHandle Sema_Arm_Rechts_Start;
extern xSemaphoreHandle Sema_Split_Start;

#endif /* MAIN_H_ */
