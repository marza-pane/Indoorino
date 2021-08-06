/*
 * indoorinoConfig.h
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */

#ifndef INDOORINOCONFIG_H_
#define INDOORINOCONFIG_H_

#include "staticspace.h"

//  ___________________________________________________________________________________________
//  |                              |            |      |               |      |               |
//  |     project config           |   project  |      |     packet    |      |     packet    |
//  |                       device | parameters | com  |               |  com |               |
//  | name | type | board | number |            | dev1 |  dev 1  data  | dev2 |  dev 2  data  |
//  |__16__|__16__|__1_6__|___1____|______x_____|___2__|_x_|_x_|_x_|_x_|___2__|_x_|_x_|_x_|_x_|
//  |                              |            |                      |                      |
//  |<---     STDCONF_SIZE     --->|         _dpos[0]               -dpos[1]                -dpos[end]                 

#if defined (ARDUINO)

//      _____________________________________________________________________
//      |                                                                   |
//      |       BOARD DEFAULT CONFIGURATION                                 |
//      |___________________________________________________________________|
//

    class   Conf_Board
    {
    protected:
        
        StaticSpace         staticspace;
        virtual void        factorydev      (iSize_t);
        void                initdev         (iSize_t);
        uint8_t             validate        (packet::ipacket *);
        iSize_t             _dpos[MAX_ATTACHED_DEVICES] {0};
        uint8_t             _propnum=0;
        

    public:
                 Conf_Board ();
        virtual ~Conf_Board ();
        
        char    *           name            (void)  { return P2C(BOARD_NAME);     };
        char    *           type            (void)  { return P2C(INDOORINO_TYPE); };
        char    *           board           (void)  { return P2C(BOARD_TYPE);     };
        uint8_t             devnum          (void)  { return staticspace.read(3 * LEN_NAME); }
       
        void                begin           (void);
        void                factory         (void);

        bool                devAdd          (packet::ipacket *);
        bool                devRem          (const char *);
        bool                devMod          (const char *, packet::ipacket *);
        bool                devSetName      (const char *, const char *);
        bool                devSetPin       (const char *, uint8_t);
        packet::ipacket *   device          (packet::ipacket *, const char *);
        packet::ipacket *   device          (packet::ipacket *, int8_t);
        
        int8_t              indexFromName   (const char *);
        int8_t              indexFromPin    (int8_t);
        void                device_name     (char *, int8_t);
        iCom_t              device_command  (int8_t);
        iCom_t              device_command  (const char * name) { return device_command(indexFromName(name)); }
        int8_t              device_pin      (int8_t);
        int8_t              device_pin      (const char * name) { return device_pin(indexFromName(name)); }

    };
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       BOARD BASE CONFIGURATION                                    |
//      |___________________________________________________________________|
//

    #if defined (ESP8266)
    
    /*
    *  ___________________________________________________________________________________________
    *  |         | WiFi |  WiFi |  SSL  | remote | remote |  local | timeout | timeout | attemps |
    *  | STDCONF | SSID |  PSK  |  KEY  |   IP   |  PORT  |  PORT  | client  | packet  | packet  |
    *  |____49___|__32__|__64___|__16___|___4____|___2____|___2____|____4____|____4____|____1____|
    *  
    */

    class   ConfRouter      : public Conf_Board
    {
    protected:

        char                _ssid[LEN_SSID] {0};
        char                _psk[LEN_PSK]   {0};
        IPAddress           _remote_address;
        uint16_t            _remote_port=0;
        
        iEpoch_t            _timeout_packet=0;
        uint8_t             _attemps_packet=0;
    
    public:

         ConfRouter():Conf_Board() { };
        ~ConfRouter()              { };
        
        void                begin           (void);
        void                factory         (void);        
        bool                set_credential  (const char *, const char *);
        
        char    *           ssid            (void) { return _ssid; }
        char    *           psk             (void) { return _psk;  }
        
        void                local_port      (const uint16_t);
        void                remote_ip       (const IPAddress&);
        void                remote_port     (const uint16_t);
        
        uint16_t            remote_port     (void) { return _remote_port;    }
        IPAddress           remote_ip       (void) { return _remote_address; }
        
        void                timeout_packet  (const iEpoch_t);
        void                attemps_packet  (const uint8_t);
        
        iEpoch_t            timeout_packet  (void) { return _timeout_packet; }
        uint8_t             attemps_packet  (void) { return _attemps_packet; }
        
    };

    #endif /* ESP8266 */
    

//      _____________________________________________________________________
//      |                                                                   |
//      |       PROJECT CONFIGURATIONS                                      |
//      |___________________________________________________________________|
//

    #if defined (INDOORINO_SAMPLER)
    
    class   ConfSampler     : public Conf_Board
    {
    public:
        void                begin           (void);
        void                factory         (void);

        void                step_probe      (uint32_t);
        void                step_status     (uint32_t);
        void                step_config     (uint32_t);

        uint32_t            step_probe      (void);
        uint32_t            step_status     (void);
        uint32_t            step_config     (void);

        //         void                cool            (uint32_t);
//         uint32_t            cool            (void);
    };
    
    #elif defined (INDOORINO_CONTROLLER)
    
    class   ConfController  : public Conf_Board
    {
    public:
        void        begin           (void);
        void        factory         (void);        
    };
        
    #endif /* PROJECTS */

#endif /* INDOORINO */

#endif /* INDOORINOCONFIG_H_ */
