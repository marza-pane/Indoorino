/*
 * packetmap.h
 *
 *  Created on: Apr 25, 2020
 *   Edited on: Mar 04, 2021 - 22:28:24
 *      Author: n00b
 *  This code is code-generated
 */

#ifndef PACKETPGM_H_
#define PACKETPGM_H_


#if defined (__linux__)
	#include "../common/comtable.h"
	#include "../common/definitions.h"
	#define VARMAP_NUMBER 85
#else
	#include "comtable.h"
	#include "definitions.h"
	#if defined (ESP8266)
		#define VARMAP_NUMBER 81
	#else
		#define VARMAP_NUMBER 75
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
		{ 's',	sizeof(uint8_t)       , "devnum"               },	/*   5 */
		{ 'c',	LEN_DEVNAME           , "devname"              },	/*   6 */
		{ 'c',	LEN_DEVNAME           , "newname"              },	/*   7 */
		{ 'c',	LEN_NAME              , "desc1"                },	/*   8 */
		{ 'c',	LEN_NAME              , "desc2"                },	/*   9 */
		{ 'c',	LEN_NAME              , "desc3"                },	/*  10 */
		{ 'c',	LEN_NAME              , "desc4"                },	/*  11 */
		{ 'u',	sizeof(uint32_t)      , "epoch"                },	/*  12 */
		{ 'u',	sizeof(uint32_t)      , "freemem"              },	/*  13 */
		{ 'u',	sizeof(uint32_t)      , "looptime"             },	/*  14 */
		{ 'n',	sizeof(int32_t)       , "errors1"              },	/*  15 */
		{ 'n',	sizeof(int32_t)       , "errors2"              },	/*  16 */
		{ 'n',	sizeof(int32_t)       , "errors3"              },	/*  17 */
		{ 'n',	sizeof(int32_t)       , "errors4"              },	/*  18 */
		{ 'u',	sizeof(uint32_t)      , "totalrx"              },	/*  19 */
		{ 'u',	sizeof(uint32_t)      , "totaltx"              },	/*  20 */
		{ 'n',	sizeof(int32_t)       , "value1"               },	/*  21 */
		{ 'n',	sizeof(int32_t)       , "value2"               },	/*  22 */
		{ 'n',	sizeof(int32_t)       , "value3"               },	/*  23 */
		{ 'n',	sizeof(int32_t)       , "value4"               },	/*  24 */
		{ 'n',	sizeof(int32_t)       , "value5"               },	/*  25 */
		{ 'n',	sizeof(int32_t)       , "value6"               },	/*  26 */
		{ 'n',	sizeof(int32_t)       , "value7"               },	/*  27 */
		{ 'n',	sizeof(int32_t)       , "value8"               },	/*  28 */
		{ 'n',	sizeof(int32_t)       , "param1"               },	/*  29 */
		{ 'n',	sizeof(int32_t)       , "param2"               },	/*  30 */
		{ 'n',	sizeof(int32_t)       , "param3"               },	/*  31 */
		{ 'n',	sizeof(int32_t)       , "param4"               },	/*  32 */
		{ 'n',	sizeof(int32_t)       , "param5"               },	/*  33 */
		{ 'n',	sizeof(int32_t)       , "param6"               },	/*  34 */
		{ 'n',	sizeof(int32_t)       , "param7"               },	/*  35 */
		{ 'n',	sizeof(int32_t)       , "param8"               },	/*  36 */
		{ 'u',	sizeof(uint32_t)      , "stepday1"             },	/*  37 */
		{ 'u',	sizeof(uint32_t)      , "stepday2"             },	/*  38 */
		{ 'u',	sizeof(uint32_t)      , "stepday3"             },	/*  39 */
		{ 'u',	sizeof(uint32_t)      , "stepday4"             },	/*  40 */
		{ 'u',	sizeof(uint32_t)      , "stephour1"            },	/*  41 */
		{ 'u',	sizeof(uint32_t)      , "stephour2"            },	/*  42 */
		{ 'u',	sizeof(uint32_t)      , "stephour3"            },	/*  43 */
		{ 'u',	sizeof(uint32_t)      , "stephour4"            },	/*  44 */
		{ 'u',	sizeof(uint32_t)      , "timeout1"             },	/*  45 */
		{ 'u',	sizeof(uint32_t)      , "timeout2"             },	/*  46 */
		{ 'u',	sizeof(uint32_t)      , "timeout3"             },	/*  47 */
		{ 'u',	sizeof(uint32_t)      , "timeout4"             },	/*  48 */
		{ 's',	sizeof(uint8_t)       , "level"                },	/*  49 */
		{ 's',	sizeof(uint8_t)       , "status"               },	/*  50 */
		{ 's',	sizeof(uint8_t)       , "count1"               },	/*  51 */
		{ 's',	sizeof(uint8_t)       , "count2"               },	/*  52 */
		{ 's',	sizeof(uint8_t)       , "count3"               },	/*  53 */
		{ 's',	sizeof(uint8_t)       , "count4"               },	/*  54 */
		{ 's',	sizeof(uint8_t)       , "count5"               },	/*  55 */
		{ 's',	sizeof(uint8_t)       , "count6"               },	/*  56 */
		{ 's',	sizeof(uint8_t)       , "count7"               },	/*  57 */
		{ 's',	sizeof(uint8_t)       , "count8"               },	/*  58 */
		{ 's',	sizeof(uint8_t)       , "pin1"                 },	/*  59 */
		{ 's',	sizeof(uint8_t)       , "pin2"                 },	/*  60 */
		{ 's',	sizeof(uint8_t)       , "pin3"                 },	/*  61 */
		{ 's',	sizeof(uint8_t)       , "pin4"                 },	/*  62 */
		{ 'u',	sizeof(uint32_t)      , "tcprx"                },	/*  63 */
		{ 'u',	sizeof(uint32_t)      , "tcptx"                },	/*  64 */
		{ 'u',	sizeof(uint32_t)      , "serialrx"             },	/*  65 */
		{ 'u',	sizeof(uint32_t)      , "serialtx"             },	/*  66 */
		{ 'c',	INET_ADDRSTRLEN       , "ip1"                  },	/*  67 */
		{ 'c',	INET_ADDRSTRLEN       , "ip2"                  },	/*  68 */
		{ 'c',	INET_ADDRSTRLEN       , "ip3"                  },	/*  69 */
		{ 'c',	INET_ADDRSTRLEN       , "ip4"                  },	/*  70 */
		{ 'u',	sizeof(uint32_t)      , "port1"                },	/*  71 */
		{ 'u',	sizeof(uint32_t)      , "port2"                },	/*  72 */
		{ 'u',	sizeof(uint32_t)      , "port3"                },	/*  73 */
		{ 'u',	sizeof(uint32_t)      , "port4"                },	/*  74 */

#if defined (ESP8266) || defined (__linux__)

		{ 'h',	SHA_DIGEST_LENGTH     , "hash1"                },	/*  75 */
		{ 'h',	SHA_DIGEST_LENGTH     , "hash2"                },	/*  76 */
		{ 'h',	SHA_DIGEST_LENGTH     , "hash3"                },	/*  77 */
		{ 'h',	SHA_DIGEST_LENGTH     , "hash4"                },	/*  78 */
		{ 'h',	LEN_AES_MASTERKEY     , "aeskey1"              },	/*  79 */
		{ 'h',	LEN_AES_MASTERKEY     , "aeskey2"              },	/*  80 */

#endif /* ESP8266 or __linux__ */

#if defined (__linux__)

		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table1"               },	/*  81 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table2"               },	/*  82 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table3"               },	/*  83 */
		{ 'c',	SERIAL_TX_BUFFER_SIZE , "table4"               },	/*  84 */

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
	//    |    IBACOM - 010 - IBACOM_UNIX_EPOCH - timestamp                      |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_UNIX_EPOCH, 1,
			"timestamp", { 12 } // epoch
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 015 - IBACOM_REPORT - generic report                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REPORT, 4,
			"generic report", { 0, 2, 12, 49 } // message, name, epoch, level
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
			"device setting", { 2, 6, 1, 21 } // name, devname, command, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 502 - IBACOM_SET_DEVNAME - set devname                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_DEVNAME, 3,
			"set devname", { 2, 6, 7 } // name, devname, newname
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 503 - IBACOM_SET_DEVPIN - set devpin                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SET_DEVPIN, 3,
			"set devpin", { 2, 6, 59 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 504 - IBACOM_REM_DEVICE - rem device                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REM_DEVICE, 2,
			"rem device", { 2, 6 } // name, devname
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1001 - IBACOM_CONF_STD - standard conf                   |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_STD, 5,
			"standard conf", { 2, 3, 4, 5, 8 } // name, type, board, devnum, desc1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1005 - IBACOM_CONF_SAMPLER - sampler conf                |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_SAMPLER, 3,
			"sampler conf", { 2, 37, 41 } // name, stepday1, stephour1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1100 - IBACOM_CONF_ESP - esp conf                        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_ESP, 13,
			"esp conf", { 2, 67, 68, 71, 72, 45, 46, 47, 48, 51, 52, 53, 54 } // name, ip1, ip2, port1, port2, timeout1, timeout2, timeout3, timeout4, count1, count2, count3, count4
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
			"analog sensor conf", { 2, 6, 59, 29, 30 } // name, devname, pin1, param1, param2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1302 - IBACOM_CONF_SWITCH - switch conf                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_SWITCH, 3,
			"switch conf", { 2, 6, 59 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1310 - IBACOM_CONF_LDR - ldr sensor conf                 |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_LDR, 5,
			"ldr sensor conf", { 2, 6, 59, 29, 30 } // name, devname, pin1, param1, param2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1522 - IBACOM_CONF_DHT22 - DHT22 sensor conf             |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_DHT22, 7,
			"DHT22 sensor conf", { 2, 6, 59, 29, 30, 31, 32 } // name, devname, pin1, param1, param2, param3, param4
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 1700 - IBACOM_CONF_RELAY - relay conf                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_RELAY, 3,
			"relay conf", { 2, 6, 59 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2000 - IBACOM_CONF_DEVSTD - device generic conf          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_DEVSTD, 3,
			"device generic conf", { 2, 6, 59 } // name, devname, pin1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2001 - IBACOM_STATUS_STD - standard status               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_STD, 8,
			"standard status", { 2, 12, 14, 13, 15, 16, 17, 18 } // name, epoch, looptime, freemem, errors1, errors2, errors3, errors4
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2005 - IBACOM_STATUS_SAMPLER - sampler status            |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_SAMPLER, 3,
			"sampler status", { 2, 37, 21 } // name, stepday1, value1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2100 - IBACOM_STATUS_ESP - esp status                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_ESP, 11,
			"esp status", { 2, 66, 65, 64, 63, 20, 19, 15, 16, 17, 18 } // name, serialtx, serialrx, tcptx, tcprx, totaltx, totalrx, errors1, errors2, errors3, errors4
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2270 - IBACOM_STATUS_SD - sd statistics                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_SD, 5,
			"sd statistics", { 2, 6, 59, 29, 30 } // name, devname, pin1, param1, param2
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
			"analog sensor stat", { 2, 6, 21, 50 } // name, devname, value1, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2302 - IBACOM_STATUS_SWITCH - switch sensor stat         |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_SWITCH, 4,
			"switch sensor stat", { 2, 6, 21, 50 } // name, devname, value1, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2310 - IBACOM_STATUS_LDR - ldr sensor stat               |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_LDR, 4,
			"ldr sensor stat", { 2, 6, 21, 50 } // name, devname, value1, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2522 - IBACOM_STATUS_DHT22 - DHT22 sensor stat           |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_DHT22, 5,
			"DHT22 sensor stat", { 2, 6, 21, 22, 50 } // name, devname, value1, value2, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 2700 - IBACOM_STATUS_RELAY - relay stat                  |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_RELAY, 4,
			"relay stat", { 2, 6, 50, 49 } // name, devname, status, level
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3000 - IBACOM_STATUS_DEVSTD - device generic stat        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_STATUS_DEVSTD, 3,
			"device generic stat", { 2, 6, 50 } // name, devname, status
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3001 - IBACOM_ALLARM - allarm                            |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_ALLARM, 5,
			"allarm", { 0, 4, 3, 12, 49 } // message, board, type, epoch, level
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3002 - IBACOM_WARNINGS - home warning                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_WARNINGS, 5,
			"home warning", { 0, 4, 3, 12, 49 } // message, board, type, epoch, level
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3003 - IBACOM_INFOREP - home report                      |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_INFOREP, 5,
			"home report", { 0, 4, 3, 12, 49 } // message, board, type, epoch, level
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
	//    |    IBACOM - 3210 - IBACOM_REQ_DEV_STATUS - dev status request        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQ_DEV_STATUS, 0,
			"dev status request", {}
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 3212 - IBACOM_REQ_DEV_RESET - dev status reset           |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_REQ_DEV_RESET, 0,
			"dev status reset", {}
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
			"ambient probe", { 4, 6, 12, 8, 9, 10, 11, 21, 22, 23, 24 } // board, devname, epoch, desc1, desc2, desc3, desc4, value1, value2, value3, value4
		},

#if defined (ESP8266) || defined (__linux__)
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5002 - IBACOM_CONF_AES - aes conf                        |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_AES, 3,
			"aes conf", { 1, 79, 80 } // command, aeskey1, aeskey2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5003 - IBACOM_CONF_NET - network conf                    |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_CONF_NET, 5,
			"network conf", { 1, 67, 71, 45, 46 } // command, ip1, port1, timeout1, timeout2
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5025 - IBACOM_ACK - acknowledge                          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_ACK, 1,
			"acknowledge", { 75 } // hash1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 5050 - IBACOM_NET_ADDRESS - network address              |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_NET_ADDRESS, 3,
			"network address", { 1, 67, 71 } // command, ip1, port1
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
			"session status", { 2, 50, 12 } // name, status, epoch
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
			"server config", { 67, 71, 72, 50, 81 } // ip1, port1, port2, status, table1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7021 - IBACOM_SRV_BOARD - board data                     |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SRV_BOARD, 7,
			"board data", { 2, 3, 4, 5, 12, 67, 71 } // name, type, board, devnum, epoch, ip1, port1
		},
	
	
	//    ________________________________________________________________________
	//    |                                                                      |
	//    |    IBACOM - 7022 - IBACOM_SRV_BOARD_CONN - board connection          |
	//    |______________________________________________________________________|
	//
		{
			IBACOM_SRV_BOARD_CONN, 2,
			"board connection", { 2, 50 } // name, status
		},

#endif /* __linux__ */

	};
}

#endif  /* PACKETPGM_H_ */