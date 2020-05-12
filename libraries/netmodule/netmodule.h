/*
 * packetUtils.h
 *
 *  Created on: Apr 28, 2020
 *      Author: n00b
 */

#ifndef _NETMODULE_SOURCE_H_
#define _NETMODULE_SOURCE_H_

#if defined(INDOORINO_PYLIB) || defined(RAWRUN)

#include "../indoorino/indoorino.h"
#include "netutils.h"

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

void            printpacket(ipacket * ptr);

#define MAX_PACKET_PER_BOARD 5

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
//    |    Boards List                                                       |
//    |______________________________________________________________________|
//

class   BoardManager
{
    BoardImage      **  _boardlist=nullptr;
    uint32_t            _boardnum=0;
    bool                add_board_p (void);

public:
    BoardManager        () {};
    
    void                add_board   (const char *, sockaddr_in *);
    void                add_board   (const char *, const char *);
    bool                rem_board   (const char *);

    BoardImage      *   board       (const char *); 
    BoardImage      *   board       (uint32_t i)    { return _boardlist[i]; }; 
    uint32_t            boardnum    (void)          { return _boardnum; };   
    
};

//    ________________________________________________________________________
//    |                                                                      |
//    |    Connections                                                       |
//    |______________________________________________________________________|
//

extern BoardManager boardlist;

class   ConnectionTemplate
{
protected:
    packetParse         _parser;
    char            *   _rxbuffer=nullptr;


public:
    
             ConnectionTemplate          ();
    virtual ~ConnectionTemplate          ();
    
    virtual void            begin       (void) {};
    virtual void            loop        (void) {};
    virtual bool            read        (void) { return false; };
};

// class   ConnectionUdp : public ConnectionTemplate
// {
// protected:
//     int                 _socket;
//     sockaddr_in         _addr_server;
//     sockaddr_in         _addr_client;
//     timeval             _read_timeout;
//     
// public:
//     
//     ConnectionUdp       ();
//     ~ConnectionUdp      ();
//     
//     void                begin       (void);
//     void                loop        (void) {};
//     bool                read        (void);
//     
// };

#endif /*  PYMODULE or RAWRUN  */

#endif /* _NETMODULE_SOURCE_H_ */

