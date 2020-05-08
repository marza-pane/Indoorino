/*
 * sdConf.h
 *
 *  Created on: Nov 11, 2019
 *      Author: n00b
 */

#ifndef SDCONF_H_
#define SDCONF_H_

#include    "definitions.h"

//  ___________________________________________________________________________________________
//  |                              |            |      |               |      |               |
//  |     project config           |   project  |      |     packet    |      |     packet    |
//  |                       device | parameters | com  |               |  com |               |
//  | name | type | board | number |            | dev1 |  dev 1  data  | dev2 |  dev 2  data  |
//  |__16__|__16__|__1 6__|___8____|______x_____|__16__|_x_|_x_|_x_|_x_|__16__|_x_|_x_|_x_|_x_|
//  |                              |            |                      |                      |
//  |<---     STDCONF_SIZE     --->|         _dpos[0]               -dpos[1]                -dpos[end]                 


class StaticSpaceTemplate
{
public:
             StaticSpaceTemplate    () {};
    virtual ~StaticSpaceTemplate    () {};
    
    virtual void        begin       (void) {};
    
    virtual uint8_t     read        (ibasize_t ) { return 0; };
    virtual void        write       (ibasize_t, uint8_t) {};
    virtual void        update      (int, uint8_t) {};
    virtual void        update      (void) {};
};

#if defined (SD_MODULE)
// #include    "sdConf.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       SD card as static space                                     |
//      |___________________________________________________________________|
//

    class StaticSpaceSD : public StaticSpaceTemplate
    {
    public:
        StaticSpaceSD   () {};
        ~StaticSpaceSD   () {};
        
        void        begin       (void) {};
        
        uint8_t     read        (ibasize_t ) { return 0; };
        void        write       (ibasize_t, uint8_t) {};
        void        update      (int, uint8_t) {};
        void        update      (void) {};

        template< typename T > T &get( int index, T &object )
        {
            /* HERE implement SD CONF get*/
            return object;
        }
        
        template< typename T > const T &put( int index, const T &object )
        {
            /* HERE implement SD CONF put*/
            return object;
        }
    };
#endif
    
#if defined(EEPROM_MODULE)
#include    "EEPROM.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       EEPROM as static space                                      |
//      |___________________________________________________________________|
//

    class StaticSpaceEEPROM : public StaticSpaceTemplate
    {
/*    
        #if defined (ESP8266)
        bool        commit      (void)
        {
            bool a = EEPROM.commit();
            if (a)  { debug("\nEEPORM COMMITTED"); }
            else    error_mem("COMMITTED FAILED");
            return a;
        };
        #endif
        */
    public:
        StaticSpaceEEPROM ()  {};
        
        void        begin       (void)
        {
            #if defined (ESP8266)
                EEPROM.begin(EEPROM_SIZE);    
            #elif defined (ARDUINO)
                EEPROM.begin();
            #endif
        };
        
        uint8_t     read        (uint32_t pos)
        {
            return EEPROM.read(pos);    
        };
        
        void        write       (uint32_t pos, uint8_t val)
        {
            EEPROM.write(pos, val);    
        };
        
        void        update      (uint32_t pos, uint8_t val)
        {
            #if defined (ESP8266)
            if (EEPROM.read(pos) != val)
            {
                EEPROM.write(pos, val);
            }
            this->update();
            #else
            EEPROM.update(pos, val);    
            #endif
        };
        
        void        update      (void)
        {
            #if defined (ESP8266)
            if (!EEPROM.commit())  error_mem("EEPROM:ERROR: commit() failed!");
            #endif
        }
                
        template< typename T > T &get( int index, T &object ){
            return EEPROM.get(index, object);    
        };
        
        template< typename T > const T &put( int index, const T &object ){
            return EEPROM.put(index, object);    
        };
    };

#endif

//      _____________________________________________________________________
//      |                                                                   |
//      |       Programmable Flash memory as static space                   |
//      |___________________________________________________________________|
//

    class   StaticSpacePGM : public StaticSpaceTemplate
    {
        public:
        StaticSpacePGM   () {};
        ~StaticSpacePGM   () {};
        
        uint8_t     read        (int ) {return 0;};
        void        write       (int, uint8_t) {};
        void        update      (int, uint8_t) {};
        
        template< typename T > T &get( int index, T &object )
        {
            // HERE IMPLEMENT A REGULAR RAM ARRAY
            return object;
        }
        
        template< typename T > const T &put( int index, const T &object )
        {
            // HERE IMPLEMENT A REGULAR RAM ARRAY
            return object;
        }
    };



#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
    
        typedef StaticSpaceEEPROM StaticSpace;

//     class StaticSpace : public StaticSpaceEEPROM
//     {
//         
//     };

#elif defined (ESP8266)
    
        typedef StaticSpaceEEPROM StaticSpace;
    
#else
    
    #if defined(SD_MODULE)
        typedef StaticSpaceSD StaticSpace;
//         #define StaticSpace StaticSpaceSD
    #else
        typedef StaticSpacePGM StaticSpace;
//         #define StaticSpace StaticSpacePGM
    #endif /* SD_MODULE */

#endif /* EEPROM as STATIC */


#endif /* SDCONF_H_ */
