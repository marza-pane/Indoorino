/*
 * epsclient.h
 *
 *  Created on: Jan 21, 2021
 *      Author: n00b
 */

#ifndef _INDOORINO_SRC_ESPCLIENT_H_
#define _INDOORINO_SRC_ESPCLIENT_H_

// #if defined(INDOORINO_NETWORK)
// #error "*******************************"
// #endif

#if defined(ESP8266)

#include "../common/definitions.h"
#include "netutils.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       WiFi Connection manager                                     |
//      |___________________________________________________________________|
// 

class               NetworkConnection
{
    uint32_t        _last=0;
    uint32_t        _reqtime=0;
    uint32_t        _counter=0;
    bool            _status=false;
    
public:
    NetworkConnection();

    void            begin           (void);
    void            stop            (void);
    void            loop            (void);

    uint32_t        disconnections  () { return _counter; }
    bool            status          () { return _status; }
};

//      _____________________________________________________________________
//      |                                                                   |
//      |       Tcp Connection manager                                      |
//      |___________________________________________________________________|
//      IPAddress().toString().toCharArray(ip,INET_ADDRSTRLEN);

class               IndoorinoEspClient
{

private:
    net::Safe       _aes;
    WiFiClient      _client;
    IPAddress       _serveraddress;
    uint16_t        _serverport;
    bool            _status=false;
    
    bool            read_data       (char *, iSize_t);
    bool            match           (const char *); // TODO

protected:

    uint32_t        _total_rx=0;
    uint32_t        _total_tx=0;
    
    uint32_t        _unmatched=0;
    uint32_t        _unsent=0;
    uint32_t        _overflows=0;

    utils::ObjectQueue<packet::netpacket>   _txqueue;
    utils::ObjectQueue<packet::netpacket>   _rxqueue;
    
    
    
    void            on_packet       (packet::netpacket *);
    bool            read_packet     (void);
    bool            write_packet    (void);
    
public:
             IndoorinoEspClient();
    virtual ~IndoorinoEspClient();
    
    bool            begin           (const char *, uint16_t);
    bool            begin           (void);
    void            stop            (void);
    void            loop            (void);
    void            send            (packet::netpacket * p, bool direct=false);
    void            send            (packet::ipacket * p,   bool direct=false);
    
    bool            status          (void) { return (_status && _client.status()); }
    uint32_t        clientport      (void) { return (uint32_t)_client.localPort(); }
    
    uint32_t        total_tx        (void) { return _total_tx;  }
    uint32_t        total_rx        (void) { return _total_rx;  }
    
    uint32_t        missing         (void) { return _unmatched; }
    uint32_t        unsent          (void) { return _unsent;    }
    uint32_t        overflows       (void) { return _overflows; }

    utils::ObjectQueue<packet::netpacket>& incoming() { return _rxqueue; }

    friend class NetworkConnection;
};

extern NetworkConnection    network;
extern IndoorinoEspClient   client;


#endif /* ESP8266 */

#endif /* _INDOORINO_SRC_ESPCLIENT_H_ */


