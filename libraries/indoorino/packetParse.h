/*
 * packetParse.h
 *
 *  Created on: Oct 26, 2020
 *      Author: n00b
 */

#ifndef PACKETPARSE_H_
#define PACKETPARSE_H_

#include "packeTemplate.h"

class packetParse
{
private:
    
    bool    _strict=true;
    
    bool    _flag_preamble=false;
    bool    _flag_name=false;
    bool    _flag_comm=false;
    
    char *  _buff_preamble=nullptr;
    char *  _buff_comm=nullptr;
    
        
protected:

    ipacket *       _packet=nullptr;
    char *          _buff_name=nullptr;
    void            parse_preamble      (uint8_t);
    void            parse_name          (uint8_t);
    void            parse_commbyte      (uint8_t);
    void            parse_plbyte        (uint8_t);
//     void            parse_packet        (ipacket *);
    const           __FSH           *   _id;

    uint32_t        _total_rx=0;
    bool            _ready=false;

public:
             packetParse        (bool strict=true);
    virtual ~packetParse        ();
    
    
    virtual void        begin       (void);
    virtual void        loop        (void);

    void                refresh     (void);
    void                append      (uint8_t);
    ipacket     *       packet      (void);
    char        *       buffername  (void)      { return _buff_name; };
    uint32_t            total_rx    (void)      { return _total_rx;  };

};


#endif /* PACKETPARSE_H_ */
