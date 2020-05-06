/*
 * utils.cpp
 *
 *  Created on: Apr 23, 2020
 *      Author: n00b
 */

#include "utils.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    LOW LEVEL BOARD UTILS                                             |
//    |______________________________________________________________________|
//

    //      ____________________________________
    //      |                                   |
    //      |       Free RAM                    |
    //      |___________________________________|
    //

size_t      freeRam         (void)
{
    #if defined(ARDUINO_SAM_DUE)
        char top;
        return &top - reinterpret_cast<char*>(sbrk(0));
    #elif defined(ESP_H)
        return ESP.getFreeHeap();
    #elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
        char top;
        extern char *__brkval;
        return &top - __brkval;
    #elif defined(INDOORINO_PYLIB) || defined(RAWRUN)
        SerialDebugPrint("\ncall:freeRam()");
        return 0;
    #endif
}
        
    //      ____________________________________
    //      |                                   |
    //      |       Reset Board                 |
    //      |___________________________________|
    //

#if defined(ARDUINO_AVR_MEGA2560)
void        wdt_init        (void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}
#endif

void        resetBoard          (void)
{
    debug("\n*** Rebooting... ***");
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
        SerialDebugPrint(F("\ncall:resetBoard()"));
    #endif
}
    //      ____________________________________
    //      |                                   |
    //      |       Comunications               |
    //      |___________________________________|
    //

#if defined(ARDUINO)

void        serialwrite         (char * payload, uint16_t size)
{
    uint16_t n=0;
    while (n < size)
    {
        if (Serial.availableForWrite() > 1)
        {
            Serial.write(payload[n]);
            n++;
        }
        else
        {
            delay(5);
            continue;
        }
    }
    Serial.flush();
}

#if defined(ESP8266)
void        udpwrite            (const char * ip, uint16_t port, char * data, ibasize_t size)
{
    debug_io("\nboardio:UDP: sending %u bytes to %s:%u", size, ip, port);
    Udp.beginPacket(ip, port);
    Udp.write(data, size);
    Udp.endPacket();
}
#endif
    
#endif /* ARDUINO COMUNICATIONS */

//    ________________________________________________________________________
//    |                                                                      |
//    |    DEBUG TOOLS                                                       |
//    |______________________________________________________________________|
//

void        debughelper         (const __FSH * format, ...)
{
        va_list args;
        static char    msg[SERIAL_TX_BUFFER_SIZE];
        memset(msg, 0, SERIAL_TX_BUFFER_SIZE);

        va_start(args, format);
        vsnprintf_P(msg, SERIAL_TX_BUFFER_SIZE, (const char *)format, args);
        va_end(args);
        SerialDebugPrint(msg);
}


void        dump_array          (char * array, ibasize_t size)
{
    // This is a debug tool
    debug("\n[");
	for (uint16_t i=0; i<size; i++)
	{
		if (isprint(array[i]))    { debug("%c", array[i]); }
		else                      { debug(" | %d", uint8_t(array[i])); }
		if (i % 16 == 0 && i > 1) { debug(">\n>"); }
    }
	debug("]\n");
}

void        time_string         (char * buf, uint32_t epoch)
{
    memset(buf, 0, LEN_DATETIMESTR);
    
    /* REVIEW for 2038 bug */
    #if defined (ARDUINO)
        DateTime(epoch).timestamp(buf);
    #else
        const int64_t  rawtime = (uint64_t)epoch - SECONDS_PER_HOUR;
        struct tm * timeinfo;
        timeinfo = localtime(&rawtime);   
        strftime (buf, LEN_DATETIMESTR, "%Y %b %d %H:%M:%S", timeinfo);
    #endif
    
//     #if defined (ARDUINO_SAM_DUE) || defined (ESP8266)
//         const long int rawtime = (long int)epoch;
//     #elif defined (ARDUINO_AVR_MEGA) ||
//           defined (ARDUINO_AVR_MEGA2560) ||
//           defined (ARDUINO_AVR_UNO) 
//         const uint32_t rawtime = (const uint32_t)epoch;
//     #else
//         const int64_t  rawtime = (const uint64_t)epoch;
//     #endif
//     
//     struct tm * timeinfo;
//     timeinfo = localtime(&rawtime);
// 
//     strftime (buf, LEN_DATETIMESTR, "%Y %b %d %H:%M:%S", timeinfo);
    return;
}


//    ________________________________________________________________________
//    |                                                                      |
//    |    INDOORINO TOOLS                                                   |
//    |______________________________________________________________________|
//


bool        is_string_devname   (const char * p, uint8_t min_size, uint8_t max_size)
{
    uint16_t len = strlen(p);
    if (len < min_size || len > max_size || !isalpha(p[0]))
	{ 
		return false;
	}
	for (uint16_t m=1; m<len; m++)
	{
		if (!(isalpha(p[m]) || isdigit(p[m]))) { return false; }
	}
	return true;
}

bool        is_string_human     (const char * p, uint8_t min_size, uint8_t max_size)
{
    uint16_t len = strlen(p);
	if (len < min_size || len > max_size)
	{ 
		return false;
	}
	for (uint16_t m=0; m<len; m++)
	{
        if (!isprint(p[m]))       { return false; }
	}
	return true;
}

bool        is_string_ip        (const char * ip)
{
    uint32_t length = strlen(ip);
    
    if (length >= LEN_IPADDRESS || length < 7)
    {
        return false;
    }
    
    uint8_t c=0;
    for (uint8_t i=0; i<length; i++)
    {
        if (isdigit(ip[i]))
        {
            continue;
        }
        else if (ip[i] == '.')
        {
            c++;
            continue;
        }
        else
        {
            return false;
        }
    }
    if (c != 3)
    {
        return false;
    }
    return true;
}

uint32_t    packet_chksum   (char * buf, ibasize_t size)
{
    uint8_t  c=0;
    uint32_t r=0;
    debug_io("\nChecksum : ");
    for (ibasize_t i=0; i < size; i++)
    {
        c = buf[i];
        r += c;
        debug_io("+%u", c);
    }
    debug_io(" = %u", r);
    return r;    
}





