
#ifndef _NETMODULE_SOURCE_H_
#define _NETMODULE_SOURCE_H_

#include "../indoorino/indoorino.h"
#include "netutils.h"

#if defined(INDOORINO_PYLIB) || defined(RAWRUN)

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAX_PACKET_PER_BOARD 20000

//    ________________________________________________________________________
//    |                                                                      |
//    |    Virtual Board Template                                            |
//    |______________________________________________________________________|
//

class BoardImage
{    
protected:
    char                _name[LEN_NAME]  {0};
    ipacket         **  _packetlist      =nullptr;
    uint32_t            _packetnum       =0;
    
public:

             BoardImage                 (const char *);
    virtual ~BoardImage                 ();
    
    virtual void    *   address         ()=0;
    virtual bool        is_serial       ()=0;

    void                add_packet      (ipacket *);
    void                list_packet     (void);
    
    
    char            *   name            (void) { return _name; };
    uint32_t            packetnum       (void) { return _packetnum; };
    ipacket         *   packet          (uint32_t i) { return _packetlist[i]; };
};


//    ________________________________________________________________________
//    |                                                                      |
//    |    Virtual Board UDP                                                 |
//    |______________________________________________________________________|
//

class BoardImageUdp : public BoardImage
{
protected:
    sockaddr_in         _address;

public:

    BoardImageUdp                   (const char *, sockaddr_in *);
    bool                is_serial   (void)  { return false; }    
    void            *   address     (void)  { return &_address; };
};

//    ________________________________________________________________________
//    |                                                                      |
//    |    Virtual Board Serial                                              |
//    |______________________________________________________________________|
//

class BoardImageSerial : public BoardImage
{
protected:
    char                _address[LEN_IPADDRESS];

public:

    BoardImageSerial                (const char *, const char *);
    bool                is_serial   (void)  { return true; }    
    void            *   address     (void)  { return _address; };
};


//    ________________________________________________________________________
//    |                                                                      |
//    |    Connection Manager                                                |
//    |______________________________________________________________________|
//

class   ConnectionManager
{
protected:
    packetParse         _parser;
    BoardImageUdp   **  _boardlist=nullptr;
    uint32_t            _boardnum=0;
    char            *   _rxbuffer=nullptr;

    void                add_board   (const char *, sockaddr_in *);
    bool                rem_board   (const char *);

public:
    
             ConnectionManager          ();
    virtual ~ConnectionManager          ();
    
    virtual void            begin       (void) {};
    virtual void            loop        (void) {};
    virtual bool            read        (void) { return false; };
};

class   ConnectionUdp : public ConnectionManager
{
protected:
    int                 _socket;
    sockaddr_in         _addr_server;
    sockaddr_in         _addr_client;
    timeval             _read_timeout;
    
public:
    
    ConnectionUdp       ();
    ~ConnectionUdp      ();
    
    void                begin       (void);
    void                loop        (void) {};
    bool                read        (void);
    
};

#endif /* ! ARDUINO */


#endif /* _NETMODULE_SOURCE_H_ */

