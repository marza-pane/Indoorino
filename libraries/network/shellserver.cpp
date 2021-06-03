/*
 * network/shellserver.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: n00b
 * 
 */

#if defined (INDOORINO_SERVER)
#include "../network/server.h"

namespace net
{
    
    serverShell::serverShell(
        utils::ObjectQueue<packet::netpacket>& queue,
        uint16_t port):
            serverTemplate(queue, port)
    {
        _timeout = std::chrono::milliseconds(TIMEOUT_CLIENT_SHELL);        
    }
    
    serverShell::~serverShell()
    {
        this->stop();        
    }

//     void        serverShell::broadcast              (packet::netpacket * p)
//     {
//         if (!utils::is_devname(p->source)) { strcpy(p->source, BOARD_NAME); }
//         serverTemplate::broadcast(p);
//     }
    
    bool        serverShell::on_new_client          (std::shared_ptr<serverConnection> client)
    {
        info_server("new shell client [%u] :address <%s>",client->id(), client->ipstring());
        return true;
    }
    
//     void        serverShell::on_lost_client         (serverConnection * client)
//     {
//         alert_server("shell client [%u] disconnected!", client->id());
//     }
// 
//     void        serverShell::on_packet              (serverConnection * client, packet::netpacket * incoming)
//     {   
//                
//         info_server("new shell packet <%s> from [%u]", incoming->description(), client->id());
//         incoming->dump();
//         
//         switch (incoming->command())
//         {
//             case IBACOM_REPORT:
//             {
//                 break;
//             }
//             case IBACOM_REQ_LOGIN:
//             {
//                 break;
//             }
//             default:
//             {
//                 
//             }
//         }
//         
//         delete incoming;
//     }

    void        serverShell::on_lost_client         (std::shared_ptr<serverConnection> client)
    {
        alert_server("shell client [%u] disconnected!", client->id());
    }

} /* namespace : net */
#endif /* INDOORINO_SERVER */
