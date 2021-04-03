/*
 * devices.cpp
 *
 *  Created 3.0.7 on: Nov 6, 2019. Edited 13 Jun 2020 (BOARDS)
 *  Created 3.0.9 on: Nov 10, 2020 (SERVER)
 * 
 *      Author: n00b
 */


#if defined (INDOORINO_SERVER)

#include "../common/common.h"
#include "devices.h"
static indoorino::DeviceTemplate invalid_device("");

//      _________________________________________
//      |                                       |
//      |       Device Template                 |
//      |_______________________________________|

namespace indoorino
{
    DeviceTemplate::DeviceTemplate(const char * boardname)
    {
        if(utils::is_devname(boardname)) _boardname.assign(boardname);
        else                             _boardname.assign("UNKNOWN");
        _conf.init(IBACOM_CONF_DEVSTD);
        _stat.init(IBACOM_STATUS_DEVSTD);
    }

    void                        DeviceTemplate::parse              (packet::ipacket * p)
    {
        debug_dev("parse: parsing <%s>", p->label());
        
        if (packet::is_devconf(p->command()))
        {
            if (p->command() != _conf.command())
            {
                warning_dev("Updating %s:%s CONF type [%u] ==> [%u]",
                            boardname(),  name(), p->command() != _conf.command());
                _conf.init(p->command());
            }
            
            if (memcmp(p->payload(), _conf.payload(), _conf.data_size()) != 0)
            {
                warning_dev("Updating device <%s>", name());
                memcpy(_conf.payload(), p->payload(), _conf.data_size());
            }
            
        }
        else if (packet::is_devstat(p->command()))
        {
            if (p->command() != _stat.command())
            {
                warning_dev("Updating %s:%s STAT type [%u] ==> [%u]",
                            boardname(),  name(), p->command() != _conf.command());
                _stat.init(p->command());
            }
            
            if (memcmp(p->payload(), _stat.payload(), _stat.data_size()) != 0)
            {
                memcpy(_stat.payload(), p->payload(), _stat.data_size());
            }
            
        }
    }

    namespace devices
    {

        //      _________________________________________
        //      |                                       |
        //      |       Device : RELAY                  |
        //      |_______________________________________|

        Relay::Relay(const char * boardname):DeviceTemplate(boardname)
        {
            _conf.init(IBACOM_CONF_RELAY);
            _stat.init(IBACOM_STATUS_RELAY);
        }
        
        void                        Relay::parse                    (packet::ipacket * p)
        {
            DeviceTemplate::parse(p);
        }


    } /* namespace : devices */

    
    //      _________________________________________
    //      |                                       |
    //      |       Boards Devices Class List       |
    //      |_______________________________________|
    
    bool                        DeviceList::exist          (const char *key)
    {
        for (auto& d: _list)
        {
            if (strcmp(d.name(), key) == 0) return true;
        }
        return false;
    }

    DeviceTemplate     &       DeviceList::operator[]     (const int i)
    {
        
        iSize_t index=0;
        if (i < 0)  { index = _list.size() + i; }
        else        { index = i; }
        
        if (index < _list.size())
        {
            return _list.at(index);
        }
        
        error_os("FATAL:devices: invalid call for index %d", i);
        return invalid_device;        
    }
    
    DeviceTemplate     &       DeviceList::operator()     (const char *key)
    {
        for (auto& d: _list)
        {
            if (strcmp(d.name(), key) == 0) return d;
        }

        error_os("FATAL:devices: invalid call for %s", key);
        return invalid_device;
    }

    bool                        DeviceList::remove         (const char *key)
    {
        iSize_t i=0;
        for (auto& d: _list)
        {
            if (strcmp(d.name(), key) == 0)
            {
                _list.erase(_list.begin() + i);
                return true;
            }
            i++;
                
        }
        return false;
    }

    bool                        DeviceList::add            (const char * boardname, packet::ipacket * p)
    {
        iCom_t com = p->command();
        
        if (!packet::is_devconf(com)) { goto handle_invalid_packet; }
            
        switch (com)
        {
            case IBACOM_CONF_ASENSOR:
            {
                break;
            }
            case IBACOM_CONF_SWITCH:
            {
                break;
            }
            case IBACOM_CONF_LDR:
            {
                break;
            }
            case IBACOM_CONF_DHT22:
            {
                break;
            }
            case IBACOM_CONF_RELAY:
            {
                warning_board("adding RELAY <%s> on pin %u", p->p_devname(), *p->p_pin1());
                _list.push_back(devices::Relay(boardname));
                _list.back().parse(p);
                break;
            }
            case IBACOM_CONF_DEVSTD:
            {
                warning_dev("adding generic device <%s> on pin %u", p->p_devname(), *p->p_pin1());
                _list.push_back(DeviceTemplate(boardname));
                _list.back().parse(p);
                break;
            }
            default:
            {
                goto handle_invalid_packet;
            }
        }
        return true;
        
        handle_invalid_packet:
        {
                warning_dev("add: unknown device %u <%s>", com, p->label());
                return false;
        }
    }

//      _____________________________________________________________________
//      |                                                                   |
//      |       Indoorino Probes and Messages                               |
//      |___________________________________________________________________|
//
    
    Probe::Probe(packet::ipacket * p)
    {
        _boardtime = std::chrono::system_clock::from_time_t (std::time_t(*p->p_epoch()));        
        _localtime = std::chrono::system_clock::now();        
        _board.assign(p->p_board());
        _device.assign(p->p_devname());
        _type.assign(p->p_type());
        
        uint32_t v = *p->p_value1();
        _value = double(v) / FLOAT2UINT_M;
    }   
    


    
    Message::Message(packet::ipacket * p)
    {
        _boardtime = std::chrono::system_clock::from_time_t (std::time_t(*p->p_epoch()));        
        _localtime = std::chrono::system_clock::now();        
        _board.assign(p->p_board());
        _device.assign(p->p_devname());
        _type.assign(p->p_type());
        
    }   

    

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
