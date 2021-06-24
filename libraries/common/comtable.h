/*
 * comtable.h
 *
 *  Created on: Apr 25, 2020
 *   Edited on: Jun 20, 2021 - 00:35:55
 *      Author: n00b
 *  This code is code-generated
 */

#ifndef SOURCE_COMTABLE_H_
#define SOURCE_COMTABLE_H_

//    ________________________________________________________________________
//    |                                                                      |
//    |    IBACOM - command list                                             |
//    |______________________________________________________________________|
//


#if defined (__linux__)
	#define TOTAL_IBACOM 84
	#define MAX_PACKET_ENTRIES 13
#elif defined (ESP8266)
	#define TOTAL_IBACOM 65
	#define MAX_PACKET_ENTRIES 13
#else
	#define TOTAL_IBACOM 61
	#define MAX_PACKET_ENTRIES 13
#endif

	/*** [ 00000 - 00500 ] : system command  ***/

#define	IBACOM_NONE                         0
#define	IBACOM_INIT                         1
#define	IBACOM_RESET                        2
#define	IBACOM_REQUEST_CONFIG               3
#define	IBACOM_REQUEST_STATUS               4
#define	IBACOM_COMMAND_STRING               5
#define	IBACOM_FACTORY_RESET                6
#define	IBACOM_BOARD_BOOT                   7
#define	IBACOM_UNIX_EPOCH                   10
#define	IBACOM_PING                         14
#define	IBACOM_REPORT                       15
#define	IBACOM_BOARD_VERSION                18
#define	IBACOM_SET_SD                       301
#define	IBACOM_REQUEST_SDDROP               302
#define	IBACOM_REQUEST_SDCLEAR              303
#define	IBACOM_REQUEST_SDWIPE               304

	/*** [ 00501 - 01000 ] : devices command ***/

#define	IBACOM_SET_DEVICE                   501
#define	IBACOM_SET_DEVNAME                  502
#define	IBACOM_SET_DEVPIN                   503
#define	IBACOM_REM_DEVICE                   504
#define	IBACOM_REQ_DEV_STATUS               505
#define	IBACOM_REQ_DEV_RESET                506

	/*** [ 01001 - 01300 ] : board config    ***/

#define	IBACOM_CONF_STD                     1001
#define	IBACOM_CONF_SAMPLER                 1005
#define	IBACOM_CONF_ESP                     1100
#define	IBACOM_CONF_CAMERA                  1300

	/*** [ 01301 - 02000 ] : device config   ***/

#define	IBACOM_CONF_ASENSOR                 1301
#define	IBACOM_CONF_SWITCH                  1302
#define	IBACOM_CONF_FLOODSWITCH             1303
#define	IBACOM_CONF_RAINSWITCH              1304
#define	IBACOM_CONF_LDR                     1310
#define	IBACOM_CONF_DUSTPM25                1405
#define	IBACOM_CONF_DHT22                   1422
#define	IBACOM_CONF_RELAY                   1700
#define	IBACOM_CONF_DEVSTD                  2000

	/*** [ 02001 - 02300 ] : board status    ***/

#define	IBACOM_STATUS_STD                   2001
#define	IBACOM_STATUS_SAMPLER               2005
#define	IBACOM_STATUS_ESP                   2100
#define	IBACOM_STATUS_SD                    2270
#define	IBACOM_STATUS_CAMERA                2300

	/*** [ 02301 - 03000 ] : device status   ***/

#define	IBACOM_STATUS_ASENSOR               2301
#define	IBACOM_STATUS_SWITCH                2302
#define	IBACOM_STAT_FLOODSWITCH             2303
#define	IBACOM_STAT_RAINSWITCH              2304
#define	IBACOM_STATUS_LDR                   2310
#define	IBACOM_STATUS_DUSTPM25              2405
#define	IBACOM_STATUS_DHT22                 2422
#define	IBACOM_STATUS_RELAY                 2700
#define	IBACOM_STATUS_DEVSTD                3000

	/*** [ 03001 - 03200 ] : alarms          ***/

#define	IBACOM_HEAT_ALARM                   3001
#define	IBACOM_OVERH_ALARM                  3002
#define	IBACOM_FIRE_ALARM                   3003
#define	IBACOM_FLOOD_ALARM                  3004
#define	IBACOM_MOIST_ALARM                  3005
#define	IBACOM_SMOG_ALARM                   3006
#define	IBACOM_HAZARD_ALARM                 3007
#define	IBACOM_SMOKE_ALARM                  3008
#define	IBACOM_GRID_ALARM                   3009
#define	IBACOM_GENERIC_ALARM                3050

	/*** [ 03201 - 03500 ] : requests        ***/

#define	IBACOM_REQ_PROBE                    3205

	/*** [ 03501 - 04000 ] : payloads        ***/

#define	IBACOM_PROBE_AMBIENT                3505

	/*** [ 05001 - 07000 ] : network         ***/

#define	IBACOM_CONF_AES                     5002
#define	IBACOM_CONF_NET                     5003
#define	IBACOM_ACK                          5025
#define	IBACOM_NET_ADDRESS                  5050

	/*** [ 07001 - 08000 ] : client comunication ***/

#define	IBACOM_REQ_LOGIN                    7001
#define	IBACOM_SESSION_STAT                 7002
#define	IBACOM_SESSION_END                  7003
#define	IBACOM_SRV_REQ                      7010
#define	IBACOM_SRV_CONF                     7012
#define	IBACOM_SYS_BRD_CNF                  7021
#define	IBACOM_SYS_BRD_STS                  7022
#define	IBACOM_SYS_DEV_CNF                  7023
#define	IBACOM_SYS_DEV_STS                  7024
#define	IBACOM_SYS_REQ                      7100
#define	IBACOM_SET_ENV_ALARM                7701
#define	IBACOM_ACK_ENV_ALARM                7702
#define	IBACOM_ENV_ALARM                    7703
#define	IBACOM_ALARM_DEVSTAT                7705
#define	IBACOM_LYT_CONF                     8008
#define	IBACOM_LYT_DEVICE                   8012
#define	IBACOM_LYT_LIGHTS                   8020
#define	IBACOM_LYT_WEATHER                  8022
#define	IBACOM_LYT_ALARMS                   8050


#endif  /* SOURCE_COMTABLE_H_ */


