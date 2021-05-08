/*
 * netconfig.cpp
 *
 *  Created on: Mar 30, 2021
 *      Author: n00b
 */

#pragma once
#ifndef SOURCE_NETCONFIG_H_
#define SOURCE_NETCONFIG_H_
#if defined(INDOORINO_NETWORK)

/** This is the default AES 128 bit key. 
 ** Feel free to change it but remember to flash all your devices and recompile server and Python module.
 ** See <future reference> **/

const uint8_t AES_INIT_MK[N_BLOCK]  PROGMEM = { 0x65 ,0x6e ,0x63 ,0x72 ,0x74 ,0x79 ,0x70 ,0x74 ,0x2d ,0x2d ,0x6d ,0x61 ,0x73 ,0x74 ,0x65 ,0x72 };

/** Your local SSID (Wifi name) and password **/

// const char DEFAULT_SSID[]           PROGMEM = "Your-WiFi-SSID";
// const char DEFAULT_PSK[]            PROGMEM = "Your-WiFi-passphrase";

#endif /* INDOORINO_NETWORK */
#endif /* SOURCE_NETCONFIG_H_ */
