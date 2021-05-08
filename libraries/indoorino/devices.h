/*
 * devices.h
 *
 *  Created 3.0.7 on: Nov 6, 2019. Edited 13 Jun 2020 (BOARDS)
 *  Created 3.0.9 on: Nov 10, 2020 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_DEVICES_H_
#define _SRC_DEVICES_H_

#if defined (INDOORINO_NETWORK)

#include <deque>
#include "packets/ipacket.h"

    /*
     * STATUS:
     * 0 = no errors
     * 1 = invalid pin
     * 2 = offline
     * 3 = io error
     * 4 = type error
     */

namespace indoorino
{
    class Probe;

    class DeviceTemplate
    {
        
    protected:
        std::deque<Probe>           _probes;
        packet::ipacket             _conf;
        packet::ipacket             _stat;
        char                        _type[LEN_NAME];
        
    public:
                 DeviceTemplate(packet::ipacket *);
        
        virtual ~DeviceTemplate() {};
        
        virtual void        parse           (packet::ipacket *);
        
        const char  *       type            () { return (const char *)_type; }
        const char  *       name            () { return (const char *)_conf.p_devname(); }
        const char  *       boardname       () { return (const char *)_conf.p_name();    }
        
        void                set_offline     () { *_stat.p_status() = 2;     }
        bool                is_connected    () { return (*_stat.p_status() == 0); }

        uint8_t             pin         () { return *_conf.p_pin1(); }
        packet::ipacket&    config      () { return _conf; }
        packet::ipacket&    status      () { return _stat; }
    };
    
    namespace devices
    {
            
        class Relay : public DeviceTemplate
        {
        private:
            uint8_t         _relay_stat=0;
        public:
            Relay(packet::ipacket *);

//             void            parse           (packet::ipacket *);

            uint8_t         relay_status()  { return _relay_stat; }
        };
        
        class DHT22 : public DeviceTemplate
        {
        private:
        public:
            DHT22(packet::ipacket *);
            
//             void            parse           (packet::ipacket *);
        };

        
        
        
    } /* namespace : devices */

    
    class DeviceList
    {
    private:
        std::vector<DeviceTemplate> _list;
    public:
        DeviceList()  {};
        ~DeviceList() { clear(); }
        
        
        DeviceTemplate      &   operator[]      (const int  i);
        DeviceTemplate      &   operator()      (const char *);
        bool                    exist           (const char *);
        bool                    remove          (const char *);
        bool                    add             (packet::ipacket *);
        
        iSize_t                 count       () { return _list.size();  }
        bool                    is_empty    () { return _list.empty(); }
        void                    clear       () { _list.clear(); }
        
        friend class BoardTemplate;
    };

    
    
    
    class Probe
    {
    protected:
        std::chrono::system_clock::time_point _boardtime;
        std::chrono::system_clock::time_point _localtime;
        std::string _type;
        std::string _board;
        std::string _device;
        double      _value {0};
    public:
        Probe(packet::ipacket *);
        
        const std::chrono::system_clock::time_point& boardtime() { return _boardtime; }
        const std::chrono::system_clock::time_point& servertime() { return _localtime; }
        const std::string& type()   { return _type; }
        const std::string& board()  { return _board; }
        const std::string& device() { return _device; }
        const double value() { return _value; }
        
    };
    
    class Message
    {
    protected:
        std::chrono::system_clock::time_point _boardtime;
        std::chrono::system_clock::time_point _localtime;
        std::string _type;
        std::string _board;
        std::string _device;
        std::string _message;
        uint8_t     _level;

    public:
        Message(packet::ipacket *);
        
        const std::chrono::system_clock::time_point& boardtime() { return _boardtime; }
        const std::chrono::system_clock::time_point& servertime() { return _localtime; }
        const std::string& type()    { return _type; }
        const std::string& board()   { return _board; }
        const std::string& device()  { return _device; }
        const std::string& message() { return _message; }
        const double level() { return _level; }
        
    };
    
    
    
    
        
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


    
    
//     enum class EnumResGroup
//     {
//         none,
//         sensor,
//         actuator
//     };
//     enum class EnumResZone
//     {
//         none,
//         house,
//         parking,
//         depot,
//         pool,
//         woodshed,
//         barbeque,
//         grove,
//         garden
//     };
//     
//     
//     enum class EnumResZoneHouseFloor
//     {
//         none,
//         living,
//         basement
//     };
//     
//     enum class EnumResZoneHouseLiving
//     {
//         living,
//         kitchen,
//         pantry,
//         hallway,
//         laundry,
//         bathroom_w,
//         bathroom_e,
//         bedroom_w,
//         bedroom_e,
//         garage,
//     };
//     
//     enum class EnumResZoneHouseBasement
//     {
//         welcoming,
//         depot,
//         lab,
//         cellar_e,
//         cellar_w,
//         heating,
//         hallway,
//         safreroom,
//         tavern,
//     };
// 
//     
// 
//     enum class EnumResGroupSensor
//     {
//         none,
//         temperatre,
//         humidity,
//         pressure,
//         light,
//         level,
//         ph,
//         ec,
//     };
// 
//     enum class EnumResTypeSensor
//     {
//         dht22,
//         ldr,
//         ph,
//         ec,
//     };
// 
//     enum class EnumResGroupActuator
//     {
//         none,
//         light,
//         gate,
//         valve
//     };
// 
//     enum class EnumResTypeActuator
//     {
//         servo,
//         relay,
//     };
//     
//     class ResourceZone
//     {
//         double location[3] {0}; // Origin is in the house corner left of front door
//         
//         const EnumResZone               zone
//         const EnumResZoneHouseFloor     floor;
//         const EnumResZoneHouseLiving    living;
//         const EnumResZoneHouseBasement  basement;
// 
//     };
//     
//     class ResourceDevice
//     {
//         const EnumResGroup          group;
//         const EnumResGroupSensor    sn_device;
//         const EnumResGroupActuator  ac_device;
//         const EnumResTypeSensor     sn_type;
//         const EnumResTypeActuator   ac_type;
//     };
// 
//     
//     class ResourceTemplate
//     {
//         ResourceZone            _zone;
//         ResourceDevice          _device;
//         
//         std::string             _name;
// 
//     public:
//         
//          ResourceTemplate(std::string) {};
//         ~ResourceTemplate() {}
//         
//         virtual const EnumResGroup& group()=0;
//         virtual const EnumResZone&  zone()=0;
//         
//         bool                        is_valid();
//         
//         const std::string& name() { return _name; }
//     };
//     
// 
//     
//     class 
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
//     class ResourceTable
//     {
//         class Sensors
//         {
//             std::vector<std::string>    _s_temperature;
//             std::vector<std::string>    _s_humidity;
//             std::vector<std::string>    _s_light;
//             std::vector<std::string>    _s_ph;
//             std::vector<std::string>    _s_ec;
//             std::vector<std::string>    _s_airquality;
//             std::vector<std::string>    _s_airspeed;
//             
//         }
//         std::vector<std::string>    _lights_out;
//         std::vector<std::string>    _lights_in;
//     };
    
    
/*
 * Resources are:
 * - Sensors:
 *      - temperature
 *      - humidity
 *      - ldr
 *      - pressure
 *      - level
 * - Actuators:
 *      - switches/relay
 *      - servos
 * 
 *          -

*/
    
    
    
    
    
    
    
    
    
    
    
} /*namespace : indoorino */

#endif /* INDOORINO_NETWORK */


#endif /* _SRC_DEVICES_H_ */


