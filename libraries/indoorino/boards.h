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
#include "layout.h"
#include "devices.h"

#include <deque>

namespace indoorino
{
    
    class   BoardTemplate;
    
    //      _________________________________________
    //      |                                       |
    //      |       Board List                      |
    //      |_______________________________________|
    //
    //

    class Boards
    {
    
    private:

    protected:        
        std::vector<BoardTemplate *>  _boards;
        void clean_nullp (void);
    public:

        Boards() {};
        ~Boards() { _boards.clear(); }
        
        BoardTemplate   *   operator[]  (const iSize_t i) { return _boards.at(i); };
        BoardTemplate   *   operator()  (const char *);
        auto&               operator()  (void) { return _boards; };
        
        int             exist       (const char *);
        bool            remove      (const char *);
        bool            add         (packet::ipacket *);
        iSize_t         size        () { return _boards.size(); }
        void            clear       (void);    
        void            update      (void);    
        
        void            begin       (void) { this->update(); }
        bool            save        (void);
        bool            load        (void);
        void            show        (void);
        
        friend class IndoorinoSystem;
    };
    
    
    //      _________________________________________
    //      |                                       |
    //      |       Board Template                  |
    //      |_______________________________________|
    //
    
    class   BoardTemplate
    {
    private:
        bool                            _enabled=true;
        bool                            _is_online=false;
        bool                            _has_services=false;

    protected:
        
        indoorino::Devices              _devices;
        std::chrono::seconds            _lag;
        void sysupdate (void);

        std::vector<packet::netpacket>  _rxqueue; // TODO
        std::vector<packet::netpacket>  _txqueue; // TODO
        
        packet::ipacket                 _config[2];
        packet::ipacket                 _status[2];

    public:
        
        BoardTemplate(const char *);
        virtual ~BoardTemplate();

    public:
        void            loop            (void);
        void            parse           (packet::netpacket *);
        void            send_config     (void);
        void            send_status     (void);
        
    public:
        const char  *   name            () { return _config[0].p_name(); }
        const char  *   type            () { return _config[0].p_type(); }
        const char  *   board           () { return _config[0].p_board(); }
        
        const iSize_t   devnum          (void);

        auto&           devices         () { return _devices;}

        const packet::ipacket * config  () { return _config; }
        const packet::ipacket * status  () { return _status; }
        
    public:
        void            show            (void);
        void            set_offline     () { _is_online = false; }
        
        std::vector<packet::netpacket>& rxpackets() { return _rxqueue; } 
        std::vector<packet::netpacket>& txpackets() { return _txqueue; } 
        std::chrono::system_clock::time_point& boardtime (void);
        
        friend class Boards;
    };

} /* namespace:indoorino */

#endif /* INDOORINO_SERVER */

#endif /* _INDOORINO_BOARDS_H_ */






