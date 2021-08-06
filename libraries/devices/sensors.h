/*
 * sensors.h
 *
 *  Created on: Nov 6, 2019. Edited 13 Jun 2020
 *      Author: n00b
 */

#ifndef SENSORS_H_
#define SENSORS_H_

/*
 * To avoid using floats and doubles, real numbers are multiplied by FLOAT2UINT_M and casted into int32_t
 * 
 * STATUS:
 * 0 = no errors
 * 1 = invalid pin
 * 2 = offline
 * 3 = io error
 * 4 = type error
 * 5 = not exist?
 * 
*/
    

#include "devices.h"
#define FLOAT2UINT_M    100
#if defined(ARDUINO) && defined(INDOORINO_DEVS)


#define WARNING_HUMIDITY_UPPER 90
#define WARNING_HUMIDITY_LOWER 10

#define WARNING_TEMPERATURE_UPPER 100
#define WARNING_TEMPERATURE_LOWER -20

#define ALERT_TEMPERATURE_UPPER 40
#define ALERT_TEMPERATURE_LOWER  0

#define ALERT_HUMIDITY_UPPER 100
#define ALERT_HUMIDITY_LOWER 0

#define DELAY_ALARM_REPEAT      300000
#define DELAY_ALARM_COOLDOWN    60000

// const char VLABEL_relay[] PROGMEM = "relay state";
// const char VLABEL_switch[] PROGMEM = "switch state";
// 
// const char VLABEL_humidity[] PROGMEM = "humidity";
// const char VLABEL_temperature[] PROGMEM = "temperature";
// const char VLABEL_dust[] PROGMEM = "air dust";

class   Actuator_Relay      : public virtualActuator
{
private:
    uint8_t     _relay_stat=0;
public:
    
             Actuator_Relay     (uint8_t);
    virtual ~Actuator_Relay     ();
    
    void        loop                (void) {}
    bool        reset               (void);    
    bool        set                 (uint32_t);
    void        send_dev_stat       (void);
    void        send_probe          (void);
};



class   Sensor_Switch       : public virtualSensor
{
public:

    
     Sensor_Switch      (uint8_t);
    ~Sensor_Switch      ();
    
    bool            reset               (void);
    void            send_dev_stat       (void);
    virtual void    send_probe          (void);
    int32_t         value               (void);
};


class SwitchSensor_Flood    : public Sensor_Switch
{
private:
    bool        _on_alarm=false;
    iEpoch_t    _last_alarm=0;
    void        send_alarm          (void);
public:
    SwitchSensor_Flood(uint8_t i):Sensor_Switch(i) {};
    
    void        loop                (void);
    void        send_dev_stat       (void);
    int32_t     value               (void);
};


class SwitchSensor_Rain     : public Sensor_Switch
{
private:
    bool    on_alarm=false;
public:
    SwitchSensor_Rain(uint8_t i):Sensor_Switch(i) {};
    void        loop                (void);
    void        send_dev_stat       (void);
    void        send_probe          (void);
    int32_t     value               (void);
    
};


class   Sensor_LDR          : public virtualSensor
{
public:    
     Sensor_LDR (uint8_t);
    ~Sensor_LDR ();
        
    bool        reset               (void);
    void        send_dev_stat       (void);
    void        send_probe          (void);
    int32_t     value               (void);
};


class   Sensor_DHT22        : public virtualSensor
{
protected:
    DHT         _dev;
    
    int32_t     _tempK = FLOAT2UINT_M;
    int32_t     _tempC = 0;
    int32_t     _humiK = FLOAT2UINT_M;
    int32_t     _humiC = 0;
    
    int32_t     _alert_T_upper=ALERT_TEMPERATURE_UPPER * FLOAT2UINT_M;
    int32_t     _alert_T_lower=ALERT_TEMPERATURE_LOWER * FLOAT2UINT_M;
    
    int32_t     _alert_H_upper=ALERT_HUMIDITY_UPPER * FLOAT2UINT_M;
    int32_t     _alert_H_lower=ALERT_HUMIDITY_LOWER * FLOAT2UINT_M;
    
public:
     Sensor_DHT22   (uint8_t);
    ~Sensor_DHT22   ();
    
    void        loop                (void) {}
    bool        reset               (void);
    
    void        send_dev_stat       (void);
    
    int32_t     value               (void)  { return uint32_t(getHumidity()); };
    int32_t     getHumidity         (void);
    int32_t     getTemperature      (void);
    void        send_probe          (void);

};

class   Sensor_PM25dust     : public virtualSensor
{
protected:
    /*
        values:
        3000 + = Very Bad
        1050-3000 = Bad
        300-1050 = Ordinary
        150-300 = Good
        75-150 = Very Good
        0-75 = Tiptop
    */    
    uint8_t     _analpin=0;
    
    int32_t     _sampling_time = 40;
    int32_t     _delta_time = 280;
    int32_t     _dustK = 170 * FLOAT2UINT_M;    /* default values from https://wiki.keyestudio.com/Ks0196_keyestudio_PM2.5_Dust_Sensor_Module */
    int32_t     _dustC = -10;
    int32_t     _alert = 1000;

public:
     Sensor_PM25dust    (uint8_t);
    ~Sensor_PM25dust    () {};

    void        loop                (void) {}
    bool        reset               (void);
    
    void        send_dev_stat       (void);
    void        send_probe          (void);
    int32_t     value               (void);
    
};


#endif /* INDOORINO_DEVS */

#endif /* SENSORS_H_ */
















// class   Sensor_Analog       : public virtualSensor
// {
// public:
// 
//     
//      Sensor_Analog      (uint8_t);
//     ~Sensor_Analog      ();
//     
//     bool        reset               (uint8_t);
//     ipacket *   status              (ipacket *);
//     ipacket **  probe               (ipacket **);
// 
//     uint32_t    value               (void);
// };
// 
// 
// 
