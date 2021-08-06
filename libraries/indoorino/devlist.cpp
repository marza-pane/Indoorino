/*
 * devices-list.cpp
 *
 *  Created 3.0.7 on: Aug 4, 2021
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
    //      |       Device Holder List              |
    //      |_______________________________________|
    
    Devices::Devices()
    {

    }
    
    Devices::~Devices()
    {
        clear();
    }
    
    void                        Devices::clear          (void) 
    {
        for (auto p: _devs)
        {
            if (p != nullptr) delete p;
        }
        _devs.clear();
    }
    
    void                        Devices::clean_nullp    (void)
    {
        iSize_t i=0;
        for (auto p: _devs)
        {
            if (p == nullptr)
            {
                error_dev("LIST: found nullptr!");
                _devs.erase(_devs.begin() + i);
                clean_nullp();
                return;
            }
            i++;
        }
    }
    
    int                         Devices::exist          (const char *key)
    {
        clean_nullp();
        int index = 0;
        for (auto p: _devs)
        {
            if (strcmp(p->name(), key) == 0) return index;
            index++;
        }
        return -1;
    }

    DeviceTemplate  *           Devices::operator[]     (const int index)
    {
        iSize_t i=0;
        if (index < 0)  { i = _devs.size() + index; }
        else            { i = index; }
        
        if (i >= _devs.size())
        {
            error_os("FATAL:devices: invalid call for index %d", index);
            return nullptr;
        }
        
        return _devs[i];
    }
    
    DeviceTemplate  *           Devices::operator()     (const char *key)
    {
        int i=this->exist(key);

        if (i == -1)
        {
            error_os("FATAL:devices: invalid call for %s", key);
            return nullptr;
        }
        return _devs[i];
    }

    bool                        Devices::remove         (const char *key)
    {
        int i=this->exist(key);
        if (i == -1)
        {
            warning_os("devices:rem: can not find %s", key);
            return false;
        }
        if (_devs.at(i) != nullptr)
            delete _devs.at(i);
        _devs.erase(_devs.begin() + i);

        return true;
    }

    bool                        Devices::add            (const char * boardname, const char * devname, iCom_t com, iPin_t pin)
    {
        if (this->exist(devname) != -1)
        {
            warning_dev("LIST:add: %s:%s already exist!", boardname, devname);
            return false;
        }
        
        switch (com)
        {
            case IBACOM_CONF_ASENSOR:
            {
                _devs.push_back(new DeviceTemplate(boardname, devname, com, pin));
                goto not_implemented_yet;  // TODO
            }
            case IBACOM_CONF_SWITCH:
            {
                _devs.push_back(new devs::GenericSwitch(boardname, devname, com, pin));
                break;
            }
            case IBACOM_CONF_FLOODSWITCH:
            {
                _devs.push_back(new devs::FloodSwitch(boardname, devname, com, pin));
                break;
            }
            case IBACOM_CONF_RAINSWITCH:
            {
                _devs.push_back(new devs::RainSwitch(boardname, devname, com, pin));
                break;
            }
            case IBACOM_CONF_RELAY:
            {
                _devs.push_back(new devs::Relay(boardname, devname, com, pin));
                break;
            }
            case IBACOM_CONF_LDR:
            {
                _devs.push_back(new DeviceTemplate(boardname, devname, com, pin));
                goto not_implemented_yet; // TODO
            }
            case IBACOM_CONF_DUSTPM25:
            {
                _devs.push_back(new devs::DustPM25(boardname, devname, com, pin));
                break;
            }
            case IBACOM_CONF_DHT22:
            {
                _devs.push_back(new devs::DHT22(boardname, devname, com, pin));
                break;            
            }
            case IBACOM_CONF_DEVSTD:
            {
                _devs.push_back(new DeviceTemplate(boardname, devname, com, pin));
                break;
            }
            default:
            {
                _devs.push_back(new DeviceTemplate(boardname, devname, IBACOM_CONF_DEVSTD, pin));
                goto not_implemented_yet;;
            }
        }
        
        debug_board("ADD:init %s <%s:%s> on pin %u", lyt::devicetype_Com2Str(com), boardname, devname, pin);
        return true;
        
        not_implemented_yet:
        {
            
            warning_board("ADD: type %s for %s:%s not implemented yet", lyt::devicetype_Com2Str(com), boardname, devname);
            return false;
        }
    }

    bool                        Devices::add            (packet::ipacket * p)
    {

        iCom_t com = p->command();
        
        if (!packet::is_devconf(com))
        {
            warning_dev("ADD: unknown device %u <%s>", com, p->label());
            return false;
        }
                
        this->add(p->p_name(), p->p_devname(), p->command(), *p->p_pin1());
        _devs[-1]->parse(p);
        
        alert_board("Now listing devices ###");
        for (auto d: _devs) { std::cout << d->boardname() << ":" << d->name() << std::endl; }
        std::cout << "###\n";
       
        return true;
        
    }


} /* namespace : indoorino */

#endif /* INDOORINO_NETWORK */
