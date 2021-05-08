/*
 * network/boardserver.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: n00b
 * 
 */

#if defined (INDOORINO_SERVER)
#include "../network/server.h"
#include "../indoorino/indoorino-system.h"

static bool _flag_ping=false;

namespace net
{

    serverBoards::serverBoards(
        utils::ObjectQueue<packet::netpacket>& queue,
        uint16_t port):
            serverTemplate(queue, port)
    {
        _timeout = std::chrono::milliseconds(TIMEOUT_CLIENT_BOARD);
        _flag_ping=true;
        _thread_ping = std::thread([this]()
        {
            iEpoch_t timeout=utils::millis() + 60000;
            info_server("ping thread start!");
            while (_flag_ping)
            {
                iEpoch_t ima = utils::millis();
                if (ima > timeout)
                {
                    timeout = ima + 60000;
                    for (auto& c: _clientlist)
                    {
                        c->ping();
                    }
                }
            }
            alert_server("Ping thread terminated!");
        });
    }
    
    serverBoards::~serverBoards()
    {
        this->stop();
    }

    void        serverBoards::stop                  (void)
    {
        if (_flag_ping)
        {
            _flag_ping=false;
            if (_thread_ping.joinable()) _thread_ping.join();
        }
        serverTemplate::stop();
    }
    
    void        serverBoards::sync_board            (const char * name)
    {
        if (!this->has_client(name))
        {
            warning_net("sync: can not find board <%s>", name);
            return;
        }
        
        packet::ipacket * p = packet::epoch();
        packet::netpacket t(p, BOARD_NAME, name);
        alert_net("Sync for board <%s>", name);
        this->send(&t);
        delete p;
    }

    void        serverBoards::sync_board            (void)
    {
        packet::ipacket * p = packet::epoch();
        
        for (auto& client : this->_clientlist)
        {
            client->broadcast(BOARD_NAME, p);
        }
        delete p;
    }
        
    bool        serverBoards::on_new_client         (std::shared_ptr<serverConnection> client)
    {        
        info_server("new board client [%u] :address <%s>",client->id(), client->ipstring());
        
//         packet::netpacket p(IBACOM_REPORT);
//         strcpy(p.p_name(), "INDOORINO SERVER");
//         strcpy(p.p_message(), "Welcome to indoorino network!");
//         *p.p_epoch() = utils::epoch_now();
//         *p.p_level() = 1;
        
//         Server.schedule.pushInSeconds(&p, 3, true);

// //         strcpy(p->source, "SERVER");
//         strcpy(p.target, "SRV.HSE.BMS");
//         
// //         time_t ima;
// //         time(&ima);
// //         
// //         time_t epc = *p->p_epoch();
// //         
// //         std::cout << "Sending welcome packet with epoch: [" << *p->p_epoch() << "]" << std::endl;
// //         std::cout << "Sending welcome packet with  time: [" <<  << "]" << std::endl;
// //         std::cout << std::put_time(std::localtime(&epc), "%Y %m %d %H:%M:%S") << std::endl;

        return true;
    }

    void        serverBoards::on_lost_client        (std::shared_ptr<serverConnection> client)
    {
        alert_server("board client [%u] disconnected!", client->id());
    }


} /* namespace : net */

#endif /* INDOORINO_SERVER */
