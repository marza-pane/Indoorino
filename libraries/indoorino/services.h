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
            std::vector<ServiceTemplate *> _threadlist;
        public:
            
             Services() {}
            ~Services() { _threadlist.clear(); } 
            
            auto& operator() (void) { return _threadlist; }
            
            int             exist       (const char *);
            bool            remove      (const char *);
            bool            add         (const lyt::LayoutServiceKey&);
            void            show        (void);
            
            void            begin       (void);
            iSize_t         size        () { return _threadlist.size(); }
            void            clear       () { _threadlist.clear(); } 
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
            const lyt::LayoutServiceKey     _layout;
            std::vector<DeviceTemplate *>   _devices;
            std::mutex                      _mtx;
            std::condition_variable         _cv;
            
        public:
            const char * name()     { return (const char *)_layout.name; }
            const char * type()     { return (const char *)_layout.type; }
            const char * area()     { return (const char *)_layout.area; }
            const char * location() { return (const char *)_layout.location; }

        public:
            
            ServiceTemplate (const lyt::LayoutServiceKey&);
            
            virtual ~ServiceTemplate() { this->stop(); _devices.clear(); }
            
            virtual void    begin       (void);
            virtual void    stop        (void);
            
            bool            add_device  (lyt::LayoutKey&);
            bool            rem_device  (const char *, const char *);
            int             has_device  (const char *, const char *);
            
        };

        
        
        
        
        class LightsTemplate : public ServiceTemplate
        {
        protected:
            void    loop    (void);
        
        public:
            
             LightsTemplate(const lyt::LayoutServiceKey&);
            ~LightsTemplate();
            
            void    begin   (void);
            void    stop    (void);
            
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
