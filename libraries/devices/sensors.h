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
};




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
// class   Sensor_switch       : public virtualSensor
// {
// public:
// 
//     
//      Sensor_switch      (uint8_t);
//     ~Sensor_switch      ();
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
// 
// class   Sensor_LDR          : public Sensor_Analog
// {
// public:    
//      Sensor_LDR (uint8_t);
//     ~Sensor_LDR ();
//         
//     bool        reset               (uint8_t);
//     ipacket *   status              (ipacket *);
//     ipacket **  probe               (ipacket **);
//     
//     uint32_t    value               (void);
// };


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
    
    void        send_probe          (void) {};
    void        send_dev_stat       (void);
    
    int32_t     value               (void)  { return uint32_t(getHumidity()); };
    int32_t     getHumidity         (void);
    int32_t     getTemperature      (void);

};

#endif /* INDOORINO_DEVS */

#endif /* SENSORS_H_ */
