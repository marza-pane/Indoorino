/*
 * alarms.h
 *
 *  Created 3.1.0 on: Apr 21, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_ALARMS_H_
#define _SRC_ALARMS_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"
#include "layout.h"
#include <deque>

namespace indoorino
{
    class AlarmDevice
    {
    private:
        bool                    _enabled    =false;
        bool                    _on_alarm   =false;
        
        void    send_updates    (void);
        void    acknowledge     (void);
    protected:
        std::vector<iEpoch_t>           _alarm_ack;
        std::vector<packet::netpacket>  _signals;
        layout::dev_alarm_t &   _layout;
        
    public:
        AlarmDevice(layout::dev_alarm_t &);
        ~AlarmDevice() {}
        void    parse       (packet::netpacket *);
        
        bool    is_enabled() { return _enabled;  }
        bool    is_onalarm() { return _on_alarm; }
        
        friend  class       AlarmList;
    };
    
    class AlarmList : public layout::LinkedDevList<AlarmDevice>
    {

    public:
        AlarmList() {}
        ~AlarmList() {}
        
        void    begin       (void);
        void    parse       (packet::netpacket *);
        
        bool    load        (const char *);
        bool    save        (const char *);

    };

} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_ALARMS_H_ */
