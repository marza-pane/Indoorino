/*
 * service-lights.cpp
 *
 *  Created 3.1.0 on: Aug 14, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#if defined (INDOORINO_NETWORK)

#include "service-lights.h"
#include "indoorino-system.h"
#include "../network/server.h"

namespace indoorino
{
    namespace lgt
    {
        
//      _________________________________________
//      |                                       |
//      |       Light Service Device            |
//      |_______________________________________|
//

        LightDevice::~LightDevice()
        {
            abort_turnoff();
            timer.clear();
        }

        void        LightDevice::turn_on        (void)
        {
            alert_os("LIGHTS:Turning %s:%s <ON>", device->boardname(), device->name());
            packet::ipacket p(IBACOM_SET_DEVICE);
            
            uint32_t value=1;
            
            strcpy(p.p_name(), device->boardname());
            strcpy(p.p_devname(), device->name());
            strcpy(p.p_devname(), "SET");
            memcpy(p.p_value1(), &value, sizeof(uint32_t));
            
            Server.shell.broadcast(&p);

        }
            
        void        LightDevice::turn_off       (void)
        {
            info_os("LIGHTS:Turning %s:%s <OFF>", device->boardname(), device->name());
            packet::ipacket p(IBACOM_SET_DEVICE);
            
            uint32_t value=0;
            
            strcpy(p.p_name(), device->boardname());
            strcpy(p.p_devname(), device->name());
            strcpy(p.p_devname(), "SET");
            memcpy(p.p_value1(), &value, sizeof(uint32_t));
            
            Server.board.broadcast(&p);
            
        }
        
        uint32_t    LightDevice::auto_turnoff   (void)
        {
            if (_flag_turnoff)
                return std::chrono::duration_cast<std::chrono::seconds>((_turnoff_time.first + _turnoff_time.second) - std::chrono::system_clock::now()).count();
            return 0;
        }
                        
        bool        LightDevice::auto_turnoff   (uint32_t delay)
        {
            
            if (_flag_turnoff)
                abort_turnoff();
            
            _flag_turnoff = true;
            
            alert_os("LIGHTS:Auto Turn-off for %s:%s in %u sec.", device->boardname(), device->name(), delay);
            
            _turnoff_time.first  = std::chrono::system_clock::now();
            _turnoff_time.second = std::chrono::seconds(delay);
            
            _th_turnoff = std::thread( [this, delay]  {
                
                std::unique_lock<std::mutex> lck(_mtx);
                _cv.wait_for(lck, std::chrono::seconds(delay));
                alert_os("LIGHTS:Auto Turn-off %s:%s broke", device->boardname(), device->name());
                if (_flag_turnoff) turn_off();
            });

            return true;
        }
        
        void        LightDevice::abort_turnoff  (void)
        {
            
            std::unique_lock<std::mutex> lck(_mtx);
            if (_flag_turnoff)
            {
                _flag_turnoff = false;
                alert_os("LIGHTS:Aborting Auto Turn-off for %s:%s", device->boardname(), device->name());
            }
            _cv.notify_all();
            if (_th_turnoff.joinable())
            {
                alert_os("LIGHTS:Aborting Auto Turn-off thread for %s:%s termianted", device->boardname(), device->name());
                _th_turnoff.join();
            }
        }
        
//      _________________________________________
//      |                                       |
//      |       Light Service implementation    |
//      |_______________________________________|
//

        LightService::LightService(lyt::Service * s):svc::ServiceTemplate(s)
        {
             
        }
        
        LightService::~LightService()
        {
            this->stop();
            _devices.clear();
        }
    
        void        LightService::begin                (void)
        {
            alert_os("SERVICE:LIGHTS:begin %s @ [%s:%s]",_layout->name(), _layout->area(), _layout->location());
            svc::ServiceTemplate::begin();
        }

        void        LightService::loop                 (void)
        {
            svc::ServiceTemplate::loop();
        }
        
        void        LightService::stop                 (void)
        {
            for (auto dev: _devices)
            {
                dev->abort_turnoff();
                dev->timer.clear();
            }
            svc::ServiceTemplate::stop();
        }

        void        LightService::parse                 (packet::ipacket * p)
        {

            if ( p->command() == IBACOM_SET_DEVICE )
            {
                int index = has_device(p->p_name(), p->p_devname());
                
                if ( index != -1 )
                {
                    // a set device is passing through
                }
            }
            
            else if ( p->command() == IBACOM_LGT_AUTO_OFF )
            {
                int index = has_device(p->p_board(), p->p_devname());
                
                if ( index != -1 )
                {
                    if (strcmp(p->p_command(), "SET") == 0)
                    {
                        uint32_t secs = *p->p_value1();
                        _devices.at(index)->auto_turnoff(secs);
                    }
                    
                    if (strcmp(p->p_command(), "ABORT") == 0)
                    {
                        _devices.at(index)->abort_turnoff();
                    }
                    
                    this->on_update();
                }
                
            }
            else if ( p->command() == IBACOM_LGT_TIMER_SET )
            {
                int index = has_device(p->p_board(), p->p_devname());
                
                if ( index != -1 )
                {

                    if (strcmp(p->p_command(), "ADD") == 0)
                    {
                        uint32_t secs = *p->p_value1();
                        std::time_t epoch = *p->p_epoch();                        
                        
                        _devices.at(index)->timer.add(
                            std::chrono::system_clock::from_time_t(epoch),
                            std::chrono::seconds(secs));
                    }
                    
                    if (strcmp(p->p_command(), "CLEAR") == 0)
                    {
                        _devices.at(index)->timer.clear();
                    }
                }
            }
        }

        int         LightService::has_device            (const char * bname, const char * dname)
        {
            int i=0;
            for (auto dev: _devices)
            {
                if (strcmp(dev->device->boardname(), bname) == 0 && strcmp(dev->device->name(), dname) == 0) return i;
                i++;
            }
            return -1;
        }

        void        LightService::on_update            (void)
        {
            this->send_status();
            _cv.notify_all();
        }

        void        LightService::send_status          (void)
        {
            for (auto dev: _devices)
            {
                alert_os("LIGHTS:%s: sending updates for dev %s:%s",_layout->name(), dev->device->boardname(), dev->device->name());
                
                uint32_t value = uint32_t(dev->auto_turnoff());
                uint8_t enabled = uint8_t(dev->timer.is_enabled());
                uint8_t on = uint8_t(dev->timer.is_on());
                uint32_t ntimers = uint8_t(dev->timer._eventable.size());
//                                 
                packet::ipacket p(IBACOM_LGT_AUTO_OFF);
                
                strcpy(p.p_board(),   dev->device->boardname());
                strcpy(p.p_devname(), dev->device->name());
                strcpy(p.p_desc1(),   this->name());
                memcpy(p.p_value1(), &value, sizeof(uint32_t));

                Server.shell.broadcast(&p);             

                p.init(IBACOM_LGT_TIMER_STAT);
                
                strcpy(p.p_board(),   dev->device->boardname());
                strcpy(p.p_devname(), dev->device->name());
                strcpy(p.p_desc1(),   this->name());
                memcpy(p.p_status(),  &enabled, sizeof(uint8_t));
                memcpy(p.p_level(),   &on, sizeof(uint8_t));
                memcpy(p.p_value1(),  &ntimers, sizeof(uint32_t));
//                 
                Server.shell.broadcast(&p);             

                p.init(IBACOM_LGT_TIMER_ENTRY);
                strcpy(p.p_board(),   dev->device->boardname());
                strcpy(p.p_devname(), dev->device->name());
                strcpy(p.p_desc1(),   this->name());
                
                uint8_t index = 0;
                for (auto e: dev->timer._eventable)
                {
                    uint32_t epoch = utils::chrono2unixepoch(e.first);
                    uint32_t duration = e.second.count();
                    memcpy(p.p_level(), &index,     sizeof(uint8_t));
                    memcpy(p.p_epoch(), &epoch,     sizeof(uint32_t));
                    memcpy(p.p_value1(), &duration, sizeof(uint32_t));
                    Server.shell.broadcast(&p);             
                }
                
                Server.shell.broadcast(&dev->device->status());
            }

        }
        
        void        LightService::read_layout           (void)
        {
            uint i=0;
            
            for (auto &dev: _devices)
            {
                if (_layout->exist(dev->device->boardname(), dev->device->name()) == -1)
                {
                    dev->abort_turnoff();
//                     dev->timers.stop();
                    _devices.erase(_devices.begin() + i);
                    this->on_update();
                    return;
                }
            }
            
            for (auto dev: _layout->devices())
            {
                if (this->has_device(dev.boardname(), dev.devname()) == -1)
                {
                    if (System.is_device(dev.boardname(), dev.devname()))
                    {            
                        auto t = std::make_shared<LightDevice>(System.get_device(dev.boardname(), dev.devname()));
                        _devices.push_back( std::move(t) );
                        
//                          reinterpret_cast<devs::GenericSwitch *> (System.get_device(dev.boardname(), dev.devname())) );
//                         _devices.push_back( reinterpret_cast<devs::GenericSwitch *> (System.get_device(dev.boardname(), dev.devname())) );
                    }
                }
            }            
        }
        
    } /* namespace:lgt */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */
