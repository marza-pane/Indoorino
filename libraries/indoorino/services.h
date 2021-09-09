/*
 * database.h
 *
 *  Created 3.1.0 on: May 8, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_SERVICE_H_
#define _SRC_SERVICE_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"
#include "layout.h"
#include "devices.h"
#include <deque>
#include <mutex>
#include <condition_variable>

/*
 * Possible conditions:
 * 
 * - device is added or removed from one or more services:
 * - service has terminated unexpetedly and must be restarted:
 * - user wants to restart a specific service
 * 
 * TODO implement these line above
 * 
 */

namespace indoorino
{
    namespace svc
    {
        
        class ServiceTemplate;
    
//      _________________________________________
//      |                                       |
//      |       Services List                   |
//      |_______________________________________|
//        

        class Services
        {
        private:
            std::mutex                      _mtx;
            std::vector<ServiceTemplate *>  _threadlist;
        public:
            
             Services() {}
            ~Services() { this->clear(); } 
            
            ServiceTemplate * operator() (const char *);
            auto operator[] (int i) { return _threadlist.at(i); }
            auto operator() (void)  { return _threadlist; }
            
            int             exist       (const char *);
            bool            remove      (const char *);
            bool            add         (lyt::Service *);
            void            show        (void);
            
            void            begin       (void);
            iSize_t         size        () { return _threadlist.size(); }
            void            clear       (void);
            void            read_layout (void);
            
            bool            load        (void) { return 0; };
            bool            save        (void) { return 0; };
        };

//      _________________________________________
//      |                                       |
//      |       Service Template                |
//      |_______________________________________|
//
            
        class ServiceTemplate
        {
        protected:
            virtual void    loop    (void);

        protected:            
            std::thread _thread;
            bool        _running=false;
            bool        _update=false;

        protected:
            lyt::Service               *    _layout;
            std::mutex                      _mtx;
            std::condition_variable         _cv;
            
        public:
            const char * name()     { return (const char *)_layout->name(); }
            const char * type()     { return (const char *)_layout->type(); }
            const char * area()     { return (const char *)_layout->area(); }
            const char * location() { return (const char *)_layout->location(); }            
            
        public:
            
            ServiceTemplate (lyt::Service *);
            
            virtual ~ServiceTemplate() { this->stop(); }
            
            virtual void    begin       (void);
            virtual void    stop        (void);
            virtual void    read_layout (void)=0;
            virtual void    on_update   (void)=0;
            virtual void    send_status (void)=0;
            virtual void    parse       (packet::ipacket*)=0;
            
            virtual bool    is_alarm    (void) { return false; }
            virtual bool    is_light    (void) { return false; }
            virtual bool    is_climate  (void) { return false; }

            int             has_device  (const char *, const char *);
            
            friend class Services;
        };


        
//         class AlarmFlood: public ServiceTemplate
//         {
//         protected:
//             void    loop    (void);
//         
//         public:
//             
//             AlarmFlood();
//             ~AlarmFflood();
//             
//             void    begin   (void);
//             void    stop    (void);
//             
//         };
        
        
    } /* namespace:svc*/
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_SERVICE_H_ */
