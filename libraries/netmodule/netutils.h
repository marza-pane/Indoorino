/*
 * espServerUtils.h
 *
 *  Created on: Nov 13, 2019
 *      Author: n00b
 */

#ifndef _NETUTILS_SOURCE_H_
#define _NETUTILS_SOURCE_H_

#include "../indoorino/definitions.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    IP and port class holder                                          |
//    |______________________________________________________________________|
//

struct st_address
{
    uint8_t     ip[4] {0};
    uint16_t    port=0;
};

class                               netaddress
{
protected:
    st_address  _address;
    char        _buffer[LEN_IPADDRESS] {0};
    bool        _is_valid    (void);
    
public:
    
     netaddress     (const char * a) { set(a,0); };
     netaddress     (const char * a, uint16_t b) { set(a,b); };
     netaddress     (uint8_t,uint8_t,uint8_t,uint8_t,uint16_t p=0);
     netaddress     (st_address * a) { set(a); };
     netaddress     () {};
    ~netaddress     () {};
    
    bool            is_valid    (void);
    bool            set         (const char *, uint16_t);
    bool            set         (st_address *);
    st_address  *   get         (void)      { return &_address;      };
    uint8_t         ip          (uint8_t i) { return  _address.ip[i];};
    char    *       ip          (void)      { return  _buffer;       };
    uint16_t        port        (void)      { return  _address.port; };
};


#endif /* _NETUTILS_SOURCE_H_*/
