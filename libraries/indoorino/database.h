/*
 * database.h
 *
 *  Created 3.1.0 on: May 8, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_DATABASE_H_
#define _SRC_DATABASE_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"
#include "layout.h"
#include <deque>

namespace indoorino
{
    namespace db
    {
        /** NON IMPLEMENTED YET. PLEASE DO (ALMOST) ALL **/
        
        struct EntryTemplate
        {
            char        boardname[LEN_NAME] {0};
            char        devname[LEN_DEVNAME] {0};
            char        message[SERIAL_TX_BUFFER_SIZE] {0};
            iEpoch_t    epoch;
            int         value;
        };
        
        struct DeviceTemplate
        {
            DeviceTemplate() {}
            DeviceTemplate(const char *, const char *);
            ~DeviceTemplate() { data.clear(); }
            
            char        boardname[LEN_NAME] {0};
            char        devname[LEN_DEVNAME] {0};
            std::vector<EntryTemplate> data;
        };
        
        class DataBase
        {
        private:
            std::vector<DeviceTemplate>     _db;
        public:
            DataBase() {}
            ~DataBase() { _db.clear(); }
            
            void        parse       (packet::netpacket *);
            bool        is_board    (const char *);
            bool        is_device   (const char *);
            
            std::vector<DeviceTemplate>& db() { return _db; }
        };

        
    } /* namespace:db */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_DATABASE_H_ */
