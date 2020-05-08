/*
 * espServerUtils.h
 *
 *  Created on: Nov 13, 2019
 *      Author: n00b
 */

#ifndef NETUTILS_SOURCE_H_
#define NETUTILS_SOURCE_H_

#include "../indoorino/indoorino.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <ctype.h>
// #include <string.h>
// #include <stdint.h>
// 
// #include <ESP8266WiFi.h>
// #include <WiFiUdp.h>

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

class                               address
{
protected:
    st_address  _address;
    char        _buffer[LEN_IPADDRESS];
    
public:
    
     address    (const char * a) { set(a,0); };
     address    (const char * a, uint16_t b) { set(a,b); };
     address    (uint8_t,uint8_t,uint8_t,uint8_t,uint16_t p=0);
     address    () {};
    ~address    () {};
    
    bool            set         (const char *, uint16_t);
    uint8_t         ip          (uint8_t i) { return _address.ip[i];};
    char    *       ip          (void)      { return _buffer;       };
    uint16_t        port        (void)      { return _address.port; };
};


#endif /* NETUTILS_SOURCE_H_*/
