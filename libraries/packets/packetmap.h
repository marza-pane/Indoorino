/*
 * packetmap.h
 *
 *  Created on: Apr 25, 2020
 *   Edited on: Apr 26, 2021 - 03:14:14
 *      Author: n00b
 *  This code is code-generated
 */

#ifndef PACKETPGM_H_
#define PACKETPGM_H_


#if defined (__linux__)
	#include "../common/comtable.h"
	#include "../common/definitions.h"
	#define VARMAP_NUMBER 89
#else
	#include "comtable.h"
	#include "definitions.h"
	#if defined (ESP8266)
		#define VARMAP_NUMBER 85
	#else
		#define VARMAP_NUMBER 79
	#endif /* ARDUINO */
#endif /* __linux__ */


//    ________________________________________________________________________
//    |                                                                      |
//    |    mapped variables                                                  |
//    |______________________________________________________________________|
//

/*
 * possible types for variable_fst_ are:
 * c = char
 * s = uint8_t
 * u = uint32_t
 * n = int32_t
 * h = uint8_t * (hashes)
*/

namespace packet
{
	struct variable_fst_
	{
		char           type;
		iSize_t        size;
		char           name[LEN_FIELD];
	};

	struct packet_fst_
	{
		iCom_t         comm  ;
		uint8_t        count ;
		char           label [LEN_LABEL];
		uint8_t        params[MAX_PACKET_ENTRIES];
	};

	const variable_fst_ VariableMap[VARMAP_NUMBER] PROGMEM = {

		{ 'c',	SERIAL_TX_BUFFER_SIZE , "message"              },	/*   0 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "command"              },	/*   1 */
		{ 'c',	LEN_NAME              , "name"                 },	/*   2 */
		{ 'c',	LEN_NAME              , "type"                 },	/*   3 */
		{ 'c',	LEN_NAME              , "board"                },	/*   4 */
		{ 'c',	LEN_LABEL             , "label1"               },	/*   5 */
		{ 'c',	LEN_LABEL             , "label2"               },	/*   6 */
		{ 'c',	LEN_LABEL             , "label3"               },	/*   7 */
		{ 'c',	LEN_LABEL             , "label4"               },	/*   8 */
		{ 's',	sizeof(uint8_t)       , "devnum"               },	/*   9 */
		{ 'c',	LEN_DEVNAME           , "devname"              },	/*  10 */
		{ 'c',	LEN_DEVNAME           , "newname"              },	/*  11 */
		{ 'c',	LEN_NAME              , "desc1"                },	/*  12 */
		{ 'c',	LEN_NAME              , "desc2"                },	/*  13 */
		{ 'c',	LEN_NAME              , "desc3"                },	/*  14 */
		{ 'c',	LEN_NAME              , "desc4"                },	/*  15 */
		{ 'u',	sizeof(uint32_t)      , "epoch"                },	/*  16 */
		{ 'u',	sizeof(uint32_t)      , "freemem"              },	/*  17 */
		{ 'u',	sizeof(uint32_t)      , "looptime"             },	/*  18 */
		{ 'n',	sizeof(int32_t)       , "errors1"              },	/*  19 */
		{ 'n',	sizeof(int32_t)       , "errors2"              },	/*  20 */
		{ 'n',	sizeof(int32_t)       , "errors3"              },	/*  21 */
		{ 'n',	sizeof(int32_t)       , "errors4"              },	/*  22 */
		{ 'u',	sizeof(uint32_t)      , "totalrx"              },	/*  23 */
		{ 'u',	sizeof(uint32_t)      , "totaltx"              },	/*  24 */
		{ 'n',	sizeof(int32_t)       , "value1"               },	/*  25 */
		{ 'n',	sizeof(int32_t)       , "value2"               },	/*  26 */
		{ 'n',	sizeof(int32_t)       , "value3"               },	/*  27 */
		{ 'n',	sizeof(int32_t)       , "value4"               },	/*  28 */
		{ 'n',	sizeof(int32_t)       , "value5"               },	/*  29 */
		{ 'n',	sizeof(int32_t)       , "value6"               },	/*  30 */
		{ 'n',	sizeof(int32_t)       , "value7"               },	/*  31 */
		{ 'n',	sizeof(int32_t)       , "value8"               },	/*  32 */
		{ 'n',	sizeof(int32_t)       , "param1"               },	/*  33 */
		{ 'n',	sizeof(int32_t)       , "param2"               },	/*  34 */
		{ 'n',	sizeof(int32_t)       , "param3"               },	/*  35 */
		{ 'n',	sizeof(int32_t)       , "param4"               },	/*  36 */
		{ 'n',	sizeof(int32_t)       , "param5"               },	/*  37 */
		{ 'n',	sizeof(int32_t)       , "param6"               },	/*  38 */
		{ 'n',	sizeof(int32_t)       , "param7"               },	/*  39 */
		{ 'n',	sizeof(int32_t)       , "param8"               },	/*  40 */
		{ 'u',	sizeof(uint32_t)      , "stepday1"             },	/*  41 */
		{ 'u',	sizeof(uint32_t)      , "stepday2"             },	/*  42 */
		{ 'u',	sizeof(uint32_t)      , "stepday3"             },	/*  43 */
		{ 'u',	sizeof(uint32_t)      , "stepday4"             },	/*  44 */
		{ 'u',	sizeof(uint32_t)      , "stephour1"            },	/*  45 */
		{ 'u',	sizeof(uint32_t)      , "stephour2"            },	/*  46 */
		{ 'u',	sizeof(uint32_t)      , "stephour3"            },	/*  47 */
		{ 'u',	sizeof(uint32_t)      , "stephour4"            },	/*  48 */
		{ 'u',	sizeof(uint32_t)      , "timeout1"             },	/*  49 */
		{ 'u',	sizeof(uint32_t)      , "timeout2"             },	/*  50 */
		{ 'u',	sizeof(uint32_t)      , "timeout3"             },	/*  51 */
		{ 'u',	sizeof(uint32_t)      , "timeout4"             },	/*  52 */
		{ 's',	sizeof(uint8_t)       , "level"                },	/*  53 */
		{ 's',	sizeof(uint8_t)       , "status"               },	/*  54 */
		{ 's',	sizeof(uint8_t)       , "count1"               },	/*  55 */
		{ 's',	sizeof(uint8_t)       , "count2"               },	/*  56 */
		{ 's',	sizeof(uint8_t)       , "count3"               },	/*  57 */
		{ 's',	sizeof(uint8_t)       , "count4"               },	/*  58 */
		{ 's',	sizeof(uint8_t)       , "count5"               },	/*  59 */
		{ 's',	sizeof(uint8_t)       , "count6"               },	/*  60 */
		{ 's',	sizeof(uint8_t)       , "count7"               },	/*  61 */
		{ 's',	sizeof(uint8_t)       , "count8"               },	/*  62 */
		{ 's',	sizeof(uint8_t)       , "pin1"                 },	/*  63 */
		{ 's',	sizeof(uint8_t)       , "pin2"                 },	/*  64 */
		{ 's',	sizeof(uint8_t)       , "pin3"                 },	/*  65 */
		{ 's',	sizeof(uint8_t)       , "pin4"                 },	/*  66 */
		{ 'u',	sizeof(uint32_t)      , "tcprx"                },	/*  67 */
		{ 'u',	sizeof(uint32_t)      , "tcptx"                },	/*  68 */
		{ 'u',	sizeof(uint32_t)      , "serialrx"             },	/*  69 */
		{ 'u',	sizeof(uint32_t)      , "serialtx"             },	/*  70 */
		{ 'c',	INET_ADDRSTRLEN       , "ip1"                  },	/*  71 */
		{ 'c',	INET_ADDRSTRLEN       , "ip2"                  },	/*  72 */
		{ 'c',	INET_ADDRSTRLEN       , "ip3"                  },	/*  73 */
		{ 'c',	INET_ADDRSTRLEN       , "ip4"                  },	/*  74 */
		{ 'u',	sizeof(uint32_t)      , "port1"                },	/*  75 */
		{ 'u',	sizeof(uint32_t)      , "port2"                },	/*  76 */
		{ 'u',	sizeof(uint32_t)      , "port3"                },	/*  77 */
		{ 'u',	sizeof(uint32_t)      , "port4"                },	/*  78 */

#if defined (ESP8266) || defined (__linux__)

		{ 'h',	SHA_DIGEST_LENGTH     , "hash1"                },	/*  79 */
		{ 'h',	SHA_DIGEST_LENGTH     , "hash2"                },	/*  80 */
		{ 'h',	SHA_DIGEST_LENGTH     , "hash3"                },	/*  81 */
		{ 'h',	SHA_DIGEST_LENGTH     , "hash4"                },	/*  82 */
		{ 'h',	LEN_AES_MASTERKEY     , "aeskey1"              },	/*  83 */
		{ 'h',	LEN_AES_MASTERKEY     , "aeskey2"              },	/*  84 */

#endif /* ESP8266 or __linux__ */

#if defined (__linux__)

		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table1"               },	/*  85 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table2"               },	/*  86 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table3"               },	/*  87 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table4"               },	/*  88 */

#endif /* __linux__ */

	};


	const packet_fst_ PacketMap[TOTAL_IBACOM] PROGMEM = {
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 000 - IBACOM_NONE - empty packet                         |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_NONE, 0,
			"empty packet", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 001 - IBACOM_INIT - init byte                            |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_INIT, 0,
			"init byte", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 002 - IBACOM_RESET - reset command                       |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_RESET, 0,
			"reset command", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 003 - IBACOM_REQUEST_CONFIG - config request             |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQUEST_CONFIG, 0,
			"config request", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 004 - IBACOM_REQUEST_STATUS - status request             |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQUEST_STATUS, 0,
			"status request", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 005 - IBACOM_COMMAND_STRING - command string             |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_COMMAND_STRING, 1,
			"command string", { 1 } // command
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 006 - IBACOM_FACTORY_RESET - factory reset               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_FACTORY_RESET, 0,
			"factory reset", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 007 - IBACOM_BOARD_BOOT - board boot signal              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_BOARD_BOOT, 2,
			"board boot signal", { 2, 25 } // name, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 010 - IBACOM_UNIX_EPOCH - timestamp                      |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_UNIX_EPOCH, 1,
			"timestamp", { 16 } // epoch
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 014 - IBACOM_PING - system ping                          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_PING, 1,
			"system ping", { 16 } // epoch
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 015 - IBACOM_REPORT - generic report                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REPORT, 4,
			"generic report", { 0, 2, 16, 53 } // message, name, epoch, level
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 018 - IBACOM_BOARD_VERSION - board firmware version      |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_BOARD_VERSION, 5,
			"board firmware version", { 33, 34, 17, 63, 64 } // param1, param2, freemem, pin1, pin2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 301 - IBACOM_SET_SD - sd card setting                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_SD, 1,
			"sd card setting", { 1 } // command
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 302 - IBACOM_REQUEST_SDDROP - drop request               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQUEST_SDDROP, 0,
			"drop request", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 303 - IBACOM_REQUEST_SDCLEAR - clear request sd          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQUEST_SDCLEAR, 0,
			"clear request sd", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 304 - IBACOM_REQUEST_SDWIPE - wipe request sd            |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQUEST_SDWIPE, 0,
			"wipe request sd", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 501 - IBACOM_SET_DEVICE - device setting                 |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_DEVICE, 4,
			"device setting", { 2, 10, 1, 25 } // name, devname, command, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 502 - IBACOM_SET_DEVNAME - set devname                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_DEVNAME, 3,
			"set devname", { 2, 10, 11 } // name, devname, newname
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 503 - IBACOM_SET_DEVPIN - set devpin                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_DEVPIN, 3,
			"set devpin", { 2, 10, 63 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 504 - IBACOM_REM_DEVICE - rem device                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REM_DEVICE, 2,
			"rem device", { 2, 10 } // name, devname
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 505 - IBACOM_REQ_DEV_STATUS - dev status request         |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQ_DEV_STATUS, 2,
			"dev status request", { 2, 10 } // name, devname
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 506 - IBACOM_REQ_DEV_RESET - dev status reset            |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQ_DEV_RESET, 2,
			"dev status reset", { 2, 10 } // name, devname
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1001 - IBACOM_CONF_STD - standard conf                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_STD, 5,
			"standard conf", { 2, 3, 4, 9, 12 } // name, type, board, devnum, desc1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1005 - IBACOM_CONF_SAMPLER - sampler conf                |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_SAMPLER, 3,
			"sampler conf", { 2, 41, 45 } // name, stepday1, stephour1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1100 - IBACOM_CONF_ESP - esp conf                        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_ESP, 13,
			"esp conf", { 2, 71, 72, 75, 76, 49, 50, 51, 52, 55, 56, 57, 58 } // name, ip1, ip2, port1, port2, timeout1, timeout2, timeout3, timeout4, count1, count2, count3, count4
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1300 - IBACOM_CONF_CAMERA - camera conf                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_CAMERA, 1,
			"camera conf", { 2 } // name
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1301 - IBACOM_CONF_ASENSOR - analog sensor conf          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_ASENSOR, 5,
			"analog sensor conf", { 2, 10, 63, 33, 34 } // name, devname, pin1, param1, param2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1302 - IBACOM_CONF_SWITCH - switch conf                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_SWITCH, 3,
			"switch conf", { 2, 10, 63 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1310 - IBACOM_CONF_LDR - ldr sensor conf                 |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_LDR, 5,
			"ldr sensor conf", { 2, 10, 63, 33, 34 } // name, devname, pin1, param1, param2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1422 - IBACOM_CONF_DHT22 - DHT22 sensor conf             |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_DHT22, 8,
			"DHT22 sensor conf", { 2, 10, 63, 33, 34, 35, 36, 37 } // name, devname, pin1, param1, param2, param3, param4, param5
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1700 - IBACOM_CONF_RELAY - relay conf                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_RELAY, 3,
			"relay conf", { 2, 10, 63 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2000 - IBACOM_CONF_DEVSTD - device generic conf          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_DEVSTD, 3,
			"device generic conf", { 2, 10, 63 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2001 - IBACOM_STATUS_STD - standard status               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_STD, 8,
			"standard status", { 2, 16, 18, 17, 19, 20, 21, 22 } // name, epoch, looptime, freemem, errors1, errors2, errors3, errors4
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2005 - IBACOM_STATUS_SAMPLER - sampler status            |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_SAMPLER, 3,
			"sampler status", { 2, 41, 25 } // name, stepday1, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2100 - IBACOM_STATUS_ESP - esp status                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_ESP, 11,
			"esp status", { 2, 70, 69, 68, 67, 24, 23, 19, 20, 21, 22 } // name, serialtx, serialrx, tcptx, tcprx, totaltx, totalrx, errors1, errors2, errors3, errors4
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2270 - IBACOM_STATUS_SD - sd statistics                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_SD, 5,
			"sd statistics", { 2, 10, 63, 33, 34 } // name, devname, pin1, param1, param2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2300 - IBACOM_STATUS_CAMERA - camera status              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_CAMERA, 1,
			"camera status", { 2 } // name
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2301 - IBACOM_STATUS_ASENSOR - analog sensor stat        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_ASENSOR, 4,
			"analog sensor stat", { 2, 10, 25, 54 } // name, devname, value1, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2302 - IBACOM_STATUS_SWITCH - switch sensor stat         |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_SWITCH, 4,
			"switch sensor stat", { 2, 10, 25, 54 } // name, devname, value1, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2310 - IBACOM_STATUS_LDR - ldr sensor stat               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_LDR, 4,
			"ldr sensor stat", { 2, 10, 25, 54 } // name, devname, value1, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2522 - IBACOM_STATUS_DHT22 - DHT22 sensor stat           |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_DHT22, 5,
			"DHT22 sensor stat", { 2, 10, 25, 26, 54 } // name, devname, value1, value2, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2700 - IBACOM_STATUS_RELAY - relay stat                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_RELAY, 4,
			"relay stat", { 2, 10, 54, 53 } // name, devname, status, level
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3000 - IBACOM_STATUS_DEVSTD - device generic stat        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_DEVSTD, 3,
			"device generic stat", { 2, 10, 54 } // name, devname, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3001 - IBACOM_HEAT_ALARM - heat alarm                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_HEAT_ALARM, 4,
			"heat alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3002 - IBACOM_OVERH_ALARM - overheat alarm               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_OVERH_ALARM, 4,
			"overheat alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3003 - IBACOM_FIRE_ALARM - fire alarm                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_FIRE_ALARM, 4,
			"fire alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3004 - IBACOM_FLOOD_ALARM - flood alarm                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_FLOOD_ALARM, 4,
			"flood alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3005 - IBACOM_MOIST_ALARM - moist alarm                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_MOIST_ALARM, 4,
			"moist alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3006 - IBACOM_SMOG_ALARM - smog alarm                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SMOG_ALARM, 4,
			"smog alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3007 - IBACOM_HAZARD_ALARM - hazard alarm                |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_HAZARD_ALARM, 4,
			"hazard alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3008 - IBACOM_SMOKE_ALARM - smoke alarm                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SMOKE_ALARM, 4,
			"smoke alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3009 - IBACOM_GRID_ALARM - grid load alarm               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_GRID_ALARM, 4,
			"grid load alarm", { 4, 10, 16, 25 } // board, devname, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3050 - IBACOM_GENERIC_ALARM - generic alarm              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_GENERIC_ALARM, 5,
			"generic alarm", { 4, 10, 16, 25, 3 } // board, devname, epoch, value1, type
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3205 - IBACOM_REQ_PROBE - probe request                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQ_PROBE, 0,
			"probe request", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3205 - IBACOM_REQUEST_PROBE - probe request              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQUEST_PROBE, 0,
			"probe request", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3505 - IBACOM_PROBE_AMBIENT - ambient probe              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_PROBE_AMBIENT, 11,
			"ambient probe", { 4, 10, 16, 12, 13, 14, 15, 25, 26, 27, 28 } // board, devname, epoch, desc1, desc2, desc3, desc4, value1, value2, value3, value4
		},

#if defined (ESP8266) || defined (__linux__)
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5002 - IBACOM_CONF_AES - aes conf                        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_AES, 3,
			"aes conf", { 1, 83, 84 } // command, aeskey1, aeskey2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5003 - IBACOM_CONF_NET - network conf                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_NET, 5,
			"network conf", { 1, 71, 75, 49, 50 } // command, ip1, port1, timeout1, timeout2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5025 - IBACOM_ACK - acknowledge                          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_ACK, 1,
			"acknowledge", { 79 } // hash1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5050 - IBACOM_NET_ADDRESS - network address              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_NET_ADDRESS, 3,
			"network address", { 1, 71, 75 } // command, ip1, port1
		},

#endif /* ESP8266 or __linux__ */

#if defined (__linux__)
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7001 - IBACOM_REQ_LOGIN - login request                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQ_LOGIN, 2,
			"login request", { 2, 1 } // name, command
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7002 - IBACOM_SESSION_STAT - session status              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SESSION_STAT, 3,
			"session status", { 2, 54, 16 } // name, status, epoch
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7003 - IBACOM_SESSION_END - session end                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SESSION_END, 0,
			"session end", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7010 - IBACOM_SRV_REQ - server request                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SRV_REQ, 1,
			"server request", { 1 } // command
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7012 - IBACOM_SRV_CONF - server config                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SRV_CONF, 5,
			"server config", { 71, 75, 76, 54, 85 } // ip1, port1, port2, status, table1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7021 - IBACOM_SRV_BOARD - board data                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SRV_BOARD, 7,
			"board data", { 2, 3, 4, 9, 16, 71, 75 } // name, type, board, devnum, epoch, ip1, port1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7022 - IBACOM_SRV_BOARD_CONN - board connection          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SRV_BOARD_CONN, 2,
			"board connection", { 2, 54 } // name, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7701 - IBACOM_SET_ENV_ALARM - set environment alarm      |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_ENV_ALARM, 3,
			"set environment alarm", { 10, 4, 25 } // devname, board, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7702 - IBACOM_ACK_ENV_ALARM - alarm acknowledge          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_ACK_ENV_ALARM, 3,
			"alarm acknowledge", { 10, 4, 25 } // devname, board, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7703 - IBACOM_ENV_ALARM - environment alarm signal       |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_ENV_ALARM, 4,
			"environment alarm signal", { 10, 4, 16, 25 } // devname, board, epoch, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 8010 - IBACOM_LYT_BOARD - layout board                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_LYT_BOARD, 4,
			"layout board", { 2, 9, 5, 6 } // name, devnum, label1, label2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 8012 - IBACOM_LYT_DEVICE - layout device                 |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_LYT_DEVICE, 5,
			"layout device", { 10, 4, 5, 6, 3 } // devname, board, label1, label2, type
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 8020 - IBACOM_LYT_LIGHTS - layout lights                 |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_LYT_LIGHTS, 6,
			"layout lights", { 10, 4, 5, 6, 3, 12 } // devname, board, label1, label2, type, desc1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 8050 - IBACOM_LYT_ALARMS - layout alarms                 |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_LYT_ALARMS, 6,
			"layout alarms", { 10, 4, 5, 6, 3, 12 } // devname, board, label1, label2, type, desc1
		},

#endif /* __linux__ */

	};
}

#endif  /* PACKETPGM_H_ */