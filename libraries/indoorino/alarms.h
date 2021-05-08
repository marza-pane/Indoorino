/*
 * alarms.h
 *
 *  Created 3.1.0 on: Apr 21, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_ALARMS_H_
#define _SRC_ALARMS_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"
#include "layout.h"
// #include <deque>

namespace indoorino
{
    namespace alarms
    {
        class AlarmDevice
        {
        private:
            bool                    _enabled    =false;
            bool                    _on_alarm   =false;
        
        protected:
            std::vector<std::chrono::system_clock::time_point> _alarm_ack;
            std::vector<packet::netpacket>  _signals;
            layout::dev_alarm_t &           _layout;
            
        public:
            AlarmDevice(layout::dev_alarm_t &);
            ~AlarmDevice() {}
            void    parse           (packet::netpacket *);
            void    send_updates    (void);
            
            bool    is_enabled() { return _enabled;  }
            bool    is_onalarm() { return _on_alarm; }
            
            const std::vector<packet::netpacket>& rxpackets () { return _signals; }
            const std::vector<std::chrono::system_clock::time_point>& acknowledges () { return _alarm_ack; }
            
            friend  class       Alarms;
        };

        class Alarms : public layout::LinkedDevList<AlarmDevice>
        {
        public:

             Alarms()   {};
            ~Alarms()   {};
            
            void    begin       (void);
            void    load_layout (void);
            void    send_status (void);
            void    parse       (packet::netpacket *);
            
        };
    
    } /* namespace:alarms */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_ALARMS_H_ */
