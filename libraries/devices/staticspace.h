/*
 * sdConf.h
 *
 *  Created on: Nov 11, 2019
 *      Author: n00b
 */

#ifndef SDCONF_H_
#define SDCONF_H_

#if defined (ARDUINO)

#include    "definitions.h"

#if defined(EEPROM_MODULE)
#include    "EEPROM.h"
#endif

class StaticSpace
{
public:
     StaticSpace                    () {};
    ~StaticSpace                    () {};
    
    void                            begin       (void)
    {
        #if defined(EEPROM_MODULE)
        
            debug_io("EEPROM initialized");
            #if defined (ESP8266)
                EEPROM.begin(EEPROM_SIZE);    
            #else
                EEPROM.begin();
            #endif
        
        #elif defined (SD_MODULE)
                /* implement SD (will never be implemented) */
            info_dev("SDCONF begin"); 
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
            info_dev("conf:FLASHCONF begin"); 
        #endif        
    };
    
    
    uint8_t                         read        (iSize_t position)
    {
        #if defined(EEPROM_MODULE)
            return EEPROM.read(position);    
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };

    void                            read        (iSize_t position, char * buffer, iSize_t size)
    {
        #if defined(EEPROM_MODULE)            
            memset(buffer, 0, size);
            for (iSize_t i=0; i<size; i++)
            {
                buffer[i] = EEPROM.read(position + i);
            }
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };
        
    
    void                            write       (iSize_t position, uint8_t value)
    {
        #if defined(EEPROM_MODULE)
            EEPROM.write(position, value);
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };

    void                            write       (iSize_t position, const  char * buffer, iSize_t size)
    {
        #if defined(EEPROM_MODULE)
            for (iSize_t i=0; i<size; i++)
            {
                EEPROM.write(position + i, buffer[i]);
            }
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };    
    
    
    void                            update      (iSize_t position, uint8_t value)
    {
        #if defined(EEPROM_MODULE)
        
        
            #if defined (ESP8266)
            if (EEPROM.read(position) != value)
            {
                EEPROM.write(position, value);
            }
            #else
            EEPROM.update(position, value);    
            #endif
        
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };
    
    void                            update      (iSize_t position, const  char * buffer, iSize_t size)
    {
        #if defined(EEPROM_MODULE)
        
        
            #if defined (ESP8266)
            for (iSize_t i=0; i<size; i++)
            {
                if (EEPROM.read(position + i) != buffer[i])
                {
                    EEPROM.write(position + i, buffer[i]);
                }
            }
            #else
            for (iSize_t i=0; i<size; i++)
            {
                EEPROM.update(position + i, buffer[i]);
            }
            #endif
        
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };
    
    void                            update      (void)
    {
        #if defined(EEPROM_MODULE)

         #if defined (ESP8266)
            if (!EEPROM.commit())  error_mem("EEPROM:ERROR: commit() failed!");
        #endif

        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
    };
    
    
    template <typename T> T&        get         (int index, T &object)
    {
        #if defined(EEPROM_MODULE)
            return EEPROM.get(index, object);    
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
            return object;
    };
    
    template <typename T> const T&  put         (int index, const T &object )
    {
        #if defined(EEPROM_MODULE)
            return EEPROM.put(index, object);    
        #elif defined (SD_MODULE)
                /* implement SD */
        #else
                /* implement SRAM array and read init valaues from PROGMEM */
        #endif        
            return object;
    };   
};


#endif /* ARDUINO */

#endif /* SDCONF_H_ */
