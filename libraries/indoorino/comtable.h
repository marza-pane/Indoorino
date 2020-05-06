/*
 * comtable.h
 *
 *  Created on: Apr 25, 2020
 *   Edited on: May 02, 2020 - 01:48:17
 *      Author: n00b
 */

#ifndef SOURCE_COMTABLE_H_
#define SOURCE_COMTABLE_H_

//    ________________________________________________________________________
//    |                                                                      |
//    |    IBACOM - command list                                             |
//    |______________________________________________________________________|
//

#define TOTAL_IBACOM 38



	/***    [00000  - 00300 ] : system command ***/

#define	IBACOM_NONE                         0
#define	IBACOM_INIT                         1
#define	IBACOM_RESET                        2
#define	IBACOM_REQUEST_CONFIG               3
#define	IBACOM_REQUEST_STATUS               4
#define	IBACOM_COMMAND_STRING               5
#define	IBACOM_UNIX_EPOCH                   10
#define	IBACOM_REPORT                       15
#define	IBACOM_ACK                          25
#define	IBACOM_RESET_CONFIG                 30

	/***    [00301  - 00500 ] : general command ***/

#define	IBACOM_SET_SD                       301

	/***    [00501  - 01000 ] : devices command ***/

#define	IBACOM_SET_DEVICE                   501

	/***    [01001  - 01300 ] : board config ***/

#define	IBACOM_CONF_STD                     1001
#define	IBACOM_CONF_ESP                     1003
#define	IBACOM_CONF_SAMPLER                 1005

	/***    [01301  - 02000 ] : device config ***/

#define	IBACOM_CONF_ASENSOR                 1301
#define	IBACOM_CONF_SWITCH                  1302
#define	IBACOM_CONF_LDR                     1310
#define	IBACOM_CONF_DHT22                   1522
#define	IBACOM_CONF_RELAY                   1700
#define	IBACOM_CONF_DEVSTD                  2000

	/***    [02001  - 02300 ] : board status ***/

#define	IBACOM_STATUS_STD                   2001
#define	IBACOM_STATUS_ESP                   2003
#define	IBACOM_STATUS_SAMPLER               2005

	/***    [02301  - 03000 ] : device status ***/

#define	IBACOM_STATUS_ASENSOR               2301
#define	IBACOM_STATUS_SWITCH                2302
#define	IBACOM_STATUS_LDR                   2310
#define	IBACOM_STATUS_DHT22                 2522
#define	IBACOM_STATUS_RELAY                 2700
#define	IBACOM_STATUS_SD                    2880
#define	IBACOM_STATUS_DEVSTD                3000

	/***    [03001  - 03200 ] : alarms ***/

#define	IBACOM_ALLARM                       3001
#define	IBACOM_WARNINGS                     3002

	/***    [03201  - 03500 ] : requests ***/

#define	IBACOM_REQUEST_PROBE                3205

	/***    [03501  - 04000 ] : payloads ***/

#define	IBACOM_PROBE_AMBIENT                3505

	/***    [05001  - 06000 ] : network ***/

#define	IBACOM_SET_ADDR                     5001
#define	IBACOM_ESP_ADDRESS                  5002
#define	IBACOM_DOORBELL                     65530


#endif  /* SOURCE_COMTABLE_H_ */


