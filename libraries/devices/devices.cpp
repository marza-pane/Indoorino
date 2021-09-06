/*
 * devices.cpp
 *
 *  Created 3.0.7 on: Nov 6, 2019. Edited 13 Jun 2020 (BOARDS)
 *  Created 3.0.9 on: Nov 10, 2020 (SERVER)
 * 
 *      Author: n00b
 */

#if defined(ARDUINO)
#include "indoorino.h"
#if defined(INDOORINO_DEVS)

//      _____________________________________________________________________
//      |                                                                   |
//      |       VIRTUAL DEVICE                                              |
//      |___________________________________________________________________|
//

virtualDevice::virtualDevice(uint8_t index)
{
    _confindex = index;
    
    int p = conf.device_pin(_confindex);
    if (!utils::board::is_pin(p))
    {
        error_dev("init:invalid pin [%u] for device [%u]!", p, _confindex);
        _status=1;
    }
    else
    {
        _status=0;
        _pin=conf.device_pin(_confindex);
        conf.device_name(_name, _confindex);
    }
}

virtualDevice::~virtualDevice()
{
    debug_dev("deleting [%u]: pin [%u]", _confindex, _pin);
}

packet::ipacket *   virtualDevice::config               (packet::ipacket * p)
{
    return conf.device(p, _confindex);
}

void                virtualDevice::send_dev_conf        (void)
{
    packet::ipacket p;
    utils::board::io.send(conf.device(&p, _confindex));
}

//      _____________________________________________________________________
//      |                                                                   |
//      |       ACTUATOR GENERIC                                            |
//      |___________________________________________________________________|
//

virtualActuator::~virtualActuator()
{
    debug_dev("actuator [%u]: deleting pin [%u]", _confindex, _pin);
    digitalWrite(_pin, LOW);
    pinMode(_pin, INPUT);
}

//      _____________________________________________________________________
//      |                                                                   |
//      |       SENSOR GENERIC                                              |
//      |___________________________________________________________________|
//

virtualSensor::~virtualSensor()
{
    debug_dev("sensor [%u]: refreshing pin [%u]", _confindex, _pin);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    pinMode(_pin, INPUT);
}




//      _____________________________________________________________________
//      |                                                                   |
//      |       DEVICE LIST                                                 |
//      |___________________________________________________________________|
//

IndoorinoDeviceList::IndoorinoDeviceList() {}

IndoorinoDeviceList::~IndoorinoDeviceList()
{
    _devices.clear();
}

// void                IndoorinoDeviceList::clearList               (void)
// {
//     for(uint8_t i=0; i<conf.devnum(); i++)
//     {
//         delete _devices[i];
//     }
//     free(_devices);   
// }

void                IndoorinoDeviceList::begin                  (void)
{
    debug_dev("Initializing dev list (size = %u)", conf.devnum());

    _devices.clear();
    
    for (uint8_t i=0; i<conf.devnum(); i++)
    {
        _alloc_type(i);            
    }
}

void                IndoorinoDeviceList::loop                   (void)
{
    for (uint8_t i=0; i<_devices.count(); i++)
    {
        _devices[i]->loop();
    }
}

void                IndoorinoDeviceList::reset                  (void)
{
    for(uint8_t i=0; i<_devices.count(); i++)
    {
        if (! _devices[i]->reset())
        {
            error_dev("Can not reset device [%u]", i);
        }
    }
}

bool                IndoorinoDeviceList::add                    (packet::ipacket * ptr)
{

    if (conf.devAdd(ptr))
    {
        uint8_t index = conf.indexFromName(ptr->p_devname());
        debug_dev("devices: new device at [%u]: total devices [%u]",index, conf.devnum());
        return _alloc_type(index);
    }
    else
    {
        error_dev("ERROR:device list can not add %s", ptr->p_devname());
    }

    return false;
}

bool                IndoorinoDeviceList::_alloc_type            (uint8_t index)
{
    
    if (index < _devices.count() && _devices[index] != nullptr)
    {
        delete _devices[index];
        warning_dev("devlist:overwrite device at index [%u]",index);
    }
    switch (conf.device_command(index))
    {
//         case IBACOM_CONF_ASENSOR:
//         {
//             _devices[index] = new Sensor_Analog(index);
//             info_dev("devlist:alloc_type: init Analog Sensor");
//             break;
//         }
        case IBACOM_CONF_SWITCH:
        {
            Sensor_Switch * p = new Sensor_Switch(index);
            _devices.append(p);
            debug_dev("devlist:alloc_type: init Switch");
            break;
        }
        case IBACOM_CONF_FLOODSWITCH:
        {
            SwitchSensor_Flood * p = new SwitchSensor_Flood(index);
            _devices.append(p);
            debug_dev("devlist:alloc_type: init Flood Switch");
            break;
        }
        case IBACOM_CONF_RELAY:
        {
            Actuator_Relay * p = new Actuator_Relay(index);
            _devices.append(p);
            debug_dev("devlist:alloc_type: init Relay");
            break;
        }
        case IBACOM_CONF_DUSTPM25:
        {
            Sensor_PM25dust * p = new Sensor_PM25dust(index);
            _devices.append(p);
            debug_dev("devlist:alloc_type: init Relay");
            break;
        }
        case IBACOM_CONF_DHT22:
        {
            Sensor_DHT22 * p = new Sensor_DHT22(index);
            _devices.append(p);
            debug_dev("devlist:alloc_type: init DHT22");
            break;
        }
        
//         case IBACOM_CONF_TIMER:
//         {
//             _devices[index] = new Actuator_Relay(index);
//             debug(F("\n\tInit Timer"));
//             break;
//         }
        default:
        {
            error_mem("FATAL:class not defined for %u", conf.device_command(index));
            return false;
        }
    }
//     debug_dev("initialized device %u:%s on pin %u", 
//              _devices[index]->config()->command(),
//              _devices[index]->config()->p_devname(),
//              *_devices[index]->config()->p_pin());
             
    return true;
}

bool                IndoorinoDeviceList::rem                    (const char * name)
{

    int8_t index = conf.indexFromName(name);
    
    if (index >= 0)
    {
        debug_dev("---Removing device [%u] <%s>", conf.indexFromName(name), name);
        _devices.rem(index);        
        return true;
    }
    else
    {
        error_dev("devicelist:rem: device %s not found", name);
        return false;
    }
}

bool                IndoorinoDeviceList::mod                    (packet::ipacket * ptr)
{
    char * name = ptr->p_devname();
    int8_t index = conf.indexFromName(name);
    
    if (name != nullptr && index >= 0)
    {
        if (conf.devMod(name, ptr))
        {
            _devices[index]->reset();
            return true;
        }
    }
    else error_dev("devicelist:mod: wrong packet type %s", F2C(ptr->label()));
    
    return false;
}

bool                IndoorinoDeviceList::setName                (const char * name, const char * new_name)
{
    bool flag = conf.devSetName(name, new_name);
//     if (flag)
//         sendReport(1, F("DEVICES"), F("setName: %s has been renamed %s"),name, new_name);
    return flag;

}

bool                IndoorinoDeviceList::setPin                 (const char * name, uint8_t pin)
{
    int8_t index = conf.indexFromName(name);
    if (conf.devSetPin(name, pin))
    {
//         sendReport(1, F("DEVICES"), F("setPin: %s has been set to pin %u"),name, pin);
        return _devices[index]->reset();
    }
    return false;
}


virtualDevice   *   IndoorinoDeviceList::operator[]             (uint8_t index)
{   
    if (index >= _devices.count())
    {
        error_dev("FATAL:device list: index %u out of range %u", index, _devices.count());
        return nullptr;
    }
    
    return _devices[index];
}

virtualDevice   *   IndoorinoDeviceList::operator[]             (const char * name)
{   
    int8_t index = conf.indexFromName(name);
    
    if (index >= 0)
    {
        return _devices[index];
    }
    error_dev("ERROR:invalid call for device %s",name);
    return nullptr;
}

#endif /* INDOORINO_DEVS */
#endif /* INDOORINO */

// #endif /* INDOORINO_NETWORK */



















// #if defined (INDOORINO_NETWORK)
// 
// #include "../indoorino/imports.h"
// 
//      _________________________________________
//      |                                       |
//      |       Device Template                 |
//      |_______________________________________|
// 
// IndoorinoDevice::IndoorinoDevice(const char *d_name, const char *d_board, uint8_t *d_pin)
// {
//     debug_dev("New device <%s> on pin [%u]", d_name, *d_pin);
//     strcpy(_name, d_name);
//     strcpy(_boardname, d_board);
//     memcpy(&_pin, d_pin, sizeof(uint8_t));
// }
// 
// IndoorinoDevice::IndoorinoDevice(ipacket * p)
// {
//     if (p->command() == IBACOM_CONF_DEVSTD)
//     {
//         info_dev("Initializing device <%s> on pin [%u]", p->p_devname(), *p->p_pin1());
//         strcpy(_boardname, p->p_name());
//         strcpy(_name, p->p_devname());
//         memcpy(&_pin, p->p_pin1(), sizeof(uint8_t));
//     }
//     else
//     {
//         error_dev("Invalid initializing packet <%s>", p->label());
//         strcpy(_boardname, "INVALIDBOARD");
//         strcpy(_name, "INVALID");
//         _pin=0;
//     }
// }
// 
// bool                        IndoorinoDevice::parse              (ipacket * packet)
// {
//     debug_dev("parse: parsing <%s>", packet->label());
//     
//     if (packet->p_name() == nullptr) { return false; }
//     
//     if (strcmp(_boardname, packet->p_name()) != 0)
//     {
//         warning_dev("parse:mismatching boardnames <%s> <%s>",_boardname, packet->p_name());
//         return false;
//     }
//     
//     if (packet->p_devname() == nullptr) { return false; }
//     
//     if (!(strcmp(_name, packet->p_devname()) == 0))
//     {
//         warning_dev("Updating device name <%s> ==> <%s>",_name, packet->p_devname());
//         strcpy(_name, packet->p_devname());
//     }
// 
//     if (packet->p_pin1() == nullptr) { return false; }
//     
//     if (_pin != *packet->p_pin1())
//     {
//         warning_dev("Updating device pin [%u] ==> [%u]",_pin, *packet->p_pin1());
//         _pin = *packet->p_pin1();
//     }
//     return true;
// }
// ipacket     *               IndoorinoDevice::get_conf           (ipacket * packet)
// {
//     if (!packet) { error_os("conf:device <%s:%s> nullpointer exception!", _boardname, _name); }
//     strcpy(packet->p_name(), _boardname);
//     strcpy(packet->p_devname(), _name);
//     memcpy(packet->p_pin1(), &_pin, sizeof(uint8_t));
//     return packet;
// }
// ipacket     *               IndoorinoDevice::get_stat           (ipacket * packet)
// {
//     strcpy(packet->p_name(), _boardname);
//     strcpy(packet->p_devname(), _name);
//     return packet;
// }
// 
// ipacket     *               IndoorinoDevice::as_packet          (ipacket * packet)
// {
//     packet = reallocPacket(packet, IBACOM_CONF_DEVSTD);
//     packet = get_conf(packet);
//     return packet;
// }
// 
// 
//      _________________________________________
//      |                                       |
//      |       Device : RELAY                  |
//      |_______________________________________|
// 
// bool                        IndoorinoDeviceRelay::parse         (ipacket * packet)
// {
//     if (IndoorinoDevice::parse(packet))
//     {
//         if (packet->command() == IBACOM_STATUS_RELAY)
//         {
//             if (strcmp(_name, packet->p_devname()) != 0)
//             {
//                 warning_dev("parse:mismatching devnames <%s> <%s>",_name, packet->p_devname());
//                 return false;
//             }
//             _status=*packet->p_status();
//             _relay_stat=*packet->p_level();
//             return true;
//         }
//     }
//     return false;
// }
// 
// ipacket     *               IndoorinoDeviceRelay::get_conf      (ipacket * packet)
// {
//     packet = reallocPacket(packet, IBACOM_CONF_RELAY);
//     packet = IndoorinoDevice::get_conf(packet);
//     return packet;
// }
// 
// ipacket     *               IndoorinoDeviceRelay::get_stat      (ipacket * packet)
// {
//     uint8_t s = _status;
//     uint8_t l = _relay_stat;
//     
//     packet = reallocPacket(packet, IBACOM_STATUS_RELAY);
//     packet = IndoorinoDevice::get_stat(packet);
//     memcpy(packet->p_status(), &s, sizeof(uint8_t));    
//     memcpy(packet->p_level(), &l, sizeof(uint8_t));    
//     return packet;
// }
// 
// ipacket     *               IndoorinoDeviceRelay::as_packet     (ipacket * packet)
// {
//     const uint32_t c = IBACOM_CONF_RELAY;
//     packet = IndoorinoDevice::as_packet(packet);
//     memcpy(packet->p_value1(), &c, sizeof(uint32_t));
//     return packet;
// }
// 
//      _________________________________________
//      |                                       |
//      |       Boards Devices Class List       |
//      |_______________________________________|
// 
// bool                        IndoorinoDeviceList::exist          (const char *key)
// {
//     for (ibasize_t i=0; i<list.len(); i++)
//     {
//         warning_os("comparing <%s> : <%s>",list[i]->name(), key);
//         if (strcmp(list[i]->name(), key) == 0) return true;
//     }
//     return false;
// }
// 
// IndoorinoDevice     *       IndoorinoDeviceList::operator[]     (const char *key)
// {
//     for (ibasize_t i=0; i<list.len(); i++)
//     {
//         if (strcmp(list[i]->name(), key) == 0)
//         {
//             return list[i];
//         }
//     }
//     error_os("FATAL:devices: invalid call for %s", key);
//     return nullptr;
// }
// 
// bool                        IndoorinoDeviceList::remove         (const char *key)
// {
//     for (ibasize_t i=0; i<list.len(); i++)
//     {
//         if (strcmp(list[i]->name(), key) == 0)
//         {
//             return list.rem(i);
//         }
//     }
//     return false;
// }
// 
// bool                        IndoorinoDeviceList::add            (ipacket * p)
// {
//     ibacomm_t com = p->command();
//     IndoorinoDevice * d = nullptr;
//     switch (com)
//     {
//         case IBACOM_CONF_ASENSOR:
//         {
//             break;
//         }
//         case IBACOM_CONF_SWITCH:
//         {
//             break;
//         }
//         case IBACOM_CONF_LDR:
//         {
//             break;
//         }
//         case IBACOM_CONF_DHT22:
//         {
//             break;
//         }
//         case IBACOM_CONF_RELAY:
//         {
//             warning_board("adding RELAY <%s> on pin %u", p->p_devname(), *p->p_pin1());
//             d = new IndoorinoDeviceRelay(p->p_devname(), p->p_name(), p->p_pin1());
//             break;
//         }
//         case IBACOM_CONF_DEVSTD:
//         {
//             warning_dev("adding generic device <%s> on pin %u", p->p_devname(), *p->p_pin1());
//             d = new IndoorinoDevice(p->p_devname(), p->p_name(), p->p_pin1());
//         }
//         default:
//         {
//             warning_dev("adding unknown device <%s> [ibacomm=%u] on pin %u", p->p_devname(), com, *p->p_pin1());
//             d = new IndoorinoDevice(p->p_devname(), p->p_name(), p->p_pin1());
//         }
//     }
//     return list.add(d);
// }
// 
// #else



