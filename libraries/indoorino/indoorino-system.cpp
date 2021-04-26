/*
 * system.h
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#include "indoorino-system.h"

namespace indoorino 
{

    void    system::begin           (void)
    {
        layout.reset();
        layout.check();
        layout.show();
        
        alarms.begin();
    }
       
    void    system::parse           (packet::netpacket * p)
    {
        if (p == nullptr || p->command() == 0)
        {
            error_os("SYSTEM: parsing invalid packet");
        }
        info_os("SYSTEM: parsing packet: %s ", p->description());
        
    }
       
    void    system::loop            (void)
    {
        for (iSize_t i=0; i<boards.size(); i++)
        {
            boards[i].loop();
        }
    }
    
    bool    system::save_state      (void)
    {
        return false;
    }

    bool    system::load_state      (void)
    {
        return false;
    }


    
    ////// FROM EHEHEHEHEEERRRRE!!!! adesso implementi le boards database! TODO TODO
    

} /* namespace:indoorino */
