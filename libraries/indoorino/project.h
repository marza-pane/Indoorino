/*
 * boardmacros.h
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


/*-----------------------------------------*
 * Put custom build flags here:            *
 * vvv vvv vvv vvv vvv vvv vvv vvv vvv vvv */


    /* Uncomment the project you want to copile */    
    #if defined (ARDUINO)
        #include <Arduino.h>

        /* MANDATORY FOR ARDUINO: choose a unique board name */
        const char BOARD_NAME[] PROGMEM = "TEST";

//         #define INDOORINO_SAMPLER
        #define INDOORINO_ESPSERVER
//         #define INDOORINO_CAMERA
//         #define INDOORINO_PHONEDOOR

    #endif

    /* Uncomment this if SD card is attached */
    #define SD_MODULE

    /* Uncomment this if RTC clock is attached */
    #define RTC_MODULE
    
    /* Uncomment one or more of these for debugging */    
    #define DEBUG_GENERIC
    #define DEBUG_DEVICES
    #define DEBUG_MEMORY
    #define DEBUG_SERVER
//     #define DEBUG_IO

/* ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ *
 * INDOORINO_PYLIB is defined by setup.py  *
 *-----------------------------------------*/

#endif /* SOURCE_BOARDMACROS_H_ */
