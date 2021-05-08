/*
 * common/project.h
 *
 *  Created on: May 01, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_BOARDMACROS_H_
#define SOURCE_BOARDMACROS_H_

//      _____________________________________________________________________
//      |                                                                   |
//      |       PROJECT MACRO FLAGS                                         |
//      |___________________________________________________________________|
//

/*/
 * Following macros defines hardware target and specifications:
 * 
 * ARDUINO              : defined when compiling with Arduino IDE (ESP included)
 * ESP8266              : defined when compiling ESP board with Arduino IDE
 * __linux__            : defined when compiled by gcc
 * 
 * RTC_MODULE           : define this if an RTC clock is attached via I2C
 * 
 * INDOORINO_ROUTER     : flag to prgram a ROUTER on ESP board
 * INDOORINO_CONTROLLER : flag to prgram a CONTROLLER on AVR board
 * INDOORINO_CAMERA     : flag to prgram a CAMERA on ESP board (TODO)
 * 
 * INDOORINO_DEVS       : allow device-on-pin management
 * 
 * INDOORINO_SERVER     : defined defined by SERVER compile script. For SERVER compiling
 * INDOORINO_CLIENT     : defined defined by CLIENT compile script. For CLIENT compiling
 * INDOORINO_NETWORK    : defined when compiling for SERVER/CLIENT in their own scripts or for ROUTER
 * 
 * Note: compile scripts are usually in build/setup.py
 *
 * How to: Map new sensor into the system
 
    - add public virtualSensor/virtualActuator class into indoorino source (sensor.h and sensor.cpp)
    - add class init into device list (sensor.h and sensor.cpp)
    - add sensor status and config packet 
    - 
 * 
/*/

/*-----------------------------------------*
 * Put custom build flags here:            *
 * vvv vvv vvv vvv vvv vvv vvv vvv vvv vvv */


    /* Uncomment the project you want to compile */    
    #if defined (ARDUINO)
    
        #include <Arduino.h>

        /* MANDATORY FOR ARDUINO: choose a unique board name */

        const char BOARD_NAME[] PROGMEM = "HOUSE.BEAMS";
        const char INDOORINO_RESOURCE_DESC[] PROGMEM = "LIGHTS";
        #define INDOORINO_CONTROLLER

//         const char BOARD_NAME[] PROGMEM = "SRV.HSE.BMS";
//         const char INDOORINO_RESOURCE_DESC[] PROGMEM = "ROUTER";
//         #define INDOORINO_ROUTER

//         const char BOARD_NAME[] PROGMEM = "KITCHEN.WEATHER";
//         const char INDOORINO_RESOURCE_DESC[] PROGMEM = "WEATHER";
//         #define INDOORINO_CONTROLLER

//         const char BOARD_NAME[] PROGMEM = "WEATHER.SOUTH";
//         #define INDOORINO_SAMPLER


//         const char BOARD_NAME[] PROGMEM = "CAMERA.GATE";
//         #define INDOORINO_CAMERA
        
//         const char BOARD_NAME[] PROGMEM = "PHONE.DOOR";
//         #define INDOORINO_CONTROLLER
        
//         const char BOARD_NAME[] PROGMEM = "GATE.KEEPER";
//         const char INDOORINO_RESOURCE_DESC[] PROGMEM = "ROUTER";
//         #define INDOORINO_ROUTER
        
    #endif /*ARDUINO */

    /***********************************************************\
    |*      Uncomment this to enable attached devices manager   |
    \***********************************************************/
        
//     #define INDOORINO_DEVS

    /***********************************************************\
    |*      Uncomment this if RTC clock is attached             |
    \***********************************************************/
    
//     #define RTC_MODULE

    /***********************************************************\
    |*      Uncomment one or more of these for debugging        |
    \***********************************************************/

//     #define DEBUG_MEMORY
//     #define DEBUG_IO
//     #define DEBUG_OS
//     #define DEBUG_SERIAL
//     #define DEBUG_PARSER
//     #define DEBUG_TCP 
//     #define DEBUG_DEVICES
//     #define DEBUG_NETWORK
//     #define DEBUG_SERVER
//     #define DEBUG_CLIENT
//     #define DEBUG_MODULE
//     #define DEBUG_PACKET
//     #define DEBUG_PACKETDUMP
//     #define DEBUG_CHKSUM // TODO
    
    
/* ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ *
 *                                         *
 *-----------------------------------------*/






#endif /* SOURCE_BOARDMACROS_H_ */
