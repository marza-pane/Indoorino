/*
 * packetSend.h
 *
 *  Created on: Apr 28, 2020
 *      Author: n00b
 */

#ifndef PACKESEND_H_
#define PACKESEND_H_

#include "packeTemplate.h"

class packetTransmit
{
    uint32_t    _total_tx=0;

public:
    packetTransmit                  () {};
    void            send            (ipacket *);              
    void            send            (ipacket *, const char *);
    
#if defined(ESP8266)
    void            sendUdp         (ipacket *);
    void            sendUdp         (ipacket *, const char*);
#endif
    
    void            sendSerial      (ipacket *);
    void            sendSerial      (ipacket *, const char*);
    
    uint32_t        total_tx        (void)      { return _total_tx;  };
};
#endif /* PACKESEND_H_ */
