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
 */
#if defined (INDOORINO_SERVER)

#include "../common/utils.h"
#include "paths.h"
#include "boards.h"
#include "layout.h"
#include "services.h"
// #include "database.h"

namespace indoorino 
{
    
    class IndoorinoSystem
    {

    public:
        
         IndoorinoSystem();
        ~IndoorinoSystem();
        
        indoorino::Boards           boards;
        indoorino::lyt::Layout      layout;
        indoorino::svc::Services    services;
        
//         alarms::Alarms  alarms;
//         db::DataBase    database;
        
        
        
        void            begin           (void);
        void            loop            (void);
        void            parse           (packet::netpacket *) {};
        void            parse_request   (packet::netpacket *);
        
        void            on_exit         (void);
        bool            save_state      (void);
        bool            load_state      (void);
    };
    
} /* namespace:indoorino */

extern indoorino::IndoorinoSystem System;

#endif /* INDOORINO_SERVER */

#endif /* _INDOORINO_SYSTEM_H_ */
