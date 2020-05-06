/*
 * packetUtils.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: n00b
 */

#include "packetUtils.h"
#if defined(ARDUINO)
#include "stddev.h"
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Board IO                                                    |
//      |___________________________________________________________________|
//
BoardIO::BoardIO                (bool strict)
{
    _rx = new packetParse(strict);
    _tx = new packetTransmit();
}

void            BoardIO::begin      (void)
{
    Serial.begin(SERIAL_DEFAULT_BAUDRATE);
    Serial.setTimeout(0);
    _rx->begin();
}
void            BoardIO::loop       (void)
{
    /* This is the default behaviour for incoming packets on boards */
    if (Serial.available())
    {
        ipacket * incoming = nullptr;
        while (Serial.available())
        {
            _rx->append((uint8_t)Serial.read());
            incoming = _rx->packet();
            if (incoming != nullptr)
            {
//                 if (incoming->check())       { incoming->eval(); }
            }
        }
    }
}
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Report Helper                                               |
//      |___________________________________________________________________|
//
    
void            _sendreport
        (const uint8_t level, const char * device, const char * message, va_list args)
{
    char * msg = (char*)calloc(SERIAL_TX_BUFFER_SIZE, sizeof(char));
    vsnprintf_P(msg, SERIAL_TX_BUFFER_SIZE, message, args);
    
    ipacket * ptr = new ipacket(IBACOM_REPORT); 
    
    strcpy(ptr->p_message(), msg);
    strcpy(ptr->p_name(), device);
    *ptr->p_epoch() = rtc.epoch();
    *ptr->p_level() = level;
    
    boardio.tx()->send(ptr);
    free(msg);
    delete ptr;
}

void            sendReport
        (const uint8_t level, const __FSH* device, const __FSH * message, ...)
{	
	va_list args;

	va_start(args, message);
	_sendreport(level, F2C(device),(const char *)message, args);
	va_end(args);
}

void            sendReport
        (const uint8_t level, const char * device, const __FSH * message, ...)
{	
	va_list args;

	va_start(args, message);
	_sendreport(level, device,(const char *)message, args);
	va_end(args);
}

void            sendReport         
        (const uint8_t level, const __FSH * message, ...)
{	
	va_list args;

	va_start(args, message);
	_sendreport(level, P2C(INDOORINO_TYPE),(const char *)message, args);
	va_end(args);
}

#endif /* ARDUINO */

    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Packet Utils                                                |
//      |___________________________________________________________________|
//
   
ipacket *   reallocPacket       (ipacket * ptr, ibacomm_t type)
{
    if (ptr != nullptr)
    {
        if (ptr->command() == type)
        {
            ptr->clear(); 
            debug_mem("** Hit! **");
            return ptr;
        }
        else
        {
            delete ptr;
        }
    }
    
    ptr = new ipacket(type);
    debug_mem("** Miss! **");
    return ptr;    
}

ibasize_t   packet_size         (ibacomm_t com)
{
    ibasize_t s=0;
    ipacket * p=new ipacket(com);
    
    if (p != nullptr)
    {
        s=p->data_size();
        delete p;
    }
    return s;
}



// bool                isDeviceConf                    (ipacket * ptr)
// {
//     if (ptr->command() > IBACOM_CONF_ASENSOR && 
//         ptr->command() <= IBACOM_CONF_DEVSTD)
//     {
//         return true;
//     }
//     return false;
// }

// bool                isSensorConf                    (ipacket * ptr)
// {
//     ibacomm_t list[SENSOR_NUM] = SENSOR_LIST;
//     
//     for (uint16_t i=0; i<SENSOR_NUM; i++)
//     {
//         if (ptr->command() == list[i])  { return true; }
//     }
//     return (ptr==nullptr) ? false : true;
// }


// bool                isActuatorConf                  (ipacket * ptr)
// {
//     ibacomm_t list[ACTUATOR_NUM] = ACTUATOR_LIST;
//     
//     for (uint16_t i=0; i<ACTUATOR_NUM; i++)
//     {
//         if (ptr->command() == list[i])  { return true; }
//     }
//     return (ptr==nullptr) ? false : true;
// }
