/*
 * schedule.h
 *
 *  Created on: Nov 1, 2020
 *      Author: n00b
 */
#pragma once

#ifndef SOURCE_SCHEDULE_H_
#define SOURCE_SCHEDULE_H_

#if defined (__linux__)

#include "../common/list.h"
#include "../packets/ipacket.h"
namespace indoorino
{
    class Schedule
    {
        std::deque<packet::schedPacket *>  _txlist;

    public:
                 Schedule() {};
        virtual ~Schedule();
        
        bool            pushInTime      (packet::netpacket *, time_t, bool b=false, bool s=false);
        bool            pushInSeconds   (packet::netpacket *, uint32_t, bool b=false, bool s=false);
        void            loop            (void);
        
        
        std::deque<packet::schedPacket *>& sending() { return _txlist; }
        
    };
} /* namespace : indoorino */

/*
 * Ah ragazzo qui stiamo andando sullo scientifico
    Boards["LIGHT_BEAM"].device["DHT1"].get_temperature(buffer, 0, 50);
*/

#endif /* __linux__ */
#endif /* SOURCE_SCHEDULE_H_ */
