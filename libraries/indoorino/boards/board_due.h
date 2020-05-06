/*
 * board_due.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_ARDUINO_BOARD_DUE_H_
#define SOURCE_ARDUINO_BOARD_DUE_H_

    const char BOARD_TYPE[] PROGMEM = "DUE";
    #pragma message "Compiling for ARDUINO DUE"
    #if ! defined(SD_MODULE)
        #warning "No EEPROM or SD for persistent configuration"
    #endif
    
    #define RAMEND 96000

//      ________________________________
//      |                               |
//      |       PROGMEM TOOLS           |
//      |_______________________________|
//
    #ifndef FPSTR
    #define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
    #endif

    #ifndef vsnprintf_P
    #define vsnprintf_P(...) vsnprintf(__VA_ARGS__)
    #endif

//      ________________________________
//      |                               |
//      |       RESET BOARD             |
//      |_______________________________|
//
    #include 		<malloc.h>
    #define         SYSRESETREQ             (1<<2)
    #define         VECTKEY                 (0x05fa0000UL)
    #define         VECTKEY_MASK            (0x0000ffffUL)
    #define         AIRCR                   (*(unsigned int*)0xe000ed0cUL)
    #define         REQUEST_RESET           (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)

//      ________________________________
//      |                               |
//      |       DEBUG TOOL              |
//      |_______________________________|
//
        
    #define SerialDebug Serial1
    #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)

#endif /* SOURCE_ARDUINO_BOARD_DUE_H_ */
