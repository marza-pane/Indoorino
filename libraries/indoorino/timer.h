
/*
 * database.h
 *
 *  Created 3.1.0 on: May 8, 2021 (SERVER)
 * 
 *      Author: n00b
 */

// #include <stdio.h>
// #include <cmath>
// #include <time.h>
// #include <string.h>
// #include <iostream>
// #include <chrono>
// #include <algorithm>
// #include <vector>

#include "common/utils.h"

#ifndef _SRC_TIMER_H
#define _SRC_TIMER_H

#if defined(INDOORINO_SERVER)

#include <thread>
#include <mutex>
#include <condition_variable>
        
namespace indoorino
{
    
    /* Usage:
     * 
     * TimerTemplate has 2 pure virtual function: on_start() and on_stop() that must be overriden
     * 
     */
    
    class TimerTemplate
    {

    protected:
        
        std::vector<NDO_DURATION>   _eventable;
        std::mutex                  _mtx;
        std::condition_variable     _stop_condition;
        std::vector<std::thread>    _threads;

        char                        _devname[LEN_DEVNAME]  {0};
        char                        _boardname[LEN_NAME]   {0};
        bool                        _enabled=false;
        
        void    timerthread     (std::chrono::system_clock::time_point start, std::chrono::seconds period)
        {
            _threads.push_back(
                std::thread( [this, start, period] 
                    {
                        if (!_enabled) return;

                        auto now = std::chrono::system_clock::now();
                        std::unique_lock<std::mutex> lck(_mtx);
                        
                        if (now < start)
                        {
                            
                            std::chrono::duration start_in = std::chrono::duration_cast<std::chrono::seconds>(start - now);
                            info_os("TIMER: %s:%s waiting %s", _boardname, _devname, utils::format_duration(start_in));

                            _stop_condition.wait_for(lck, start_in);
                            
                            if (!_enabled) return;

                        }
                        
                        else if (now >= (start + period))
                        {
                            std::cout << "Timer Event added too late!" << std::endl;
                            return;
                        }
                        
                        
                        /* System send turn ON command to relay */
                        on_start();
                        std::cout << "TURN ON !" << std::endl;

                        std::chrono::duration stop_in = std::chrono::duration_cast<std::chrono::seconds>( (start + period) - now);
                        info_os("TIMER: %s:%s turned on for %s", _boardname, _devname, utils::format_duration(stop_in));
                        
                        _stop_condition.wait_for(lck, stop_in);                    
                        
                        /* System send turn OFF command to relay */
                        on_stop();
                        std::cout << "TURN OFF !" << std::endl;

                    }
                )
            );
        }
        
        bool    existhread      (std::chrono::system_clock::time_point start, std::chrono::seconds period)
        {
            if (_enabled)
            {
                for (auto e: _eventable)
                {
                    if (e.first == start && e.second == period) return true;
                }
            }
            return false;            
        }
        
        void    readtable       (void)
        {
            this->clear();
            
            for (auto e: _eventable)
            {
                timerthread(e.first, e.second);
            }
        }
        
    public:
                
        void    enable  (void)
        {
            info_os("TIMER:%s:%s enabled!", _boardname, _devname);
            _enabled=true;
            readtable();
        }
        
        void    disable (void)
        {
            if (_enabled)
            {
                info_os("TIMER: stopping timer event for %s:%s", _boardname, _devname);
                std::unique_lock<std::mutex> lck(_mtx);
                _enabled=false;
            }
            _stop_condition.notify_all();
        }
        
        void    add     (std::chrono::system_clock::time_point start, std::chrono::seconds period)
        {
            
            if (existhread(start, period))
            {
                warning_os("TIMER:event already in list: %s ==> %s",
                           utils::timestring_chrono(start),
                           utils::format_duration(period));
                return;
            }

            info_os("TIMER:%s:%s adding event!", _boardname, _devname);
            info_os("TIMER: ^^^ start at %s", _boardname, _devname, utils::timestring_chrono(start));
            info_os("TIMER: ^^^ for %s seconds", _boardname, _devname, utils::format_duration(period));
            
            
            _eventable.push_back(std::move(std::make_pair(start, period)));
            this->timerthread(start, period);                

        }

        void    clear   (void)
        {
            this->disable();
            for (auto &t : _threads)
            {
                if (t.joinable())
                {
                    std::cout << "Joining Timer Event" << std::endl; 
                    t.join();
                }
            }
        }
        
        virtual void on_start   (void)=0;  
        virtual void on_stop    (void)=0;  
        
        const char * boardname  (void) { return _boardname; }
        const char * devname    (void) { return _devname; }
        
        bool        is_enabled  (void) { return _enabled; }
        bool        is_on       (void)
        {
            auto now = std::chrono::system_clock::now();
            
            if (_enabled)
            {
                for (auto e: _eventable)
                {
                    if (now > e.first && now < (e.first + e.second)) return true;
                }
            }
            return false;
        }
        
    public:
        
        
        TimerTemplate(const char * bname, const char * dname)
        {
            strcpy(_boardname, bname);
            strcpy(_devname, dname);
            this->enable();        
        }
        ~TimerTemplate()
        {
            this->clear();
        }
    };

} /* namespace:indoorino */
    
#endif /* INDOORINO_SERVER */

#endif /* _SRC_TIMER_H */
