/*
 * devices.cpp
 *
 *  Created 3.0.7 on: Nov 6, 2019. Edited 13 Jun 2020 (BOARDS)
 *  Created 3.0.9 on: Nov 10, 2020 (SERVER)
 * 
 *      Author: n00b
 */

/*
 * STATUS:
 * 0 = no errors
 * 1 = invalid pin
 * 2 = offline
 * 3 = io error
 * 4 = type error
 */


#if defined (INDOORINO_SERVER)

#include "../common/indoorino.h"
#include "devices.h"
#include "indoorino-system.h"
#include "../network/server.h"
#include "paths.h"

namespace indoorino
{
    
    namespace devs
    {


//      ___________________________________________________________________________________________________
//                                                                                                 
//                ══════⊹⊱≼≽⊰⊹══════      B O A R D    T E M P L A T E      ══════⊹⊱≼≽⊰⊹══════           
//      ___________________________________________________________________________________________________
//


        BoardTemplate::BoardTemplate(const char * bname)
        {
            _config[0].init(IBACOM_CONF_STD);
            _config[1].init(IBACOM_CONF_STD);
            _status[0].init(IBACOM_STATUS_STD);
            _status[1].init(IBACOM_STATUS_STD);
                    
            strcpy(_config[0].p_name(), bname);
            strcpy(_config[1].p_name(), bname);
            strcpy(_status[0].p_name(), bname);
            strcpy(_status[1].p_name(), bname);
            alert_board("NEW BOARD: added %s to System.boards", this->name());
        }
        
        BoardTemplate::~BoardTemplate()
        {          
            // TODO for auto d in devs : delete d
            _devices.clear();
            _rxqueue.clear();
            _txqueue.clear();
        }

        void                BoardTemplate::parse            (packet::netpacket * incoming)
        {
            if (!incoming) 
            {
                error_board("%s: parsing nullptr", name());
                return;
            }
            
            info_board("%s: parsing %s", name(), incoming->description());
            
            /* packet is a BOARD CONFIG */
            if (packet::is_boardconf(incoming->command()))
            {
                if (incoming->command() != _config[1].command())
                {
                    warning_board("<%s> changed type [%u] ==> [%u]", name(), _config[1].command(), incoming->command());
                    _config[1].init(incoming->command());
                }
                if (memcmp(_config[1].payload(), incoming->payload(), incoming->data_size()) != 0)
                {
                    alert_board("Updating config for <%s>", name());
                    memcpy(_config[1].payload(), incoming->payload(), incoming->data_size());
                }
            }
            
            /* packet is a BOARD STATUS */
            else if (packet::is_boardstat(incoming->command()))
            {
                if (incoming->command() != _status[1].command())
                {
                    warning_board("<%s> changed type [%u] ==> [%u]", name(), _status[1].command(), incoming->command());
                    _status[1].init(incoming->command());
                }            
                memcpy(_status[1].payload(), incoming->payload(), incoming->data_size());
            }
            
            /* packet is a DEVICE CONFIG */
            else if (packet::is_devconf(incoming->command()))
            {
                if (_devices.exist(incoming->p_devname()) == -1)
                {
                    _devices.add(incoming);
                    return;
                }
                _devices(incoming->p_devname())->parse(incoming);
            }
            
            /* packet is a DEVICE STATUS */
            else if (packet::is_devstat(incoming->command()))
            {
                if (_devices.exist(incoming->p_devname()) == -1)
                {
                    warning_board("PARSE:set-status: device %s:%s does not exist!", incoming->p_name(), incoming->p_devname());
                    /* How not send 100 req packets?? */                
                    return;
                }
                _devices(incoming->p_devname())->parse(incoming);
            }      
        }
        
        void                BoardTemplate::send_config      (void)
        {
            alert_board("Sending sys conf! (%u devs)", _devices.size());
            Server.shell.broadcast(&_config[0]);
            Server.shell.broadcast(&_config[1]);
//             std::cout << "sending CONF <" << name() << "> \nname " <<_config[0].p_name() << "\nboard " << _config[0].p_board() << "\ntype " << _config[0].p_type() << "\ndevnum " << _config[0].p_devnum() << std::endl;
            for (auto d: _devices()) Server.shell.broadcast(&d->config());
        }

        void                BoardTemplate::send_status      (void)
        {
            alert_board("Sending sys stat! (%u devs)", _devices.size());
            Server.shell.broadcast(&_status[0]);
            Server.shell.broadcast(&_status[1]);
        
            for (auto d: _devices()) Server.shell.broadcast(&d->status());

        }
        
        void                BoardTemplate::read_layout      (void)
        {
            info_board("Reading devices...");
            
            if (!System.layout.is_board(name()))
            {
                warning_board("Could not find %s in System.layout", name());
                return;
            }
                    
            for (auto & d : _devices())
            {
                /* if not board in layout, delete it */
                if (!System.layout.is_device(name(), d->name()))
                {
                    info_board("read-layout: popping device %s:%s", name(), d->name());
                    _devices.remove(d->name());
                }
            }
            
            for (auto& service : System.layout.services())
            {
                for (auto& d : service.devices())
                {
                    if ( (strcmp(name(), d.boardname()) == 0) && (_devices.exist(d.devname()) == -1) )
                    {
                        if (!_devices.add(d.boardname(), d.devname(), d.type().command)) continue; // add something
                    }
                }
            }
        }
        
        void                BoardTemplate::show             (void)
        {
            
            std::cout << "\nBoard           : [" << name() << "]" << std::endl;
            std::cout << "\tenabled : " << _enabled << std::endl;
            std::cout << "\tonline  : " << _is_online << std::endl;
            std::cout << "\tcurrent lag : " << _lag.count() << " [sec.]" << std::endl;
            
            std::cout << "\n\t\tBOARD CONFIG: " << std::endl;
            _config[0].dump();
            _config[1].dump();
            std::cout << "\n\t\tBOARD STATUS: " << std::endl;
            _status[0].dump();
            _status[1].dump();
            std::cout << std::endl;
            
            for (auto d : _devices())
            {
                std::cout << "\t";
                for (uint i=0; i<LEN_NAME; i++) std::cout << "_";
                std::cout << std::endl;
                d->show();    
            }
        }
    
        
        
        
        
//      ___________________________________________________________________________________________________
//                                                                                                 
//                ══════⊹⊱≼≽⊰⊹══════    D E V I C E S    T E M P L A T E    ══════⊹⊱≼≽⊰⊹══════           
//      ___________________________________________________________________________________________________
//

        DeviceTemplate::DeviceTemplate(const char *bname, const char *dname, iCom_t comm, iPin_t pin)
        {

            _conf.init(comm);
            _stat.init(IBACOM_STATUS_DEVSTD);
            
            strcpy(_conf.p_name(),    bname);
            strcpy(_conf.p_devname(), dname);
            memcpy(_conf.p_pin1(),    &pin, sizeof(iPin_t));
                    
            alert_dev("NEW DEVICE: added %s:%s to System.boards.%s on pin [%u]", lyt::devicetype_Com2Str(comm), name(), boardname(), pin);
            
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());

        }

        void                        DeviceTemplate::show                (void)
        {
            std::cout << "\n\tDevice          : [" << boardname() << ":" << name() << "]" << std::endl;
            
            std::cout << "\t\tonline  : " << is_connected() << std::endl;
            std::cout << "\t\tpin     : " << pin() << std::endl;
    //         std::cout << "\t\tservices [ " << _services.size() << " ]: ";
    //         
    //         for (auto &s : _services)
    //         {
    //             std::cout << s << ", ";
    //         }

            std::cout << std::endl;
            std::cout << "\n\t\tDEVICE CONFIG: " << std::endl;
            _conf.dump();
            std::cout << "\n\t\tDEVICE STATUS: " << std::endl;
            _stat.dump();
        }

        void                        DeviceTemplate::parse               (packet::ipacket * p)
        {
            debug_dev("parse: parsing <%s>", p->label());
            
            if (packet::is_devconf(p->command()))
            {
                if (p->command() != _conf.command())
                {
                    warning_dev("Updating %s:%s CONF type [%u] ==> [%u]",
                                boardname(),  name(), _conf.command(), p->command());
                    _conf.init(p->command());
                }
                
                if (memcmp(p->payload(), _conf.payload(), _conf.data_size()) != 0)
                {
                    warning_dev("Updating device <%s:%s>", boardname(), name());
                    memcpy(_conf.payload(), p->payload(), _conf.data_size());
                }
                
            }
            else if (packet::is_devstat(p->command()))
            {
                if (p->command() != _stat.command())
                {
                    warning_dev("Updating %s:%s STAT type [%u] ==> [%u]",
                                boardname(),  name(), _stat.command(), p->command());
                    _stat.init(p->command());
                }
                
                if (memcmp(p->payload(), _stat.payload(), _stat.data_size()) != 0)
                {
                    memcpy(_stat.payload(), p->payload(), _stat.data_size());
                }
                
            }
        }


    } /* namespace:devs */

} /* namespace : indoorino */

#endif /* INDOORINO_NETWORK */


















//     int                         DeviceTemplate::has_service         (const char * n)
//     {
//         int index = 0;
//         for (auto s : _services)
//         {
//             if (strcmp(s->name(), n) == 0) return index;
//             index++;
//         }
//         return -1;
//     }
//     
//     void                        DeviceTemplate::add_service         (const char * n)
//     {
//         if (this->has_service(n) == -1)
//         {
//             int i = System.services.exist(n);
// 
//             if (i != -1)
//             {
//                 _services.push_back(System.services[i]);
//             }
//             else
//             {
//                 warning_dev("add-service: service %s not found in System.services()", n);
//             }
//             return;
//         }
//         warning_dev("add-service: <%s> already in list", n);
//     }

    //              _________________________________________
    //              |                                       |
    //              |       A L A R M                       |
    //              |_______________________________________|
    //        

//     void                        DeviceTemplate::update_alarms       (void)
//     {
//         for (auto service: System.services())
//         {
//             if (service->is_alarm() && service->has_device(this->boardname(), this->name()) != -1)
//             {
//                 service->on_update();
//             }
//         }        
//     }
// 
//     void                        DeviceTemplate::parse_alarm         (packet::ipacket * p)
//     {
//         DeviceTemplate::parse(p);
// 
//         if ( (p->command() >= IBACOM_HEAT_ALARM) && (p->command() <= IBACOM_GENERIC_ALARM) )
//         {
//             if ( (strcmp(p->p_board(), boardname()) == 0) && (strcmp(p->p_devname(), name()) == 0) )
//             {
//                 alarm._value = * p->p_value1();
//                 alarm._signals.push_back(packet::netpacket(p));
//                 alert_os("ALARM:got %s from <%s:%s> : level=%d ",p->label(), p->p_board(), p->p_devname(), *p->p_value1());  
//                 this->update_alarms();


//                 for (auto s : _services)
//                 {
// 
//                     if ( (p->command() == IBACOM_HEAT_ALARM     && strcmp(s->type(), "ALARM:FIRE") == 0)  ||
//                          (p->command() == IBACOM_FLOOD_ALARM    && strcmp(s->type(), "ALARM:FLOOD") == 0) )
//                     {
//                         alert_os("ALARM:updating service: %s ", s->name());
//                         s->on_update();
//                         // call _service.alarm.update()
//                         break;
//                     }
//                 }
//             }
//         }
//         
//         else if ( p->command() == IBACOM_SET_ENV_ALARM )
//         {
//             if ( (strcmp(p->p_board(), boardname()) == 0) && (strcmp(p->p_devname(), name()) == 0) )
//             {
//                 bool b=bool(*p->p_value1());
//                 
//                 if ( alarm._enabled && (!b) )
//                     alert_os("ALARM:SET <%s:%s> DISABLED", p->p_board(), p->p_devname());                    
//                 else if ( (!alarm._enabled) && b )
//                     alert_os("ALARM:SET <%s:%s> ENABLED", p->p_board(), p->p_devname());
//                     
//                 alarm._enabled=b;                        
//                 alarm._signals.push_back(packet::netpacket(p));
//                 this->update_alarms();
//             }
//         }
//         else if ( p->command() == IBACOM_ACK_ENV_ALARM )
//         {
//             if ( (strcmp(p->p_board(), boardname()) == 0) && (strcmp(p->p_devname(), name()) == 0) )
//             {
//                 alert_os("ALARM:parsing ACK for <%s:%s>", p->p_board(), p->p_devname());
//                 if (alarm._value > 0)
//                 {
//                     alarm._value=0;
//                     alert_os("ALARM:ACK for <%s:%s>", p->p_board(), p->p_devname());
//                     alarm._signals.push_back(packet::netpacket(p));
//                     this->update_alarms();
//                 }
//             }
//         }
//     }
        
// //     namespace devs
// //     {
// // 
// //         //              _________________________________________
// //         //              |                                       |
// //         //              |       Device : R E L A Y              |
// //         //              |_______________________________________|
// //         // 
// //     
// //         Relay::Relay(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
// //         {
// //             _stat.init(IBACOM_STATUS_RELAY);
// //             strcpy(_stat.p_name(),      _conf.p_name());
// //             strcpy(_stat.p_devname(),   _conf.p_devname());
// //             
// //         }
// //         void                        Relay::parse                    (packet::ipacket * p)
// //         {
// //             DeviceTemplate::parse(p);
// //         }
// //             
// //         //              _________________________________________
// //         //              |                                       |
// //         //              |       Base : S W I t C H              |
// //         //              |_______________________________________|
// //         // 
// //         
// //         GenericSwitch::GenericSwitch(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
// //         {
// //             _stat.init(IBACOM_STATUS_SWITCH);
// //             strcpy(_stat.p_name(),      _conf.p_name());
// //             strcpy(_stat.p_devname(),   _conf.p_devname());
// //             
// //         }
// //         void                        GenericSwitch::parse            (packet::ipacket * p)
// //         {
// //             DeviceTemplate::parse(p);
// //             DeviceTemplate::parse_alarm(p);
// //         }
// // 
// // 
// //         //              _________________________________________
// //         //              |                                       |
// //         //              |       Device : D H T 2 2              |
// //         //              |_______________________________________|
// //         // 
// //         
// //         DHT22::DHT22(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
// //         {
// //             _stat.init(IBACOM_STATUS_DHT22);
// //             strcpy(_stat.p_name(),      _conf.p_name());
// //             strcpy(_stat.p_devname(),   _conf.p_devname());
// //             
// //         }
// //         void                        DHT22::parse                    (packet::ipacket * p)
// //         {
// //             DeviceTemplate::parse(p);
// //             DeviceTemplate::parse_alarm(p);
// //         }
// //         //              _________________________________________
// //         //              |                                       |
// //         //              |       Device : F L O O D S W I T C H  |
// //         //              |_______________________________________|
// //         // 
// //         FloodSwitch::FloodSwitch(const char *b, const char *d, iCom_t c, iPin_t p):GenericSwitch(b, d, c, p)
// //         {
// //             _stat.init(IBACOM_STAT_FLOODSWITCH);
// //             strcpy(_stat.p_name(),      _conf.p_name());
// //             strcpy(_stat.p_devname(),   _conf.p_devname());
// //             
// //         }
// //         
// //         //              _________________________________________
// //         //              |                                       |
// //         //              |       Device : R A I N S W I T C H    |
// //         //              |_______________________________________|
// //         // 
// //         
// //         RainSwitch::RainSwitch(const char *b, const char *d, iCom_t c, iPin_t p):GenericSwitch(b, d, c, p)
// //         {
// //             _stat.init(IBACOM_STAT_RAINSWITCH);
// //             strcpy(_stat.p_name(),      _conf.p_name());
// //             strcpy(_stat.p_devname(),   _conf.p_devname());
// //         }
// //         
// //         //              _________________________________________
// //         //              |                                       |
// //         //              |       Device : P M 2 5   dust sensor  |
// //         //              |_______________________________________|
// //         //
// //         
// //         DustPM25::DustPM25(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
// //         {
// //             _stat.init(IBACOM_STATUS_DUSTPM25);
// //             strcpy(_stat.p_name(),      _conf.p_name());
// //             strcpy(_stat.p_devname(),   _conf.p_devname());
// //             
// //         }
// //         void                        DustPM25::parse                 (packet::ipacket * p)
// //         {
// //             DeviceTemplate::parse(p);
// //             DeviceTemplate::parse_alarm(p);
// //         }
// // 
// // 
// //     } /* namespace : devs */

//      _____________________________________________________________________
//      |                                                                   |
//      |       Indoorino Probes and Messages                               |
//      |___________________________________________________________________|
//
    
//     Probe::Probe(packet::ipacket * p)
//     {
//         _boardtime = std::chrono::system_clock::from_time_t (std::time_t(*p->p_epoch()));        
//         _localtime = std::chrono::system_clock::now();        
//         _board.assign(p->p_board());
//         _device.assign(p->p_devname());
//         _type.assign(p->p_type());
//         
//         uint32_t v = *p->p_value1();
//         _value = double(v) / FLOAT2UINT_M;
//     }   
//     
// 
// 
//     
//     Message::Message(packet::ipacket * p)
//     {
//         _boardtime = std::chrono::system_clock::from_time_t (std::time_t(*p->p_epoch()));        
//         _localtime = std::chrono::system_clock::now();        
//         _board.assign(p->p_board());
//         _device.assign(p->p_devname());
//         _type.assign(p->p_type());
//         
//     }   

    




// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// /*
//  
//  
//  
//  
//  //         _conf.init(p->command());
// //         memcpy(_conf.payload(), p->payload(), _conf.data_size());
// //         debug_dev("New device <%s> on pin [%u]", name(), pin());
// //         strcpy(_name, d_name);
// //         strcpy(_boardname, d_board);
// //         memcpy(&_pin, d_pin, sizeof(uint8_t));
// 
// //     DeviceTemplate::DeviceTemplate(ipacket * p)
// //     {
// //         if (p->command() == IBACOM_CONF_DEVSTD)
// //         {
// //             info_dev("Initializing device <%s> on pin [%u]", p->p_devname(), *p->p_pin1());
// //             strcpy(_boardname, p->p_name());
// //             strcpy(_name, p->p_devname());
// //             memcpy(&_pin, p->p_pin1(), sizeof(uint8_t));
// //         }
// //         else
// //         {
// //             error_dev("Invalid initializing packet <%s>", p->label());
// //             strcpy(_boardname, "INVALIDBOARD");
// //             strcpy(_name, "INVALID");
// //             _pin=0;
// //         }
// //     }
// 
//  
//  
//  
//  
//  
//  */
