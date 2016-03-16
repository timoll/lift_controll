/*
 * messages.h
 *
 *  Created on: 12.04.2012
 *      Author: zobrf1
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

/**IDs Stationen
 * @{
 */
#define ID_FB_LINKS	 		0x110
#define ID_FB_MITTE   		0x120
#define ID_FB_RECHTS  		0x130
#define ID_VERTEILER  		0x140
#define ID_ARM_LINKS  		0x150
#define ID_ARM_RECHTS		0x160
/**@}*/


/**Meldungsart
  * @{
 */
#define MSG_STAT_REQ		0x0
#define MSG_STAT_ANS		0x1
#define MSG_CMD				0x2
#define MSG_CMD_ANS			0x3
#define MSG_RESET			0xF
/**@}*/

/**Funktionstyp DB0
  * @{
 */
#define FUN_FEHLER			0x00
#define FUN_ZUSTAND			0x01
#define FUN_START			0x01
#define FUN_POSITION		0x01
#define FUN_ACHSE			0x01
#define FUN_STOPP			0x02
#define FUN_ARM				0x02
#define FUN_STOPP_POSITION  0x03
#define FUN_FERTIG			0x04
/**@}*/

/*************Daten DB1 - DB7********************/

/**DB für DB0=FUN_FEHLER
  * @{
 */
#define DB_FEHLER			0x1
/**@}*/

/**DB für DB0=FUN_FEHLER
  * @{
 */
#define DB_ZUS_STATUS		0x1
#define DB_ZUS_ECTS			0x2
#define DB_ZUS_POSTION		0x3
#define DB_ZUS_LAGE			0x5
/**@}*/

/**DB für DB0= FUN_STOPP_POSITION
  * @{
 */
#define DB_STOPP_POSITION	0x1
/**@}*/

/**DB für DB0=FUN_POSITION
  * @{
 */
#define DB_POS_WERT			0x1
#define DB_POS_GESCHW		0x2
/**@}*/

/**DB für DB0=FUN_ACHSE
  * @{
 */
#define DB_ACHSE_NR			0x1
#define DB_ACHSE_WERT		0x2
#define DB_ACHSE_GESCHW		0x3
/**@}*/

/**DB für DB0=FUN_ARM
  * @{
 */
#define DB_ARM_BASIS		0x1
#define DB_ARM_SCHULTER		0x2
#define DB_ARM_ELLBOGEN		0x3
#define DB_ARM_HAND			0x4
#define DB_ARM_GREIFFER		0x5
/**@}*/


/**Struktur für die Can-meldungen*/
typedef struct{
	char DB0;
	char DB1;
	char DB2;
	char DB3;
	char DB4;
	char DB5;
	char DB6;
	char DB7;
} Can_Data;



#endif /* MESSAGES_H_ */
