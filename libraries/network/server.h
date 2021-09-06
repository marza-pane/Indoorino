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

namespace net
{
    class IndoorinoServer
    {
    protected:
        bool                    _flag_netchek=false;
        std::thread             _thr_netcheck;
        std::mutex              _mtx_netcheck;
        std::condition_variable _cv_netchek;
        
//         void        start_netchek   (void);
//         void        stop_netchek    (void);
        
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
    
} /* namespace:net */

extern  net::IndoorinoServer    Server;


#endif /* INDOORINO_SERVER */
#endif /* SOURCE_INDOORINO_SERVER_H_ */

