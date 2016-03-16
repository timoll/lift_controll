#ifndef __CAN_H__
#define __CAN_H__
/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @addtogroup	CARME_CAN_EXT
 * @{
 *
 * @file		can.h
 * @version		1.0
 * @date		2007-04-13
 * @author		M. Muehlemann
 *
 * @brief		Drivers for the CAN interface. Uses the SJA1000 CAN controller
 *				on the CARME motherboard.
 *
 *****************************************************************************
 * @copyright
 * @{
 *
 * This software can be used by students and other personal of the Bern
 * University of Applied Sciences under the terms of the MIT license.
 * For other persons this software is under the terms of the GNU General
 * Public License version 2.
 *
 * Copyright &copy; 2013, Bern University of Applied Sciences.
 * All rights reserved.
 *
 *
 * ##### MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *
 * ##### GNU GENERAL PUBLIC LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * @}
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <can_sja1000.h>			/* CAN Controller SJA1000 defines		*/

/*----- Macros -------------------------------------------------------------*/
/* baudrates */
#define CARME_CAN_BAUD_125K			125000		/**< Baudrade 125K			*/
#define CARME_CAN_BAUD_250K			250000		/**< Baudrade 250K			*/
#define CARME_CAN_BAUD_500K			500000		/**< Baudrade 500K			*/
#define CARME_CAN_BAUD_1M			1000000		/**< Baudrade 1M			*/

/* error codes */
#define	CARME_ERROR_CAN							CARME_ERROR_CAN_BASE + 0	/**< \brief Common CAN error		*/
#define CARME_ERROR_CAN_INVALID_BAUDRATE		CARME_ERROR_CAN_BASE + 1	/**< \brief Invalid baudrate		*/
#define CARME_ERROR_CAN_RXFIFO_EMPTY			CARME_ERROR_CAN_BASE + 2	/**< \brief RxFIFO empty			*/
#define CARME_ERROR_CAN_INVALID_MODE			CARME_ERROR_CAN_BASE + 3	/**< \brief Invalid mode			*/
#define CARME_ERROR_CAN_INVALID_OPMODE			CARME_ERROR_CAN_BASE + 4	/**< \brief Invalid opmode			*/
#define CARME_ERROR_CAN_INVALID_ACCEPTANCE_MODE	CARME_ERROR_CAN_BASE + 5	/**< \brief Invalid acceptance mode	*/
#define CARME_ERROR_CAN_ERROR_STATUS			CARME_ERROR_CAN_BASE + 6	/**< \brief Status error			*/

/* interrupt enable flags */
#define CARME_CAN_INT_BUSERR		(SJA1000_IER_BEIE)	/**< \brief Bus Error Interrupt Enable			*/
#define CARME_CAN_INT_ARBIT_LOST	(SJA1000_IER_ALIE)	/**< \brief Arbitration Lost Interrupt Enable	*/
#define CARME_CAN_INT_PASSIVE		(SJA1000_IER_EPIE)	/**< \brief Error Passive Interrupt Enable		*/
#define CARME_CAN_INT_WAKEUP		(SJA1000_IER_WUIE)	/**< \brief Wake-up Interrupt Enable			*/
#define CARME_CAN_INT_OVERRUN		(SJA1000_IER_DOIE)	/**< \brief Data overrun Interrupt Enable		*/
#define CARME_CAN_INT_ERROR			(SJA1000_IER_EIE)	/**< \brief Error Warning Interrupt Enable		*/
#define CARME_CAN_INT_TX			(SJA1000_IER_TIE)	/**< \brief Transmit Interrupt Enable			*/
#define CARME_CAN_INT_RX			(SJA1000_IER_RIE)	/**< \brief Receive Interrupt Enable			*/

/* driver flags */
#define CARME_CAN_DF_RESET			0x00	/**< RM-Bit in MOD register set		*/
#define CARME_CAN_DF_NORMAL			0x01	/**< RM-Bit in MOD register cleared	*/
#define CARME_CAN_DF_LISTEN_ONLY	0x02	/**< LOM-Bit in MOD register set	*/
//#define CARME_CAN_DF_SLEEP		0x04

/*----- Data types ---------------------------------------------------------*/
/**
 * \enum	CARME_CAN_IRQ_CALLBACKS
 * \brief	Interrupt sources of the CAN-Controller
 */
enum CARME_CAN_IRQ_CALLBACKS {
	CARME_CAN_IRQID_RX_INTERRUPT = 0,			/**< Telegram received Interrupt							*/
	CARME_CAN_IRQID_TX_INTERRUPT,				/**< Telegram transmitted Interrupt							*/
	CARME_CAN_IRQID_ERROR_INTERRUPT,			/**< Error warning Interrupt								*/
	CARME_CAN_IRQID_DATAOVERFLOW_INTERRUPT,		/**< Overflow Interrupt										*/
	CARME_CAN_IRQID_WAKEUP_INTERRUPT,			/**< Wakeup Interrupt Occurs when the sleep mode is left	*/
	CARME_CAN_IRQID_PASSIVE_INTERRUPT,			/**< Passive Interrupt										*/
	CARME_CAN_IRQID_ARITRATION_LOST_INTERRUPT,	/**< Arbitration Lost Interrupt								*/
	CARME_CAN_IRQID_BUS_ERROR_INTERRUPT,		/**< Bus Error Interrupt									*/

	CARME_CAN_IRQID_COUNT						/**< this must be the last entry (used for array size)		*/
};

/**
 * \struct	_CARME_CAN_MESSAGE
 * \typedef	CARME_CAN_MESSAGE
 * \brief	This struct encapsulates a CAN message.
 */
typedef struct _CARME_CAN_MESSAGE {
	uint32_t id;		/**< standard or extended Identifier */
	uint8_t ext;		/**< Frame format:
							 \arg 0: Standard Frame Format (SFF)
							 \arg 1: Extended Frame Format (EFF) */
	uint8_t rtr;		/**< If 1 the RTR Bit was set in the received CAN message */
	uint8_t dlc;		/**< Number of data-bytes in the received CAN message */
	uint8_t data[8];	/**< Array with received databytes */
} CARME_CAN_MESSAGE;

/**
 * \enum	CARME_CAN_ACCEPTANCE_FILTER_MODE
 * \brief	Acceptance filter modes  \see CARME_CAN_setAcceptaceFilter()
 */
enum CARME_CAN_ACCEPTANCE_FILTER_MODE {
	MODE_SINGLE = 1,	/**< single acceptance filter option is enabled
							 (one filter with the length of 32 bits) */
	MODE_DUAL = 2		/**< dual acceptance filter option is enabled
							 (two filters with the length of 16 bits) */
};

/**
 * \struct	_CARME_CAN_ACCEPTANCE_FILTER
 * \typedef	CARME_CAN_ACCEPTANCE_FILTER
 * \brief	Information about the acceptance filter.
 * \see		CARME_CAN_setAcceptaceFilter()
 */
typedef struct _CARME_CAN_ACCEPTANCE_FILTER {
	uint8_t acr[4];		/**< Content of the 4 Accept Filter Code Register	*/
	uint8_t amr[4];		/**< Content of the 4 Accept Filter Mask Register	*/
	enum CARME_CAN_ACCEPTANCE_FILTER_MODE afm; /**< acceptance filter mode	*/
} CARME_CAN_ACCEPTANCE_FILTER;

/**
 * \typedef	IRQ_CALLBACK
 * \brief	Pointer to a function.
 */
typedef void (*IRQ_CALLBACK)();

/*----- Function prototypes ------------------------------------------------*/
extern void CARME_CAN_Init(uint32_t baud, uint8_t flags);
extern void CARME_CAN_InitI(uint32_t baud, uint8_t flags, uint32_t interrupts);
extern ERROR_CODES CARME_CAN_Write(CARME_CAN_MESSAGE* txMsg);
extern ERROR_CODES CARME_CAN_Read(CARME_CAN_MESSAGE* rxMsg);

extern void CARME_CAN_RegisterIRQCallback(enum CARME_CAN_IRQ_CALLBACKS id,
                                          IRQ_CALLBACK pIRQCallback);
extern void CARME_CAN_UnregisterIRQCallback(enum CARME_CAN_IRQ_CALLBACKS id);

extern ERROR_CODES CARME_CAN_SetMode(uint8_t flags);
extern ERROR_CODES CARME_CAN_SetBaudrate(uint32_t baud);
extern ERROR_CODES CARME_CAN_SetAcceptaceFilter(
        CARME_CAN_ACCEPTANCE_FILTER* af);
extern ERROR_CODES CARME_CAN_GetAcceptaceFilter(
        CARME_CAN_ACCEPTANCE_FILTER* af);
#ifdef CARME_CAN_DEBUG
/* debug functions */
extern void CARME_CAN_PrintRegisters();
extern void CARME_CAN_GetRegisterString(char* pStr);
#endif

/*----- Data ---------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */

#endif /* __CAN_H__ */
