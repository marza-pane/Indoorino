/*
 * network/netutils.cpp
 *
 *  Created on: Jan 01, 2021
 *      Author: n00b
 */

#include "../common/indoorino.h"
#if defined(INDOORINO_NETWORK)

#if defined(__linux__)
#include <arpa/inet.h>
    char ipstring[INET_ADDRSTRLEN] = "0.0.0.0";
#endif /* __linux__ */

namespace net
{
#if defined(__linux__)
//      _____________________________________________________________________
//      |                                                                   |
//      |       Generic network functions                                   |
//      |___________________________________________________________________|
//

    bool            netstatus               (bool force)
    {
        static time_t   last=0;
        static bool     flag=false;
        time_t          now=utils::millis();

        do 
        {
            if (!force && last > now) break;
            last = now + time_t(TIME_NETWORK_CHECK);
            
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (fd == -1)
            { 
                error_net("ERROR:Socket creation failed");
                flag=false;
                break;
            }
            socklen_t len = sizeof(sockaddr);
            sockaddr_in target {0};
            target.sin_family = AF_INET; 
            target.sin_addr.s_addr = inet_addr("8.8.8.8"); 
            target.sin_port = htons(80); 
            if (connect(fd, (sockaddr*)&target, sizeof(sockaddr)) >= 0)
            { 
                if(!flag) {
                    getsockname(fd, (sockaddr*)&target, &len);
                    inet_ntop( AF_INET, &target.sin_addr, ipstring, INET_ADDRSTRLEN);
                    info_net("Connected to local network! IP:%s", local_ip());
                }
                flag=true;
            }
            else
            {
                if(flag) { warning_net("Disconnected from local network!"); }
                strcpy(ipstring, "0.0.0.0");
                flag=false;
            }
            close(fd);
        }
        while (0);
        return flag;
    }

    char    *       local_ip                (void)
    {
        return ipstring;
    }

#endif /* __linux__ */

//      ________________________________________________________________________
//      |                                                                      |
//      |    AES Encryption                                                    |
//      |______________________________________________________________________|
//

    void        Safe::begin         (void)
    {
        memcpy_P(_masterkey, (char *)AES_INIT_MK, LEN_AES_MASTERKEY);
        _aes.set_paddingmode((paddingMode)0);
    }

    void        Safe::_geniv        (uint8_t * iv)
    {
        uint8_t c;
        memset(iv, 0, N_BLOCK);
        for (uint8_t i=0; i< N_BLOCK; i++)
        {
            c = uint8_t(rand() % UINT8_MAX);
            memcpy(iv + i, &c, sizeof(uint8_t));
        }
    }

    bool        Safe::set_key       (const char * key)
    {
        if (!key)
        {
            error_aes("WARNING:setkey: nullpointer key");
            return false;
        }
        info_aes("Setting new masterkey!");
//         utils::dump_hex(key, LEN_AES_MASTERKEY);
        memcpy(_masterkey, (uint8_t *)key, LEN_AES_MASTERKEY);
        return true;
    }

    iSize_t     Safe::encrypt       (char * buffer, char * iv, const char * data, size_t size)
    {    
        _geniv(_iv);
        memcpy(iv, _iv, N_BLOCK);
        
//         alert_io("encrypt with IV:");
//         utils::dump_hex(iv, N_BLOCK);
//         alert_io("KEY:");
//         utils::dump_hex((char *)_masterkey, LEN_AES_MASTERKEY);

//         memcpy(buffer, data, size);
//         return size;

        int r = _aes.encrypt( (unsigned char*) data, size, buffer, _masterkey, LEN_AES_MASTERKEY, _iv);
        if (r<=0) { error_aes("WARNING: encryption failed!"); }
        return r;    
    }

    iSize_t     Safe::decrypt       (char * buffer,const char * iv, char * data, size_t size)
    {
//         alert_io("decrypt with IV:");
//         utils::dump_hex(iv, N_BLOCK);
//         alert_io("KEY:");
//         utils::dump_hex((char *)_masterkey, LEN_AES_MASTERKEY);

//         memcpy(buffer, data, size);
//         return size;
        
        int r = _aes.decrypt( (unsigned char*) data, size, buffer, _masterkey, LEN_AES_MASTERKEY, (unsigned char *) iv );
        if (r<=0) { error_aes("WARNING: encryption failed!"); }
        return r;    
    }
    
    int         Safe::cipherlen     (iSize_t size)
    {
//         return size;
        return _aes.get_cipher64_length(size);
    }
}

//
// namespace packet
// {
//     buffPacket::buffPacket(net::Safe& s, netpacket * p):_aes(s)
//     {
//         _item = p;
//     }
// 
//     void        buffPacket::netbuild    (void)
//     {
//         _item->header(_header);
//         
//         iSize_t size=_item->full_size();
//         _size=_aes.cipherlen(size);
//         
//         char *  b_clear  = (char *)calloc(size, sizeof(char));
//         char *  b_cipher = (char *)calloc(_size, sizeof(char));
//         char    b_iv[N_BLOCK];
//        
//         _item->forge(b_clear);
//         
//         _size = iSize_t(_aes.encrypt(b_cipher, b_iv, b_clear, size));
//         _body = std::vector<uint8_t>(b_cipher, b_cipher + _size);
//         
//         memcpy(_netheader, PREAMBLE, SIZEOF_PREAMBLE);
//         memcpy(_netheader + SIZEOF_PREAMBLE, &_size, sizeof(iSize_t));
//         memcpy(_netheader + SIZEOF_PREAMBLE + sizeof(iSize_t), b_iv, N_BLOCK);
//         
//         free (b_clear);
//         free (b_cipher);        
//     }
//     
// } /* namespace : packet */
//


#endif /* INDOORINO_NETWORK */
