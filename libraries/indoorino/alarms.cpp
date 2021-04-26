/*
 * alarms.cpp
 *
 *  Created 3.1.0 on: Apr 21, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#include "alarms.h"
#include "../network/server.h"

#if defined (INDOORINO_NETWORK)

namespace indoorino
{
    AlarmDevice::AlarmDevice(layout::dev_alarm_t& p):_layout(p)
    {}
    
    void        AlarmDevice::parse          (packet::netpacket * p)
    {
        if ( (p->command() >= IBACOM_HEAT_ALARM) && (p->command() <= IBACOM_GENERIC_ALARM) )
        {
            if ( (strcmp(p->p_board(), _layout.boardname) == 0) && (strcmp(p->p_devname(), _layout.devname) == 0) )
            {
                _signals.push_back(packet::netpacket(p));
                
                if (_enabled)
                {
                    /* Here process alarm packets */                    
                }
                _on_alarm=true;
                this->send_updates();
            }
        }
        else if ( p->command() == IBACOM_ACK_ENV_ALARM )
        {
            if ( (strcmp(p->p_board(), _layout.boardname) == 0) && (strcmp(p->p_devname(), _layout.devname) == 0) )
            {
                _on_alarm=false;
                this->send_updates();
            }
        }
        else if ( p->command() == IBACOM_SET_ENV_ALARM )
        {
            if ( (strcmp(p->p_board(), _layout.boardname) == 0) && (strcmp(p->p_devname(), _layout.devname) == 0) )
            {
                _enabled=bool(*p->p_value1());
                this->send_updates();
            }
        }
    }

    void        AlarmDevice::send_updates   (void)
    {
        packet::ipacket p(IBACOM_ENV_ALARM);
        uint32_t epoch=utils::epoch_now();
        strcpy(p.p_devname(), _layout.devname);
        strcpy(p.p_board(), _layout.boardname);
        memcpy(p.p_status(), &_enabled, sizeof(uint8_t));
        memcpy(p.p_epoch(), &epoch, sizeof(uint32_t));
        
        if (_on_alarm)  *p.p_value1() = 1;
        else            *p.p_value1() = 0;

        Server.shell.broadcast(&p);
    }
    
    void        AlarmDevice::acknowledge    (void)
    {
        _on_alarm=true;
        this->send_updates();    
    }

    

    void        AlarmList::begin            (void)
    {
        this->clear();
        
        for (uint i=0; i<LYT_DEFAULT_NUM_ALARMS; i++)
        {
            _classlist.push_back(AlarmDevice(layout::AlarmsDefaultLayout[i]));
        }
    }

    void        AlarmList::parse            (packet::netpacket * p)
    {
        for (auto& a: _classlist)
        {
            a.parse(p);
        }
    }

    
//     1) 2 campi --> rifare il template
//     2) 1 campo --> scrivere un parser
//     3) dare un nome agli allarmi

//     BoardTemplate&  BoardList::operator()       (const char * name)
//     {       
//         for (auto& b: _blist)
//         {
//             if (strcmp(b.name(), name) == 0)
//             {
//                 return b;
//             }
//         }
//         error_os("FATAL:boardlist:invalid call name <%s>!", name);
//         return invalid_board;
//     }
// 
//     void            BoardList::add_board   (packet::netpacket * p)
//     {
//         _blist.push_back(BoardTemplate(p->p_name()));
//         _blist.back().parse(p);
//     }
//     
//     bool            BoardList::rem_board   (const char *)
//     {
//         return false;
//     }
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */
