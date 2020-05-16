/*
 * packetSend.cpp
 *
 *  Created on: Apr 28, 2020
 *      Author: n00b
 */

#include "indoorino.h"

#if defined (ARDUINO)


void        packetTransmit::send                (ipacket * ptr)
{
    send(ptr, P2C(BOARD_NAME));
}

void        packetTransmit::send                (ipacket * ptr, const char * name)
{
    _total_tx++;
#if defined (ESP8266)
    sendUdp(ptr, name);
#else
    sendSerial(ptr, name);
#endif
}

void        packetTransmit::sendSerial          (ipacket * ptr)
{
    sendSerial(ptr, P2C(BOARD_NAME));
}

void        packetTransmit::sendSerial          (ipacket * ptr, const char * name)
{
    if (ptr != nullptr)
    {
        char * body=(char*)calloc(ptr->full_size(), sizeof(char));
        ptr->forge(body, name);
        serialwrite(body, ptr->full_size());
        free(body);
    }
}

#if defined(ESP8266)


    #if defined(INDOORINO_ESPSERVER)
    
    // *** These are shortcuts for appending packets to server list. DO NOT EDIT ***
    extern PacketList packetlist;
    
    void        packetTransmit::sendUdp             (ipacket * ptr)
    {
        packetlist.push(ptr);
    }

    void        packetTransmit::sendUdp             (ipacket * ptr, const char * name)
    {
        packetlist.push(ptr, name);
    }
    
    #else

    void        packetTransmit::sendUdp             (ipacket * ptr)
    {
        sendUdp(ptr, P2C(BOARD_NAME));
    }

    void        packetTransmit::sendUdp             (ipacket * ptr, const char * name)
    {
        if (ptr != nullptr)
        {
            char * body=(char*)calloc(ptr->full_size(), sizeof(char));
            ptr->forge(body, name);
            serialwrite(body, ptr->full_size());
            udpwrite(P2C(DEFAULT_TARGET_IP), DEFAULT_TARGET_PORT, body, ptr->full_size());
            free(body);
        }
    }
    
    #endif /* INDOORINO_ESPSERVER */


#endif /* ESP8266 */


#endif /* ARDUINO */


















