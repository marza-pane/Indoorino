/*
 * packetUtils.h
 *
 *  Created on: Apr 28, 2020
 *      Author: n00b
 */

#include "../indoorino/indoorino.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    TargetAddress                                                     |
//    |______________________________________________________________________|
//

char errorbuffer[F2C_BUFFER_SIZE] {0};

netaddress::netaddress  (uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint16_t p)
{
    _address.ip[0]=c0;
    _address.ip[1]=c1;
    _address.ip[2]=c2;
    _address.ip[3]=c3;
    _address.port = p;
}

bool            netaddress::_is_valid   (void)
{
    if (!is_string_ip(_buffer))
    {
        error_net("address:%s:invalid ip %s", errorbuffer, _buffer);
        return false;
    }
    else if (_address.port < PORT_SOCKET_FIRST)
    {
        error_net("address:%s:invalid port %u for %s", errorbuffer, _address.port, _buffer);
        return false;
    }
    return true;

}

bool            netaddress::is_valid    (void)
{
    strcpy(errorbuffer, "check");
    return _is_valid();
}

bool            netaddress::set         (const char * ip_addr, uint16_t port_addr)
{
    memset(_buffer, 0, LEN_IPADDRESS);
    
    /* Thanks unknown pal on stackoverlow */
    
    char buf[LEN_IPADDRESS];
    strcpy(buf, ip_addr);

    char  *  pch = strtok (buf,".");
    uint8_t j=0;
    while (pch != NULL)
    {
        _address.ip[j] = (uint8_t)atoi(pch);
        pch = strtok (NULL, ".");
        j++;
    }
    
    _address.port = port_addr;
    sprintf(_buffer, "%u.%u.%u.%u",
            _address.ip[0],
            _address.ip[1],
            _address.ip[2],
            _address.ip[3]);
    
    strcpy(errorbuffer, "set:ip-and-port");
    return (_is_valid());
}

bool            netaddress::set         (st_address * address)
{
    for (uint8_t i=0; i<4; i++) _address.ip[i]=address->ip[i];
    _address.port = address->port;
    
     sprintf(_buffer, "%u.%u.%u.%u",
            _address.ip[0],
            _address.ip[1],
            _address.ip[2],
            _address.ip[3]);
     
     strcpy(errorbuffer, "set:st_address");
     return (_is_valid());
}

