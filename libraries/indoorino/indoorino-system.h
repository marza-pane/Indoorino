/*
 * system.h
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#pragma once

#ifndef _INDOORINO_SYSTEM_H_
#define _INDOORINO_SYSTEM_H_

/*
 * System.boards
 * System.
 * System.services
 * System.services.lights
 * System.services.alarms
 * 
 * 
 * 
 * indoorino:
 *      IndoorinoSystem
 *
 *      Boards
 *      BoardTemplate
 *
 *      Devices
 *      DeviceTemplate
 * 
 *      Timer
 *      TimerBase     
 * 
 *      devs:
 *          Relay
 *          DHT22
 *          GenericSwitch
 *          FloodSwitch
 *          RainSwitch
 *          DustPM25
 * 
 *      lyt:
 *          LayoutServiceKey
 *          LayoutDevKey
 *          LayoutKey
 *          HomeMap
 *          Service
 *          Layout
 * 
 *      svc:
 *          Services
 *          ServiceTemplate
 *          LightsTemplate
 * 
 *      alm:
 *          AlarmTemplate
 *          AlarmFlood
 */


#if defined (INDOORINO_SERVER)

#include "../common/utils.h"
#include "layout.h"
#include "services.h"
#include "devices.h"
// #include "probe.h"

// #include "database.h"

namespace indoorino 
{
    
    class IndoorinoSystem
    {

    public:
        
         IndoorinoSystem();
        ~IndoorinoSystem();
        
        indoorino::devs::Boards     boards;
        indoorino::lyt::Layout      layout;
        indoorino::svc::Services    services;
        

        devs::DeviceTemplate    *   get_device (const char *, const char *);

        
//         indoorino::grow::GrowRoom   growroom;
//         db::DataBase    database;
        
        
        void            begin           (void);
        void            parse           (packet::netpacket *);
        void            parse_request   (packet::netpacket *);
        
        int             is_service      (const char * s) { return services.exist(s); }
        int             is_board        (const char * s) { return boards.exist(s);   }

        bool            is_device       (const char *, const char *);
        
        void            on_exit         (void);
        bool            save_state      (void);
        bool            load_state      (void);
    };
    
} /* namespace:indoorino */

extern indoorino::IndoorinoSystem System;

#endif /* INDOORINO_SERVER */

#endif /* _INDOORINO_SYSTEM_H_ */
