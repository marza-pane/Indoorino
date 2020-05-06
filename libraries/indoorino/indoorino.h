/*
 * indoorino.h
 *
 *  Created on: Oct 18, 2019
 *      Author: n00b
 * 
 * 
 *  PROGRAMS DATA USAGE TABLE (Bytes)
 * 
 *  Arduino     :           :   9
 *  RTlib.h     :include    : 107
 *  time.h      :include    : 142
 *  Serial      :begin      : 179
 *  _va_debug   :buffer     :  64
 *  F2C         :buffer     :  16
 *  prinTime    :buffer     :  22
*/

#ifndef SOURCE_INDOORINO_H_
#define SOURCE_INDOORINO_H_

/*      INDOORINO - a simple intro:
 * 
 * Indoorino is way to remotly control AVR and small ARM boards (Arduino generally) from a PC with a 
 * Python desktop application. This source code is used by all Indoorino projects and for building a Python 
 * module needed by the desktop application
 * 
 * Data can be sent via serial or via UDP (ESP8266). Ususally an Arduino board talk via serial 
 * to an ESP8266 that act like a router. Data is build up as packets. All packets have the following layout
 * 
 * |PREAMBLE|NAME|1|COMMAND|PAYLOAD|TRAILING ZEROS|
 * 
 * PREAMBLE : unique sequence of byte that set teh packet reader on. Default is 255,0,0,255
 * 
 * NAME: fixed size (LEN_NAME) name of target device (see packetParse.h)
 * 
 * COMMAND: 16 bit unsigned int that specify the type of packet
 * 
 * PAYLOAD: variable number of bytes carried by the packet
 *
 * TRAILING ZEROS: needed at least one 0 to terminate packet transmission
 * 
 * comtable.h       :all packet definitions are here
 * definitions.h    :holds all most important macros
 * utils.h          :keeps general and debug functions
 * 
 * 
 *  TODO:   C/C++: python module will be a board manager written in C/C++.
 *          write a C/C++ module that will autoatically recieve and send WiFiUdp
 *          and a function connect_serial(const char * serial_path) to add serials.
 *          See BoardManager in Python project and make it with C
 * 
 *  TODO:   Python: make the codeGenerator parse the code before writing evals.cpp
 *  TODO:   Python: 
 */

#include "packetUtils.h"

#if defined (ARDUINO)

    #if defined (INDOORINO_SAMPLER)

        #include "stddev.h"
    //         #include "indoorinoConfig.h"
    //         #include "devices.h"
    //         #include "sensors.h"
    //         #include "sdCard.h"
        
        extern  blinkingLed     blinker;
        extern  ClockDS3231     rtc; 
        extern  BoardIO         boardio;
    //         extern  ConfSampler     conf;        
    //         extern  deviceList      devices;      
    //         extern  sdProbe         sd;



    #elif defined (INDOORINO_PHONEDOOR)

        #include "stddev.h"
    //         #include "indoorinoConfig.h"
    //         #include "devices.h"
    //         #include "sensors.h"
        
        extern  blinkingLed     blinker;
        extern  millisClock     rtc; 
        extern  BoardIO         boardio;
    //         extern  ConfBase        conf;        

    #elif defined (INDOORINO_ESP)

    //         #include "stddev.h"
    //         #include "indoorinoConfig.h"
    //         #include "espServerUtils.h"
    //         extern  blinkingLed     blinker;
    //         extern  millisClock     rtc; 
    //         extern  packetRouter    boardio;
    //         extern  ConfEspServer   conf;
    //         extern  AddressList     addresslist;
    //         extern  connectionLoop  connection;
        

    #elif defined (INDOORINO_CAMERA)

    #else
    
    #endif /* PROJECTS */

        
#elif defined (INDOORINO_PYLIB)
        
    extern  packetParse boardio;
    
#elif defined (RAWRUN)

#endif /* ARDUINO */


#endif /* SOURCE_INDOORINO_H_ */

        
/*
 * 

Question is whether we can somehow write the following expression without both then and else parts

    (integer == 5) ? (THENEXPR) : (ELSEEXPR);

If you only need the then part you can use &&:

    (integer == 5) && (THENEXPR)

If you only need the else part use ||:

    (integer == 5) || (ELSEEXPR)

 *
 */
