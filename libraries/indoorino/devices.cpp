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

namespace indoorino
{

//      _________________________________________
//      |                                       |
//      |       Device Template                 |
//      |_______________________________________|

    
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
        std::cout << "\t\tservices [ " << _services.size() << " ]: ";
        
        for (auto &s : _services)
        {
            std::cout << s << ", ";
        }
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
                            boardname(),  name(), p->command(), _conf.command());
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
                            boardname(),  name(), p->command(), _conf.command());
                _stat.init(p->command());
            }
            
            if (memcmp(p->payload(), _stat.payload(), _stat.data_size()) != 0)
            {
                memcpy(_stat.payload(), p->payload(), _stat.data_size());
            }
            
        }
    }

        
    int                         DeviceTemplate::has_service         (const char * n)
    {
        int index = 0;
        for (auto &s : _services)
        {
            if (s.compare(n) == 0) return index;
            index++;
        }
        return -1;

        
    }
    
    void                        DeviceTemplate::add_service         (const char * n)
    {
        if (this->has_service(n) == -1)
        {
            _services.push_back(std::string(n));
            return;
        }
        warning_dev("add-service: <%s> already in list", n);
    }

    
//      _____________________________________________________________________________________________
//                                                                                                 
//             ══════⊹⊱≼≽⊰⊹══════    D E V I C E S    S P A C E    ══════⊹⊱≼≽⊰⊹══════             
//      _____________________________________________________________________________________________
//


    namespace devs
    {

    //              _________________________________________
    //              |                                       |
    //              |       Base : A L A R M                |
    //              |_______________________________________|
    //
        void                        DeviceAlarm::parse              (packet::ipacket * p)
        {
            DeviceTemplate::parse(p);

            if ( (p->command() >= IBACOM_HEAT_ALARM) && (p->command() <= IBACOM_GENERIC_ALARM) )
            {
                if ( (strcmp(p->p_board(), boardname()) == 0) && (strcmp(p->p_devname(), name()) == 0) )
                {
                    uint c = 0;
                    for (auto &s : _services)
                    {
                        int i = System.services.exist(s.c_str());
                        
                        if (i == -1)
                        {
                            _services.erase(_services.begin() + c);
                        }
                        else
                        {
                            c++;

                            if ( (p->command() == IBACOM_HEAT_ALARM     && strcmp(System.services().at(i)->type(), "ALARM:FIRE") == 0)  ||
                                (p->command() == IBACOM_FLOOD_ALARM    && strcmp(System.services().at(i)->type(), "ALARM:FLOOD") == 0) )
                            {
                                _on_alarm=true;
                                _alarm_value = * p->p_value1();
                                _signals.push_back(packet::netpacket(p));
                                break;
                            }
                        }

                    }
                }
            }
            else if ( p->command() == IBACOM_SET_ENV_ALARM )
            {
                if ( (strcmp(p->p_board(), boardname()) == 0) && (strcmp(p->p_devname(), name()) == 0) )
                {
                    bool b=bool(*p->p_value1());
                    if (b)
                        alert_os("ALARM:SET <%s:%s> ENABLED", p->p_board(), p->p_devname());
                    else
                        alert_os("ALARM:SET <%s:%s> DISABLED", p->p_board(), p->p_devname());                    
                        
                    _enabled=b;
                    _signals.push_back(packet::netpacket(p));
//                     this->send_updates();
                }
            }
                    
        }        
        
    //              _________________________________________
    //              |                                       |
    //              |       Device : R E L A Y              |
    //              |_______________________________________|
    // 
        Relay::Relay(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
        {
            _stat.init(IBACOM_STATUS_RELAY);
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());
            
        }
        void                        Relay::parse                    (packet::ipacket * p)
        {
            DeviceTemplate::parse(p);
        }
        
    //              _________________________________________
    //              |                                       |
    //              |       Base : S W I t C H              |
    //              |_______________________________________|
    // 
        GenericSwitch::GenericSwitch(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
        {
            _stat.init(IBACOM_STATUS_SWITCH);
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());
            
        }


    //              _________________________________________
    //              |                                       |
    //              |       Device : D H T 2 2              |
    //              |_______________________________________|
    // 
        DHT22::DHT22(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
        {
            _stat.init(IBACOM_STATUS_DHT22);
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());
            
        }
        
    //              _________________________________________
    //              |                                       |
    //              |       Device : F L O O D S W I T C H  |
    //              |_______________________________________|
    // 
        FloodSwitch::FloodSwitch(const char *b, const char *d, iCom_t c, iPin_t p):GenericSwitch(b, d, c, p)
        {
            _stat.init(IBACOM_STAT_FLOODSWITCH);
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());
            
        }
        
        void                        FloodSwitch::parse                  (packet::ipacket * p)
        {
            DeviceTemplate::parse(p);
        }
    //              _________________________________________
    //              |                                       |
    //              |       Device : R A I N S W I T C H    |
    //              |_______________________________________|
    // 
        RainSwitch::RainSwitch(const char *b, const char *d, iCom_t c, iPin_t p):GenericSwitch(b, d, c, p)
        {
            _stat.init(IBACOM_STAT_RAINSWITCH);
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());
        }
        
    //              _________________________________________
    //              |                                       |
    //              |       Device : P M 2 5   dust sensor  |
    //              |_______________________________________|
    // 
        DustPM25::DustPM25(const char *b, const char *d, iCom_t c, iPin_t p):DeviceTemplate(b, d, c, p)
        {
            _stat.init(IBACOM_STATUS_DUSTPM25);
            strcpy(_stat.p_name(),      _conf.p_name());
            strcpy(_stat.p_devname(),   _conf.p_devname());
            
        }


    } /* namespace : devs */

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

    

} /* namespace : indoorino */

#endif /* INDOORINO_NETWORK */



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
