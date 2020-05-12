/*
 * packetUtils.h
 *
 *  Created on: Apr 28, 2020
 *      Author: n00b
 */

#ifndef PACKETUTILS_H_
#define PACKETUTILS_H_

#include "packetParse.h"
#include "packetSend.h"

#if defined (ARDUINO)

class BoardIO
{
    packetParse     *   _rx=nullptr;
    packetTransmit  *   _tx=nullptr;
public:
    BoardIO             (bool);
    void                begin       (void);
    void                loop        (void);
    packetParse     *   rx          (void)  { return _rx; };
    packetTransmit  *   tx          (void)  { return _tx; };  
};

extern BoardIO boardio;

void                sendReport          (const uint8_t, const char *, const __FSH*, ...);
void                sendReport          (const uint8_t, const __FSH*, const __FSH*, ...);
void                sendReport          (const uint8_t, const __FSH*, ...);

void                sendConfig          (void);
void                sendStatus          (void);
    
    /* allarm types:
     * 0=flood
     * 1=smoke
     * 2=temperature
     * 3=fire
     * 4=power
     *
     * warning types:
     * 0=open door
     * 1=open window
     * 2=unlocked
     * 3=power
    */

void                sendAllarm          (const uint8_t, const uint8_t);  
void                sendWarning         (const uint8_t, const uint8_t);

#endif /* ARDUINO */

#define ATT_DEVICE_LIST_NUM 6
const ibacomm_t ATT_DEVICE_LIST[] PROGMEM = {
    IBACOM_CONF_ASENSOR,
    IBACOM_CONF_SWITCH,
    IBACOM_CONF_LDR,
    IBACOM_CONF_DHT22,
    IBACOM_CONF_RELAY,
    IBACOM_CONF_DEVSTD
};

bool                is_type_devconf     (ipacket *);
bool                isSensorConf        (ipacket *);
bool                isActuatorConf      (ipacket *);


ibasize_t           packet_size         (ibacomm_t);
void                dump_packet         (ipacket * ptr);
ipacket     *       reallocPacket       (ipacket *, ibacomm_t);

#endif /* PACKETUTILS_H_ */
