/*
 * network/client.h
 *
 *  Created on: Jan 04, 2021
 *      Author: n00b
 * 
 * Thanks to Javidx9 - OneLoneCoder
 * 
 * http://www.onelonecoder.com
 * https://github.com/EinarArnason/ArduinoQueue
 * https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA
 * 
 */

#pragma once

#ifndef INDOORINO_SHELL_CLIENT_H_
#define INDOORINO_SHELL_CLIENT_H_

#if defined(INDOORINO_CLIENT)

#include "../network/connection.h"


class ShellClient
{
    
private:

    std::string _name;    
    std::string _host;
    uint16_t    _port;

protected:

    boost::asio::io_context                      _asio_context;
    std::thread                                  _thread_context;
    std::unique_ptr<net::clientConnection>       _connection;
    utils::ObjectQueue<packet::netpacket>        _rxqueue;

public:
    
     ShellClient();
    ~ShellClient();
    
    bool            begin               (const std::string& n, const std::string& h, const uint16_t p);
    void            loop                (void);
    void            stop                (void);
    bool            is_connected        (void);
    bool            connect2server      (void);
    
    void            send                (const char *, packet::ipacket *);    
    void            send2server         (packet::ipacket * p) { send(DEFAULT_TARGET, p); }
public:
    
    void            send                (packet::netpacket *);
    void            login               (const char * name, const char * passwd)
    {
        
    }
    
    void            PingServer          (void)
	{
	}

	void           MessageAll          ()
	{
	}
	
public:
    std::string& name() { return _name; }
    utils::ObjectQueue<packet::netpacket>& incoming() { return _rxqueue; }
};

extern ShellClient * Client;
// static      ShellClient Client;

#endif /* INDOORINO_CLIENT */

#endif /* INDOORINO_SHELL_CLIENT_H_*/




