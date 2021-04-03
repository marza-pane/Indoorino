/*
 * network/netutils.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_NETUTILS_H_
#define SOURCE_NETUTILS_H_

#include "../common/definitions.h"
#include "../packets/ipacket.h"

#if defined(INDOORINO_NETWORK)

// #include <netinet/in.h>
// #include <linux/input.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>

namespace net
{

//      _____________________________________________________________________
//      |                                                                   |
//      |       Generic network functions                                   |
//      |___________________________________________________________________|
//
    
#if defined(__linux__)
    bool            netstatus       (bool force=false);
    char    *       local_ip        (void);
//     void            on_exit         (void);

#endif /* __linux__ */


//      ________________________________________________________________________
//      |                                                                      |
//      |    AES Encryption                                                    |
//      |______________________________________________________________________|
//

    class Safe
    {
    private:
        uint8_t     _masterkey  [LEN_AES_MASTERKEY] {0};
        uint8_t     _iv         [N_BLOCK]           {0};

    protected:
        AESLib      _aes;
        void        _geniv      (uint8_t *);
    public:
        
        /* look at the fool we got here today, mr. juicy asshole */
        char    *   masterkey   (void) { return (char *)_masterkey; }
        
        Safe() {};
        ~Safe() { memset(_masterkey, 0, LEN_AES_MASTERKEY); };
        
        void        begin       (void);
        
        
        bool        set_key     (const char *);    
        int         cipherlen   (const iSize_t);
        iSize_t     encrypt     (char *, char *, const char *, size_t);
        iSize_t     decrypt     (char *, const char *, char *, size_t);
        void        generate    (char * p) { _geniv((uint8_t *) p); }
    };

}

// namespace packet
// {
// 
//     class buffPacket
//     {
//         net::Safe&  _aes;
//         netpacket * _item;
//         char _netheader[SIZEOF_NET_HEADER];
//         char _header[SIZEOF_PACKET_HEADER];
//         std::vector<uint8_t> _body;
//         iSize_t _size=0;
// 
//     public:
//         buffPacket(net::Safe&, netpacket *);
//         void        netbuild        (void);
//         netpacket   * item()        { return _item; }
//         const char  * netheader()   { return _netheader; }
//         const char  * header()      { return _header; }
//         const iSize_t size()        { return _size; }
//         
//         const std::vector<uint8_t>& body() { return _body; }
//     };
//     
// } /* namespace : packet */

#endif /* INDOORINO_NETWORK */

#endif /* SOURCE_NETUTILS_H_ */


/*
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std::chrono_literals;

int f()
{
    std::this_thread::sleep_for(10s); //change value here to less than 1 second to see Success
    return 1;
}

int f_wrapper()
{
    std::mutex m;
    std::condition_variable cv;
    int retValue;

    std::thread t([&cv, &retValue]() 
    {
        retValue = f();
        cv.notify_one();
    });

    t.detach();

    {
        std::unique_lock<std::mutex> l(m);
        if(cv.wait_for(l, 1s) == std::cv_status::timeout) 
            throw std::runtime_error("Timeout");
    }
*/
