#include "net_utils.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    TargetAddress                                                     |
//    |______________________________________________________________________|
//

address::address (uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint16_t p)
{
    _ip[0]=c0;
    _ip[1]=c1;
    _ip[2]=c2;
    _ip[3]=c3;
    _port = p;
}

bool            address::set            (const char * ip_addr, uint16_t port_addr)
{
    memset(_buffer,0,LEN_IPADDRESS);
    
    if (!is_string_ip(ip_addr))
    {
        error_net("address:set:invalid ip %s",ip_addr);
        return false;
    }
    
    char buf[LEN_IPADDRESS];
    strcpy(buf, ip_addr);

    /* Thanks unknown pal on stackoverlow */
    char  *  pch = strtok (buf,".");
    uint8_t j=0;
    while (pch != NULL)
    {
        _ip[j] = (uint8_t)atoi(pch);
        pch = strtok (NULL, ".");
        j++;
    }    
    _port = port_addr;
    sprintf(_buffer, "%u.%u.%u.%u", _ip[0], _ip[1], _ip[2], _ip[3]);
    return true;
}
