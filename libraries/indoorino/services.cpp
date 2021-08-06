/*
 * service.cpp
 *
 *  Created 3.1.0 on: Jul 27, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#if defined (INDOORINO_NETWORK)

#include "common/indoorino.h"
#include "indoorino-system.h"
#include "services.h"
#include "alarms.h"

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


        void        Services::begin                 (void)
        {
            for (auto s: System.layout.services())
            {
                auto p = lyt::LayoutServiceKey(s.type(), s.name(), s.desc(), s.area(), s.location());
                if (this->add(p))
                {
                    for (auto d: s.devices())
                        _threadlist.back()->add_device(d);
                }
                
            }
        }
        
        int         Services::exist                 (const char *name)
        {
            int index=0;
            
            for (auto &s : _threadlist)
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
            _threadlist.erase(_threadlist.begin() + index);
            return true;
        }

        bool        Services::add                   (const lyt::LayoutServiceKey& key)
        {
            int index = this->exist(key.name);

            if (index != -1)
            {
                warning_os("SERVICE:LIST:add: %s already in list [%d]", key.name, index);
                return false;
            }

            const char * command = key.type;
        
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
                    command, key.name, key.area, key.location);
            
            bool flag = false;
            if ( strcmp(c[0], "ALARM") == 0 )
            {
                if ( strcmp(c[1], "FIRE") == 0 )
                {
                    _threadlist.push_back(new alm::AlarmTemplate(key));
                    _threadlist.back()->begin();
                    flag=true;
                }
                if ( strcmp(c[1], "FLOOD") == 0 )
                {
                    _threadlist.push_back(new alm::AlarmTemplate(key));                
                    _threadlist.back()->begin();
                    flag=true;
                }
                else
                {
                    
                }
                
            }
            if ( strcmp(c[0], "LIGHTS") == 0 )
            {
                if ( strcmp(c[1], "INDOOR") == 0 )
                {
                    _threadlist.push_back(new LightsTemplate(key));
                    _threadlist.back()->begin();
                    flag=true;
                }            
                if ( strcmp(c[1], "OUTDOOR") == 0 )
                {
                    _threadlist.push_back(new LightsTemplate(key));                
                    _threadlist.back()->begin();
                    flag=true;
                }
                else
                {
                    
                }
                
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

        ServiceTemplate::ServiceTemplate(const lyt::LayoutServiceKey& s):_layout(s)
        {

        };
        
                    /*     B E G I N     */
        void        ServiceTemplate::begin          (void)
        {
            info_os("SERVICE:%s: %s @ [%s:%s] thread start!",
                        _layout.type, _layout.name, _layout.area, _layout.location);            
            _running = true;
            _thread = std::thread(

            [this]
            
            {
                
//                 bool flag=true;
//                 std::mutex mtx;
//                 mtx.lock();
//                 flag = your_fucking_var;
//                 mtx.unlock();

                while (_running)
                {
                    this->loop();
//                     std::cout << "@";
                }
                
                warning_os("SERVICE:%s: %s @ [%s:%s] shutting down!",
                           _layout.type, _layout.name, _layout.area, _layout.location);
                return;
            });
        
        }

                    /*      L O O P     */
        void        ServiceTemplate::loop           (void)
        {
            std::unique_lock<std::mutex> lck(_mtx);
            _cv.wait_for(lck, std::chrono::milliseconds(1000));
        }
        
                    /*      S T O P     */
        void        ServiceTemplate::stop           (void)
        {
            debug_os("SERVICE:%s: stopping!", _layout.name);
            if (_running)
            {
//                 _mtx.lock();
                std::unique_lock<std::mutex> lck(_mtx);
                _running = false;
                _cv.notify_all();
//                 _mtx.unlock();
                debug_os("SERVICE:%s: set thread flag", _layout.name);
            }
            
            if (_thread.joinable())
            {
                _thread.join();
                alert_os("SERVICE:%s: thread terminated!", _layout.name);
            }
        
        }
    
    
    
        bool        ServiceTemplate::add_device     (lyt::LayoutKey& key)
        {                
            if (has_device(key.boardname(), key.devname()) == -1 )
            {
                DeviceTemplate * p = System.boards(key.boardname())->devices()(key.devname());
                if (p != nullptr)
                {
                    info_os("SERVICE:%s:add-device: %s:%s", _layout.name, key.boardname(), key.devname()); 
                    _devices.push_back(p);
                    return true;
                }
                else error_dev("SERVICE:add-device: call to System.board().device() returned nullptr", key.boardname(), key.devname());
            }
            else warning_os("SERVICE:add-device: invalid board/device %s/%s", key.boardname(), key.devname());
            return false;
        }
        
        bool        ServiceTemplate::rem_device     (const char *bname, const char *dname)
        {
            int index = has_device(bname, dname);
            if (index == -1)
            {
                warning_os("SERVICE:rem-device: could not find %s:%s", bname, dname);
                return false;
            }
            
            _devices.erase(_devices.begin() + index);
            return true;
        }
        
        int         ServiceTemplate::has_device     (const char * bname, const char *dname)
        {
            int index = 0;
            for (auto &dev : _devices)
            {   
                if ( (strcmp(dev->boardname(), bname) == 0) && (strcmp(dev->name(), dname) == 0) )
                    return index;
                index++;
            }
            return -1;
        }

            
//      _________________________________________
//      |                                       |
//      |       Services implementation         |
//      |_______________________________________|
//
    
    
        LightsTemplate::LightsTemplate(const lyt::LayoutServiceKey& s):ServiceTemplate(s)
        {
             
        }
        
        LightsTemplate::~LightsTemplate()
        {
            this->stop();
        }
    
        void        LightsTemplate::begin                (void)
        {
            alert_os("SERVICE:LIGHTS:begin %s @ [%s:%s]",_layout.name, _layout.area, _layout.location);
            ServiceTemplate::begin();
        }

        void        LightsTemplate::loop                 (void)
        {
            ServiceTemplate::loop();
        }
        
        void        LightsTemplate::stop                 (void)
        {
            ServiceTemplate::stop();
        }
    
    } /* namespace:db */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */
