/*
 * service-lights.h
 *
 *  Created 3.1.0 on: Aug 14, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_SERVICE_LIGHTS_H_
#define _SRC_SERVICE_LIGHTS_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"
#include "layout.h"
#include "services.h"
#include "timer.h"
// #include <deque>

#define SERVICEWAIT_ALARM_LOOP 60000
namespace indoorino
{
    namespace lgt
    {
        
        class LightService;
        
        class LightDevice
        {
        protected:
            
         
            class Timer : public indoorino::TimerTemplate
            {
                LightDevice * _mpointer=nullptr;

            public:
                
                Timer (LightDevice * p):TimerTemplate(p->device->boardname(),p->device->name()),_mpointer(p) {}; 
                
                void    on_start   (void) override  { _mpointer->abort_turnoff(); _mpointer->turn_on(); } 
                void    on_stop    (void) override  { _mpointer->turn_off(); }
                
                friend class indoorino::lgt::LightDevice;
                friend class indoorino::lgt::LightService;
            };

            
            NDO_DURATION                _turnoff_time;
            std::thread                 _th_turnoff;
            bool                        _flag_turnoff=false;
            
            std::vector<NDO_EVENTBOOL>  _signals;
            std::mutex                  _mtx;
            std::condition_variable     _cv;
            

            
        public:
            
            /* A pointer to System.board.device */
            indoorino::devs::DeviceTemplate  *  device;
            indoorino::lgt::LightDevice::Timer  timer;
            
           ~LightDevice();
            LightDevice(indoorino::devs::DeviceTemplate * p):device(p), timer(this) {}


        public:
            bool        state           (void)      { return bool(*device->status().p_level()); }
            void        turn_on         (void);
            void        turn_off        (void);
            bool        auto_turnoff    (uint32_t delay);
            uint32_t    auto_turnoff    (void);
            void        abort_turnoff   (void);
            
            const auto& events          (void) { return _signals; }
            
            friend class LightService;
        };

        
        
        
        class LightService : public svc::ServiceTemplate
        {
        protected:
            std::vector<std::shared_ptr<LightDevice>>  _devices;
        
        public:
            
             LightService(lyt::Service *);
            ~LightService();
            
            void        begin       (void);
            void        stop        (void);
            void        loop        (void);

            void        send_status (void);
            void        read_layout (void);
            void        on_update   (void);
            void        parse       (packet::ipacket *);
            
            bool        is_light    (void) { return true; }
            int         has_device  (const char *, const char *);
            const auto& devices     (void) { return _devices; }

        };


    } /* namespace:lgt */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_SERVICE_LIGHTS_H_ */
