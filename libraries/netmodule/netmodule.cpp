

#if defined(INDOORINO_PYLIB) || defined(RAWRUN)

#include "netmodule.h"

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
    for (uint32_t i=0; i<_packetnum - 1; i++)
    {
        delete (_packetlist[i]);        
    }
    
    free (_packetlist);
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
//    |    Connection Manager                                                |
//    |______________________________________________________________________|
//

ConnectionManager::ConnectionManager    ()
{
    _rxbuffer=(char *)calloc(MAX_PAYLOAD_SIZE, sizeof(char));    
    _parser.begin();
};

ConnectionManager::~ConnectionManager   ()
{
    free(_rxbuffer);
};

void            ConnectionManager::add_board   (const char * name, sockaddr_in * address)
{}
bool            ConnectionManager::rem_board   (const char * name)
{}


//    ________________________________________________________________________
//    |                                                                      |
//    |    Connection Manager                                                |
//    |______________________________________________________________________|
//

ConnectionUdp::ConnectionUdp    ()
{
    _read_timeout.tv_sec = 0;
    _read_timeout.tv_usec = 100;
};

ConnectionUdp::~ConnectionUdp   ()
{};




void            ConnectionUdp::begin        (void)
{
    if ( (_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    {
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    memset(&_addr_server, 0, sizeof(_addr_server)); 
    memset(&_addr_client, 0, sizeof(_addr_client)); 
    
    // Filling server information 
    _addr_server.sin_family = AF_INET; // IPv4 
    _addr_server.sin_addr.s_addr = INADDR_ANY; 
    _addr_server.sin_port = htons(DEFAULT_TARGET_PORT); 
    
    // Bind the socket with the server address 
    if ( bind(_socket, (const struct sockaddr *)&_addr_server, sizeof(_addr_server)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,&_read_timeout,sizeof(_read_timeout)) < 0)
    {
        printf("\nError setting timeout");
        perror("Error");
    }
};



bool            ConnectionUdp::read         (void)
{
    
    ipacket *   ptr=nullptr;
    
    const char *hello = "Hello from server"; 

    int n=0; 
    unsigned int len = sizeof(_addr_client); //len is value/resuslt 

    n = recvfrom(_socket, (char *)_rxbuffer, MAX_PAYLOAD_SIZE, 
        MSG_WAITALL, ( struct sockaddr *) &_addr_client, 
        &len);

    if (n<=0)   return false;
    
    for (ibasize_t i=0; i<n; i++)
    {
        _parser.append(_rxbuffer[i]);
        ptr = _parser.packet();
        if (ptr != nullptr)
        {
            
            /* HERE DO SOMETHING WITH THE PACKET */
            
            printf("\n\n\t---> GOT PACKET FROM:%s:%u",
               inet_ntoa(_addr_client.sin_addr),
               ntohs(_addr_server.sin_port));
            
            uint32_t k = ptr->checksum();
            ipacket * chp = new ipacket(IBACOM_ACK);
            ibasize_t s = chp->full_size();
            char chbuff[s];

            memcpy(chp->p_value(), &k, sizeof(uint32_t));
            chp->forge(chbuff, _parser.buffername());

            sendto(_socket, (const char *)chbuff, s, 
            MSG_CONFIRM, (const struct sockaddr *) &_addr_client, len); 
            printf("\nCHECKSUM sent.\n");
            
        }
    }
    
    return true;
}

#endif /* ! ARDUINO */
