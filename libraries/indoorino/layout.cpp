/*
 * layout.cpp
 *
 *  Created 3.1.0 on: Apr 19, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#include "../common/utils.h"

#if defined (INDOORINO_NETWORK)

#include "layout.h"

namespace indoorino
{

    namespace layout
    {
        
        bool    check_in_areas              (const char * entry)
        {
            for (uint8_t i=0; i<LYT_NUM_AREAS; i++)
            {
                if ( strcmp(global_area[i], entry) == 0 )
                {
                    return true;
                }
            }
            return false;
        }
        
        bool    check_in_area_home          (const char * entry)
        {
            for (uint8_t i=0; i<LYT_NUM_L_HOME; i++)
            {
                if ( strcmp(global_home[i], entry) == 0 )
                {
                    return true;
                }
            }
            return false;
        }
        
        bool    check_in_area_perimeter     (const char * entry)
        {
            for (uint8_t i=0; i<LYT_NUM_L_PERIMETER; i++)
            {
                if ( strcmp(global_perimeter[i], entry) == 0 )
                {
                    return true;
                }
            }
            return false;
        }
        
        bool    check_in_devtypes           (const char * entry)
        {
            for (uint8_t i=0; i<LYT_NUM_DEVTYPES; i++)
            {
                if ( strcmp(global_devtypes[i], entry) == 0 )
                {
                    return true;
                }
            }
            return false;
        }
        
        
        
        void    Layout::reset       (void)
        {
            alert_os("layout: resetting to default!");
            _boards.clear();
            _lights.clear();
            _alarms.clear();
            
            for (int i=0; i<LYT_DEFAULT_NUM_BOARDS; i++)
            {
                _boards.push_back(board_t(BoardDefaultLayout[i]));
            }
            
            for (int i=0; i<LYT_DEFAULT_NUM_LIGHTS; i++)
            {
                _lights.push_back(dev_light_t(LightsDefaultLayout[i]));
            }
            for (int i=0; i<LYT_DEFAULT_NUM_ALARMS; i++)
            {
                _alarms.push_back(dev_alarm_t(AlarmsDefaultLayout[i]));
            }

//             std::vector<board_t>        _boards;
//             std::vector<dev_light_t>    _lights;
//             std::vector<dev_alarm_t>    _alarms;
        }
        
        int     Layout::check       (void)
        {
            int r = 0;
            
            for (auto& b: _boards)
            {
                if (!utils::is_devname(b.name))
                {
                    warning_os("layout:boards: invalid board name <%s>", b.name);
                    r += 11;
                }

                if (b.devnum > MAX_ATTACHED_DEVICES)
                {
                    warning_os("layout:boards: too many devices [%u]", b.devnum);
                    r += 12;
                }
                
                //area location
                
                if (!check_in_areas(b.area))
                {
                    warning_os("layout:boards: invalid area <%s> on board %s",b.area, b.name);
                    r += 13;
                }
                
                
                if ( strcmp(b.area, "home") == 0 )
                {
                    
                    if (!check_in_area_home(b.location))
                    {
                        warning_os("layout:boards: invalid location <%s> for area <%s> on board %s",b.location, b.area, b.name);
                        r += 14;
                    }
                }
                else if ( strcmp(b.area, "perimeter") == 0 )
                {
                    if (!check_in_area_perimeter(b.location))
                    {
                        warning_os("layout:boards: invalid location <%s> for area <%s> on board %s",b.location, b.area, b.name);
                        r += 14;
                    }                    
                }
                
                for (int j=0; j<b.devnum; j++)
                {
                    if (!utils::is_devname(b.devices[j].name, 4, LEN_DEVNAME))
                    {
                        warning_os("layout:boards: invalid device name <%s> on board %s", b.devices[j].name, b.name);
                        r += 15;
                    }

                    if (!check_in_devtypes(b.devices[j].type))
                    {
                        warning_os("layout:boards: invalid device %s type <%s> on board %s", b.devices[j].name, b.devices[j].type, b.name);
                        r += 16;
                        
                    }                    
                }
            }
            
            
            for (auto& d: _lights)
            {
                uint8_t c = this->is_board(d.boardname);
                if (c == -1)
                {
                    warning_os("layout:lights: invalid board name <%s>", d.boardname);
                    r += 30;
                }
                else
                {
                    for (uint8_t i=0; i<_boards[c].devnum; i++)
                    {
                        if ( strcmp(_boards[c].devices[i].name, d.devname) == 0 )
                        {
                            goto check_lights_devs_success;
                        }
                    }
                    warning_os("layout:lights: invalid device name <%s> on board %s", d.devname, d.boardname);
                    r += 31;
                }
                
                check_lights_devs_success : {}
                
            }
            
            for (auto& d: _alarms)
            {
                uint8_t c = this->is_board(d.boardname);
                if (c == -1)
                {
                    warning_os("layout:alarms: invalid board name <%s>", d.boardname);
                    r += 20;
                }
                else
                {
                    for (uint8_t i=0; i<_boards[c].devnum; i++)
                    {
                        if ( strcmp(_boards[c].devices[i].name, d.devname) == 0 )
                        {
                            goto check_alarm_devs_success;
                        }
                    }
                    warning_os("layout:alarms: invalid device name <%s> on board %s", d.devname, d.boardname);
                    r += 21;
                }
                
                check_alarm_devs_success : {}
            }
            
            if (r == 0) info_os("layout: check completed succesfully");
            return r;
        }
        
        int     Layout::is_board    (const char * entry)
        {
            for (int i=0; i<_boards.size(); i++)
            {
                if (strcmp(_boards[i].name, entry) == 0) { return i; }
                
            }
            return -1;
        }
        
        void    Layout::show        (void)
        {
            for (auto& b: _boards)
            {
                std::cout << b.name << " @ " << b.area << ":" << b.location << std::endl;
                std::cout << "\t[" << int(b.devnum )<< "] device(s) attached" << std::endl;
                for (int j=0; j<b.devnum; j++)
                {
                    std::cout << "\t - " << b.devices[j].name << " : " << b.devices[j].type << std::endl;
                }
            }
        }
        
        
        
        
        
   
        
        
    } /* namespace:layout */

} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

/*
gcc -DINDOORINO_NETWORK -DINDOORINO_SERVER -I. -I/usr/include/openssl/ -I/usr/include/boost/ -Wall -pthread -std=c++17 

/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/common/utils.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/common/benchmark.cpp 
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/devices/stddev.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/devices/sensors.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/devices/boardconf.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/devices/boardio.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/devices/devices.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/server.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/client-start.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/shellserver.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/client.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/boardserver.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/netutils.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/espclient.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/server-start.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/network/connection.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/packets/packeteval.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/packets/ipacket.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/packets/packetstubs.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/packets/packetutils.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/indoorino/schedule.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/indoorino/paths.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/indoorino/indoorino-system.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/indoorino/boards.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/indoorino/devices.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/indoorino/layout.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/AES/src/AES.cpp /home/n00b/Code/Develop/Indoorino.3.1.0/libraries/AES/src/xbase64.cpp
/home/n00b/Code/Develop/Indoorino.3.1.0/libraries/AES/src/AESLib.cpp 

-lm -ldl -lstdc++ -lboost_system -lcrypto -o serverbuilt
*/
