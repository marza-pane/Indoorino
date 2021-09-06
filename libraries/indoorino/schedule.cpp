/*
 * schedule.cpp
 *
 *  Created on: Nov 1, 2020
 *      Author: n00b
 */

#if defined (__linux__)

#include "../common/indoorino.h"
namespace indoorino
{
    
    Schedule::~Schedule()
    {
        while (!_txlist.empty())
        {
            delete _txlist.back();
            _txlist.pop_back();
        }
        _txlist.clear();
    }
    
    bool            Schedule::pushInTime        (packet::netpacket * p, time_t date, bool _f_broadcast, bool _f_shell)
    {
        time_t now=utils::millis();
        if (now > date)
        {
            warning_os("schedule:add: too late [%ld:%ld]", now, date);
            return false;
        }
        
        packet::schedPacket * s = new packet::schedPacket(p);
        
        s->sendtime = date;
        if (_f_broadcast)   { s->broadcast = true; }
        if (_f_shell)       { s->shell = true; }
               
        debug_io("scheduled(date) %s [%d:%d] @ %s",
                 s->description(),
                 s->broadcast,
                 s->shell,
                 utils::timestring_epoch32(date));
        
        _txlist.push_back(s);
        return true;
    }
    
    bool            Schedule::pushInSeconds     (packet::netpacket * p, uint32_t seconds, bool _f_broadcast, bool _f_shell)
    {
        time_t date=utils::millis() + seconds * 1000;

        packet::schedPacket * s = new packet::schedPacket(p);
        
        s->sendtime = date;
        if (_f_broadcast)   { s->broadcast = true; }
        if (_f_shell)       { s->shell = true; }
        
        debug_io("scheduled(date) %s [%d:%d] @ %s",
                 s->description(),
                 s->broadcast,
                 s->shell,
                 utils::timestring_epoch32(date));

        _txlist.push_back(s);
        return true;
    }
    
    void            Schedule::loop              (void)
    {
        time_t now=utils::millis();
        
        if (_txlist.empty()) { return; }
        
        iSize_t c=0;
        
        for(const auto& p: _txlist)
        {
            if (now >= p->sendtime)
            {
                if (p->broadcast)
                {
                    info_os("schedule:broadcasting <%s> ", p->label());
                    if (p->shell)
                    {
                        Server.shell.broadcast((packet::netpacket *) p);                    
                    }
                    else
                    {
                        Server.board.broadcast((packet::netpacket *) p);                
                    }                
                }
                else
                {
                    info_os("schedule:sending <%s> ", p->label());
                    if (Server.board.has_client(p->target))
                    {
                        Server.board.send((packet::netpacket *) p);
                    }
                    else
                    {
                        Server.shell.send((packet::netpacket *) p);                
                    }                
                }
                _txlist.erase(_txlist.begin() + c);
                loop();
                return;
            }
            else { c++; };
        }
                
    //     if (list[i]->status == 1)
    //     {
    //         uint8_t hashes[SHA_DIGEST_LENGTH]{0};
    //         list[i]->packet->checksum(hashes);
    //         if Server.board.match(hashes))
    //         {
    //             list.rem(i);
    //             loop();
    //             return;
    //         }
    //     }

//         while (_txlist.count() > 5000)
//         {
//             delete _txlist.pop();
//         }
    }

} /* namespace : indoorino */

#endif /* INDOORINO_NETWORK */
