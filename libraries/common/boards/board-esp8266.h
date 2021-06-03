/*
 * board_esp8266.h
 *
 *  Created on: 7 May, 2020
 *      Author: n00b
 */

#ifndef SOURCE_BOARD_ESP8266_H_
#define SOURCE_BOARD_ESP8266_H_

    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h>
    #include <SoftwareSerial.h>
    #include <Hash.h>
    
    #if !defined(ESP8266)
    #error "Invalid loading <board_esp8266.h> on non-ESP board!"
    #endif
    
    #if     defined(ARDUINO_ESP8266_GENERIC)

        //      ________________________________
        //      |                               |
        //      |       ESP8266 Generic         |
        //      |_______________________________|
        //    
    
        const char BOARD_TYPE[] PROGMEM = "ESP8266";
        #pragma message "Compiling for GENERIC ESP8266"

        #define EEPROM_MODULE
        #define EEPROM_SIZE 512
        #define RAMEND 32000

            /*
                PIN MAP:
                
                1: VDDA                 17: VDDPST
                2: LNA                  18: SDIO_DATA_2
                3: VDD3P3               19: SDIO_DATA_3
                4: VDD3P3               20: SDIO_CMD
                5: VDD_RTC              21: SDIO_CLK
                6: TOUT                 22: SDIO_DATA_0
                7: CHIP_EN              23: SDIO_DATA_1
                8: XPD_DCDC             24: GPIO5
                9: MTMS                 25: U0RXD
                10: MTDI                26: U0TXD
                11: VDDPST              27: XTAL_OUT
                12: MTCK                28: XTAL_IN
                13: MTDO                29: VDDD
                14: GPIO2               30: VDDA
                15: GPIO0               31: RES12K
                16: GPIO4               32: EXT_RSBT
            */

        #define DEBUG_PIN_RX 16 
        #define DEBUG_PIN_TX 24
        
        /* On ESP8266, if I try 
        * <static inline SoftwareSerial SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);>,
        * compiler returns <error: 'SerialDebug' declared as an 'inline' variable>.
        * Can't figure out why, if you find a solution, pls mail annsatsu@yandex.com.
        */
        
        extern SoftwareSerial  SerialDebug;
        #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)
    
    
    #elif   defined(ARDUINO_ESP8266_WEMOS_D1R1)
        
        //      ________________________________
        //      |                               |
        //      |       Wemos Mini              |
        //      |_______________________________|
        //
        
        const char BOARD_TYPE[] PROGMEM = "WEMOS.MINI";
        #pragma message "Compiling for WEMOS D1R1"

        #define EEPROM_MODULE
        #define EEPROM_SIZE 512
        #define RAMEND 32000

            /*
                PIN MAP:
                
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

        #define DEBUG_PIN_RX D5
        #define DEBUG_PIN_TX D6

        
        extern SoftwareSerial  SerialDebug;
        #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)
        
    
    #elif   defined(ARDUINO_ESP8266_WEMOS_D1MINI)     ||\
            defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO ) ||\
            defined (ARDUINO_ESP8266_WEMOS_D1MINILITE)

        //      ________________________________
        //      |                               |
        //      |       Wemos LoLin             |
        //      |_______________________________|
        //
                
        const char BOARD_TYPE[] PROGMEM = "WEMOS.LOLIN";
        #pragma message "Compiling for WEMOS LOLIN"

        #define EEPROM_MODULE
        #define EEPROM_SIZE 512
        #define RAMEND 32000

            /*
                PIN MAP:
                
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

        #define DEBUG_PIN_RX D5
        #define DEBUG_PIN_TX D6
        
        extern SoftwareSerial  SerialDebug;
        #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)
    
    #else
        
        #error "CANT FIND SELECTED ARDUINO BOARD"
    
    #endif /* ESP BOARD TYPES */

    #define SRAM_LIMIT 5000
    #define SRAM_FATAL 5000

//      _____________________________________________________________________
//      |                                                                   |
//      |       Network default parameters                                  |
//      |___________________________________________________________________|
//

    // _______                                                                      _______
    // |     |                                                                      |     |
    // |     | ip (dynamic): 192.168.X.Y                  ip (static): 192.168.1.42 |     |
    // | ESP |   <--------------------------------------------------------------->  | APP |   
    // |     | port: 7870 (DEFAULT_LOCAL_PORT)     port: 7890 (DEFAULT_REMOTE_PORT) |     |
    // |_____|                                                                      |_____|

    #ifndef PORT_SOCKET_FIRST
        #define PORT_SOCKET_FIRST 1023
    #endif

    #ifndef PORT_SOCKET_LAST
        #define PORT_SOCKET_LAST UINT16_MAX //65535
    #endif

    #ifndef INDOORINO_NETWORK
        #define INDOORINO_NETWORK
    #endif

#endif /* SOURCE_ARDUINO_BOARD_ESP8266_H_ */
