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

#define SERVICEWAIT_ALARM_OFF_LOOP  60000
#define SERVICEWAIT_ALARM_ON_LOOP   1000
#define SERVICE_SEND_ALARM_RATE     10000

namespace indoorino
{
    namespace alm
    {

        class AlarmService : public svc::ServiceTemplate
        {
            
        public:
            
            class AlarmDevice
            {
            protected:
                
                bool        _enabled  =false;
                uint32_t    _value    {0};
                
                std::vector<packet::ipacket>    _signals;
                
            protected:
                AlarmDevice(indoorino::devs::DeviceTemplate * p):device(p) {};
                
            public:
               ~AlarmDevice() {};

            public:
                bool        enabled         (void) { return _enabled;  }
                uint32_t    on_alarm        (void) { return _value;    }
                const auto& rxpackets       (void) { return _signals;  }
                
                indoorino::devs::DeviceTemplate * device;
                friend class alm::AlarmService;
                
            };

            
            
//             JUST A REMINDER
//         protected:            
//             std::thread _thread;
//             bool        _running=false;
//             bool        _update=false;
// 
//         protected:
//             lyt::Service               *    _layout;
//             std::mutex                      _mtx;
//             std::condition_variable         _cv;
            
        protected:
            
            std::vector<AlarmDevice>  _devices;
            std::chrono::system_clock::time_point _last_alarm;
            void    send_alarm  (uint32_t *);
        
        public:
            
             AlarmService(lyt::Service *);
            ~AlarmService();
            
            void    begin       (void);
            void    stop        (void);
            void    loop        (void);

            void    read_layout (void);
            void    on_update   (void);
            void    parse       (packet::ipacket *);

            void    send_status (void);
            uint8_t on_alarm    (void);

        public:
            
            bool            is_alarm    (void)  { return true; }
            int             has_device  (const char *, const char *);
            const auto&     devices     (void)  { return _devices; };

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
