/*
 * service.cpp
 *
 *  Created 3.1.0 on: Jul 27, 2021 (SERVER)
 * 
 *      Author: n00b
 * 
 *

//                 bool flag=true;
//                 std::mutex mtx;
//                 mtx.lock();
//                 flag = your_fucking_var;
//                 mtx.unlock();

 */

#if defined (INDOORINO_NETWORK)

#include "common/indoorino.h"
#include "indoorino-system.h"
#include "services.h"
#include "alarms.h"
#include "service-lights.h"

// #include "../network/server.h"

namespace indoorino
{
    namespace svc
    {
    

//      _________________________________________
//      |                                       |
//      |       Services List                   |
//      |_______________________________________|
//

        ServiceTemplate * Services::operator()      (const char * name)
        {
            int i=this->exist(name);
            
            if (i == -1)
            {
                error_os("FATAL:SERVICE: invalid call for service <%s>", name);
                return nullptr;
            }
            
            return _threadlist.at(i);
        }
        
        void        Services::begin                 (void)
        {
            alert_os("SERVICES: starting services...");
            this->read_layout();
        }
        
        void        Services::clear                 (void)
        {
            for (auto p : _threadlist)
            {
                p->stop();
                delete p;
            }
            _threadlist.clear();
        }
        
        void        Services::read_layout           (void)
        {
            info_os("SERVICE:reading layout...");
            for (auto p : _threadlist)
            {
                int i = System.layout.is_service(p->name()); 
                if (i == -1)
                {
                    warning_os("SERVICE:update: not found in System.layout.services() <%s> removing service", p->name());
                    this->remove(p->name());
                    this->read_layout();
                    return;
                }
            }

            
            for (auto &s: System.layout.services())
            {
                
                if (this->exist(s.name()) == -1) this->add(&s);
                
                _threadlist.back()->read_layout();
            }            
        }
        
        int         Services::exist                 (const char *name)
        {
            int index=0;
            
            for (auto s : _threadlist)
            {
                if ( strcmp(s->name(), name) == 0 ) return index;
                index++;
            }
            return -1;
        }
        
        bool        Services::remove                (const char *name)
        {
            int index = this->exist(name);

            if (index == -1)
            {
                warning_os("SERVICE:LIST:remove: can not find %s",name);
                return false;
            }
            
            _threadlist.at(index)->stop();
            delete _threadlist.at(index);
            _threadlist.erase(_threadlist.begin() + index);
            return true;
        }

        bool        Services::add                   (lyt::Service * key)
        {
            int index = this->exist(key->name());

            if (index != -1)
            {
                warning_os("SERVICE:LIST:add: %s already in list [%d]", key->name(), index);
                return false;
            }

            const char * command = key->type();
        
            if (strlen(command) == 0)
            {
                warning_os("SERVICE:LIST:add: empty type!");
                return false;
            }
                
            int  n=0;
            char ** c=nullptr;
            
            {
                if (strlen(command) > 0)
                {
                    n = 1;
                }
                
                char *  _chp_;
                char *  _cptr_;
                
                _chp_=(char *)malloc(strlen(command) + 1);
                strcpy(_chp_, command);
                
                _cptr_ = strchr(_chp_, ':');
                while (_cptr_ != NULL)
                {
                    n++;
                    _cptr_=strchr(_cptr_ + 1,':');
                }
                
                char * index = _chp_;
                
                c = (char**)calloc(n, sizeof(char*));

                for (uint8_t i=0; i<n; i++)
                {
                    c[i] = (char *)calloc(LEN_NAME, sizeof(char));
                    _cptr_ = strchr(index, ':');

                    if (_cptr_==NULL)  { strcpy(c[i], index); }
                    else            
                    { 
                        strncpy(c[i], index, _cptr_ - index);
                        index = _cptr_ + 1;
                    }

                    debug_io("-->Chunk %u : %s",i, c[i]);
                }
                
                free (_chp_);
            }
            
            if (!c)
            {
                error_os("SERVICE:LIST:add: nullptr type buffer! - memory leaked");
                return false;
            }               
            
            debug_os("SERVICE:LIST:add: new service %s ==> %s @ [%s:%s] ",
                    command, key->name(), key->area(), key->location());
            
            bool flag = false;
            if ( strcmp(c[0], "ALARM") == 0 )
            {
                _threadlist.push_back(new alm::AlarmService(key));
                _threadlist.back()->begin();
                flag=true;                
            }
            else if ( strcmp(c[0], "LIGHTS") == 0 )
            {
                _threadlist.push_back(new lgt::LightService(key));
                _threadlist.back()->begin();
                flag=true;
            }
            else
            {
                error_os("SERVICES: unknow service type <%s>", command);
            }
            
            for (uint8_t i=0; i<n; i++)
            {
                free(c[i]);
            }
            free(c);
            
            return flag;
        }
        
        
        
//         void            show        (void);

//      _________________________________________
//      |                                       |
//      |       Service Template                |
//      |_______________________________________|
//

        ServiceTemplate::ServiceTemplate(lyt::Service * s):_layout(s)
        {

        };
        
                    /*     B E G I N     */
        void        ServiceTemplate::begin          (void)
        {
            info_os("SERVICE:%s: %s @ [%s:%s] thread start!",
                        _layout->type(), _layout->name(), _layout->area(), _layout->location());            
            _running = true;
            _thread = std::thread(

            [this]
            
            {

                while (_running)
                {
                    this->loop();
                    // std::cout << "@";
                }
                
                warning_os("SERVICE:%s: %s @ [%s:%s] shutting down!",
                           _layout->type(), _layout->name(), _layout->area(), _layout->location());
                return;
            });
        
        }

                    /*      L O O P     */
        void        ServiceTemplate::loop           (void)
        {
            std::unique_lock<std::mutex> lck(_mtx);
            _cv.wait_for(lck, std::chrono::milliseconds(60000));
        }
        
                    /*      S T O P     */
        void        ServiceTemplate::stop           (void)
        {
            debug_os("SERVICE:%s: stopping!", _layout->name());
            if (_running)
            {
                std::unique_lock<std::mutex> lck(_mtx);
                _running = false;
                _cv.notify_all();
                debug_os("SERVICE:%s: set thread flag", _layout->name());
            }
            
            if (_thread.joinable())
            {
                _thread.join();
                alert_os("SERVICE:%s: thread terminated!", _layout->name());
            }
        
        }
    
        int         ServiceTemplate::has_device     (const char * bname, const char *dname)
        {
            int index = 0;
            for (auto &dev : _layout->devices())
            {   
                if ( (strcmp(dev.boardname(), bname) == 0) && (strcmp(dev.devname(), dname) == 0) )
                    return index;
                index++;
            }
            return -1;
        }


    } /* namespace:svc */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */
