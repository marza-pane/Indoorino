/*
 * common/definitions.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_DEFINITIONS_H_
#define SOURCE_DEFINITIONS_H_
#define INDOORINO_VERSION "3.1.0"

#if !defined(__linux__) && !defined(ARDUINO)
#error "Indoorino can only be compiled for Linux, AVR and ESP"
#endif

/* Some people ask why the first release is number 3. Here is why.
 * 0 was my first idea working with strings in serial
 * 1 when I realized that a packet system would have been a great improvement
 * 2 comes with ESP and UDP transmission
 * 3 uses TCP and AES encryption
 * I have little IT knowledge and it took me 3 years as a hobby lo learn and develop.
 */

//     
//      _____________________________________________________________________
//      |                                                                   |
//      |       Boards default macros and definition                        |
//      |___________________________________________________________________|
//

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>

#include "project.h"
#include "comtable.h"

#if defined (ARDUINO)
                
        #include "RTClib.h"
        #include "AESLib.h"

        #define     DEBUG_BUFFER_SIZE       64
        #define P2C(str) ({ char buf[SERIAL_TX_BUFFER_SIZE]; strcpy_P(buf, str); (char *)buf; })
        #define F2C(str) ({ P2C(reinterpret_cast<PGM_P>(str)); })
        const char DEFAULT_TARGET[] PROGMEM = "SERVER";
//         typedef uint32_t ibatime_t ;

        #if defined(ARDUINO_AVR_UNO)
        
            #include "boards/board-uno.h"

        #elif defined(ARDUINO_AVR_MEGA2560)
        
            #include "boards/board-mega2560.h"
        
        #elif defined(ARDUINO_SAM_DUE)
        
            #include "boards/board-due.h"
        
        #elif defined(ESP8266)

            #include "boards/board-esp8266.h"

        #else
        
            #error "CANT FIND SELECTED ARDUINO BOARD"
        
        #endif

#else        

    #include <unistd.h>
    #include <netdb.h>
    #if 0
    #ifdef _WIN32
    #define _WIN32_WINNT 0x0A00
    #endif
    #endif

    #define DEBUG_BUFFER_SIZE 256
    #include <signal.h>
    #include <sys/time.h>
    #include <openssl/sha.h>

    #include <fstream>
//     #include <ctime>
    #include <atomic> 
    #include <random>
    #include <vector>
    #include <thread>
    #include <chrono>
    #include <iostream>
    #include <algorithm>
    #include <memory>
//     #include <list>

//     #include <optional>
//     #include <iterator>
//     #include <cstdint>
//     #include <iomanip>
        
    #include "boards/board-x86pc.h"
    #include "../AES/src/AESLib.h"

//     typedef time_t ibatime_t;

    #if !defined(INDOORINO_NETWORK)
        #define INDOORINO_NETWORK
    #endif
    
#endif

#define     __FSH   __FlashStringHelper

#define     MIN_INTEGER_M_(X,Y) (((X) < (Y)) ? (X) : (Y))
#define     MAX_INTEGER_M_(X,Y) (((X) > (Y)) ? (X) : (Y))
//      _____________________________________________________________________
//      |                                                                   |
//      |       Indoorino standard types and values                         |
//      |___________________________________________________________________|
//

typedef     uint8_t     iPin_t;
typedef     uint16_t    iCom_t;
typedef     uint16_t    iSize_t;
typedef     uint32_t    iEpoch_t;
typedef     uint32_t    iPid_t;

#ifndef SECONDS_PER_HOUR
#define SECONDS_PER_HOUR            3600
#endif

#ifndef SECONDS_PER_DAY
#define SECONDS_PER_DAY             86400L
#endif


#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE       64
#endif

#ifndef SHA_DIGEST_LENGTH
#define SHA_DIGEST_LENGTH           20
#endif
              
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN             16
#endif

#ifndef SERIAL_DEFAULT_BAUDRATE
#define SERIAL_DEFAULT_BAUDRATE     115200L
#endif
    
#ifndef SECONDS_FROM_1970_TO_2000
#define SECONDS_FROM_1970_TO_2000  946684800L
#endif

#ifndef SECONDS_FROM_1970_TO_2010
#define SECONDS_FROM_1970_TO_2010  1262304000L
#endif

#ifndef SECONDS_FROM_1970_TO_2100
#define SECONDS_FROM_1970_TO_2100  4102444800L
#endif

#define     LEN_NAME                24
#define     LEN_LABEL               32
#define     LEN_FIELD               12
#define     LEN_DEVNAME             16
#define     LEN_SSID                32
#define     LEN_PSK                 64
#define     LEN_DATETIMESTR         22
#define     LEN_AES_MASTERKEY       N_BLOCK
#define     LEN_NETADDRESS          INET_ADDRSTRLEN + 8
#define     LEN_SERVER_PATH         256
#define     LEN_BOARDPATH           LEN_NAME + LEN_DEVNAME + 2
#define     LEN_LOCATIONAME         32
#define     F2C_BUFFER_SIZE         SERIAL_TX_BUFFER_SIZE
// #define     CLIENT_BUFFER_SIZE      2048
#define     FLOAT2UINT_M            100

#define     SIZEOF_STDCONF          (3 * LEN_NAME + sizeof(uint8_t))
#define     SIZEOF_STDESPCONF       (SIZEOF_STDCONF + LEN_SSID + LEN_PSK + (4 * sizeof(uint8_t)) + sizeof(uint16_t) + (2 * sizeof(iEpoch_t)) + sizeof(uint8_t))
#define     SIZEOF_PREAMBLE         (4 * sizeof(uint8_t))
#define     SIZEOF_PACKET_HEADER    (SIZEOF_PREAMBLE + sizeof(iCom_t) + 2 * LEN_NAME + sizeof(iPid_t)) // 4 + 2 + 48 + 4 = 58
#define     SIZEOF_NET_HEADER       (SIZEOF_PREAMBLE + sizeof(iSize_t) + N_BLOCK)

// #define     SIZEOF_COMMAND          sizeof(ibacomm_t)
// #define     SIZEOF_EPOCH            sizeof(ibaepoch_t)

    /*
     * Packets shape:
     * 
     * DEFAULT IPACKET:
     *      [                h   e   a   d   e   r                ][  body   ]
     *      [ PREAMBLE ][ COMMAND ][ SOURCE ][ DESTINATION ][ pid ][ PAYLOAD ]
     *           4           2      LEN_NAME     LEN_NAME      4        #
     * 
     * TCP PACKET:
     *      [           h   e   a   d   e   r         ][       body      ]
     *      [ PREAMBLE ][ SIZE ][ INITIALIZING VECTOR ][ DEFAULT IPACKET ]
     *           4          4      LEN_AES_MASTERKEY         58 + #
     * 
     */
    
#define     MAX_ATTACHED_DEVICES    35
#define     MAX_CLIENTS             200
// #define     MAX_PAYLOAD_SIZE        MAX_PACKET_SIZE - (LEN_NAME + SIZEOF_PREAMBLE + 4) /* 488 */
#define     MAX_SIZE_PACKET         4096
#define     MAX_QUEUED_PACKETS      300
// #define     MAX_ADM_DELAY           10

#define     RATE_UPDATE             32500L
#define     RATE_BEACON             77200L
#define     BUSY_SERIAL             100
#define     BUSY_SYSTEM             50
#define     BUSY_NETWORK            500

const uint8_t   PREAMBLE[]          PROGMEM = {UINT8_MAX,1,1,UINT8_MAX};
const char      DEFAULT_SERVER_IP[] PROGMEM = "192.168.1.42";

#define     DEFAULT_BOARD_PORT      7890        // Default ESP TCP server port
#define     DEFAULT_SHELL_PORT      7870        // Default Shell TCP server port

#define     TIME_NETWORK_CHECK      1000        // check network connection is established
#define     TIME_NETWORK_CONNECT    20000       // ESP timeout between WiFi connection attemps
#define     TIME_BIND_CONNECT       5000        // Delay if new client connection is refused

#define     TIMEOUT_CLIENT          30000L      // default life time of a client
#define     TIMEOUT_SERIAL_READ     700         // Serial read timeout (while waiting packet)
#define     TIMEOUT_CLIENT_READ     500         // delay if not available for reading      
#define     TIMEOUT_SERIAL_WRITE    400         // Serial read timeout (while waiting packet)
#define     TIMEOUT_BOARD           60000L      // board timeout before changing status (MODULE)
// #define     TIMEOUT_SESSION         600000L     // shell session timeout

#define     TIMEOUT_CLIENT_BOARD    40000L      // life time of a board client
#define     TIMEOUT_CLIENT_SHELL    70000L      // life time of a shell client



#define TIMEOUT_SEND_PACKET         1000        // Delay between packet send from list      
#define RETRY_SEND_PACKET           10          // Packet send from list retry

// #define TIMEOUT_BIND_TCP            1000        // Delay if server can not bind to port
// #define TIMEOUT_SEND_CLIENT         100         // Delay if not available for writing       
// #define RETRY_SEND_CLIENT           10          // client.write retry

//      _____________________________________________________________________
//      |                                                                   |
//      |       Project specific macros                                     |
//      |___________________________________________________________________|
//

typedef struct {
    char        name[LEN_DEVNAME];
    iPin_t      pin;
    iCom_t      type;
} device_conf_template;



#if defined (ARDUINO)

//      _____________________________________________________________________
//      |                                                                   |
//      |       CONTROLLER                                                  |
//      |___________________________________________________________________|
//

    #if defined (INDOORINO_CONTROLLER)

        const char INDOORINO_TYPE[] PROGMEM = "CONTROLLER";

        #if ! defined(RTC_MODULE)
            #define RTC_MODULE
            #warning "No rtc_MODULE detected. Please attach rtc CLOCK device for project SAMPLER"
        #endif

        #define INDOORINO_DEVS

        /*
        *  Here you can define your own standard devices.
        *  Set DEFAULT_DEVNUM to the number of device you want and compile the table below.
        *  Table parameters are: <device name>, <pin>, <device type>
        */

        #define DEFAULT_DEVNUM      9

        const device_conf_template DEFAULT_DEVCONF[DEFAULT_DEVNUM] PROGMEM = 
        {
//             "AIR1",  3, IBACOM_CONF_DUSTPM25,
//             "DHT1",  5, IBACOM_CONF_DHT22,
//             "DHT2",  6, IBACOM_CONF_DHT22,
//             "DHT3",  7, IBACOM_CONF_DHT22,
            "HEAT1",  7, IBACOM_CONF_DHT22,
            "BEAM1", 28, IBACOM_CONF_RELAY,
            "BEAM2", 30, IBACOM_CONF_RELAY,
            "BEAM3", 32, IBACOM_CONF_RELAY,
            "BEAM4", 34, IBACOM_CONF_RELAY,
            "BEAM5", 36, IBACOM_CONF_RELAY,
            "BEAM6", 38, IBACOM_CONF_RELAY,
            "BEAM7", 40, IBACOM_CONF_RELAY,
            "BEAM8", 42, IBACOM_CONF_RELAY
            
        };

//      _____________________________________________________________________
//      |                                                                   |
//      |       ESP ROUTER                                                  |
//      |___________________________________________________________________|
//
        
    #elif defined (INDOORINO_ROUTER)

        #if ! defined (ESP8266)
        #error "ESPROUTER can only be uploaded to ESP devices"
        #endif
                
        const char INDOORINO_TYPE[] PROGMEM = "ESPROUTER";
        #define DEFAULT_DEVNUM 0

    #elif defined (INDOORINO_CAMERA)

        // TODO //
    #else
        const char INDOORINO_TYPE[] PROGMEM = "RAWRUN";
        const char BOARD_NAME[] PROGMEM = "DEBUG";
        #define DEFAULT_DEVNUM 0
        #warning "NO APPLICATION TYPE SELECTED"
    #endif
        
#else

//      _____________________________________________________________________
//      |                                                                   |
//      |       SHELL CLINET                                                |
//      |___________________________________________________________________|
//        

    #if defined (INDOORINO_CLIENT)

        #if defined (ARDUINO)
        #error "CLIENT can not be uploaded to a Arduino board!"
        #endif

        #define PY_SSIZE_T_CLEAN
        #include <Python.h>

        const char DEFAULT_TARGET[] PROGMEM = "SERVER";
        const char INDOORINO_TYPE[] PROGMEM = "INDOORINO";     
        const char BOARD_NAME[] PROGMEM = "SHELLCLIENT";
        #define DEFAULT_DEVNUM 0

//      _____________________________________________________________________
//      |                                                                   |
//      |       SERVER                                                      |
//      |___________________________________________________________________|
//
        
    #elif defined (INDOORINO_SERVER)

        #if defined (ARDUINO)
        #error "SERVER can not be uploaded to a Arduino board!"
        #endif
        
        /* WARNING: Server default target must be invalid. Be carefull using packet.header() and packet.forge() */
        const char DEFAULT_TARGET[] PROGMEM = "";
        
        const char INDOORINO_TYPE[] PROGMEM = "INDOORINO";     
        const char BOARD_NAME[] PROGMEM = "SERVER";
        #define DEFAULT_DEVNUM 0
        
//      _____________________________________________________________________
//      |                                                                   |
//      |       OTHER                                                       |
//      |___________________________________________________________________|
//
    #elif defined (RAWRUN)
        
        const char INDOORINO_TYPE[] PROGMEM = "RAWRUN";
        const char BOARD_NAME[] PROGMEM = "DEBUG";
        #define DEFAULT_DEVNUM 0
        
    #else

        #define INDOORINO_GENERIC
        const char INDOORINO_TYPE[] PROGMEM = "UNKNOWN";
        const char BOARD_NAME[] PROGMEM = "UNKNOWN";
        #define DEFAULT_DEVNUM 0
        #warning "NO APPLICATION TYPE SELECTED"
        
    #endif
        
#endif /* ARDUINO / LINUX */    

#endif /* SOURCE_DEFINITIONS_H_ */

//     #include <errno.h>
//     #include <stdlib.h>
//     #include <sys/types.h> 
//     #include <arpa/inet.h>
//     #include <netinet/in.h>




