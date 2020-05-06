/*
 * board_WeMos_miniD1R1.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_ARDUINO_BOARD_WEMOS_D1R1_H_
#define SOURCE_ARDUINO_BOARD_WEMOS_D1R1_H_
    
    #include "ESP8266WiFi.h"
    #include <WiFiUdp.h>
    
    const char BOARD_TYPE[] PROGMEM = "WEMOS";
    #pragma message "Compiling for WEMOS D1R1"

    #define EEPROM_MODULE
    #define RAMEND 32000
    extern WiFiUDP Udp;

//      ________________________________
//      |                               |
//      |       PIN MAP                 |
//      |_______________________________|
//
    /*
        D0 = GPIO16;
        D1 = GPIO5;
        D2 = GPIO4;
        D3 = GPIO0;
        D4 = GPIO2;
        D5 = GPIO14;
        D6 = GPIO12;
        D7 = GPIO13;
        D8 = GPIO15;
        D9 = GPIO3;
        D10 = GPIO1;
        LED_BUILTIN = GPIO16    
    */

//      ________________________________
//      |                               |
//      |       DEBUG TOOL              |
//      |_______________________________|
//

    #include <SoftwareSerial.h>
    #define DEBUG_PIN_RX D5
    #define DEBUG_PIN_TX D6
    
    /* see board_genericESP8266.h for details */
    extern SoftwareSerial  SerialDebug;
    #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)


#endif /* SOURCE_ARDUINO_BOARD_WEMOS_D1R1_H_ */
