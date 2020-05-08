/*
 * packetParse.cpp
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */

#include "packetParse.h"
#include "packetUtils.h"

ibasize_t count=0;

packetParse::packetParse(bool strict)
{
    _strict=strict;
    _id=F("IOSYS");
}

packetParse::~packetParse()
{
    free(_buff_preamble);
    free(_buff_name);
    free(_buff_comm);
    if (_packet) delete _packet;
}

void            packetParse::begin              (void)
{
    _packet = new ipacket(0);
    debug_io("\nparser:begin");
    _buff_preamble=(char*)calloc(SIZEOF_PREAMBLE,sizeof(char));
    _buff_name=(char*)calloc(LEN_NAME,sizeof(char));
    _buff_comm=(char*)calloc(1,sizeof(ibacomm_t));
}

void            packetParse::loop               (void)
{
#if defined(ARDUINO)
    // da mettere in BoardIO
    static uint32_t m=0;
    uint32_t t=micros();
    _looptime=(t - m);
    m=t;
#endif
}

void            packetParse::refresh            (void)
{
    _flag_preamble=false;
    _flag_name=false;
    _flag_comm=false;
    _ready=false;
}

ipacket     *   packetParse::packet             (void)
{
    if (_ready)
    {
        _ready=false;
        return _packet;
    }
    else
    {
        return nullptr;
    }
    
}

void            packetParse::append             (uint8_t incoming)
{
//     debug_io("\nincoming: %u - preamble: %u", incoming, _flag_preamble);

    if (!_flag_preamble)    { parse_preamble(incoming); }
    
    else if (!_flag_name)   { parse_name(incoming);     }
    
    else if (!_flag_comm)   { parse_commbyte(incoming); }
    
    else                    { parse_plbyte(incoming);   }    
}

void            packetParse::parse_preamble     (uint8_t incoming)
{
    
//     debug_io("\npreamble: %u", incoming);
    debug_io("%c",incoming);
    memmove(_buff_preamble, _buff_preamble + 1, SIZEOF_PREAMBLE - 1);
    _buff_preamble[SIZEOF_PREAMBLE - 1] = incoming;
    
    if (memcmp_P(_buff_preamble, PREAMBLE, SIZEOF_PREAMBLE) == 0)
    {
        debug_io("\nparse:preamble PASS");
        memset(_buff_name, 0, LEN_NAME);
        _flag_preamble=true;
    }
//     else    { debug_io(" (skip)"); }
}

void            packetParse::parse_name         (uint8_t incoming)
{
    static uint8_t count=0;
    const char * boardname { P2C(BOARD_NAME) };

    debug_io("\nparse:name: %u [%u]",count, incoming);

    if (count < LEN_NAME)
    {
        _buff_name[count] = incoming;
        count ++;
        return;
    }
    else
    {
        if (incoming == 1)
        {
            if (!_strict || 
            (   strncmp(_buff_name, boardname, strlen(boardname)) == 0 &&
                is_string_devname(_buff_name)) )
            {
                count=0;
                _flag_name=true;
                memset(_buff_comm, 0, SIZEOF_COMMAND);
                debug_io("\nparse:name: [%s:%s] PASS", boardname, _buff_name);
                return;
            }
            else    { error_io("WARNING:parse:name: [%s:%s] comparison FAILED!",boardname, _buff_name); }
        }
        else        { error_io("\nWARNING:parse:name: could not find init byte"); }
    }
    
    _flag_preamble=false;
    count=0;
}

void            packetParse::parse_commbyte    (uint8_t incoming)
{
    static bool _flag=false;
    
    if (!_flag)
    {
        debug_io("\nparse:command: [%u.", incoming);
        _buff_comm[0]=incoming;
        _flag=true;
    }
    else
    {
        ibacomm_t c=0;
        _buff_comm[1] = incoming;
        memcpy(&c, _buff_comm, SIZEOF_COMMAND);
        
        debug_io("%u] => %u", incoming, c);
        
        // Here we have name and command 
        // and we can call the proper class
        
        _packet = reallocPacket(_packet, c);
        
        _flag=false;
        _ready=false;
        if (_packet->data_size() == 0)
        {
            _flag_name=false;
            _flag_preamble=false;
            _ready=true;
            _total_rx++;
//             parse_packet(_packet);
        }
        else
        {
            _flag_comm=true;
        }
    }
}

void            packetParse::parse_plbyte      (uint8_t incoming)
{

    if (!_packet->append(incoming))
    {
        debug_io("\nparse:payload: downloaded <%s> [%u Bytes]", F2C(_packet->label()), _packet->data_size());
        _flag_comm=false;
        _flag_name=false;
        _flag_preamble=false;
        _ready=true;
        _total_rx++;
       count=0;

//         parse_packet(_packet);
    }
    else
    {
        count++;
        debug_io("\nparse:payload: got %u (%u/%u) for %s",
                incoming, count, _packet->data_size(), F2C(_packet->label()));
    }
    
}

// void            packetParse::parse_packet      (ipacket * ptr)
// {
// #if defined (ARDUINO)
//     
//     char * boardname { P2C(BOARD_NAME) };
//     
//     debug_io("\npayload: EVAL packet [%s %s]",_buff_name, boardname);
//     
//     #if defined (INDOORINO_ESPSERVER)        
//     if (strcmp(_buff_name, boardname) == 0)
//     {
//         if (ptr->check())   { ptr->eval(); }
//     }
//     #elif defined (INDOORINO_ECHOER)
//     if (strcmp(_buff_name, boardname) == 0)
//     {
//         if (ptr->check())   { ptr->eval(); }
//     }
//     else
//     {
//         if (ptr->check())   { boardio.send(ptr); }
//     }
//     #elif defined (INDOORINO_SAMPLER)
//     if (ptr->check())       { ptr->eval(); }
//     #endif
// 
// #elif defined (INDOORINO_PYLIB)
//     printf("\nadded new %s to pylist\n", F2C(ptr->label()));
//     ptr->eval();
//     PyList_Append(_pylist, ptr->pydict(_buff_name));
// #endif
// 
// }



