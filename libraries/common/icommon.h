/*
 * common/common.h
 *
 *  Created on: Dec 28, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_INDOORINO_COMMON_H_
#define SOURCE_INDOORINO_COMMON_H_

#include "utils.h"
#include "queue.h"
#include "list.h"
#include "../packets/ipacket.h"

#if defined(ARDUINO)

    #include "stddev.h"
    #include "boardio.h"
    #include "boardconf.h"

    #if defined(ESP8266)
        #include "netconfig.h"
        #include "espclient.h"
        extern NetworkConnection    network;
        extern IndoorinoEspClient   client;  
        
        #if defined(INDOORINO_ROUTER)
            extern ConfRouter           conf;
        #elif defined(INDOORINO_CONTROLLER)
            extern ConfEspController    conf; ???
        #endif
        
    #else
        #if defined(INDOORINO_ROUTER)
            #error "INDOORINO_ROUTER can only be flashed onto ESP8266 chips"
        #elif defined(INDOORINO_CONTROLLER)
            extern ConfController       conf;
        #endif    
    #endif
    

    #if defined(INDOORINO_DEVS)
        #include "devices.h"
        #include "sensors.h"
        extern IndoorinoDeviceList  devices;
    #endif
        
#else  /* __linux__ */

    #include "../network/netconfig.h"
    #if defined(INDOORINO_SERVER)
        #include "../network/server.h"
        #include "../indoorino/paths.h"
        #include "../indoorino/schedule.h"
    #elif defined(INDOORINO_CLIENT)
        #include "../network/connection.h"
    #endif
        
#endif /* ARDUINO */
        
#endif /* SOURCE_INDOORINO_COMMON_H_ */
