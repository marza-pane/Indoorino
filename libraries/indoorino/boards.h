/*
 * board.h
 *
 *  Created on: Jan 23, 2021
 *      Author: n00b
 */

#pragma once

#ifndef _INDOORINO_BOARDS_H_
#define _INDOORINO_BOARDS_H_

#if defined (INDOORINO_SERVER)
#include "../packets/ipacket.h"
#include "devices.h"

#include <deque>
// #include <mutex>
// #include <condition_variable>

//      _________________________________________
//      |                                       |
//      |       Board Template                  |
//      |_______________________________________|

namespace indoorino
{
    class   BoardTemplate
    {
    private:
        bool                            _is_online=false;
        bool                            _is_valid=false;

    protected:
        std::string                     _address;
        std::chrono::seconds            _lag{0};
        
        DeviceList                      _devices;
        std::vector<packet::netpacket>  _rxqueue;
        std::vector<packet::netpacket>  _txqueue;
        
        packet::ipacket                 _config[2] { packet::ipacket(IBACOM_CONF_STD), packet::ipacket(IBACOM_STATUS_STD)   };
        packet::ipacket                 _status[2] { packet::ipacket(IBACOM_STATUS_STD), packet::ipacket(IBACOM_STATUS_STD) };

    public:
        BoardTemplate(packet::ipacket *);
        virtual ~BoardTemplate();

    public:
        void            loop            (void);
        void            parse           (packet::netpacket *);

    public:
        const char  *   name            () { return _config[0].p_name(); }
        const char  *   type            () { return _config[0].p_type(); }
        const char  *   board           () { return _config[0].p_board(); }
        
        const iSize_t   devnum          (void);

        const packet::ipacket * config  () { return _config; }
        const packet::ipacket * status  () { return _status; }
        
        DeviceList&             devices () { return _devices;}
        
        void            send_config     (void);
        void            send_status     (void);
        
        std::vector<packet::netpacket>& rxpackets() { return _rxqueue; } 
        std::chrono::system_clock::time_point& boardtime (void);
        
        friend class BoardList;
    };
    
//      _________________________________________
//      |                                       |
//      |       Board List                      |
//      |_______________________________________|

    class   BoardList
    {
    
    private:
        int         get_index       (const char *);

    protected:        
        std::vector<BoardTemplate>  _blist;
    
    public:
        BoardList() {}
        ~BoardList() { _blist.clear(); }
        
        BoardTemplate&      operator[]  (const iSize_t i) { return _blist.at(i); };
        BoardTemplate&      operator()  (const char *);
        
        iSize_t             size        () { return _blist.size(); }
        void                clear       () { _blist.clear(); }    
        
        void                add_board   (packet::netpacket *);
        bool                rem_board   (const char *);
        bool                exist       (const char *);
        void                show        (void);
        
        bool                save        (void);
        bool                load        (void);
        
        friend class IndoorinoSystem;
    };
    
} /* namespace:indoorino */

#endif /* INDOORINO_SERVER */

#endif /* _INDOORINO_BOARDS_H_ */






