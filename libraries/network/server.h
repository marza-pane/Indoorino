/*
 * network/server.h
 *
 *  Created on: Jan 04, 2021
 *      Author: n00b
 * 
 */

#pragma once

#ifndef SOURCE_INDOORINO_SERVER_H_
#define SOURCE_INDOORINO_SERVER_H_

#if defined(INDOORINO_SERVER)

#include "connection.h"

class IndoorinoServer
{
private:
    std::thread             _thr_netcheck;
protected:
    utils::ObjectQueue<packet::netpacket> _rxqueue;
    
public:
    IndoorinoServer();
    ~IndoorinoServer();
    
    void        begin   (void);
    void        loop    (void);
    void        stop    (void);
    
    void        on_packet       (packet::netpacket *);
    
    void        parse_request   (const char *);
    
    net::serverBoards   board;
    net::serverShell    shell;
    
    indoorino::Schedule schedule; 
};

extern  IndoorinoServer      Server;


#endif /* INDOORINO_SERVER */
#endif /* SOURCE_INDOORINO_SERVER_H_ */

