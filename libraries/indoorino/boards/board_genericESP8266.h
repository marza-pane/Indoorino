/*
 * board_genericESP8266.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_ARDUINO_BOARD_GENERICESP8266_H_
#define SOURCE_ARDUINO_BOARD_GENERICESP8266_H_

    #include "ESP8266WiFi.h"
    #include <WiFiUdp.h>
    
    const char BOARD_TYPE[] PROGMEM = "ESP8266";
    #pragma message "Compiling for GENERIC ESP8266"

    #define EEPROM_MODULE
    #define RAMEND 32000
    extern WiFiUDP Udp;

//      ________________________________
//      |                               |
//      |       PIN MAP                 |
//      |_______________________________|
//    
    /*
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

//      ________________________________
//      |                               |
//      |       DEBUG TOOL              |
//      |_______________________________|
//

    #include <SoftwareSerial.h>
    #define DEBUG_PIN_RX 16 
    #define DEBUG_PIN_TX 24
    
    /* On ESP8266, if I try 
     * <static inline SoftwareSerial SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);>,
     * compiler returns <error: 'SerialDebug' declared as an 'inline' variable>.
     * Can't figure out why, if you find a solution, pls mail annsatsu@yandex.com.
     */
    
    extern SoftwareSerial  SerialDebug;
    #define SerialDebugPrint(S,...) SerialDebug.print(S, ## __VA_ARGS__)

#endif /* SOURCE_ARDUINO_BOARD_GENERICESP8266_H_ */

    
    
    
    
    
    
    
/***
    Pin layout of 32-pin QFN Package.

    Pin 1: VDDA is a power pin for analog power ranges from 2.5V to 3.6V.
    Pin 2: LNA is an input/output pin specifically used for RF antenna interface. Chip produces impedance of 39+j6 Ω.
    Pin 3: VDD3P3 is a power pin to provide amplifier power ranges from 2.5V to 3.6V.
    Pin 4: VDD3P3 is a power pin provide amplifier power ranges from 2.5V to 3.6V similar to pin3.
    Pin 5: VDD_RTC is categorized as a power pin and supply 1.1V but this pin is not connected.
    Pin 6: TOUT is an input pin functions as ADC pin to test the supply voltages of Pin3 and Pin4 and the input voltages of TOUT pin6. These two functions cannot perform simultaneously.
    Pin 7: CHIP_EN I an input pin. When CHIP_EN pin is HIGH chip works properly when LOW chip consumed an only small amount of current.
    Pin 8: XPD_DCDC is an input/output pin which is used to wake up the chip from deep sleep mode. Commonly it is connected with GPIO16.
    Pin 9: MTMS is an input/output pin labeled as GPIO14 and it is used in SPI as clock pin (SPI_CLK).
    Pin 10: MTDI is an input/output pin labeled as GPIO12 and it is used in SPI as Master-In-Slave-Out pin (SPI_MISO).
    Pin 11: VDDPST is a power pin. It is a digital input/output power supply whose voltages ranges from 1.8V to 3.6V. Similar to pin17.
    Pin 12: MTCK is an input/output pin labeled as GPIO13 and it is used in SPI as Master-Out Slave-In pin (SPI_MOSI) as well as used in UART as Clear To Send pin (UART_CTS).
    Pin 13: MTDO is an input/output pin labeled as GPIO15 and it is used in SPI as Chip Select pin (SPI_CS) as well as used in UART as Request To Send pin (UART_RTS).
    Pin 14: GPIO2 is an input/output pin used as UART TX during flash programming.
    Pin 15: GPIO0 is an input/output used as Chip Select pin2 in SPI (SPI_CS2).
    Pin 16: GPIO4 is an input/output pin purely used for input and output purposes.
    Pin 17: VDDPST is a power pin. It is a digital input/output power supply whose voltages ranges from 1.8V to 3.6V. Similar to pin11.
    Pin 18: SDIO_DATA_2 is an input/output pin labeled as GPIO9 and used to connect with data pin 2 of SD card.
    Pin 19: SDIO_DATA_3 is an input/output pin labeled as GPIO10 and used to connect with data pin 3 of SD card.
    Pin 20: SDIO_CMD is an input/output pin labeled as GPIO11 and used to connect with command pin of SD card
    Pin 21: SDIO_CLK is an input/output pin labeled as GPIO6 and used to connect with the clock pin of SD card.
    Pin 22: SDIO_DATA_0 is an input/output pin labeled as GPIO7 and used to connect with data pin 0 of SD card.
    Pin 23: SDIO_DATA_1 is an input/output pin labeled as GPIO8 and used to connect with data pin 1 of SD card.
    Pin 24: GPIO5 is an input/output pin purely used for input and output purposes.
    Pin 25: U0RXD is an input/output pin labeled as GPIO3 and used as UART RX during flash programming.
    Pin 26: U0TXD is an input/output pin labeled as GPIO1 and used as UART TX during flash programming. Also used as SPI Chip Select pin 1 (SPI_CS1).
    Pin 27: XTAL_OUT is classified as an input/output pin and connected to the output of the crystal oscillator.
    Pin 28: XTAL_IN is classified as an input/output pin and connected to the input of the crystal oscillator.
    Pin 29: VDDD is a power pin provide analog power ranges from 2.5V to 3.6V.
    Pin 30: VDDA is a power pin provide analog power ranges from 2.5V to 3.6V. Similar to pin29.
    Pin 31: RES12K is an input pin which is serial connected with 12 kΩ resistors and connected to the ground.
    Pin 32: EXT_RSBT is an input pin used to rest the chip by providing an external reset signal which is active at a low voltage level.
    Pin 33: GND is a power pin acts as a ground for the chip.
***/
