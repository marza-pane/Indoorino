/*
 * sensors.h
 *
 *  Created on: Nov 6, 2019. Edited 13 Jun 2020
 *      Author: n00b
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include "devices.h"
#if defined(ARDUINO) && defined(INDOORINO_DEVS)

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
// 
// 
// class   Sensor_DHT22        : public virtualSensor
// {
// protected:
//     DHT *       _dev=nullptr;
//     
// public:
//      Sensor_DHT22   (uint8_t);
//     ~Sensor_DHT22   ();
//     
//     bool        reset               (uint8_t);
//     ipacket *   status              (ipacket *);
//     ipacket **  probe               (ipacket **);
//     
//     uint32_t    getHumidity         (void);
//     uint32_t    getTemperature      (void);
//     uint32_t    value               (void)  { return getTemperature(); };
// 
// };

#endif /* INDOORINO_DEVS */

#endif /* SENSORS_H_ */
