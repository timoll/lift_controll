#ifndef __INCLUDES_H__
#define __INCLUDES_H__
/*****************************************************************************
 * \brief		Startup file. In this file is the main function, which is
 *				called at the startup of the controller.
 *
 * \file		main.h
 * \version		1.0
 * \date		2012-12-12
 * \author		rct1
 ****************************************************************************/
/*
 * functions global:
 *				main
 * functions local:
 *				.
 *
 ****************************************************************************/

/*----- Header-Files -------------------------------------------------------*/
#include <lcd.h>					/* CARME display module					*/
#include <ff.h>						/* Fat file system						*/
#include <memPoolService.h>			/* Memory pool manager service			*/

#include <stdio.h>					/* Standard input and output			*/

#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <carme_io1.h>				/* CARME IO1 Module						*/
#include <carme_io2.h>				/* CARME IO2 Module						*/
#include <can.h>					/* CARME CAN Module						*/
#include <uart.h>					/* CARME BSP UART port					*/
#include <rtc.h>					/* CARME RTC							*/
#include <ff.h>						/* Fat file system						*/

#include <FreeRTOS.h>				/* FreeRTOS main include				*/
#include <task.h>					/* FreeRTOS task functionality			*/
#include <queue.h>					/* FreeRTOS queue functionality			*/
#include <semphr.h>					/* FreeRTOS semaphore functionality		*/
#include <memPoolService.h>			/* Memory pool manager service			*/


#include "messages.h"
#include "main.h"
#include "com.h"
#include "lift.h"

#endif /* __INCLUDES_H__ */
