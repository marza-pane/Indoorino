/*
 * sensor.cpp
 *
 *  Created on: Nov 6, 2019. Edited 13 Jun 2020
 *      Author: n00b
 */

#include "../common/common.h"

#if defined(ARDUINO) && defined(INDOORINO_DEVS)
#define FLOAT2UINT_M    100
/*
    virtualSensor      (uint8_t index):virtualDevice(index) {};
    virtualActuator      (uint8_t index):virtualDevice(index) {};

    void        begin               ()=0;
    void        loop                ()=0;

    ipacket *   status              ()=0;   // use _packet as output  
    ipacket *   probe               ()=0;   // use _packet as output

    uint32_t    value               ()=0;
    bool        set                 ()=0;   // this set the device to input value.
*/
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       RELAY                                                       |
//      |___________________________________________________________________|
//

Actuator_Relay::Actuator_Relay(uint8_t index):virtualActuator(index)
{
    if (_status == 0)
    {   
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
        
        char nbuf[LEN_DEVNAME];
        conf.device_name(nbuf, _confindex);
        info_dev("init:RELAY <%s> points to conf.device[%u]: pin[%u]", nbuf, _confindex, _pin);
    }
}

Actuator_Relay::~Actuator_Relay()
{
    digitalWrite(_pin, LOW);
    pinMode(_pin, INPUT);    
}

bool                    Actuator_Relay::reset               (void)
{
    packet::ipacket c;

    char buffname[LEN_DEVNAME];
    uint8_t pin;
    
    packet::ipacket * p = conf.device(&c, _confindex);
    strcpy(buffname, p->p_devname());
    
    if (p->command() != IBACOM_CONF_RELAY)
    {
        error_dev("reset: RELAY <%s> cant be set to dev type [%u].", buffname, p->command());
        return false;
    }
    
    pin = *p->p_pin1();
    
    if (!utils::board::is_pin(pin))
    {
        error_dev("reset:invalid pin [%u] for RELAY <%s>!", pin, buffname);
        _status=1;
        return false;
    }

    _pin = pin;
    _status=0;
    info_dev("reset:device RELAY <%s>: pin[%u]", buffname, _pin);

    digitalWrite(_pin, LOW);    
    pinMode(_pin, OUTPUT);
    
    if (_relay_stat)        {   digitalWrite(_pin, HIGH);   }
    else                    {   digitalWrite(_pin, LOW);    }
    
    debug_dev("RELAY <%s>: device resetted on pin [%u]!", buffname, _pin);
    return true;
}

bool                    Actuator_Relay::set                 (uint32_t value)
{
    if (_status > 0)
    {
        warning_dev("set:can not set device in ERROR state");
        return false;
    }
    
    debug_dev("setting: RELAY (index=%u, pin=%u)",_confindex, _pin);
    char buffname[LEN_DEVNAME];
    conf.device_name(buffname, _confindex);
    
    if (value == 0)
    {
        if (_relay_stat != 0)
        {
            _relay_stat=0;
//             sendReport(1, F("RELAY"), F("%s on pin %u turned OFF"), buffname, _pin);
        }
        digitalWrite(_pin, LOW);
        return true;
    }
    else
    {
        if (_relay_stat == 0)
        {
            _relay_stat=1;
//             sendReport(1, F("RELAY"), F("%s on pin %u turned ON"), buffname, _pin);

        }
        digitalWrite(_pin, HIGH);
        return true;
    }
    return false;
}

void                    Actuator_Relay::send_dev_stat       (void)
{
    packet::ipacket p(IBACOM_STATUS_RELAY);
    
    if (_status > 0)
    {
        warning_dev("Invalid device name!");
        strcpy(p.p_devname(), "INVALID");        
    }
    else    strcpy(p.p_devname(), _name);
    
    
    debug_mem("Free heap @ <send_dev_stat> [%u] KB", utils::board::available_ram());
    
    strcpy(p.p_name(), P2C(BOARD_NAME));
    memcpy(p.p_status(), &_status, sizeof(uint8_t));
    memcpy(p.p_level(),  &_relay_stat, sizeof(uint8_t));
    utils::board::io.send(&p);
        
}



// //      _____________________________________________________________________
// //      |                                                                   |
// //      |       ANALOG SENSORS                                              |
// //      |___________________________________________________________________|
// //
// 
// Sensor_Analog::Sensor_Analog(uint8_t index):virtualSensor(index)
// {
//     
//     uint8_t p = *_config->p_pin();
//     pinMode(p, INPUT);
//     _mode=2;
//     
//     if (*_config->p_param1() == 0)
//     {
//         *_config->p_param1() = FLOAT2UINT_M;
//     }
// 
//     debug_dev("ANALOG %s on pin %u",_config->p_devname(), p);
// }
// 
// Sensor_Analog::~Sensor_Analog()
// {
//     debug_dev("\nAnalog %s:", _config->p_devname()); 
// }
// 
// ipacket         *       Sensor_Analog::status               (ipacket * ptr)
// {
//     ptr = reallocPacket(ptr, IBACOM_STATUS_ASENSOR);
// 
//     strcpy(ptr->p_devname(), _config->p_devname());
//     *ptr->p_svalue1() = value();
//     *ptr->p_status()  = _mode;
//     
//     return ptr;    
// }
// 
// bool                    Sensor_Analog::reset               (uint8_t index)
// {
//     ipacket * p=nullptr;
//     p = conf.device(p, index);
//     if (p->command() == _config->command())
//     {
//         _index=index;
//         
//         memcpy(_config->payload(), p->payload(), p->data_size());
//         delete p;
//         
//         uint8_t pin = *_config->p_pin();
//         pinMode(pin, INPUT);
//         debug_dev("\nAnalog %s: device resetted!",_config->p_devname());
//         return true;
//     }
//     if (p) delete p;
//     return false;
// }
// 
// ipacket         **      Sensor_Analog::probe                (ipacket ** ptr)
// {
//     _config=conf.device(_config, _index);
//     uint32_t variable = value();
//     
//     if (_mode < 2)
//     {
//         return ptr;
//     }
//         
//     ptr[0] = reallocPacket(ptr[0], IBACOM_PROBE_AMBIENT);
// 
//     char type[8];
//     strcpy(type, F2C(F("generic")));    
//     
//     strcpy(ptr[0]->p_devname(), _config->p_devname());
//     strcpy(ptr[0]->p_type(),type);
//     *ptr[0]->p_epoch()  = rtc.epoch();
//     *ptr[0]->p_value() = variable;
//     
//     return ptr;
// 
// }
// 
// uint32_t                Sensor_Analog::value                (void)
// {
//     uint32_t a= *_config->p_param1();
//     uint32_t b= *_config->p_param2();
//     uint32_t p= *_config->p_pin();
//     
//     return b + a * uint32_t(FLOAT2UINT_M * analogRead(p)) / FLOAT2UINT_M / FLOAT2UINT_M;
// }
// 
// 
// 
// 
// //      _____________________________________________________________________
// //      |                                                                   |
// //      |       SWITCH SENSORS                                              |
// //      |___________________________________________________________________|
// //
// 
// Sensor_switch::Sensor_switch(uint8_t index):virtualSensor(index)
// {
//     
//     uint8_t p = *_config->p_pin();
//     pinMode(p, INPUT);
//     _mode=2;
//     
//     debug_dev("SWITCH %s on pin %u",_config->p_devname(), p);
// }
// 
// Sensor_switch::~Sensor_switch()
// {
//     debug_dev("\nSwitch %s:", _config->p_devname()); 
// }
// 
// ipacket         *       Sensor_switch::status               (ipacket * ptr)
// {
//     ptr = reallocPacket(ptr, IBACOM_STATUS_SWITCH);
// 
//     strcpy(ptr->p_devname(), _config->p_devname());
//     *ptr->p_svalue1() = value();
//     *ptr->p_status()  = _mode;
//     
//     return ptr;    
// }
// 
// bool                    Sensor_switch::reset               (uint8_t index)
// {
//     ipacket * p=nullptr;
//     p = conf.device(p, index);
//     if (p->command() == _config->command())
//     {
//         _index=index;
//         
//         memcpy(_config->payload(), p->payload(), p->data_size());
//         delete p;
//         
//         uint8_t pin = *_config->p_pin();
//         pinMode(pin, INPUT);
//         debug_dev("\nSwitch %s: device resetted!",_config->p_devname());
//         return true;
//     }
//     if (p) delete p;
//     return false;
// }
// 
// ipacket         **      Sensor_switch::probe                (ipacket ** ptr)
// {
//     return ptr;
// }
// 
// uint32_t                Sensor_switch::value                (void)
// {
//     uint32_t p= *_config->p_pin();
//     digitalRead(p);
//     return p;    
// }
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
// //      _____________________________________________________________________
// //      |                                                                   |
// //      |       DHT22 SENSOR                                                |
// //      |___________________________________________________________________|
// //
// 
// Sensor_DHT22::Sensor_DHT22(uint8_t index):virtualSensor(index)
// {
//     uint8_t pin = *_config->p_pin();
//     pinMode(pin, INPUT);
//     _mode=2;
//     _dev = new DHT(pin, DHT22);
//     _dev->begin();
// 
//     if (*_config->p_param1() == 0)  *_config->p_param1() = FLOAT2UINT_M;
//     if (*_config->p_param3() == 0)  *_config->p_param3() = FLOAT2UINT_M;
//     
//     debug_dev("\nDHT22 %s on pin %u", _config->p_devname(), pin);
// }
//     
// Sensor_DHT22::~Sensor_DHT22()
// {
//     if (_dev != nullptr)
//     {
//         delete _dev;
//         _dev=nullptr;
//     }
//     debug_dev("\nDHT22 %s:", _config->p_devname()); 
// }
// 
// 
// bool                    Sensor_DHT22::reset               (uint8_t index)
// {
//     ipacket * p=nullptr;
//     p = conf.device(p, index);
//     if (p->command() == _config->command())
//     {
//         uint8_t pin = *p->p_pin();
//         if (pin != *_config->p_pin())
//         {
//             delete _dev;
//             pinMode(pin, INPUT);
//              _dev = new DHT(pin, DHT22);
//             debug_dev("\nDHT22 %s: Changing pin!",_config->p_devname());
//         }
//         
//         _index=index; // funziona così??
//         
//         memcpy(_config->payload(), p->payload(), p->data_size());
//         delete p;
//         
//         debug_dev("\nDHT22 %s: device resetted!",_config->p_devname());
//         return true;
//     }
//     if (p) delete p;
//     return false;
// }
// 
// uint32_t                Sensor_DHT22::getHumidity           (void)
// {
//     uint32_t a= *_config->p_param3();
//     uint32_t b= *_config->p_param4();
//     uint32_t p= *_config->p_pin();
//     char *   m= _config->p_devname();
// 
//     float  val= (a * _dev->readHumidity() * FLOAT2UINT_M) + b;
//         
//     if (isnan(val))
//     {
//         if (_mode != 1)
//         {
//             _mode=1;
//             sendReport(3, m, F("sensor DHT:%s on pin %u OFFLINE"), m, p);
//         }
//         return 0;
//     }
//     else if (val < -270.0)
//     {
//         if (_mode != 0)
//         {
//             _mode=0;
//             sendReport(3, m, F("sensor DHT:%s on pin %u ERROR"), m, p);
//         }
//         return 0;
//     }
//     else
//     {
//         if (_mode != 2)
//         {
//             _mode=2;
//             sendReport(1, m, F("sensor DHT:%s on pin %u ONLINE"), m, p);
//         }
//         
//         debug_dev("\nGET:DHT22:%s on pin %u:", m, p);
//         debug_dev("Humidity [a:%u b:%u]= %u\n",a, b, val);
//         return uint32_t(val);
//     }
// }
// 
// uint32_t                Sensor_DHT22::getTemperature        (void)
// {
//     uint32_t a= *_config->p_param1();
//     uint32_t b= *_config->p_param2();
//     uint32_t p= *_config->p_pin();
//     char *   m= _config->p_devname();
// 
//     float  val= (a * _dev->readTemperature() * FLOAT2UINT_M) + b;
// 
// 
// 
//     if (isnan(val))
//     {
//         if (_mode != 1)
//         {
//             _mode=1;
//             sendReport(3, m, F("sensor DHT:%s on pin %u OFFLINE"), m, p);
//         }
//         return 0;
//     }
//     else if (val < -270.0)
//     {
//         if (_mode != 0)
//         {
//             _mode=0;
//             sendReport(3, m, F("sensor DHT:%s on pin %u ERROR"), m, p);
//         }
//         return 0;
//     }
//     else
//     {
//         if (_mode != 2)
//         {
//             _mode=2;
//             sendReport(1, m, F("sensor DHT:%s on pin %u ONLINE"), m, p);
//         }
//         
//         debug_dev("\nGET:DHT22:%s on pin %u:", m, p);
//         debug_dev("Temperature  [a:%u b:%u]= %u\n",a, b, val);
//         return val;
//     }
// }
// 
// ipacket         *       Sensor_DHT22::status                (ipacket * ptr)
// {
// 
//     ptr = reallocPacket(ptr, IBACOM_STATUS_DHT22);   
// 
//     strcpy(ptr->p_devname(), _config->p_devname());
//     *ptr->p_svalue1()       = getTemperature();
//     *ptr->p_svalue2()       = getHumidity();
//     *ptr->p_status()        = _mode;
//     
//     return ptr;
// }
// 
// ipacket         **      Sensor_DHT22::probe                (ipacket ** ptr)
// {
//     
//     _config=conf.device(_config, _index);
//     
//     if (_mode < 2)  return ptr;
//         
//     ptr[0] = reallocPacket(ptr[0], IBACOM_PROBE_AMBIENT);
//     
//     char type[12];
//     strcpy(type, F2C(F("temperature")));  
//     strcpy(ptr[0]->p_devname(), _config->p_devname());
//     strcpy(ptr[0]->p_type(), type);
//     *ptr[0]->p_epoch()  = rtc.epoch();
//     *ptr[0]->p_value() = getTemperature();;
//     
//     ptr[1] = reallocPacket(ptr[1], IBACOM_PROBE_AMBIENT);
//     
//     strcpy(type, F2C(F("humidity")));  
//     strcpy(ptr[1]->p_devname(), _config->p_devname());
//     strcpy(ptr[1]->p_type(), type);
//     *ptr[1]->p_epoch()  = rtc.epoch();
//     *ptr[1]->p_value() = getHumidity();
//     
//     return ptr;
// 
// }

    #endif /* INDOORINO_DEVS */











