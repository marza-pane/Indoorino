/*
 * board_mega2560.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_ARDUINO_BOARD_MEGA2560_H_
#define SOURCE_ARDUINO_BOARD_MEGA2560_H_

    const char BOARD_TYPE[] PROGMEM = "MEGA2560";
    #define EEPROM_MODULE
    #pragma message "Compiling for ARDUINO MEGA2560"
    #define SRAM_LIMIT 800
    #define SRAM_FATAL 200
    extern char * __brkval;

    #ifndef FPSTR
    #define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
    #endif

//      ________________________________
//      |                               |
//      |       RESET BOARD             |
//      |_______________________________|
// 

    #include <avr/wdt.h>
    void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

//      ________________________________
//      |                               |
//      |       DEBUG TOOL              |
//      |_______________________________|
//

    #define SerialDebug Serial1
    #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)

#endif /* SOURCE_ARDUINO_BOARD_MEGA2560_H_ */
