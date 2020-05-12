/*
 * packetUtils.h
 *
 *  Created on: Apr 28, 2020
 *      Author: n00b
 */

#if defined(INDOORINO_PYLIB) || defined(RAWRUN)

#include "netmodule.h"

void            printpacket(ipacket * ptr)
{
    printf("\nPacket <%u:%s> [size:total:checksum] [%u:%u:%u]",
           ptr->command(),
           ptr->label(),
           ptr->data_size(),
           ptr->full_size(),
           ptr->checksum());
    if (ptr->command() == IBACOM_REPORT)
    {
        printf("\n\tName : <%s>\n\tMessage : <%s>\n\tEpoch : <%u>, Level : <%u>",
               ptr->p_name(),
               ptr->p_message(),
               *ptr->p_epoch(),
               *ptr->p_level());
    }
}

//    ________________________________________________________________________
//    |                                                                      |
//    |    Virtual Board Template                                            |
//    |______________________________________________________________________|
//

BoardImage::BoardImage          (const char * name)
{
    if(is_string_devname(name)) strcpy(_name, name);
    else                        strcpy(_name, "INVALID");
}

BoardImage::~BoardImage         ()
{
    for (uint32_t i=0; i<_packetnum; i++)
    {
        delete (_packetlist[i]);        
    }
    
    if (_packetnum != 0)    free (_packetlist);
}

void            BoardImage::add_packet          (ipacket * ptr)
{
    if (_packetnum == 0 || _packetlist == nullptr)
    {
        debug_mem("\nInit packet list for board %s", _name);
        _packetlist = (ipacket**)calloc(1,sizeof(ipacket *));
        
        _packetlist[_packetnum] = new ipacket(ptr->command());
        _packetlist[_packetnum]->fill(ptr->payload(), ptr->data_size());
        _packetnum++;
    }
    
    else
    {
        if (_packetnum <= MAX_PACKET_PER_BOARD)
        {
            
            ipacket ** trp = (ipacket **)realloc(_packetlist, (_packetnum + 1) * sizeof(ipacket *));
            
            if (trp)
            {
                _packetlist = trp;
                _packetnum++;
                debug_mem("\nReallocating packet list for board %s to %u", _name, _packetnum);
            }
            else
            {
                error_mem("FATAL: can not reallocate %u packet list", _packetnum + 1);
                return;
            }
        }
        else
        {
            delete (_packetlist[_packetnum - 1]);
        }
        
        debug_mem("\nPacket list for board %s - sorting", _name);
        for (uint32_t i=_packetnum - 1; i>0; i--)
        {
            _packetlist[i] = _packetlist[i - 1];
        }
        _packetlist[0] = new ipacket(ptr->command());
        _packetlist[0]->fill(ptr->payload(), ptr->data_size());

    }
}

//    ________________________________________________________________________
//    |                                                                      |
//    |    Virtual Board UDP                                                 |
//    |______________________________________________________________________|
//

BoardImageUdp::BoardImageUdp    (const char * name, sockaddr_in * addr): BoardImage(name)
{
    memcpy(&_address, addr, sizeof(sockaddr_in));
}

//    ________________________________________________________________________
//    |                                                                      |
//    |    Virtual Board Serial                                              |
//    |______________________________________________________________________|
//

BoardImageSerial::BoardImageSerial (const char * name, const char * addr): BoardImage(name)
{
    strcpy(_address, addr);
}


//    ________________________________________________________________________
//    |                                                                      |
//    |    Board list manager                                                |
//    |______________________________________________________________________|
//

BoardImage  *   BoardManager::board         (const char *name)
{
    for (uint32_t i=0; i<_boardnum; i++)
    {
        if (strcmp(_boardlist[i]->name(), name) == 0)
        {
            return _boardlist[i];
        }
    }
    return nullptr;
}

bool            BoardManager::add_board_p   (void)
{
    if (_boardnum == 0)
    {
        debug("\nboardManager:add_board: new list");
        _boardlist = (BoardImage **)calloc(1, sizeof(BoardImage *));
    }
    else
    {
        debug("\nboardManager:add_board: reallocating to %u", _boardnum + 1);
        BoardImage ** ptr = 
            (BoardImage **)realloc(_boardlist, (_boardnum + 1) * sizeof(BoardImage *));
        if (ptr)
        {
            _boardlist = ptr;
        }
        else
        {
            error_mem("FATAL: can not reallocate board list");
            return false;
        }
    }
    return true;
}

void            BoardManager::add_board     (const char * name, sockaddr_in * address)
{
    if (add_board_p())
    {
        _boardlist[_boardnum] = new BoardImageUdp(name, address);
        _boardnum++;
    }
}

void            BoardManager::add_board   (const char * name, const char * address)
{
    debug_dev("\nconf:address: Adding UDP board %s", name);
    if (add_board_p())
    {
        _boardlist[_boardnum] = new BoardImageSerial(name, address);
        _boardnum++;
    }
}

bool            BoardManager::rem_board   (const char * name)
{
    for (uint32_t i=0; i<_boardnum; i++)
    {
        if (strcmp(_boardlist[i]->name(), name) == 0)
        {
            debug_dev("\nconf:address: Removing %s",name);
            fflush(stdout);
            delete _boardlist[i];
            for (uint32_t j=i; j<(_boardnum - 1); j++)
            {
                debug_dev("\nconf:address: realloc [%u]%s to [%u]",
                          i + 1, _boardlist[i+1]->name(), i);
                fflush(stdout);
                _boardlist[i] = _boardlist[i+1];
            }
            debug_dev("\nconf:address: Removed %s",name);
            fflush(stdout);

            BoardImage ** ptr = 
                (BoardImage **)realloc(_boardlist, (_boardnum - 1) * sizeof(BoardImage *));
            _boardlist=ptr;
            _boardnum--;
            return true;
        }
    }
    
    debug_dev("\nconf:address:remove: could not find %s", name);
    return false;
}


//    ________________________________________________________________________
//    |                                                                      |
//    |    Connection Template                                               |
//    |______________________________________________________________________|
//

ConnectionTemplate::ConnectionTemplate    ()
{
    _rxbuffer=(char *)calloc(MAX_PAYLOAD_SIZE, sizeof(char));    
    _parser.begin();
};

ConnectionTemplate::~ConnectionTemplate   ()
{
    free(_rxbuffer);
};

//    ________________________________________________________________________
//    |                                                                      |
//    |    Connection Udp                                                    |
//    |______________________________________________________________________|
//

// ConnectionUdp::ConnectionUdp    ()
// {
//     _read_timeout.tv_sec = 0;
//     _read_timeout.tv_usec = 100;
// };
// 
// ConnectionUdp::~ConnectionUdp   ()
// {};
// 
// void            ConnectionUdp::begin        (void)
// {
//     if ( (_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
//     {
//         perror("socket creation failed"); 
//         exit(EXIT_FAILURE); 
//     } 
//     
//     memset(&_addr_server, 0, sizeof(_addr_server)); 
//     memset(&_addr_client, 0, sizeof(_addr_client)); 
//     
//     // Filling server information 
//     _addr_server.sin_family = AF_INET; // IPv4 
//     _addr_server.sin_addr.s_addr = INADDR_ANY; 
//     _addr_server.sin_port = htons(DEFAULT_TARGET_PORT); 
//     
//     // Bind the socket with the server address 
//     if ( bind(_socket, (const struct sockaddr *)&_addr_server, sizeof(_addr_server)) < 0 ) 
//     { 
//         perror("bind failed"); 
//         exit(EXIT_FAILURE); 
//     } 
//     
//     if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,&_read_timeout,sizeof(_read_timeout)) < 0)
//     {
//         printf("\nError setting timeout");
//         perror("Error");
//     }
// };
// 
// bool            ConnectionUdp::read         (void)
// {
//     
//     ipacket *   ptr=nullptr;
//     
//     int n=0; 
//     unsigned int len = sizeof(_addr_client); //len is value/resuslt 
// 
//     n = recvfrom(_socket, (char *)_rxbuffer, MAX_PAYLOAD_SIZE, 
//         MSG_WAITALL, ( struct sockaddr *) &_addr_client, 
//         &len);
// 
//     if (n<=0)   return false;
//     
//     for (ibasize_t i=0; i<n; i++)
//     {
//         _parser.append(_rxbuffer[i]);
//         ptr = _parser.packet();
//         if (ptr != nullptr)
//         {
//             printf("\n\n\t---> GOT PACKET FROM:%s:%s:%u",
//                    _parser.buffername(),
//                    inet_ntoa(_addr_client.sin_addr),
//                    ntohs(_addr_server.sin_port));
//             printpacket(ptr);
// 
//             
//             if (boardlist.board(_parser.buffername()) == nullptr)
//             {
//                 boardlist.add_board(_parser.buffername(), &_addr_client);
//             }
//             boardlist.board(_parser.buffername())->add_packet(ptr);
// 
//             /* HERE DO SOMETHING WITH THE PACKET */
//             
//             uint32_t k = ptr->checksum();
//             ipacket * chp = new ipacket(IBACOM_ACK);
//             ibasize_t s = chp->full_size();
//             char chbuff[s];
// 
//             memcpy(chp->p_value(), &k, sizeof(uint32_t));
//             chp->forge(chbuff, _parser.buffername());
// 
//             sendto(_socket, (const char *)chbuff, s, 
//             MSG_CONFIRM, (const struct sockaddr *) &_addr_client, len); 
//             printf("\nCHECKSUM sent.\n");
//             
//         }
//     }
//     
//     return true;
// }

#endif /* PYMODULE or RAWRUN */





