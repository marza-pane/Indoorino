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
    
    class system
    {
    public:
        
         system()       {};
        ~system()       { boards.clear(); }
        
        layout::Layout  layout;
        
        BoardList       boards;
        AlarmList       alarms;
        
        
        void            begin           (void);
        void            loop            (void);
        void            parse           (packet::netpacket *);
        
        bool            save_state      (void);
        bool            load_state      (void);
    };
    
} /* namespace:indoorino */

extern indoorino::system System;

#endif /* INDOORINO_SERVER */

#endif /* _INDOORINO_SYSTEM_H_ */
