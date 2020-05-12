/*
 * packetUtils.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: n00b
 */

#include "indoorino.h"
#if defined(ARDUINO)
    
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

//      _____________________________________________________________________
//      |                                                                   |
//      |       Send Config                                                 |
//      |___________________________________________________________________|
//

void        sendConfig            (void)
{
    
    ipacket * ptr = new ipacket(IBACOM_CONF_STD);    
    char buf[LEN_NAME];
    strcpy(buf,P2C(BOARD_NAME));
    strcpy(ptr->p_name(), buf);

    strcpy(buf,P2C(INDOORINO_TYPE));
    strcpy(ptr->p_type(), buf);

    strcpy(buf,P2C(BOARD_TYPE));
    strcpy(ptr->p_board(), buf);
    *ptr->p_devnum() = conf.devnum();
    
    boardio.tx()->send(ptr);
    debug("\niop:sendConfig:devnum is %u",conf.devnum());

#if defined INDOORINO_SAMPLER

    ptr = reallocPacket(ptr, IBACOM_CONF_SAMPLER);
    
    *ptr->p_stepday1()  = conf.step();
    *ptr->p_stephour1() = conf.cool();

    debug("\niop:sendConfig:devnum is %u",conf.devnum());
    for (uint8_t i=0; i<conf.devnum(); i++)
    {
        ptr=conf.device(ptr,i);
        boardio.tx()->send(ptr);
    }

#elif defined INDOORINO_ESPSERVER
    
//     ptr = reallocPacket(ptr, IBACOM_CONF_ESP);
//     
//     uint32_t p = conf.localport();
//     uint8_t  a = conf.attempts();
//     uint32_t t = conf.timeout();
//     
//     char ip[LEN_IPADDRESS];
//     WiFi.localIP().toString().toCharArray(ip,LEN_IPADDRESS);
//     strcpy(ptr->p_ip(), ip);
//     memcpy(ptr->p_port(), &p, sizeof(uint32_t));
//     memcpy(ptr->p_timeout(), &t, sizeof(uint32_t));
//     memcpy(ptr->p_level(), &a, sizeof(uint8_t));
//     boardio.tx()->send(ptr);
//     
//     debug("\niop:sendConfig:devnum is %u",conf.devnum());
//     for (uint8_t i=0; i<conf.devnum(); i++)
//     {
//         ptr=conf.device(ptr,i);
//         boardio.tx()->send(ptr);
//     }
    
#endif
    delete(ptr);
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

bool        is_type_devconf     (ipacket * ptr)
{
    ibacomm_t c = ptr->command();
    for (uint8_t i=0; i<ATT_DEVICE_LIST_NUM; i++)
    {
        if (c == ATT_DEVICE_LIST[i]) return true;
    }
    return false;
}

void        dump_packet         (ipacket * ptr)
{
    varmap_template s;
    packetmap_template p;
    ibasize_t position=0;
    ibacomm_t c = ptr->command();
    
    char        c_buffer    [SERIAL_TX_BUFFER_SIZE] {0};
    uint8_t     c_uint8     =0;
    uint32_t    c_uint32    =0;
    
    debug("\nDUMP: Packet <%u:%s> ",
           ptr->command(),
           ptr->label());
    debug("[size:total:checksum] [%u:%u:%u]",
           ptr->data_size(),
           ptr->full_size(),
           ptr->checksum());
    
    for (ibacomm_t i=0; i<TOTAL_IBACOM; i++)
    {
        memcpy_P(&p, &PackeTable[i], sizeof(p));
        if (p.comm == c)
        {            
            for (ibavar_t  n=0; n<p.fields; n++)
            {
                for (ibavar_t m=0; m<VARMAP_NUMBER; m++)
                {
                    if (p.var[n] == m)
                    {
                        memcpy_P(&s, &VarTable[m], sizeof(s));
                        switch (s.type)
                        {
                            case 'c':
                            {
                                memset(c_buffer, 0, SERIAL_TX_BUFFER_SIZE);
                                strncpy(c_buffer, ptr->payload() + position, s.size);
                                debug("\n\t --> %s: <%s>",s.name, c_buffer);
                                break;
                            }
                            case 's':
                            {
                                memcpy(&c_uint8, ptr->payload() + position, sizeof(uint8_t));
                                debug("\n\t --> %s: [ %u ]",s.name, c_uint8);
                                break;
                            }
                            case 'l':
                            {
                                memcpy(&c_uint32, ptr->payload() + position, sizeof(uint32_t));
                                debug("\n\t --> %s: [ %u ]",s.name, c_uint32);
                                break;
                            }
                            default:    error_mem("\n\t *** INVALID VAR TYPE <%s:%c> ***", s.name, s.type);
                        }
                        position += s.size;
                    }
                }
            }
            break;
        }
    }
    
//     if (ptr->command() == IBACOM_REPORT)
//     {
//         printf("\n\tName : <%s>\n\tMessage : <%s>\n\tEpoch : <%u>, Level : <%u>",
//                ptr->p_name(),
//                ptr->p_message(),
//                *ptr->p_epoch(),
//                *ptr->p_level());
//     }
}

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
