/*
 * samplerStd.h
 *
 *  Created on: Oct 23, 2019
 *      Author: n00b
 */

#ifndef SAMPLERSTD_H_
#define SAMPLERSTD_H_

#if defined (ESP8266)
    #error "SAMPLER can not be uploaded to ESP8266"
#endif

#include <Arduino.h>

const char INDOORINO_TYPE[] PROGMEM = "SAMPLER";

#if ! defined(SD_MODULE)
    #define SD_MODULE
    #warning "No SD_MODULE detected. Please attach SD card device for project SAMPLER"
#endif

#if ! defined(RTC_MODULE)
    #define RTC_MODULE
    #warning "No rtc_MODULE detected. Please attach rtc CLOCK device for project SAMPLER"
#endif

#define DT_DEF_STEP 300
#define DT_MIN_STEP 5
#define DT_MAX_STEP SECONDS_PER_DAY

#define DT_DEF_COOL 40
#define DT_MIN_COOL 1
#define DT_MAX_COOL 120

#define DEFAULT_DEVNUM      5

const default_dev_conf_template DEFAULT_DEVCONF[DEFAULT_DEVNUM] PROGMEM = 
{
    "DHT1", 5, 1522,
    "DHT2", 6, 1522,
    "DHT3", 7, 1522,
    "FAN1", 8, 1700,
    "FAN2", 9, 1700
};

#endif /* SAMPLERSTD_H_ */


//   for (int i = 0; i < DEFAULT_DEVNUM; i++)
//     {
//     devNameHelper oneItem;
//     memcpy_P (&oneItem, &DEFAULT_DEV_NAME [i], sizeof oneItem);
//     
//     Serial.println (oneItem.name);
//     }   // end of for loop
// 
//   }  // end of setup
 
