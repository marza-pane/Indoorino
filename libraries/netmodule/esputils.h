/*
 * espServerUtils.h
 *
 *  Created on: May 11, 2020
 *      Author: n00b
 */

#ifndef ESPUTILS_SOURCE_H_ 
#define ESPUTILS_SOURCE_H_

#if defined(ESP8266)

#include <stdint.h>
#include <stdlib.h>

//    ________________________________________________________________________
//    |                                                                      |
//    |    Keep Alive Connection loop                                        |
//    |______________________________________________________________________|
//

class                               connectionLoop
{
    uint32_t    _last=0;
    uint32_t    _reqtime=0;
    bool        _status=false;
    
public:
     connectionLoop();
    ~connectionLoop()   {};

    bool    status      (void)  { return _status; };
    void    begin       (void);
    void    loop        (void);
    
};

//    ________________________________________________________________________
//    |                                                                      |
//    |    TRANSIENT PACKET : server packet                                  |
//    |______________________________________________________________________|
//

class                               transientPacket
{
    char        *       _data=nullptr;
    ibasize_t           _size=0;
    uint32_t            _checksum=0;
    uint32_t    *       _time_sent=nullptr;
    uint16_t    *       _time_count=nullptr;
    uint8_t     *       _match=nullptr;
    uint8_t             _status=0;
    
    /*
        STATUS:
        0: idle
        1: waiting
        2: done
        3: expiried
    */

public:
     transientPacket  (ipacket *, const char *);
    ~transientPacket  ();
    
    void                start       (void);
    bool                match       (const char *, uint32_t);
    
    ibasize_t           size        (void)  { return _size;       };
    ibasize_t           checksum    (void)  { return _checksum;   };
    uint8_t             status      (void)  { return _status;     };
    void                loop        (void);
    
//     uint32_t            time_sent   (void)  { return _time_sent;  };
//     uint32_t            time_count  (void)  { return _time_count; };
    
};

//    ________________________________________________________________________
//    |                                                                      |
//    |    Server Packet List : used as packet queue                         |
//    |______________________________________________________________________|
//

class                               PacketList
{
    
    transientPacket **  _list               =nullptr;    
    uint16_t            _count              =0;
    uint32_t            _total_miss         =0;
    uint32_t            _total_tx           =0;

    void                _reallocate_list    (uint16_t);

public:
    
     PacketList ()      {};
    ~PacketList ();
    
    void                loop        (void);
    void                push        (ipacket *);
    void                push        (ipacket *, const char *);
    bool                match       (const char *, uint32_t);

    void                pop         (uint16_t);
    transientPacket *   get         (uint16_t i)    { return(_list[i]);     };
    uint16_t            waitin      (void)          { return(_count);       };
    uint32_t            total_tx    (void)          { return(_total_tx);    };
    uint32_t            total_miss  (void)          { return(_total_miss);  };

};

#endif /* ESP8266 */

#endif /* ESPUTILS_SOURCE_H_ */



