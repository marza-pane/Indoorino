/*
 * system.h
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#pragma once

#ifndef _INDOORINO_SYSTEM_H_
#define _INDOORINO_SYSTEM_H_

#if defined (INDOORINO_SERVER)

#include "../common/utils.h"
#include "paths.h"
#include "boards.h"
#include "layout.h"
#include "alarms.h"

namespace indoorino 
{
    
    class IndoorinoSystem
    {
    public:
        
         IndoorinoSystem(){};
        ~IndoorinoSystem();
        
        layout::Layout  layout;
        alarms::Alarms  alarms;
        
        BoardList       boards;
        
        
        void            begin           (void);
        void            loop            (void);
        void            parse           (packet::netpacket *);
        void            parse_request   (const char *);
        
        bool            save_state      (void);
        bool            load_state      (void);
    };
    
} /* namespace:indoorino */

extern indoorino::IndoorinoSystem System;

#endif /* INDOORINO_SERVER */

#endif /* _INDOORINO_SYSTEM_H_ */
