/*
 * esp8266Std.h
 *
 *  Created on: Oct 23, 2019
 *      Author: n00b
 */

#ifndef ESP8266STD_H_
#define ESP8266STD_H_

#include <Arduino.h>

#if defined (ARDUINO_AVR_UNO) || defined (ARDUINO_AVR_MEGA2560) || defined (ARDUINO_SAM_DUE)
    #error "ESPSERVER can only be uploaded to ESP devices"
#endif
               
const char INDOORINO_TYPE[] PROGMEM = "ESPSERVER";

#define DEFAULT_DEVNUM 0

// const char DEFAULT_TARGET_NAME[] PROGMEM = "IP0";
const char DEFAULT_SSID[]        PROGMEM = "FoxWiFi";
const char DEFAULT_PSK[]         PROGMEM = "L4nT4n1D1=14";

// #define RESEND_TIMEOUT       500
#define DEFAULT_RETRY_NUM       20
#define DEFAULT_PACKET_TIMEOUT  1000

#define MAX_RETRY_NUM           100
#define MAX_PACKET_TIMEOUT      30000  // this should be from desktop app DEFAULT_ITERATION_TIMESTEP 30000
#define RECONNECT_TIMEOUT       20000L
#define AUTH_TIMEOUT            60000L

#endif /* ESP8266STD_H_ */
