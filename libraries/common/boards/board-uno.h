/*
 * board_uno.h
 *
 *  Created on: Apr 20, 2020 0w0
 *      Author: n00b
 */

#ifndef SOURCE_ARDUINO_BOARD_UNO_H_
#define SOURCE_ARDUINO_BOARD_UNO_H_
        
    const char BOARD_TYPE[] PROGMEM = "UNO";
    #pragma message "Compiling for ARDUINO UNO"
    #define EEPROM_MODULE
    #define SRAM_LIMIT RAMEND - 512
    #define SRAM_FATAL RAMEND - 128
    extern char * __brkval;

//      ________________________________
//      |                               |
//      |       DEBUG TOOL              |
//      |_______________________________|
//

    #include <SoftwareSerial.h>
    #define DEBUG_PIN_RX 2
    #define DEBUG_PIN_TX 3

    #ifndef FPSTR
    #define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
    #endif
    
    extern SoftwareSerial SerialDebug;
//     inline SoftwareSerial SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false); 
    #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)

#endif /* SOURCE_ARDUINO_BOARD_UNO_H_ */
