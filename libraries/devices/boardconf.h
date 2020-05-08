/*
 * indoorinoConfig.h
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */

#ifndef INDOORINOCONFIG_H_
#define INDOORINOCONFIG_H_

#include "staticspace.h"
#if defined (INDOORINO_ESPSERVER)
#include "../netmodule/net_utils.h"
#endif

//  ___________________________________________________________________________________________
//  |                              |            |      |               |      |               |
//  |     project config           |   project  |      |     packet    |      |     packet    |
//  |                       device | parameters | com  |               |  com |               |
//  | name | type | board | number |            | dev1 |  dev 1  data  | dev2 |  dev 2  data  |
//  |__16__|__16__|__1 6__|___8____|______x_____|__16__|_x_|_x_|_x_|_x_|__16__|_x_|_x_|_x_|_x_|
//  |                              |            |                      |                      |
//  |<---     STDCONF_SIZE     --->|         _dpos[0]               -dpos[1]                -dpos[end]                 

#if defined (ARDUINO)

//      _____________________________________________________________________
//      |                                                                   |
//      |       BOARD DEFAULT CONFIGURATION                                 |
//      |___________________________________________________________________|
//

    class   boardConfStd
    {
    public:
         boardConfStd   ()  {};
        ~boardConfStd   ()  {};
        
        char    *           name            (void)  { return P2C(BOARD_NAME);     };
        char    *           type            (void)  { return P2C(INDOORINO_TYPE); };
        char    *           board           (void)  { return P2C(BOARD_TYPE);     };    
    };
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       BOARD BASE CONFIGURATION                                    |
//      |___________________________________________________________________|
//

    class   ConfBase        : public boardConfStd
    {
    protected:
        
        StaticSpace         staticspace;

        const   __FSH   *   _id=nullptr;        
        ibasize_t           _dpos[MAX_ATTACHED_DEVICES];
        
        void                initdev         (ibasize_t);
        virtual uint8_t     validate        (ipacket *);

    public:
        
                 ConfBase();
        virtual ~ConfBase();

        virtual void        begin           (void);
        virtual void        factory         (void);
        
        virtual bool        devAdd          (ipacket *);
        virtual bool        devRem          (const char *);
        virtual bool        devMod          (const char *, ipacket *);
        virtual bool        devSetName      (const char *, const char *);

        ipacket *           device          (ipacket *, const char *);
        ipacket *           device          (ipacket *, uint8_t);

        uint8_t             devnum          (void);
        uint8_t             devindex        (const char *);
        ibacomm_t           comByindex      (uint8_t);
    };

    
    class   ConfBase_AVR        : public ConfBase
    {
        uint8_t  validate           (ipacket *);
    public:
                    ConfBase_AVR()  {};
        virtual    ~ConfBase_AVR()  {};
        uint8_t     devindexBypin   (uint8_t);
        bool        devSetPin       (const char *, uint8_t);
        bool        devAdd          (ipacket *);
        bool        devMod          (const char *, ipacket *);
        bool        devSetName      (const char *, const char *);
    };    
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       ESP CONFIGURATION                                           |
//      |___________________________________________________________________|
//
    class   ConfBase_ESP        : public ConfBase
    {
        uint8_t  validate           (ipacket *);
   public:
                 ConfBase_ESP() {};
        virtual ~ConfBase_ESP() {};
    };

//      _____________________________________________________________________
//      |                                                                   |
//      |       PROJECT CONFIGURATIONS                                      |
//      |___________________________________________________________________|
//

    #if defined (INDOORINO_SAMPLER)
    
    class   ConfSampler     : public ConfBase_AVR
    {
    public:
        void                factory         (void);
        void                begin           (void);
        
        void                step            (uint32_t);
        void                cool            (uint32_t);
        uint32_t            step            (void);
        uint32_t            cool            (void);
    };

    #elif defined (INDOORINO_ESPSERVER)

    class   ConfEspServer   : public ConfBase_ESP
    {
    public:
        void                factory         (void);
        void                begin           (void);
        
        void                localport       (uint32_t);
        void                timeout         (uint32_t);
        void                attempts        (uint8_t);

        uint32_t            localport       (void);
        uint32_t            timeout         (void);
        uint8_t             attempts        (void);
        
        address         *   address_list[MAX_ATTACHED_DEVICES];
        void                addAddress      (address *);
        void                remAddress      (address *);
        
    };
    #endif /* PROJECTS */

#endif /* INDOORINO */

#endif /* INDOORINOCONFIG_H_ */
