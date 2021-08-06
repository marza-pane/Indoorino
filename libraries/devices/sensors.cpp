/*
 * sensor.cpp
 *
 *  Created on: Nov 6, 2019. Edited 13 Jun 2020
 *      Author: n00b
 */

#include "../common/indoorino.h"

    /*
     * STATUS:
     * 0 = no errors
     * 1 = invalid pin
     * 2 = offline
     * 3 = io error
     * 4 = type error
     */

#if defined(ARDUINO) && defined(INDOORINO_DEVS)


//      _____________________________________________________________________
//      |                                                                   |
//      |       RELAY                                                       |
//      |___________________________________________________________________|
//

Actuator_Relay::Actuator_Relay(uint8_t index):virtualActuator(index)
{
    if (_status == 0)
    {
        info_dev("init:RELAY <%s> points to conf.device[%u]: pin[%u]", _name, _confindex, _pin);
        this->reset();
    }
}

Actuator_Relay::~Actuator_Relay()
{
    digitalWrite(_pin, LOW);
    pinMode(_pin, INPUT);    
}

bool                    Actuator_Relay::reset               (void)
{
    digitalWrite(_pin, LOW);    
    pinMode(_pin, INPUT);
    
    uint8_t i;
    uint8_t pin;
    
    packet::ipacket c;
    packet::ipacket * p = conf.device(&c, _confindex);
    
    if (p->command() != IBACOM_CONF_RELAY)
    {
        error_dev("init:RELAY <%s>: cant be set to dev type [%u].", _name, p->command());
        _status=4;
        goto fail_reset;
    }
    
    pin = *p->p_pin1();
    i = conf.indexFromPin(pin);
    
    if (!utils::board::is_pin(pin))
    {
        error_dev("init:RELAY <%s>: invalid pin [%u]!", _name, pin);
        _status=1;
        goto fail_reset;
    }
    
    if (i != _confindex)
    {
        error_dev("init:RELAY <%s>: pin [%u] already in use for [%u]!", _name, pin, i);
        _status=1;
        goto fail_reset;        
    }
    
    _pin = pin;
    _status=0;
    debug_dev("reset:device RELAY <%s>: pin [%u]", _name, _pin);

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);    
    
    if (_relay_stat)        {   digitalWrite(_pin, HIGH);   }
    else                    {   digitalWrite(_pin, LOW);    }
    
    info_dev("RELAY device <%s> ready on pin [%u]!", _name, _pin);
    return true;
    
    fail_reset:
    {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);    
        return false;
    }
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
    strcpy_P(p.p_name(), BOARD_NAME);
    
    if (_status == 1)
    {
        warning_dev("Invalid device name!");
        strcpy(p.p_devname(), "INVALID");        
    }
    else    strcpy(p.p_devname(), _name);
    
    
//     debug_mem("Free heap @ <send_dev_stat> [%u] KB", utils::board::available_ram());
    
    memcpy(p.p_status(), &_status, sizeof(uint8_t));
    memcpy(p.p_level(),  &_relay_stat, sizeof(uint8_t));
    utils::board::io.send(&p);
        
}

void                    Actuator_Relay::send_probe          (void)
{
    
    if (_status != 0) return;

    packet::ipacket p(IBACOM_PROBE_AMBIENT);
    
    strcpy_P(p.p_board(), BOARD_NAME);
    strcpy(p.p_devname(), _name);    
    
    iEpoch_t e = rtc.epoch();
    
    memcpy(p.p_epoch(), &e, sizeof(uint32_t));
    strcpy_P(p.p_desc1(), VLABEL_relay);
    memcpy(p.p_value1(),  &_relay_stat, sizeof(uint8_t));
    utils::board::io.send(&p);

    
}


//      _____________________________________________________________________
//      |                                                                   |
//      |       SWITCH SENSORS                                              |
//      |___________________________________________________________________|
//

Sensor_Switch::Sensor_Switch(uint8_t index):virtualSensor(index)
{
    if (_status == 0)
    {
        info_dev("init:SWITCH <%s> points to conf.device[%u]: pin[%u]", _name, _confindex, _pin);
        this->reset();
    }
}

Sensor_Switch::~Sensor_Switch() {}

bool                    Sensor_Switch::reset               (void)
{
    uint8_t i;
    uint8_t pin;
    
    packet::ipacket c;
    packet::ipacket * p = conf.device(&c, _confindex);
    
    
    
    if (!((p->command() == IBACOM_CONF_SWITCH) || (p->command() == IBACOM_CONF_FLOODSWITCH) || (p->command() == IBACOM_CONF_RAINSWITCH) ))
    {
        error_dev("init:SWITCH <%s>: cant be set to dev type [%u].", _name, p->command());
        _status=4;
        goto fail_reset;
    }
    
    
    pin = *p->p_pin1();
    i = conf.indexFromPin(pin);
    
    if (!utils::board::is_pin(pin))
    {
        error_dev("init:SWITCH <%s>: invalid pin [%u]!", _name, pin);
        _status=1;
        goto fail_reset;
    }
    
    if (i != _confindex)
    {
        error_dev("init:SWITCH <%s>: pin [%u] already in use for [%u]!", _name, pin, i);
        _status=1;
        goto fail_reset;        
    }
    
    _pin = pin;
    _status=0;
    debug_dev("reset:device SWITCH <%s>: pin [%u]", _name, _pin);
    pinMode(_pin, INPUT);

    fail_reset:
    {
        if (_pin)
            pinMode(_pin, INPUT);
        return false;
    }
}

void                    Sensor_Switch::send_dev_stat        (void)
{
    packet::ipacket p(IBACOM_STATUS_SWITCH);

    if (_status == 1)
    {
        warning_dev("Invalid device name!");
        strcpy(p.p_devname(), "INVALID");        
    }
    else    strcpy(p.p_devname(), _name);
    
    uint8_t R=this->value();
    
    strcpy(p.p_name(), P2C(BOARD_NAME));
    memcpy(p.p_status(), &_status, sizeof(uint8_t));
    memcpy(p.p_level(), &R, sizeof(uint8_t));
    
    utils::board::io.send(&p);
}

int32_t                 Sensor_Switch::value                (void)
{    
    return int32_t(digitalRead(_pin));    
}

void                    Sensor_Switch::send_probe           (void)
{
    if (_status != 0) return;

    packet::ipacket p(IBACOM_PROBE_AMBIENT);
    
    strcpy_P(p.p_board(), BOARD_NAME);
    strcpy(p.p_devname(), _name);    
    
    iEpoch_t e = rtc.epoch();
    uint32_t v=this->value();
    
    memcpy(p.p_epoch(), &e, sizeof(uint32_t));
    strcpy_P(p.p_desc1(), VLABEL_switch);
    memcpy(p.p_value1(),  &v, sizeof(uint32_t));
    utils::board::io.send(&p);
    
}


void                    SwitchSensor_Flood::loop            (void)
{
    
    int32_t R = this->value();
    
    if (_on_alarm)
    {
//         if ( (R) && (millis() > _last_alarm + DELAY_ALARM_REPEAT))
        if ( (R) && (millis() > _last_alarm + 3000))
        {
            _last_alarm=millis();
            this->send_alarm();
            alert_dev("Repeating FLOOD alarm!");
        }
//         else if ( (!R) && (millis() > _last_alarm + DELAY_ALARM_COOLDOWN) )
        else if ( (!R) && (millis() > _last_alarm + 3000) )
        {
            _on_alarm = false;
            _last_alarm=millis();
            this->send_dev_stat();
            alert_dev("Clearing FLOOD alarm!");
        }
    }
    else
    {
        if (R)
        {
            alert_dev("Sending FLOOD alarm!");
            _on_alarm = true;
            _last_alarm=millis();
            this->send_alarm();            
        }
        
    }
}

void                    SwitchSensor_Flood::send_dev_stat   (void)
{
    packet::ipacket p(IBACOM_STAT_FLOODSWITCH);

    if (_status == 1)
    {
        warning_dev("Invalid device name!");
        strcpy(p.p_devname(), "INVALID");        
    }
    else    strcpy(p.p_devname(), _name);
    
    uint8_t R=this->value();
    
    strcpy(p.p_name(), P2C(BOARD_NAME));
    memcpy(p.p_status(), &_status, sizeof(uint8_t));
    memcpy(p.p_level(), &R, sizeof(uint8_t));
    
    utils::board::io.send(&p);    
}

int32_t                 SwitchSensor_Flood::value           (void)
{
    return int32_t(digitalRead(_pin));
}

void                    SwitchSensor_Flood::send_alarm      (void)
{
    if (_status != 0)
    {
        warning_dev("Invalid device for ALARM!");
        return;
    }

    packet::ipacket p(IBACOM_FLOOD_ALARM);

    strcpy(p.p_board(), P2C(BOARD_NAME));
    strcpy(p.p_devname(), _name);
        
    int32_t     value=1;
    uint32_t    epoch=rtc.epoch();
    
    memcpy(p.p_epoch(), &epoch, sizeof(uint32_t));
    memcpy(p.p_value1(), &value, sizeof(int32_t));

    utils::board::io.send(&p);        
}
//      _____________________________________________________________________
//      |                                                                   |
//      |       DHT22 SENSOR                                                |
//      |___________________________________________________________________|
//

Sensor_DHT22::Sensor_DHT22(uint8_t index):virtualSensor(index)
{
    info_dev("init: <%s> points to conf.device[%u]: pin [%u]", _name, _confindex, _pin);
    this->reset();
}
    
Sensor_DHT22::~Sensor_DHT22()
{
    debug_dev("\nDHT22 %s:", _name); 
}

bool                    Sensor_DHT22::reset                 (void)
{
    /* Shut down pin */

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    
    packet::ipacket c;
    char buffname[LEN_DEVNAME];
    uint8_t pin;
    uint8_t i;
    
    /* Point config packet */
    
    packet::ipacket * p = conf.device(&c, _confindex);
    strcpy(buffname, p->p_devname());
        
    /* Checking packet */

    if (p->command() != IBACOM_CONF_DHT22)
    {
        error_dev("init:DHT22 <%s>: cant be set to dev type [%u]!", buffname, p->command());
        _status=4;
        return false;
    }

    pin = *p->p_pin1();
    
    if (!utils::board::is_pin(pin))
    {
        error_dev("init:DHT22 <%s>: invalid pin [%u]!", buffname, pin);
        _status=1;
        return false;
    }
    
    i = conf.indexFromPin(pin);
    if (i != _confindex)
    {
        error_dev("init:RELAY <%s>: pin [%u] already in use for [%u]!", _name, pin, i);
        _status=1;
        return false;        
    }

    /* Set pin */
    
    _pin = pin;
    _status=0;
    debug_dev("init: device DHT22 <%s> on pin [%u]", buffname, _pin);
    
    /* Set correction parameters*/

    if (*p->p_param1() != 0)    _tempK = *p->p_param1();
    _tempC = *p->p_param2();
    
    if (*p->p_param3() == 0)    _humiK = *p->p_param3();
    _humiC= *p->p_param4();

    /* Set limits */

//     if ( *p->p_param5() != *p->p_param6() )
//     {
//         _alert_T_upper = *p->p_param5();
//         _alert_T_lower = *p->p_param6();
//     }
//     else
//     {
//         alert_dev("init:DHT22 <%s>: ignored temperature limits [%d:%d]", buffname, *p->p_param7(), *p->p_param8());
//     }
// 
//     if ( (*p->p_param7() != *p->p_param8()) && (*p->p_param7() >= 0) && (*p->p_param8() >= 0) )
//     {
//         _alert_H_upper = *p->p_param7();
//         _alert_H_lower = *p->p_param8();
//     }
//     else
//     {
//         alert_dev("init:DHT22 <%s>: ignored humidity limits [%d:%d]", buffname, *p->p_param7(), *p->p_param8());
//     }

    /* Init new DHT class */

    utils::wait(50);
   
    _dev.init(_pin, DHT22);
    _dev.begin();
    
    utils::wait(50);

    debug_dev("DHT22 sensor <%s> ready on pin [%u]!", buffname, _pin);
    return true;        
    
}

int32_t                 Sensor_DHT22::getHumidity           (void)
{
    if (_status == 1)
    {
        warning_dev("Calling %s.getHumidity() on invalid pin [%u]", _name, _pin);
        return 0;
    }
    
    double value_raw = _dev.readHumidity();
    
    if (isnan(value_raw))
    {
        if (_status != 2)
        {
            _status=2;
    
            error_dev("getHumidity: sensor %s returned NaN", _name);
//             sendReport(3, m, F("sensor DHT:%s on pin %u OFFLINE"), m, p);
        }
        return 0;
    }
    
    if (value_raw < 0)
    {
        if (_status != 3)
        {
            _status=3;

            error_dev("getHumidity: sensor %s returned negative value [%3.3f]", _name, value_raw);
//             sendReport(3, m, F("sensor DHT:%s on pin %u ERROR"), m, p);
        }
        return 0;
    }
    
    if (_status == 2 || _status == 3)
    {
        _status=0;
        error_dev("getHumidity: sensor %s is ONLINE", _name);
    }
    
//     int32_t value_int = int32_t(( (double(1 / FLOAT2UINT_M) * double(_humiK)) * value_raw + ( double(1 / FLOAT2UINT_M) * double(_humiC)) ) * FLOAT2UINT_M);
    
    int32_t value_int = value_raw * FLOAT2UINT_M;
    
//     info_dev("%s.getHumidity() returned [%e] [%d]", _name, value_raw, value_int);
    
    if (value_int < 0)
    {
        
        if (_status != 3)
        {
            _status=3;

            error_dev("getHumidity: sensor %s returned negative value [%d]", _name, value_int);
//             sendReport(3, m, F("sensor DHT:%s on pin %u ERROR"), m, p);
        }        
    }

    if (value_int >= WARNING_HUMIDITY_UPPER * FLOAT2UINT_M)
    {
        error_dev("getHumidity: sensor %s: value [%u] over SYSTEM max [%u]", _name, value_int, WARNING_HUMIDITY_UPPER);
        // send alarm
    }
    if (value_int <= WARNING_HUMIDITY_LOWER * FLOAT2UINT_M)
    {
        error_dev("getHumidity: sensor %s: value [%u] below SYSTEM min [%u]", _name, value_int, WARNING_HUMIDITY_LOWER);
        // send alarm
    }
    
    if (value_int >= _alert_H_upper)
    {
        error_dev("getHumidity: sensor %s: value [%u] over SENSOR max [%u]", _name, value_int, WARNING_HUMIDITY_UPPER);
        // send alarm
    }
    if (value_int <= _alert_H_lower)
    {
        error_dev("getHumidity: sensor %s: value [%u] under SENSOR min [%u]", _name, value_int, WARNING_HUMIDITY_LOWER);
        // send alarm
    }
    
    return value_int;
}



int32_t                 Sensor_DHT22::getTemperature        (void)
{
    if (_status == 1)
    {
        warning_dev("Calling %s.getTemperature() on invalid pin [%u]", _name, _pin);
        return 0;
    }
    
    double value_raw = _dev.readTemperature();
    
    if (isnan(value_raw))
    {
        if (_status != 2)
        {
            _status=2;
    
            error_dev("getTemperature: sensor %s returned NaN", _name);
//             sendReport(3, m, F("sensor DHT:%s on pin %u OFFLINE"), m, p);
        }
        return 0;
    }
    
    if (value_raw < 0)
    {
        if (_status != 3)
        {
            _status=3;

            error_dev("getTemperature: sensor %s returned negative value [%3.3f]", _name, value_raw);
//             sendReport(3, m, F("sensor DHT:%s on pin %u ERROR"), m, p);
        }
        return 0;
    }
    
    if (_status == 2 || _status == 3)
    {
        _status=0;
        error_dev("getTemperature: sensor %s is ONLINE", _name);
    }
    
//     int32_t value_int = int32_t(( (double(1 / FLOAT2UINT_M) * double(_humiK)) * value_raw + ( double(1 / FLOAT2UINT_M) * double(_humiC)) ) * FLOAT2UINT_M);
    
    int32_t value_int = value_raw * FLOAT2UINT_M;
    
//     info_dev("%s.getTemperature() returned [%e] [%d]", _name, value_raw, value_int);
    


    if (value_int >= WARNING_TEMPERATURE_UPPER * FLOAT2UINT_M)
    {
        error_dev("getTemperature: sensor %s: value [%u] over SYSTEM max [%u]", _name, value_int, WARNING_HUMIDITY_UPPER);
        // send alarm
    }
    if (value_int <= WARNING_TEMPERATURE_LOWER * FLOAT2UINT_M)
    {
        error_dev("getTemperature: sensor %s: value [%u] below SYSTEM min [%u]", _name, value_int, WARNING_HUMIDITY_LOWER);
        // send alarm
    }
    
    if (value_int >= _alert_H_upper)
    {
        error_dev("getTemperature: sensor %s: value [%u] over SENSOR max [%u]", _name, value_int, WARNING_HUMIDITY_UPPER);
        // send alarm
    }
    if (value_int <= _alert_H_lower)
    {
        error_dev("getHumidity: sensor %s: value [%u] under SENSOR min [%u]", _name, value_int, WARNING_HUMIDITY_LOWER);
        // send alarm
    }
    
    return value_int;

}

void                    Sensor_DHT22::send_dev_stat         (void)
{
    packet::ipacket p(IBACOM_STATUS_DHT22);
    
    if (_status == 1)
    {
        warning_dev("Invalid device name!");
        strcpy(p.p_devname(), "INVALID");        
    }
    else    strcpy(p.p_devname(), _name);
        
    debug_mem("Free heap @ <send_dev_stat> [%u] KB", utils::board::available_ram());

    int32_t T = this->getTemperature();
    int32_t H = this->getHumidity();
    
    strcpy(p.p_name(), P2C(BOARD_NAME));
    memcpy(p.p_status(), &_status, sizeof(uint8_t));
    memcpy(p.p_value1(), &T, sizeof(int32_t));
    memcpy(p.p_value2(), &H, sizeof(int32_t));

    utils::board::io.send(&p);
}

void                    Sensor_DHT22::send_probe            (void)
{
    if (_status != 0) return;

    packet::ipacket p(IBACOM_PROBE_AMBIENT);
    
    strcpy_P(p.p_board(), BOARD_NAME);
    strcpy(p.p_devname(), _name);    
    
    iEpoch_t e = rtc.epoch();
    int32_t t = getTemperature();
    int32_t h = getHumidity();
    
    memcpy(p.p_epoch(), &e, sizeof(uint32_t));
    
    strcpy_P(p.p_desc1(), VLABEL_temperature);
    memcpy(p.p_value1(),  &t, sizeof(int32_t));
    
    strcpy_P(p.p_desc2(), VLABEL_humidity);
    memcpy(p.p_value2(),  &h, sizeof(int32_t));
    
    utils::board::io.send(&p);
    
}
//      _____________________________________________________________________
//      |                                                                   |
//      |       PM25 DUST SENSOR                                            |
//      |___________________________________________________________________|
//

Sensor_PM25dust::Sensor_PM25dust    (uint8_t index):virtualSensor(index)
{
    info_dev("init: <%s> points to conf.device[%u]: pin [%u]", _name, _confindex, _pin);
    this->reset();    
}

bool                    Sensor_PM25dust::reset              (void)
{
    /* Shut down led pin */

    digitalWrite(_pin, HIGH);    
    pinMode(_pin, INPUT);
    
    packet::ipacket c;
    char buffname[LEN_DEVNAME];
    uint8_t ledpin;
    uint8_t i;
    
    /* Point config packet */
    
    packet::ipacket * p = conf.device(&c, _confindex);
    strcpy(buffname, p->p_devname());
        
    /* Checking packet */

    if (p->command() != IBACOM_CONF_DUSTPM25)
    {
        error_dev("init:DUSTPM25 <%s>: cant be set to dev type [%u]!", buffname, p->command());
        _status=4;
        return false;
    }
    
    ledpin = *p->p_pin1();
    
    if (!utils::board::is_pin(ledpin))
    {
        error_dev("init:DHT22 <%s>: invalid pin [%u]!", buffname, ledpin);
        _status=1;
        return false;
    }
    
    i = conf.indexFromPin(ledpin);
    if (i != _confindex)
    {
        error_dev("init:RELAY <%s>: pin [%u] already in use for [%u]!", _name, ledpin, i);
        _status=1;
        return false;        
    }

    /* Set pin */
    
    _pin = ledpin;
    _analpin = *p->p_pin2();
    _status=0;
    debug_dev("init: device DUSTPM25 <%s> on pins L:[%u] A:[%u]", buffname, _pin, _analpin);
    
    /* Set correction parameters*/
    
    if (*p->p_timeout1() != 0)  _sampling_time = *p->p_timeout1();
    if (*p->p_timeout2() != 0)  _delta_time = *p->p_timeout2();

    if (*p->p_param1()   != 0)  _dustK = *p->p_param1();    
    if (*p->p_param3()   != 0)  _alert = *p->p_param3();
    _dustC= *p->p_param2();

    pinMode(_pin, OUTPUT);
//     pinMode(_analpin, INPUT);

    return true;
}

void                    Sensor_PM25dust::send_dev_stat      (void)
{
    packet::ipacket p(IBACOM_STATUS_DUSTPM25);
    
    if (_status == 1)
    {
        warning_dev("Invalid device name!");
        strcpy(p.p_devname(), "INVALID");        
    }
    else    strcpy(p.p_devname(), _name);
        
    alert_mem("Free heap @ <send_dev_stat> [%u] KB", utils::board::available_ram());

    int32_t D = this->value();
    
    strcpy(p.p_name(), P2C(BOARD_NAME));
    memcpy(p.p_status(), &_status, sizeof(uint8_t));
    memcpy(p.p_value1(), &D, sizeof(int32_t));

    utils::board::io.send(&p);    
}

void                    Sensor_PM25dust::send_probe         (void)
{
    if (_status != 0) return;

    packet::ipacket p(IBACOM_PROBE_AMBIENT);
    
    strcpy_P(p.p_board(), BOARD_NAME);
    strcpy(p.p_devname(), _name);    
    
    iEpoch_t e = rtc.epoch();
    int32_t v = value();
    
    memcpy(p.p_epoch(), &e, sizeof(uint32_t));
    
    strcpy_P(p.p_desc1(), VLABEL_dust);
    memcpy(p.p_value1(),  &v, sizeof(int32_t));
        
    utils::board::io.send(&p);
    
}

int32_t                 Sensor_PM25dust::value              (void)
{
    /*
        3000 + = Very Bad
        1050-3000 = Bad
        300-1050 = Ordinary
        150-300 = Good
        75-150 = Very Good
        0-75 = Tiptop
    */

    float K = float(_dustK);
    float C = float(_dustC);
    float value_raw;
    
    if (_status == 1)
    {
        warning_dev("Calling %s.getValue() on invalid pin [%u]", _name, _pin);
        return 0;
    }

//     SerialDebug.print("ALALOG:READ: pin = ");
//     SerialDebug.print(_pin);
//     SerialDebug.print(" - analogical pin = ");
//     SerialDebug.println(_analpin);
//     SerialDebug.print("delay sample  = ");
//     SerialDebug.print(_sampling_time);
//     SerialDebug.print(" - delta time = ");
//     SerialDebug.println(_delta_time);
//     SerialDebug.print(" - dust K = ");
//     SerialDebug.print(_dustK);
//     SerialDebug.print(" : ");
//     SerialDebug.println(float(_dustK/FLOAT2UINT_M));
//     SerialDebug.print(" - dust C = ");
//     SerialDebug.print(_dustC);
//     SerialDebug.print(" : ");
//     float C = float(_dustC);
//     C = C / float(FLOAT2UINT_M);
//     SerialDebug.println(C);
//     info_dev("C is %e", C);

    digitalWrite(_pin, LOW); // power on the LED

    delayMicroseconds(_sampling_time);
    value_raw = analogRead(_analpin); // read the dust value
    delayMicroseconds(_delta_time);
    
    digitalWrite(_pin,HIGH); // turn the LED off
    utils::wait(10);
    
    // 0 - 5V mapped to 0 - 1023 integer values
    // recover voltage
    
//     SerialDebug.print("ALALOG:READ: raw = ");
//     SerialDebug.println(value_raw);
    
    C = C / float(FLOAT2UINT_M);
    K = K / float(FLOAT2UINT_M);
    
    float dustDensity = K * (value_raw * (5.0 / 1024.0)) - C;

//     SerialDebug.print("ALALOG:READ: parsed = ");
//     SerialDebug.println(dustDensity);

    int32_t value_int = FLOAT2UINT_M * dustDensity;
    
    if (_alert > 0 && value_int >= _alert * FLOAT2UINT_M)
    {
        error_dev("getValue: sensor %s: value [%u] over SYSTEM max [%u]", _name, value_int, _alert*FLOAT2UINT_M);
        // send alarm
    }

//     SerialDebug.print("ALALOG:READ: value = ");
//     SerialDebug.println(value_int);

    
    if (value_int <= 0)
    {
        if (_status != 3)
        {
            _status=3;

            error_dev("getValue: sensor %s returned invalid value [%3.3f]", _name, value_raw);
//             sendReport(3, m, F("sensor DHT:%s on pin %u ERROR"), m, p);
        }
    }
    else
    {
        if (_status == 2 || _status == 3)
        {
            _status=0;
            error_dev("getValue: sensor %s is ONLINE", _name);
        }
    }
    
    return value_int;    
}
























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
// 













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


    #endif /* INDOORINO_DEVS */











