/*
 * definitions.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_DEFINITIONS_H_
#define SOURCE_DEFINITIONS_H_

#include "project.h"
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Boards default macros and definition                        |
//      |___________________________________________________________________|
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#if defined (ARDUINO)
                
        #include "RTClib.h"

        #define P2C(str) ({ char buf[SERIAL_TX_BUFFER_SIZE]; strcpy_P(buf, str); buf; })
        #define F2C(str) ({ P2C(reinterpret_cast<PGM_P>(str)); })

        #if defined(ARDUINO_AVR_UNO)
        
            #include "boards/board_uno.h"

        #elif defined(ARDUINO_AVR_MEGA2560)
        
            #include "boards/board_mega2560.h"
        
        #elif defined(ARDUINO_SAM_DUE)
        
            #include "boards/board_due.h"
        
        #elif defined(ARDUINO_ESP8266_GENERIC)

            #include "boards/board_genericESP8266.h"

        #elif defined(ARDUINO_ESP8266_WEMOS_D1R1)
                
            #include "boards/board_WeMos_miniD1R1.h"

        #else
        
            #error "CANT FIND SELECTED ARDUINO BOARD"
        
        #endif
    
    /* Function: freeRam() */
    
    #ifdef __cplusplus
        extern "C"
        {
    #endif
            
    size_t freeRam();

    #ifdef  __cplusplus
        };  
        extern "C" char* sbrk(int i);
    #endif

#else        
    
    #include "boards/board_x86pc.h"

    #define P2C(...) __VA_ARGS__
    #define F2C(...) __VA_ARGS__

#endif



#define     __FSH   __FlashStringHelper


//      _____________________________________________________________________
//      |                                                                   |
//      |       Indoorino standard types and values                         |
//      |___________________________________________________________________|
//

#ifndef SECONDS_PER_HOUR
#define SECONDS_PER_HOUR            3600
#endif

#ifndef SECONDS_PER_DAY
#define SECONDS_PER_DAY             86400L
#endif


#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE       64
#endif

#ifndef SERIAL_DEFAULT_BAUDRATE
#define SERIAL_DEFAULT_BAUDRATE     115200L
#endif

#ifndef  SECONDS_FROM_1970_TO_2000
#define  SECONDS_FROM_1970_TO_2000  946684800L
#endif

#ifndef  SECONDS_FROM_1970_TO_2010
#define  SECONDS_FROM_1970_TO_2010  1262304000L
#endif

#ifndef  SECONDS_FROM_1970_TO_2100
#define  SECONDS_FROM_1970_TO_2100  4102444800L
#endif

typedef     uint16_t                ibasize_t;
typedef     uint16_t                ibacomm_t;
typedef     uint32_t                ibaepoch_t;
typedef     uint8_t                 ibavar_t;

#define     MAX_ATTACHED_DEVICES    20
#define     MAX_PAYLOAD_SIZE       512

#define     LEN_NAME                16
#define     LEN_LABEL               32
#define     LEN_FIELD               12
#define     LEN_DEVNAME              8
#define     LEN_SSID                32
#define     LEN_PSK                 64
#define     LEN_IPADDRESS           17
#define     LEN_DATETIMESTR         22
#define     MAX_PACKET_ENTRIES      16
#define     F2C_BUFFER_SIZE         64

#define     SIZEOF_STDCONF          (3 * LEN_NAME + sizeof(uint8_t))
#define     SIZEOF_PREAMBLE         (4 * sizeof(uint8_t))
#define     SIZEOF_COMMAND          sizeof(ibacomm_t)
#define     SIZEOF_EPOCH            sizeof(uint32_t)
#define     REFRESH_RATE_1       30000   

const uint8_t   PREAMBLE[] PROGMEM = {UINT8_MAX,1,1,UINT8_MAX};

//      _____________________________________________________________________
//      |                                                                   |
//      |       Network default parameters                                  |
//      |___________________________________________________________________|
//

const char  DEFAULT_TARGET_IP[] PROGMEM = "192.168.1.42";

// _______                                                                      _______
// |     |                                                                      |     |
// |     | ip (dynamic): 192.168.X.Y                  ip (static): 192.168.1.42 |     |
// | ESP |   <--------------------------------------------------------------->  | APP |   
// |     | port: 7870 (DEFAULT_LOCALPORT)      port: 7890 (DEFAULT_TARGET_PORT) |     |
// |_____|                                                                      |_____|

#define     DEFAULT_TARGET_PORT 7890    // Port on PC UDP server
#define     DEFAULT_LOCALPORT   7870    // Default port for ESP UDP



//      _____________________________________________________________________
//      |                                                                   |
//      |       SD card parameters                                          |
//      |___________________________________________________________________|
//

#if defined (SD_MODULE)

    #define     SD_PIN          53
    #define     SD_SPEED_MHZ    16
//     #define     SD_DROP_RATE    100
    const char SD_CONF_PATH[]  PROGMEM = "/CONF.NDO";
    const char SD_PROBE_PATH[] PROGMEM = "/PROBES.NDO";

#endif

//      _____________________________________________________________________
//      |                                                                   |
//      |       Project specific macros                                     |
//      |___________________________________________________________________|
//

typedef struct {
    char        name[LEN_DEVNAME];
    uint8_t     pin;
    ibacomm_t   type;
} default_dev_conf_template;

#if defined (INDOORINO_SAMPLER)

    #include "projectypes/samplerStd.h"
    
#elif defined (INDOORINO_ESPSERVER)

    #include "projectypes/espserverStd.h"

#elif defined (INDOORINO_CAMERA)

//     #include "types/cameraStd.h"
    
#elif defined (INDOORINO_PHONEDOOR)

//     #include "types/phonedoorStd.h"

#elif defined (INDOORINO_PYLIB)

    #define PY_SSIZE_T_CLEAN
    #include <Python.h>
    const char INDOORINO_TYPE[] PROGMEM = "INDOORINO";     
    const char BOARD_NAME[] PROGMEM = "SERVER";
    #define DEFAULT_DEVNUM 0
    
#elif defined (RAWRUN)
    
    const char INDOORINO_TYPE[] PROGMEM = "RAWRUN";
    const char BOARD_NAME[] PROGMEM = "DEBUG";
    #define DEFAULT_DEVNUM 0
    
#else

    #define INDOORINO_GENERIC
    const char INDOORINO_TYPE[] PROGMEM = "GENERIC";
    #define DEFAULT_DEVNUM 0
    #warning "NO APPLICATION TYPE SELECTED"
    
#endif
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Debug and errors macros                                     |
//      |___________________________________________________________________|
//

#if defined(DEBUG_GENERIC)  || defined(DEBUG_DEVICES)  || \
    defined(DEBUG_MEMORY)   || defined(DEBUG_IO)       || \
    defined(DEBUG_SERVER)
    
        #define DEBUG_FLAG    

#endif

void        debughelper     (const __FSH * format, ...);

#if defined(DEBUG_GENERIC)
    #define debug(S,...) debughelper(F(S), ## __VA_ARGS__)
#else
    #define debug(S,...)    {}
#endif

#if defined(DEBUG_DEVICES)
    #define debug_dev(S,...) debughelper(F(S), ## __VA_ARGS__)
#else
    #define debug_dev(S,...)    {}
#endif

#if defined(DEBUG_MEMORY)
    #define debug_mem(S,...) debughelper(F(S), ## __VA_ARGS__)
#else
    #define debug_mem(S,...)    {}
#endif

#if defined(DEBUG_IO)
    #define debug_io(S,...) debughelper(F(S), ## __VA_ARGS__)
#else
    #define debug_io(S,...)    {}
#endif

#if defined(DEBUG_SERVER)
    #define debug_net(S,...) debughelper(F(S), ## __VA_ARGS__)
#else
    #define debug_net(S,...)    {}
#endif

#define error_mem(S,...) debughelper(F("\nMEM:" S), ## __VA_ARGS__)
#define error_io(S,...) debughelper(F("\nIO:" S), ## __VA_ARGS__)
#define error_dev(S,...) debughelper(F("\nDEV:" S), ## __VA_ARGS__)
#define error_net(S,...) debughelper(F("\nNET:" S), ## __VA_ARGS__)

// #define error_mem(S,...) debughelper(F(S), ## __VA_ARGS__)
// #define error_io(S,...) debughelper(F(S), ## __VA_ARGS__)
// #define error_dev(S,...) debughelper(F(S), ## __VA_ARGS__)




#endif /* SOURCE_DEFINITIONS_H_ */




