/*
 * database.cpp
 *
 *  Created 3.1.0 on: May 8, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#if defined (INDOORINO_NETWORK)

#include "database.h"

namespace indoorino
{
    namespace db
    {

        /** NON IMPLEMENTED YET. PLEASE DO (ALMOST) ALL **/
        
        bool        DataBase::is_board      (const char * name)
        {
            for (auto &d : _db)
            {
                if ( strcmp(d.boardname, name) == 0 ) return true;
            }
            return false;
        }
        
        bool        DataBase::is_device     (const char * name)
        {
            for (auto &d : _db)
            {
                if ( strcmp(d.devname, name) == 0) return true;
            }
            return false;
        }
        
        void        DataBase::parse         (packet::netpacket * p)
        {
            
//             if (p->command() == IBACOM_BOARD_BOOT)
//             {
//                 if (!is_board(p.p_name()))
//                 {
//                     _db.push_back(DeviceTemplate(p.p_name, "");
//                     
//                 }
//             }
            
        }
        
    } /* namespace:db */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */
