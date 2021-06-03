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
    namespace alarms
    {
        AlarmDevice::AlarmDevice(layout::dev_alarm_t& p):_layout(p)
        {}
        
        void        AlarmDevice::parse          (packet::netpacket * p)
        {
            if ( (p->command() >= IBACOM_HEAT_ALARM) && (p->command() <= IBACOM_GENERIC_ALARM) )
            {
                if ( (strcmp(p->p_board(), _layout.boardname) == 0) && (strcmp(p->p_devname(), _layout.devname) == 0) )
                {
                    std::locale local;
                    std::string alarmstring=std::string(p->label());
                    for (uint i=0; i<alarmstring.length(); i++)
                        alarmstring.at(i) = std::toupper(alarmstring.at(i));
                    
                    warning_os(" %s from <%s:%s>", alarmstring.c_str(), p->p_board(), p->p_devname());
                    if (_enabled)
                    {
                        warning_os("ALARM <%s:%s> is enabled!" , _layout.boardname, _layout.devname);
                        
                        packet::ipacket q(IBACOM_ENV_ALARM);
                        iEpoch_t epoch=utils::epoch_now();
                        
                        strcpy(q.p_desc2(),   p->label());
                        strcpy(q.p_desc1(),   _layout.group);
                        strcpy(q.p_label1(),  _layout.area);
                        strcpy(q.p_label2(),  _layout.location);
                        memcpy(q.p_epoch(), &epoch, sizeof(uint32_t));
                        
                            /** se vuoi mandare una mail/azionare un device/pregare la madonna/ per un certo allarme devi farlo qui ! **/

                        if (strcmp(_layout.alarmtype, layout::global_alarms[0]) == 0) // fire
                        {
                            switch (p->command())
                            {

                                case (IBACOM_HEAT_ALARM):
                                {
                                    *q.p_value1() = 1;
                                    break;
                                }
                                case (IBACOM_OVERH_ALARM):
                                {
                                    *q.p_value1() = 2;
                                    break;
                                }
                                case (IBACOM_FIRE_ALARM):
                                {
                                    *q.p_value1() = 3;
                                    break;
                                }
                                default:
                                {
                                    goto unmatched_packet;
                                }
                            }
                        }
                        else if (strcmp(_layout.alarmtype, layout::global_alarms[1]) == 0) // flood
                        {
                            switch (p->command())
                            {

                                case (IBACOM_MOIST_ALARM):
                                {
                                    *q.p_value1() = 1;
                                    break;
                                }
                                case (IBACOM_FLOOD_ALARM):
                                {
                                    *q.p_value1() = 2;
                                    break;
                                }
                                default:
                                {
                                    goto unmatched_packet;
                                }
                            }
                        }
                        else if (strcmp(_layout.alarmtype, layout::global_alarms[2]) == 0) // hazard
                        {                        
                            switch (p->command())
                            {

                                case (IBACOM_SMOG_ALARM):
                                {
                                    *q.p_value1() = 1;
                                    break;
                                }
                                case (IBACOM_HAZARD_ALARM):
                                {
                                    *q.p_value1() = 2;
                                    break;
                                }
                                case (IBACOM_SMOKE_ALARM):
                                {
                                    *q.p_value1() = 3;
                                    break;
                                }
                                default:
                                {
                                    goto unmatched_packet;
                                }
                            }
                        }
                        else if (strcmp(_layout.alarmtype, layout::global_alarms[3]) == 0) // hazard
                        {                        
                            switch (p->command())
                            {
                                case (IBACOM_GRID_ALARM):
                                {
                                    *q.p_value1() = 2;
                                    break;
                                }
                                default:
                                {
                                    goto unmatched_packet;
                                }
                           
                            }
                        }
                        else
                        {
                            unmatched_packet:
                            {
                                *q.p_value1() = 1;
                                warning_os("ALARM: Unmatched %s for device %s:%s [%s:%s]",
                                        p->label(), _layout.boardname, _layout.devname, _layout.type, _layout.group);                                
                            }
                        }

                        
                        warning_os("ALARM: *** <%s> alarm at <%s> ***", p->label(), _layout.group);
                        Server.shell.broadcast(&q);
                    }
                    _on_alarm=true;
                    _signals.push_back(packet::netpacket(p));
                    this->send_updates();
                }
            }
            else if ( p->command() == IBACOM_ACK_ENV_ALARM )
            {
                alert_os("ALARM:parsing ACK");
                if ( (strcmp(p->p_board(), _layout.boardname) == 0) && (strcmp(p->p_devname(), _layout.devname) == 0) )
                {
                    alert_os("ALARM:parsing ACK for <%s:%s>", p->p_board(), p->p_devname());
                    if (_on_alarm)
                    {
                        _on_alarm=false;
                        _alarm_ack.push_back(std::chrono::system_clock::now());
                        alert_os("ALARM:ACK for <%s:%s>", p->p_board(), p->p_devname());
                    }
                    _signals.push_back(packet::netpacket(p));
                    this->send_updates();
                }
            }
            else if ( p->command() == IBACOM_SET_ENV_ALARM )
            {
                if ( (strcmp(p->p_board(), _layout.boardname) == 0) && (strcmp(p->p_devname(), _layout.devname) == 0) )
                {
                    bool b=bool(*p->p_value1());
                    if (b)
                        alert_os("ALARM:SET <%s:%s> ENABLED", p->p_board(), p->p_devname());
                    else
                        alert_os("ALARM:SET <%s:%s> DISABLED", p->p_board(), p->p_devname());                    
                        
                    _enabled=b;
                    _signals.push_back(packet::netpacket(p));
                    this->send_updates();
                }
            }
        }

        void        AlarmDevice::send_updates   (void)
        {
            debug_os("ALARM: sending updates!");
            packet::ipacket p(IBACOM_ALARM_DEVSTAT);
            
            strcpy(p.p_board(),   _layout.boardname);
            strcpy(p.p_devname(), _layout.devname);
            strcpy(p.p_desc1(),   _layout.group);
            
            uint32_t value = uint32_t(_on_alarm);
            
            memcpy(p.p_status(), &_enabled, sizeof(uint8_t));
            memcpy(p.p_value1(), &value, sizeof(uint32_t));

            // *p.p_value1() = uint32_t(_on_alarm);
            // if (_on_alarm)  *p.p_value1() = 1;
            // else            *p.p_value1() = 0;

            Server.shell.broadcast(&p);
        }
        
        void        Alarms::begin               (void)
        {
            this->load_layout();
        }
        
        void        Alarms::load_layout         (void)
        {
            this->clear();        
            for (uint i=0; i<LYT_DEFAULT_NUM_ALARMS; i++)
            {
                _classlist.push_back(AlarmDevice(layout::DefaultAlarmsLayout[i]));
            }
        }
        
        void        Alarms::send_status         (void)
        {
            for (auto& a: _classlist)
            {
                a.send_updates();
            }            
        }
        
        void        Alarms::parse               (packet::netpacket * p)
        {
            for (auto& d: _classlist)
            {
                d.parse(p);
            }
            

//             for (uint i=0; i<LYT_NUM_ALARMS_GROUP; i++)
//             {
//                 uint alm_before=0;
//                 uint alm_after=0;
//                 
//                 for (auto& d: _classlist)
//                 {
//                     if (strcmp(d._layout.group, layout::global_alarms_group[i]) == 0)
//                     {
//                         if (d.is_onalarm()) alm_before++;
//                         d.parse(p);
//                         if (d.is_onalarm()) alm_after++;
//                     }
//                 }
//             }
//             
//             if ( (alm_after == 0) && (alm_before > 0) ) 
//             {
//                 packet::ipacket q(IBACOM_ACK_ENV_ALARM);
//             
//                 strcpy(p.p_board(),   _layout.boardname);
//                 strcpy(p.p_devname(), _layout.devname);
//                 strcpy(p.p_desc1(),   _layout.group);
//                 
//                 uint32_t value = uint32_t(_on_alarm);
//                 
//                 memcpy(p.p_status(), &_enabled, sizeof(uint8_t));
//                 memcpy(p.p_value1(), &value, sizeof(uint32_t));
// 
//                 // *p.p_value1() = uint32_t(_on_alarm);
//                 // if (_on_alarm)  *p.p_value1() = 1;
//                 // else            *p.p_value1() = 0;
// 
//                 Server.shell.broadcast(&p);
//             }
            
//             for (uint i=0; i<LYT_NUM_ALARMS_GROUP; i++)
//             {
//                 uint    alm_count=0;
//                 char    alm_area[LEN_LABEL];
//                 char    alm_location[LEN_LABEL];
// 
//                 for (auto& d: _classlist)
//                 {
//                     if (strcmp(d._layout.group, layout::global_alarms_group[i]) == 0)
//                     {
//                         // parsing device in proper group
//                         if (d.is_enabled())
//                         {
//                             if (d.is_onalarm())
//                             {
//                                 alm_count++;
//                                 
//                                 memset(alm_area,     0, LEN_LABEL);
//                                 strcpy(alm_area,     d._layout.area);
//                                 memset(alm_location, 0, LEN_LABEL);
//                                 strcpy(alm_location, d._layout.location);
//                                 warning_os("ALARM: *** <%s> alarm at <%s> ***", d._layout.type, d._layout.group);
//                             }
//                         }
//                     }
//                 }
//                 
//                 if (alm_count > 0)
//                 {
//                     packet::ipacket p(IBACOM_ENV_ALARM);
//                     iEpoch_t epoch=utils::epoch_now();
//                     
//                     strcpy(p.p_desc1(),   layout::global_alarms_group[i]);
//                     strcpy(p.p_label1(),  alm_area);
//                     strcpy(p.p_label2(),  alm_location);
//                     memcpy(p.p_epoch(), &epoch, sizeof(uint32_t));
//                     
//                     *p.p_value1() = 1;
//                     
//                     Server.shell.broadcast(&p);
//                 }
//             }
            
        }
    
    } /* namespace:alarms */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

    //     void        AlarmDevice::acknowledge    (void)
    //     {
    //         _on_alarm=false;
    //         this->send_updates();    
    //     }
