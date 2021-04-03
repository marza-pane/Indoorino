
/*
 * client main.cpp
 *
 *  Created on: Jan 02, 2021
 *      Author: n00b
 */

#if defined (INDOORINO_CLIENT)

#include "client.h"
#include <signal.h>

// ShellClient * Client; // de-comment to run dummy client

int main (void)
{
    std::cout << "\nIndoorino dummy client start!" << std::endl;
    srand(time(NULL));
    signal(SIGINT, on_exit);         
    
    Client = new ShellClient();
    
    if (!Client->begin("ANNSATSU", DEFAULT_SERVER_IP, DEFAULT_SHELL_PORT))
    {
        warning_client("Sever seems offline");
        on_exit(3);
    }

    time_t t = utils::millis() + 20000;
    time_t m = utils::millis() + 3000;
    time_t n = utils::millis();
    
//     packet::ipacket * p = packet::report(2, "CLIENT", "This is a test message");
    
    
    utils::wait(1000);
    packet::ipacket r(IBACOM_RESET);
    Client->send("SRV.HSE.BMS", &r);
    
    
    while (n < t)
    {
        Client->loop();
        
        if (n > m)
        {
            m=n + 3000;
            Client->send("SERVER", packet::report(2, "CLIENT", "This is a test message")); // WARNING:memory leak
        }
        
        n=utils::millis();
        while(!Client->incoming().is_empty())
        {
            packet::netpacket * q = Client->incoming().pop_front();
            error_client("My lovely client got a packet <3 %s", q->description());
            delete q;
        }
    }
    
    std::cout << "Now disconnection" << std::endl;
    utils::wait(2000);
    Client->stop();
    delete Client;
    std::cout << "Now reconnection" << std::endl;
    utils::wait(2000);

    Client = new ShellClient();

    if (!Client->begin("ANNSATSU", DEFAULT_SERVER_IP, DEFAULT_SHELL_PORT))
    {
        warning_client("Sever seems offline");
        on_exit(3);
    }

    t = utils::millis() + 20000;
    m = utils::millis() + 3000;
    n = utils::millis();
    
    packet::netpacket z (IBACOM_WARNINGS);
    
    utils::wait(1000);
        
    while (n < t)
    {
        Client->loop();
        
        if (n > m)
        {
            m=n + 3000;
            std::cout << "sending.." << std::endl;
            Client->send("SERVER", &z); // WARNING:memory leak

//             Client->send("SERVER", &z);
        }
        
        n=utils::millis();
        while(!Client->incoming().is_empty())
        {
            packet::netpacket * q = Client->incoming().pop_front();
            error_client("My lovely client got a packet <3 %s", q->description());
            delete q;
        }
    }
    
//     p->dump();
//     if (p != nullptr) delete p;

//     t = utils::millis() + 80000;
//     while (utils::millis() < t)
//     {
//         Client->loop();
//         while(!Client->incoming().is_empty())
//         {
//             p = Client->incoming().pop_front();
//             error_client("My lovely client got a packet <3 %s", p->description());
//             delete p;
//         }
//     }
    
    on_exit(0);
}

#endif /* INDOORINO_CLIENT */
