/*
 * common/utils.cpp
 *
 *  Created on: Apr 23, 2020
 *      Author: n00b
 */

#include "utils.h"
#if defined(ARDUINO)
#include "stddev.h"
#endif

#if defined (ESP8266) || defined (ARDUINO_AVR_UNO)
    SoftwareSerial SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);
#elif defined (ARDUINO_AVR_MEGA2560)
    void wdt_init(void)
    {
        MCUSR = 0;
        wdt_disable();
        return;
    }
#endif

int memvcmp(void *data, unsigned char val, unsigned int size)
{
    unsigned char *mm = (unsigned char*)data;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

namespace debug
{
    void _helper (uint8_t level, const char * source, const __FSH * format, ...)
    {
        va_list args;
        
        char msg[DEBUG_BUFFER_SIZE] {0};
        char p0[20] {0};
        char p1[20] {0};
        
        va_start(args, format);
        vsnprintf_P(msg, DEBUG_BUFFER_SIZE, (const char *)format, args);
        va_end(args);
        
        strcpy_P(p1, source);

        switch(level)
        {
            case 0:
            {
                strcpy_P(p0, debug::DEBUG);
                break;
            }
            case 1:
            {
            #if defined (__linux__)
                std::cout << debug::AZR;
            #endif
                strcpy_P(p0, debug::INFO);                
                break;
            }
            case 2:
            {
            #if defined (__linux__)
                std::cout << debug::PUR;
            #endif
                strcpy_P(p0, debug::ALERT);                
                break;
            }
            case 3:
            {
            #if defined (__linux__)
                std::cout << debug::YLW;
            #endif
                strcpy_P(p0, debug::WARNING);                
                break;
            }
            case 4:
            {
            #if defined (__linux__)
                std::cout << debug::RED;
            #endif
                strcpy_P(p0, debug::ERROR);
                break;
            }
        }
        #if defined (__linux__)

        tm * timeinfo;
        time_t  rawtime;
        char    buffer[LEN_DATETIMESTR];
        time(&rawtime);
        timeinfo = localtime(&rawtime);   
        strftime (buffer, LEN_DATETIMESTR, "%Y %b %d %H.%M.%S:", timeinfo);
        std::cout << buffer;
        
        #endif /* LINUX */
        SerialDebugPrint(p0);
        SerialDebugPrint(p1);
        SerialDebugPrint(msg);

        #if defined (__linux__)        
        SerialDebugPrint(TRS);
        fflush(stdout);
        #else
        SerialDebugPrint("\n");
        #endif /* LINUX */
    }
    
} /* namespace : debug */


//      ________________________________
//      |                               |
//      |       AVAILABLE SRAM          |
//      |_______________________________|
//

size_t      __avb_sram          (void)
{
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
    char top;
    return &top - __brkval;
#elif defined(ESP_H)
    return ESP.getFreeHeap();
#elif defined(ARDUINO_SAM_DUE)
    char top;
    return &top - reinterpret_cast<char*>(sbrk(0));
#else
    warning_os("call:undefined:available_ram()");
    return 0;
#endif
}    

//    ________________________________________________________________________
//    |                                                                      |
//    |    LOW LEVEL UTILS                                                   |
//    |______________________________________________________________________|
//

namespace utils
{
    
    iEpoch_t    epoch_now           (void)
    {
#if defined(ARDUINO)
        return rtc.epoch();
#else
        return  iEpoch_t(time(NULL));
#endif
    }
    
    void        wait                (unsigned long msec)
    {
    #if defined(ARDUINO)
        
        unsigned long timeout = millis() + msec;
        while (millis() < timeout)
        {
            yield();
            #if defined(ESP_H)
            ESP.wdtFeed();
            #endif
        }
        
    #else

        std::this_thread::sleep_for(std::chrono::milliseconds(msec));

    #endif
    }
    
#if defined (__linux__)
    time_t      millis              (void)
    {
        timeval t;
        gettimeofday(&t, NULL);
        return time_t(t.tv_sec * 1000 + t.tv_usec / 1000);
    }
#endif
    
    char    *   time_string          (const uint32_t epoch)
    {
        static char buffer[LEN_DATETIMESTR];        
        memset(buffer, 0, LEN_DATETIMESTR);
        
        /* REVIEW for 2038 bug */
        #if defined (ARDUINO)
            DateTime(epoch).timestamp(buffer);
        #else
            const int64_t  rawtime = (uint64_t)epoch;
            tm * timeinfo;
            timeinfo = localtime(&rawtime);   
            strftime (buffer, LEN_DATETIMESTR, "%Y %b %d %H:%M:%S", timeinfo);
        #endif

        return (char *) buffer;
    }
    
    bool        is_devname          (const char * p, uint8_t min_size, uint8_t max_size)
    {
        if (!p) return false;
        
        uint16_t len = strlen(p);
        if ( (len < min_size) || (len > max_size) || (!isalpha(p[0])) )
        { 
            return false;
        }
        for (uint16_t m=1; m<len; m++)
        {
            if ( isalpha(p[m]) )    continue;
            if ( isdigit(p[m]) )    continue;
            if ( p[m] == '.' )      continue;
            return false;
        }
        return true;
    }
    
    bool        is_readable         (const char * p, uint8_t min_size, uint8_t max_size)
    {
        if (!p) return false;

        uint16_t len = strlen(p);
        if (len < min_size || len > max_size)
        { 
            return false;
        }
        for (uint16_t m=0; m<len; m++)
        {
            if ( isprint(p[m]) ) continue;
            if ( p[m] == '\t' )  continue;            
            if ( p[m] == '\n' )  continue;
            return false;
        }
        return true;        
    }
    
    void        dump_hex_header     (iSize_t count)
    {
    #if defined(ARDUINO)
        SerialDebug.print("\n ");
        for (uint8_t i=5; i>0; i--)
        {
            if (count < pow(10, i)) { SerialDebug.print("0"); }
            else                    { break; }
        }
        SerialDebug.print(count);
        SerialDebug.print(": ");
    #else
        std::cout << std::endl << std::setfill('0') << std::setw(5) << count << ":";
    #endif
    }
    
    void        dump_hex            (const char * buffer, iSize_t size, int mode)
    {
        if (buffer == nullptr)
        {
            error_pack("hexdump:nullpointer!");
            return;
        }
        if (mode==0)
        {
            iSize_t dropped=0;
            
            while (dropped < size)
            {
                
                char printable[20] {0};
                char * p=printable;
                
                dump_hex_header(dropped);                
                
                iSize_t left = 0;
                if ( (size - dropped) > 16 )    { left=16; }
                else                            { left=size - dropped; }
                
                // & 0xff
                
                p += sprintf(p, "  |");
                for (iSize_t i=0; i<left; i++)
                {
                    char hex[4];
                    sprintf(hex, " %02.2x", buffer[dropped + i] & 0xff);
                    SerialDebugPrint(hex);
                    
                    if (i == 7) SerialDebugPrint(" ");
                    
                    if (buffer[dropped +i] > 0 && isgraph(buffer[dropped +i]))  { p += sprintf(p, "%c", buffer[dropped + i]); }
                    else                                                        { p += sprintf(p, "."); }
                }
                
                if (left <= 7)                      { SerialDebugPrint(" "); }
                for (uint8_t j=0; j<16 - left; j++) { SerialDebugPrint("   "); }
                    
                p += sprintf(p, "|");
                SerialDebugPrint(printable);
                dropped +=left;
            }
            
            dump_hex_header(dropped);
            SerialDebugPrint("\n");
        }
        
    }
    void        dump_sha            (const char * sha)
    {
        
    }
    
    uint32_t    random_signed       (uint32_t start, uint32_t stop)
    {
    #if defined(ARDUINO)
        return (uint32_t)random(start, stop);
    #else
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> distr(start, stop); // define the range
        
        return distr(gen);
    #endif 
    }

    double      random_double       (double  start, double  stop)
    {
    #if defined(ARDUINO)
        uint32_t r = random_signed(0, 10000);
        return (double)( start + ( ( r * (stop - start) ) / 10000 ) );
    #else
        std::uniform_real_distribution<double> distr(start, stop);
        std::mt19937 gen; 
        gen.seed(std::random_device{}()); 
        return distr(gen);

    #endif 
    }
    
    
//     int         chopstring          (char ** buffer, const char * string, iSize_t size, char splitter)
//     {
//         int count = 0;
//         
//         if (strlen(string) > 0)
//         {
//             count = 1;
//         }
//         
//         char *  source;
//         char *  ptr;
//         
//         source=(char *)malloc(size + 1);
//         strcpy(source, string);
//         
//         ptr = strchr(source, splitter);
//         while (ptr != NULL)
//         {
//             count++;
//             ptr=strchr(ptr + 1, splitter);
//         }
//         
//         char * index = source;
//         
//         buffer = (char**)calloc(count, sizeof(char*));
// 
//         for (uint8_t i=0; i<count; i++)
//         {
//             buffer[i] = (char *)calloc(LEN_NAME, sizeof(char));
//             ptr = strchr(index, splitter);
// 
//             if (ptr==NULL)  { strcpy(buffer[i], index); }
//             else            
//             { 
//                 strncpy(buffer[i], index, ptr - index);
//                 index = ptr + 1;
//             }
// 
//             debug_io("-->Chunk %u : %s",i, buffer[i]);
//         }
//         
//         free (source);
//         
//         return count;
//     }
        
// for (uint8_t i=0; i<=n; i++)
// {
//     free(c[i]);
// }
// free(c);
// }
    
    
#if defined (ARDUINO)
    namespace board
    {

        size_t      available_ram       (void)
        {
            return __avb_sram();
        }

        void        reset               (void)
        {
        alert_os("*** Rebooting... ***");
        wait(1000);
        #if defined(ARDUINO_AVR_UNO)
            asm volatile ("  jmp 0");
        #elif defined(ARDUINO_SAM_DUE)
            REQUEST_RESET;
        #elif defined(ARDUINO_AVR_MEGA2560)
            wdt_init();
        #elif defined(ESP_H)
            pinMode(0,OUTPUT);  digitalWrite(0,1);
            pinMode(2,OUTPUT);  digitalWrite(2,1);
            ESP.restart();
        #else
            warning_os("call:undefined:resetBoard()");
        #endif
        }
        
        void        debug_init          (void)
        {
            SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);

            SerialDebug.print(F("\n<INIT:"));
            SerialDebug.print(P2C(BOARD_NAME));
            SerialDebug.print(F(":"));
            SerialDebug.print(P2C(INDOORINO_TYPE));
            SerialDebug.print(F(">\n"));
            

            SerialDebug.print(F("\nAvailable SRAM: "));
            SerialDebug.print(available_ram());
            SerialDebug.print(F(" KB\n"));
        }

        bool        can_alloc           (const iSize_t size)
        {
            if ( (available_ram()) - size < SRAM_LIMIT )
            {
                error_mem("LOW SRAM (%u KB) !", available_ram());
                return false;
            }
            return true;
        }
        
        bool        can_alloc_p         (const iCom_t command)
        {
            packet::ipacket * p=new packet::ipacket(command);
            
            if (can_alloc(p->full_size()))   
            {
                delete p;
                return true;
            }
            
            delete p;
            return false;
        }
        
        bool        is_pin              (const iPin_t pin)
        {
            // TODO: review pins
            #if defined(ARDUINO_AVR_UNO)
                return (pin > 1 && pin < 16);
            #elif defined(ARDUINO_AVR_MEGA2560)
                return (pin > 1 && pin <= 53);
            #elif defined(ARDUINO_SAM_DUE)
                return (pin > 1 && pin <= 53);
            #elif defined(ESP8266)
                #if     defined(ARDUINO_ESP8266_GENERIC)
                    return (pin > 1 && pin <= 16);
                #elif   defined(ARDUINO_ESP8266_WEMOS_D1R1)
                    return (pin > 1 && pin <= 16);

                #elif   defined(ARDUINO_ESP8266_WEMOS_D1MINI)       ||\
                        defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO )   ||\
                        defined (ARDUINO_ESP8266_WEMOS_D1MINILITE)
                    return (pin > 1 && pin <= 16);
                
                #else
                    warning_os("is_pin(): esp type is not defined!");
                    return true;
                #endif
                
            #else
                warning_os("is_pin(): board type is not defined!");
                return true;
            #endif
        }

    
    }
#endif /* ARDUINO */
    
} /* namespace::utils */



/* END OF UTILS */





