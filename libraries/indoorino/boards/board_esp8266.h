/*
 * board_ESP8266.h
 *
 *  Created on: 7 May, 2020
 *      Author: n00b
 */

#ifndef SOURCE_ARDUINO_BOARD_ESP8266_H_
#define SOURCE_ARDUINO_BOARD_ESP8266_H_

    #include <SoftwareSerial.h>
    #include <ESP8266WiFi.h>
    #include <WiFiUdp.h>
    
    #ifndef PORT_SOCKET_FIRST
        #define PORT_SOCKET_FIRST 1023
    #endif

    #ifndef PORT_SOCKET_LAST
        #define PORT_SOCKET_LAST UINT16_MAX //65535
    #endif

    #ifndef UDP_TX_PACKET_MAX_SIZE
    #define UDP_TX_PACKET_MAX_SIZE 8192
    #endif
    #define DEFAULT_UTC 1
    
    extern WiFiUDP Udp;

#endif /* SOURCE_ARDUINO_BOARD_ESP8266_H_ */
