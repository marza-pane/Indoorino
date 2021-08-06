/*
 * alarms.h
 *
 *  Created 3.1.0 on: Apr 21, 2021 (SERVER)
 *  Edited  3.1.0 on: Aug 02, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_ALARMS_H_
#define _SRC_ALARMS_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"
#include "layout.h"
#include "services.h"
// #include <deque>

namespace indoorino
{
    namespace alm
    {
        
//         class AlarmDevice
//         {
//         protected:
//             const lyt::LayoutKey& layout;
//         protected:
//             DeviceTemplate * dev;
//             bool    valid = false;
//             bool    enabled = false;
//             bool    on_alarm = false;
//             bool    notified = false;
// 
//             std::vector<packet::netpacket>  _signals;
//             
//         public:
//             AlarmDevices(lyt::LayoutKey*)
//             
//             bool    is_enabled() { return _enabled;  }
//             bool    is_onalarm() { return _on_alarm; }
//             
//             const std::vector<packet::netpacket>& rxpackets () { return _signals; }
//             
//             friend  class       Alarms;
//             
//             
//             
//         }

        
        
        class AlarmTemplate : public svc::ServiceTemplate
        {
            // std::vector<lyt::LayoutKey>     _devices;
    
        protected:
            void    loop    (void);
        
        public:
            
             AlarmTemplate(const lyt::LayoutServiceKey&);
            ~AlarmTemplate();
            
            void    begin   (void);
            void    stop    (void);
            
        };        
    
    } /* namespace:alm */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_ALARMS_H_ */

























//         class AlarmDevice
//         {
//         private:
//             bool                    _enabled    =false;
//             uint8_t                 _on_alarm   =false;
//         
 
//             
//         public:
//             AlarmDevice(const layout::dev_alarm_t &);
//             ~AlarmDevice() {}
//             void    parse           (packet::netpacket *);
//             void    send_updates    (void);
//             
//             bool    is_enabled() { return _enabled;  }
//             bool    is_onalarm() { return _on_alarm; }
//             
//             const std::vector<packet::netpacket>& rxpackets () { return _signals; }
//             const std::vector<std::chrono::system_clock::time_point>& acknowledges () { return _alarm_ack; }
//             
//             friend  class       Alarms;
//         };
// 
//         class Alarms : public layout::LinkedDevList<AlarmDevice>
//         {
//         public:
// 
//              Alarms()   {};
//             ~Alarms()   {};
//             
//             void    begin       (void);
//             void    load_layout (void);
//             void    send_status (void);
//             void    parse       (packet::netpacket *);
//             
//         };
