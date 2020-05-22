/*
 * espServerUtils.cpp
 *
 *  Created on: May 11, 2020
 *      Author: n00b
 */

// #if defined(ESP8266)

#include "../indoorino/indoorino.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    DEBUG TOOLS   (keeps connection alive)                            |
//    |______________________________________________________________________|
//

#if defined(ESP8266)

connectionLoop::connectionLoop ()
{
    _last = millis();
    WiFi.mode(WIFI_STA);
}

void                connectionLoop::begin       (void)
{
    char ssid[64];
    char psk[64];
    
    strcpy(ssid, P2C(DEFAULT_SSID));
    strcpy(psk, P2C(DEFAULT_PSK));
    
    debug("\n%s Connecting to : %s (%s)... ",P2C(INDOORINO_TYPE), ssid, psk);
    WiFi.begin(ssid, psk);
    _reqtime=millis() + AUTH_TIMEOUT;

}

void                connectionLoop::loop        (void)
{
    if (_status)
    {
        /* If connected to Wifi, just check for connection status every RECONNECT_TIMEOUT */
        if (millis() - _last < RECONNECT_TIMEOUT)
        {
            _last = millis();
            if (!WiFi.isConnected())
            {
                debug("\nConnection Lost! Reconnecting...");
                _status=false;
                _reqtime=millis() + AUTH_TIMEOUT;
                Udp.stop();
                blinker.start(1);
                WiFi.reconnect();
            }
        }
    }
    else
    {
        if (millis() < _reqtime)
        {
            if (WiFi.isConnected())
            {
                char ip[LEN_IPADDRESS];

                _status=true;
                _last=millis();
                
                Udp.begin(DEFAULT_LOCALPORT);
                blinker.stop();
                
                WiFi.localIP().toString().toCharArray(ip,LEN_IPADDRESS);    
                error_net(" ### Connected (channel %u)! ###",WiFi.channel());
                error_net(" ### UDP server address %s:%u ###\n", ip, DEFAULT_LOCALPORT);
                sendReport(1, BOARD_NAME, F("ESP connected to %s address :%s:%u"),
                                   P2C(DEFAULT_SSID),ip,DEFAULT_LOCALPORT);  
            }
        }
        else
        {
                error_net("\nReconnection timed out! Retrying...");
                _status=false;
                _reqtime=millis() + AUTH_TIMEOUT;
                WiFi.reconnect();            
        }
        
    }
}


//    ________________________________________________________________________
//    |                                                                      |
//    |    TRANSIENT PACKET : server packet                                  |
//    |______________________________________________________________________|
//
   
transientPacket::transientPacket (ipacket * ptr, const char * name)
{
    _size = ptr->full_size();
    _checksum = ptr->checksum();
    _data = (char *)calloc(_size, sizeof(char));

    _time_sent=(uint32_t*)calloc(conf.address_num(), sizeof(uint32_t));
    _time_count=(uint16_t*)calloc(conf.address_num(), sizeof(uint16_t));
    _match=(uint8_t*)calloc(conf.address_num(), sizeof(uint8_t));
    
    debug("\ntransientPacket: New %s:%s:%uB checksum:%u", name, F2C(ptr->label()), _size, _checksum);
    ptr->forge(_data, name);
}

transientPacket::~transientPacket ()
{
    free(_data); 
    free(_time_count);
    free(_time_sent);
    free(_match);
}

void                transientPacket::start      (void)
{
    for (uint8_t i=0; i<conf.address_num(); i++)
    {       
        udpwrite(conf.address(i)->ip(), conf.address(i)->port(), _data, _size);

        _time_sent[i]=millis();
        _time_count[i]++;
        _match[i]=false;
    }
    
    debug_mem("\ntransientPacket: Start %u:%u [B:C]",  _size, _checksum);
    _status = 1;
}

bool                transientPacket::match      (const char * ip, uint32_t checksum)
{
    
    if (checksum == _checksum)
    {
        for (uint8_t i=0; i<conf.address_num(); i++)
        {
            if (strcmp(ip, conf.address(i)->ip()) == 0)
            {
                debug_net("\ntransientPacket: MATCH %u:%u [B:C]", _size, _checksum);
                _match[i]=true;
                for (uint8_t j=0; j<conf.address_num(); j++)
                {
                    if (_match[i]==false)   return(true);
                }
                
                debug_net(" - all checksum obtained!");
                _status = 2;
                return(true);
            }
        }
        error_net("transientPacket: checksum %u from unknown IP %s", _checksum, ip);
        return(true);
    }
    return(false);
}

void                transientPacket::loop       (void)
{
    if (_status == 3 || _status == 2)
    {
        return;
    }
    else if (_status == 1)
    {
        uint32_t t0 = millis();
        for (uint8_t i=0; i<conf.address_num(); i++)
        {
            if (!_match[i] && ((t0 - _time_sent[i]) > conf.timeout()) )
            {
                _time_sent[i]=millis();
                _time_count[i]++;
                
                udpwrite(conf.address(i)->ip(), conf.address(i)->port(), _data, _size);
                
                if (_time_count[i] >= conf.attempts())
                {
                    debug_net("\ntransientPacket: TIMED OUT %u:%u [B:C]", _size, _checksum);
                    _status = 3;
                }
                
            }
        }
    }
    else if (_status == 0)
    {
        start();
    }
}

//    ________________________________________________________________________
//    |                                                                      |
//    |    Server Packet List : used as packet queue                         |
//    |______________________________________________________________________|
//

PacketList::~PacketList ()
{
    for (uint16_t i=0; i<_count; i++)   delete(_list[i]);
    free(_list);
}

void                PacketList::_reallocate_list        (uint16_t size)
{
    if (size == 0)
    {
        
        debug_net("\nPacketlist:emptylist");
        free(_list);
        _list = nullptr;
    }
    else if (size == _count)
    {
        debug_net("\nPacketlist: No size difference [%u]", size);
        return;
    }
    else if (_list == nullptr)
    {
        debug_net("\nPacketlist: Empty list --> allocating %u", size);
        _list = (transientPacket **)calloc(size, sizeof(transientPacket *));
    }
    else
    {
        transientPacket ** p = (transientPacket **)realloc(_list, size * sizeof(transientPacket *));
        if (p != nullptr)
        {
            debug_net("\nPacketlist: Reallocated to %u", size);
            _list = p;
        }
        else
        {
            error_mem("FATAL:Packetlist: can not reallocate transientPacket list");
        }
    }
    _count = size;
}

void                PacketList::push             (ipacket * p)
{
//     debug_net("\n\n*** Board name is %s", P2C(BOARD_NAME));
    push(p, P2C(BOARD_NAME));
}

void                PacketList::push             (ipacket * p, const char * name)
{
    char target[LEN_NAME]{0};
    strcpy(target, name);
        
    _reallocate_list(_count + 1);
    _list[_count - 1] = new transientPacket(p, target);
    debug_net("\nPacketlist:push: %s:%s (total %u)", target, F2C(p->label()), _count);
}

void                PacketList::pop              (uint16_t index)
{
    if (_count == 0)
    {
        debug_net("\nPacketlist:pop: no packet to pop at index %u!",index);
        return;
    }
    
    if (index >= _count)
    {
         debug_net("\nPacketlist:pop: element %u is out of range (count = %u)!", index, _count);
         return;
    }
    
    delete _list[index];
    for (uint16_t i=index; i<_count - 1; i++)
    {
        _list[i] = _list[i + 1];
    }
    
    _reallocate_list(_count - 1);
}

void                PacketList::loop             (void)
{
    static uint16_t index=0;
    
//     debug_net("\nCOUNT  is  %u,  INDEX  is  %u",_count, index);
//     delay(500);

    if (_count == 0) return;

    _list[index]->loop();

    if (_list[index]->status() == 2)
    {
        pop(index);
    }
    else if (_list[index]->status() == 3)
    {
        _total_miss++;
        error_net("packetlist:WARNING: packet %u:%u timed out", index, _list[index]->checksum());
        pop(index);
    }
    
    index++;
    if (index >= _count)    index = 0;
}

bool                PacketList::match               (const char * ip, uint32_t checksum)
{
    bool flag=false;
    for (uint32_t i=0; i<_count; i++)
    {
        if (_list[i]->match(ip, checksum))
        {
            flag=true;
            _total_tx++;
            break;
        }
    }
    return(flag);
}

#endif /* ESP8266 */




