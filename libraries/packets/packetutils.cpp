/*
 * packetutils.cpp
 *
 *  Created on: Jan 05, 2020
 *      Author: n00b
 */

#include "ipacket.h"

namespace packet
{
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Packet utils                                                |
//      |___________________________________________________________________|
//    
    
    int16_t     payloadsize         (iCom_t command)
    {
        int16_t s;
        ipacket p(command);
        
        if (p.command() > 0)    {   s=p.data_size();    }
        else                    {   s = -1;             }
        
        return s;
    }
    
    int16_t      packetsize         (iCom_t command)
    {
        int16_t r=payloadsize(command);
        if (r > 0)  { return int16_t(r + SIZEOF_PACKET_HEADER); }
        else        { return r; }
    }
    
    /* WARNING this section relays on how user defines packet order. So don't touch packet order and follow the comments in <comtable.h> */
    bool        is_boardconf        (iCom_t command)
    {
        if (command > IBACOM_CONF_STD && command <= IBACOM_CONF_CAMERA) { return true; }
        if (command == IBACOM_CONF_ESP) { return true; }
        return false;
    }
    
    bool        is_boardstat        (iCom_t command)
    {
        if (command > IBACOM_STATUS_STD && command <= IBACOM_STATUS_CAMERA) { return true; }
        return false;       
    }
        
    bool        is_devconf          (iCom_t command)
    {
        if (command >= IBACOM_CONF_ASENSOR && command <= IBACOM_CONF_DEVSTD) { return true; }
        return false;        
    }
    
    bool        is_devstat          (iCom_t command)
    {
        if (command >= IBACOM_STATUS_ASENSOR && command <= IBACOM_STATUS_DEVSTD) { return true; }
        return false;        
    }
    /* end of section */

//      _____________________________________________________________________
//      |                                                                   |
//      |       Packet shortcuts                                            |
//      |___________________________________________________________________|
//    

    ipacket *   report              (uint8_t level, const char * source, const char * message)
    {
        uint8_t u=level;
        iEpoch_t t=utils::epoch_now();
        ipacket * p = new ipacket(IBACOM_REPORT);
        strncpy(p->p_name(), source, LEN_NAME);
        strncpy(p->p_message(), message, SERIAL_TX_BUFFER_SIZE);
        memcpy(p->p_level(), &u, sizeof(uint8_t));
        memcpy(p->p_epoch(), &t, sizeof(iEpoch_t));
        return p;
    }

    ipacket *       epoch                       (void)
    {
        ipacket * p = new ipacket(IBACOM_UNIX_EPOCH);
        iEpoch_t t=utils::epoch_now();
        memcpy(p->p_epoch(), &t, sizeof(iEpoch_t));
        return p;
    }
    
} /* packet*/
