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
#include "layout.h"

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
    namespace devs
    {
    
        class   BoardTemplate;
        
        class   DeviceTemplate;
        
        
        //      _________________________________________
        //      |                                       |
        //      |       Board List                      |
        //      |_______________________________________|
        //


        class Boards
        {
        
        private:

        protected:        
            std::vector<BoardTemplate *>  _boards;
        public:

            Boards() {};
            ~Boards() { this->clear(); }
            
            BoardTemplate   *   operator[]  (const iSize_t i) { return _boards.at(i); };
            BoardTemplate   *   operator()  (const char *);
            auto&               operator()  (void) { return _boards; };
            
            int             exist       (const char *);
            bool            remove      (const char *);
            bool            add         (packet::ipacket *);
            iSize_t         size        () { return _boards.size(); }
            void            clear       (void);    
            void            read_layout (void);    
            
            void            begin       (void) { this->read_layout(); }
            bool            save        (void);
            bool            load        (void);
            void            show        (void);
            
//             friend class indoorino::IndoorinoSystem;
        };

        //      _________________________________________
        //      |                                       |
        //      |       Device List Wrapper             |
        //      |_______________________________________|
        //

        
        class Devices
        {
        protected:
            std::vector<DeviceTemplate *>   _devs;

        public:
            Devices();
            ~Devices();
            
        public:
            DeviceTemplate *  operator[]  (const int  i);
            DeviceTemplate *  operator()  (const char *);
            const auto     &  operator()  (void) { return _devs; }
            
        public:

                    
            int         exist       (const char *);
            bool        remove      (const char *);
            bool        add         (packet::ipacket *);
            bool        add         (const char *b, const char *d, iCom_t c, iPin_t p=0);
            void        show        (void);
            iSize_t     size        (void) { return _devs.size(); }
            void        clear       (void);
                    
            friend class indoorino::devs::BoardTemplate;
        };

//      ___________________________________________________________________________________________________
//                                                                                                 
//                ══════⊹⊱≼≽⊰⊹══════      B O A R D    T E M P L A T E      ══════⊹⊱≼≽⊰⊹══════           
//      ___________________________________________________________________________________________________
//
        
        class   BoardTemplate
        {
        private:
            bool                            _enabled=true;
            bool                            _is_online=false; // IS THIS IMPLEMENTED ??

        protected:
            
            indoorino::devs::Devices        _devices;
            std::chrono::seconds            _lag;

            std::vector<packet::netpacket>  _rxqueue; // TODO
            std::vector<packet::netpacket>  _txqueue; // TODO
            
            packet::ipacket                 _config[2];
            packet::ipacket                 _status[2];

        public:
            
            BoardTemplate(const char *);
            virtual ~BoardTemplate();

        public:
            void            parse           (packet::netpacket *);
            void            send_config     (void);
            void            send_status     (void);
            void            read_layout     (void);
            
        public:
            const char  *   name            () { return _config[0].p_name(); }
            const char  *   type            () { return _config[0].p_type(); }
            const char  *   board           () { return _config[0].p_board(); }
            
            const iSize_t   devnum          (void);

            auto&           devices         (void) { return _devices; }

            const packet::ipacket * config  () { return _config; }
            const packet::ipacket * status  () { return _status; }
            
        public:
            void            show            (void);
            void            set_offline     () { _is_online = false; }
            
            std::vector<packet::netpacket>& rxpackets() { return _rxqueue; } 
            std::vector<packet::netpacket>& txpackets() { return _txqueue; } 
            std::chrono::system_clock::time_point& boardtime (void);
            
            friend class indoorino::devs::Boards;
        };
        
    
//      ___________________________________________________________________________________________________
//                                                                                                 
//                ══════⊹⊱≼≽⊰⊹══════    D E V I C E S    T E M P L A T E    ══════⊹⊱≼≽⊰⊹══════           
//      ___________________________________________________________________________________________________
//
            
        class DeviceTemplate
        {

        protected:
            packet::ipacket             _conf;
            packet::ipacket             _stat;

        protected:

            
        public:

            DeviceTemplate(const char *b, const char *d, iCom_t c, iPin_t p=0);
            
            virtual ~DeviceTemplate() {}
            
            virtual void        parse           (packet::ipacket *);
            
            iCom_t              devtype         () { return _conf.command();                    }
            const char  *       name            () { return (const char *)_conf.p_devname();    } 
            const char  *       boardname       () { return (const char *)_conf.p_name();       }
            const char  *       type            () { return (const char *)lyt::devicetype_Com2Str(_conf.command()); }
            
            void                set_offline     () { *_stat.p_status() = 2;                     }
            bool                is_connected    () { return (*_stat.p_status() == 0);           }
            void                show            (void);
            uint8_t             pin             () { return (uint8_t)(*_conf.p_pin1());         }

            packet::ipacket&    config          () { return _conf; }
            packet::ipacket&    status          () { return _stat; }
            
            friend class indoorino::devs::BoardTemplate;

        };
        
    } /* namespace:devs */

} /*namespace : indoorino */

#endif /* INDOORINO_NETWORK */

#endif /* _SRC_DEVICES_H_ */


//              _________________________________________
//              |                                       |
//              |       D E V I C E S P A C E           |
//              |_______________________________________|
// 
    
//     namespace devs
//     {        
//         /*          R E L A Y                       */
//         class Relay : public DeviceTemplate
//         {
//             /*
//              * This class is suitable for all kind of remote switch controllers
//              */
// 
//         public:
//             Relay(const char *b, const char *d, iCom_t c, iPin_t p=0);
//             void            parse           (packet::ipacket *);
//             uint8_t         state           (void) { return bool(*_stat.p_level()); }
//             
//             void            set_on          (void) {};
//             void            set_off         (void) {};
//         };
//         
//         /*          D H T 2 2                       */
//         class DHT22 : public DeviceTemplate
//         {
//         private:
//         public:
//             DHT22(const char *b, const char *d, iCom_t c, iPin_t p=0);
//             void            parse           (packet::ipacket *);
// 
//             double          temperature     (void);
//             double          humidity        (void);            
//         };
// 
//         
//         /*          G E N E R I C   S W I T C H     */
//         class GenericSwitch : public DeviceTemplate
//         {
//         public:
//             GenericSwitch(const char *b, const char *d, iCom_t c, iPin_t p=0);  
//             void            parse           (packet::ipacket *);
//             bool            state           (void) { return bool(*_stat.p_level()); }
//         };
// 
//         /*          F L O O D   S W I T C H         */
//         class FloodSwitch : public GenericSwitch
//         {
//         private:
//         public:
//             FloodSwitch(const char *b, const char *d, iCom_t c, iPin_t p=0);            
//         };
//         
//         /*          R A I N   S W I T C H           */
//         class RainSwitch : public GenericSwitch
//         {
//         private:
//         public:
//             RainSwitch(const char *b, const char *d, iCom_t c, iPin_t p=0);            
//         };
// 
//         
//         /*          D U S T   P. M.   25            */
//         class DustPM25 : public DeviceTemplate
//         {
//         private:
//         public:
//             DustPM25(const char *b, const char *d, iCom_t c, iPin_t p=0);            
//             void            parse           (packet::ipacket *);
//         };
// 
//         
//         
//         
//     } /* namespace : devs */

    

    
    
    
//     class Probe
//     {
//     protected:
//         std::chrono::system_clock::time_point _boardtime;
//         std::chrono::system_clock::time_point _localtime;
//         std::string _type;
//         std::string _board;
//         std::string _device;
//         double      _value {0};
//     public:
//         Probe(packet::ipacket *);
//         
//         const std::chrono::system_clock::time_point& boardtime() { return _boardtime; }
//         const std::chrono::system_clock::time_point& servertime() { return _localtime; }
//         const std::string& type()   { return _type; }
//         const std::string& board()  { return _board; }
//         const std::string& device() { return _device; }
//         const double value() { return _value; }
//         
//     };
//     
//     class Message //??
//     {
//     protected:
//         std::chrono::system_clock::time_point _boardtime;
//         std::chrono::system_clock::time_point _localtime;
//         std::string _type;
//         std::string _board;
//         std::string _device;
//         std::string _message;
//         uint8_t     _level;
// 
//     public:
//         Message(packet::ipacket *);
//         
//         const std::chrono::system_clock::time_point& boardtime() { return _boardtime; }
//         const std::chrono::system_clock::time_point& servertime() { return _localtime; }
//         const std::string& type()    { return _type; }
//         const std::string& board()   { return _board; }
//         const std::string& device()  { return _device; }
//         const std::string& message() { return _message; }
//         const double level() { return _level; }
//         
//     };
    
    
    
    
        
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


    
    
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
    
    
    
    
    
    
    
    

