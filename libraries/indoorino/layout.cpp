/*
 * layout.cpp
 *
 *  Created 3.1.0 on: Apr 19, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#include "../common/utils.h"
#include "../network/server.h"

#if defined (INDOORINO_NETWORK)

#include "layout.h"
#include "paths.h"

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

        void    Layout::clear       (void)
        {            
            _devices.clear();
            _lights.clear();
            _alarms.clear();
            _weather.clear();
        }
        
        void    Layout::reset       (void)
        {
            alert_os("layout: resetting to default!");
            
            this->clear();
            
            for (int i=0; i<LYT_DEFAULT_NUM_DEVICES; i++)
            {
                _devices.push_back(linked_dev_t(DefaultDeviceLayout[i]));
            }
            
            for (int i=0; i<LYT_DEFAULT_NUM_LIGHTS; i++)
            {
                _lights.push_back(dev_light_t(DefaultLightsLayout[i]));
            }
            
            for (int i=0; i<LYT_DEFAULT_NUM_ALARMS; i++)
            {
                _alarms.push_back(dev_alarm_t(DefaultAlarmsLayout[i]));
            }
            
            for (int i=0; i<LYT_DEFAULT_DEV_WEATHER; i++)
            {
                _weather.push_back(dev_weather_t(DefaultWeatherStationLayout[i]));
            }

//             std::vector<board_t>        _devices;
//             std::vector<dev_light_t>    _lights;
//             std::vector<dev_alarm_t>    _alarms;
        }
        
        int     Layout::check       (void)
        {
            int r = 0;
            
            for (auto& b: _devices)
            {
                if (!utils::is_devname(b.boardname, 4, LEN_NAME))
                {
                    warning_os("layout:boards: invalid board name <%s>", b.boardname);
                    r += 11;
                }

                if (!utils::is_devname(b.devname, 4, LEN_DEVNAME))
                {
                    warning_os("layout:boards: invalid device name <%s>", b.devname);
                    r += 12;
                }
                
                
                if (!check_in_devtypes(b.type))
                {
                    warning_os("layout:boards: invalid type <%s>", b.type);
                    r += 13;
                }

                //area location
                
                if (!check_in_areas(b.area))
                {
                    warning_os("layout:boards: invalid area <%s> on device %s:%s",b.area, b.boardname, b.devname);
                    r += 14;
                }
                
                
                if ( strcmp(b.area, "home") == 0 )
                {
                    
                    if (!check_in_area_home(b.location))
                    {
                        warning_os("layout:boards: invalid location <%s> for area <%s> on device %s:%s",b.location, b.area, b.boardname, b.devname);
                        r += 15;
                    }
                }
                else if ( strcmp(b.area, "perimeter") == 0 )
                {
                    if (!check_in_area_perimeter(b.location))
                    {
                        warning_os("layout:boards: invalid location <%s> for area <%s> on device %s:%s",b.location, b.area, b.boardname, b.devname);
                        r += 15;
                    }                    
                }

            }
            
            
            for (auto& d: _alarms)
            {
                uint8_t c = this->is_device(d.boardname, d.devname);
                
                if (c == -1)
                {
                    warning_os("layout:alarms: invalid board/device name <%s:%s>", d.boardname, d.devname);
                    r += 20;
                }
                for (uint i=0; i<LYT_NUM_ALARMS; i++)
                {
                    if ( strcmp(d.alarmtype, global_alarms[i]) == 0) goto success_alarmtype;
                }
                warning_os("layout:alarms: <%s:%s> invalid alarm type <%s>", d.boardname, d.devname, d.alarmtype);
                r += 21;
                
                success_alarmtype: {}
                                
                for (uint i=0; i<LYT_NUM_ALARMS_GROUP; i++)
                {
                    if ( strcmp(d.group, global_alarms_group[i]) == 0) goto success_alarmgroup;
                }
                warning_os("layout:alarms: <%s:%s> invalid alarm group <%s>", d.boardname, d.devname, d.group);
                r += 22;
                
                success_alarmgroup: {}
                
                
            }
            
            for (auto& d: _lights)
            {
                uint8_t c = this->is_device(d.boardname, d.devname);
                if (c == -1)
                {
                    warning_os("layout:lights: invalid board/device name <%s:%s>", d.boardname, d.devname);
                    r += 30;
                }                
            }

            for (auto& d: _weather)
            {
                uint8_t c = this->is_device(d.boardname, d.devname);
                if (c == -1)
                {
                    warning_os("layout:weather: invalid board/device name <%s:%s>", d.boardname, d.devname);
                    r += 40;
                }                
            }
            
            if (r == 0) info_os("layout: check completed succesfully");
            return r;
        }
        
        int     Layout::is_device   (const char * bname, const char * dname)
        {
            for (uint i=0; i<_devices.size(); i++)
            {
                if ( (strcmp(_devices[i].devname, dname) == 0) && (strcmp(_devices[i].boardname, bname) == 0) )
                { return i; }
            }
            return -1;
        }
        
        int     Layout::is_light    (const char * bname, const char * dname)
        {
            for (uint i=0; i<_lights.size(); i++)
            {
                if ( (strcmp(_lights[i].devname, dname) == 0) && (strcmp(_lights[i].boardname, bname) == 0) )
                { return i; }
            }
            return -1;
        }
        
        int     Layout::is_alarm    (const char * bname, const char * dname)
        {
            for (uint i=0; i<_alarms.size(); i++)
            {
                if ( (strcmp(_alarms[i].devname, dname) == 0) && (strcmp(_alarms[i].boardname, bname) == 0) )
                { return i; }
            }
            return -1;
        }
        
        int     Layout::is_weather  (const char * bname, const char * dname)
        {
            for (uint i=0; i<_weather.size(); i++)
            {
                if ( (strcmp(_weather[i].devname, dname) == 0) && (strcmp(_weather[i].boardname, bname) == 0) )
                { return i; }
            }
            return -1;
        }
        
        void    Layout::parse       (packet::netpacket * p)
        {
            if (p->command() == IBACOM_LYT_CONF)
            {

                int  n=0;
                char ** c=nullptr;
                
                {
                    if (strlen(p->p_command()) > 0)
                    {
                        n = 1;
                    }
                    
                    char *  _chp_;
                    char *  _cptr_;
                    
                    _chp_=(char *)malloc(strlen(p->p_command()) + 1);
                    strcpy(_chp_, p->p_command());
                    
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
                
                /*
                 * GET
                 * LOAD
                 * SAVE
                 * RESET
                 * CLEAR    BOARDS
                 *          LIGHTS
                 *          ALARMS
                 *          WEATHER
                 * REM      <group>     <boardname>     <devname>
                 * 
                 */
                
                if (n == 1)
                {
                    if (strcmp(c[0], "GET") == 0)
                    {
                        alert_os("layout:parse:broadcasting to clients");
                        this->send_config();
                    }
                    else if (strcmp(c[0], "LOAD") == 0)
                    {
                        alert_os("layout:parse:loading from file");
                        this->load();
                    }
                    else if (strcmp(c[0], "RESET") == 0)
                    {
                        this->reset();
                    }
                    else if (strcmp(c[0], "SAVE") == 0)
                    {
                        alert_os("layout:parse:saving to file");
                        this->save();
                    }
                    
                
                }
                else if (n == 2)
                {
                    if (strcmp(c[0], "CLEAR") == 0)
                    {
                        if (strcmp(c[1], "DEVICES") == 0)
                        {
                            alert_os("layout:parse:clearing boards layout");
                            _devices.clear();
                        }
                        else if (strcmp(c[1], "LIGHTS") == 0)
                        {
                            alert_os("layout:parse:clearing lights layout");
                            _lights.clear();
                        }
                        else if (strcmp(c[1], "ALARMS") == 0)
                        {
                            alert_os("layout:parse:clearing lights layout");
                            _alarms.clear();
                        }
                        else if (strcmp(c[1], "WEATHER") == 0)
                        {
                            alert_os("layout:parse:clearing weather layout");
                            _weather.clear();
                        }
                        else
                        {
                            warning_os("layout:parse: invalid CLEAR command %s", p->p_command());
                        }

                    }
                    else
                    {
                        warning_os("layout:parse: invalid command %s", p->p_command());
                    }
                    
                }
                else if ( n == 4 && strcmp(c[0], "REM") == 0 )
                {
                    int index;
                    
                    if (strcmp(c[1], "DEVICES") == 0)
                    {
                        index = is_device(c[2], c[3]);
                        if ( index == -1 )
                        {
                            warning_os("layout:parse:rem:board invalid board:device %s:%s", c[2], c[3]);
                        }
                        else
                        {
                            _devices.erase(_devices.begin() + index);
                            alert_os("layout:parse:removing global device %s:%s", c[2], c[3]);
                        }
                    }
                    else if (strcmp(c[1], "LIGHTS") == 0)
                    {
                        index = is_light(c[2], c[3]);
                        if ( index == -1 )
                        {
                            warning_os("layout:parse:rem:lights invalid board:device %s:%s", c[2], c[3]);
                        }
                        else
                        {
                            alert_os("layout:parse:removing light device %s:%s", c[2], c[3]);
                            _lights.erase(_lights.begin() + index);
                        }
                    }
                    else if (strcmp(c[1], "ALARMS") == 0)
                    {
                        index = is_alarm(c[2], c[3]);
                        if ( index == -1 )
                        {
                            warning_os("layout:parse:rem:alarms invalid board:device %s:%s", c[2], c[3]);
                        }
                        else
                        {
                            alert_os("layout:parse:removing alarm device %s:%s", c[2], c[3]);
                            _alarms.erase(_alarms.begin() + index);
                        }
                    }
                    else if (strcmp(c[1], "WEATHER") == 0)
                    {
                        index = is_weather(c[2], c[3]);
                        if ( index == -1 )
                        {
                            warning_os("layout:parse:rem:weather invalid board:device %s:%s", c[2], c[3]);
                        }
                        else
                        {
                            alert_os("layout:parse:removing weather device %s:%s", c[2], c[3]);
                            _weather.erase(_weather.begin() + index);
                        }
                    }
                    else
                    {
                        warning_os("layout:parse: invalid command %s", p->p_command());
                    } 
                }
                else
                {
                    warning_os("layout:parse: invalid command %s", p->p_command());
                }
                                
                for (uint8_t i=0; i<n; i++)
                {
                    free(c[i]);
                }
                free(c);
                
            }
            else if (p->command() == IBACOM_LYT_DEVICE)
            {
                int index = is_device(p->p_board(), p->p_devname());
            
                if (index == -1)
                {
                    linked_dev_t d;
                    strcpy(d.boardname, p->p_board());
                    strcpy(d.devname,   p->p_devname());
                    strcpy(d.type,      p->p_type());
                    strcpy(d.area,      p->p_label1());
                    strcpy(d.location,  p->p_label2());
                    alert_os("layout:parse:adding new global device %s:%s", p->p_board(), p->p_devname());

                    _devices.push_back(d);
                }
                else
                {
                    linked_dev_t &d = _devices.at(index);
                    alert_os("layout:parse:editing GLOBAL device %s:%s", p->p_board(), p->p_devname());

                    strcpy(d.type,      p->p_type());
                    strcpy(d.area,      p->p_label1());
                    strcpy(d.location,  p->p_label2());
                    
                }
            }
            else if (p->command() == IBACOM_LYT_LIGHTS)
            {
                int index = is_light(p->p_board(), p->p_devname());
            
                if (index == -1)
                {
                    dev_light_t d;
                    strcpy(d.boardname, p->p_board());
                    strcpy(d.devname,   p->p_devname());
                    strcpy(d.lightype,  p->p_type());
                    strcpy(d.group,     p->p_label1());
                    alert_os("layout:parse:adding new LIGHT device %s:%s", p->p_board(), p->p_devname());

                    _lights.push_back(d);
                }
                else
                {
                    dev_light_t &d = _lights.at(index);
                    alert_os("layout:parse:editing LIGHT device %s:%s", p->p_board(), p->p_devname());

                    strcpy(d.lightype,  p->p_type());
                    strcpy(d.group,     p->p_label1());
                }
            }

            else if (p->command() == IBACOM_LYT_ALARMS)
            {
                int index = is_alarm(p->p_board(), p->p_devname());
            
                if (index == -1)
                {
                    dev_alarm_t d;
                    strcpy(d.boardname, p->p_board());
                    strcpy(d.devname,   p->p_devname());
                    strcpy(d.alarmtype, p->p_type());
                    strcpy(d.group,     p->p_label1());
                    alert_os("layout:parse:adding new ALARM device %s:%s", p->p_board(), p->p_devname());

                    _alarms.push_back(d);
                }                
                else
                {
                    dev_alarm_t &d = _alarms.at(index);
                    alert_os("layout:parse:editing ALARM device %s:%s", p->p_board(), p->p_devname());

                    strcpy(d.alarmtype, p->p_type());
                    strcpy(d.group,     p->p_label1());
                }
            }

            else if (p->command() == IBACOM_LYT_WEATHER)
            {
                int index = is_weather(p->p_board(), p->p_devname());
            
                if (index == -1)
                {
                    dev_weather_t d;
                    strcpy(d.boardname, p->p_board());
                    strcpy(d.devname,   p->p_devname());
                    strcpy(d.group,     p->p_label1());
                    alert_os("layout:parse:adding new WEATHER device %s:%s", p->p_board(), p->p_devname());

                    _weather.push_back(d);
                }                
                else
                {
                    dev_weather_t &d = _weather.at(index);
                    alert_os("layout:parse:editing WEATHER device %s:%s", p->p_board(), p->p_devname());

                    strcpy(d.group,     p->p_label1());
                }
            }
            
            return;
        }
        
        void    Layout::show            (void)
        {
            std::cout << "\tLayout devices:" << std::endl;
            for (auto& b: _devices)
            {
                printf("%s:%s (%s) @ %s:%s",b.boardname, b.devname, b.type, b.area, b.location);
                std::cout << std::endl;
            }
            std::cout << "\tLayout lights:" << std::endl;
            for (auto& b: _lights)
            {
                printf("%s:%s ==> %s:%s",b.boardname, b.devname, b.group, b.lightype);
                std::cout << std::endl;
            }
            std::cout << "\tLayout alarms:" << std::endl;
            for (auto& b: _alarms)
            {
                printf("%s:%s ==> %s:%s",b.boardname, b.devname, b.group, b.alarmtype);
                std::cout << std::endl;
            }
            std::cout << "\tLayout weather station:" << std::endl;
            for (auto& b: _weather)
            {
                printf("%s:%s ==> %s",b.boardname, b.devname, b.group);
                std::cout << std::endl;
            }
        }
   
        bool    Layout::save            (void)
        {
            const char * filepath = paths.db_layout();
            
            std::ofstream file;
            file.open(filepath, std::ios::out | std::ios::binary);
            
            /** Checking if file is good **/

            if(!file.is_open())
            {
                error_io("save-boards: could not open to <%s>", filepath);
                return false;
            }
            
            if(!file.good())
            {
                error_io("save-boards: could not write to <%s>", filepath);
                return false;
            }

            iSize_t packetcount = _devices.size();
//             iSize_t size_p = _devices.size();
//             iSize_t size_l = _lights.size();
//             iSize_t size_a = _alarms.size();
            
            info_io("LAYOUT:save: writing System.layout to %s [%u packets]", filepath, packetcount);

            packet::ipacket p(IBACOM_LYT_DEVICE);
            std::vector<char> buffer;
            buffer.reserve(p.full_size());

            file.write((char *)(&packetcount), sizeof(iSize_t));
            for (auto& d: _devices)
            {
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.area);
                strcpy(p.p_label2(),    d.location);
                strcpy(p.p_type(),      d.type);
                
                p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
                file.write((char *)(buffer.data()), p.full_size());

                if(!file.good())
                {
                    error_io("LAYOUT:save: io error while writing to <%s>", filepath);
                    file.close();
                    return false;
                }                
            }

            packetcount = _lights.size();
            p.init(IBACOM_LYT_LIGHTS);
            buffer.reserve(p.full_size());
            file.write((char *)(&packetcount), sizeof(iSize_t));
            for (auto& d: _lights)
            {
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.group);
                strcpy(p.p_type(),      d.lightype);
                
                p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
                file.write((char *)(buffer.data()), p.full_size());

                if(!file.good())
                {
                    error_io("LAYOUT:save: io error while writing to <%s>", filepath);
                    file.close();
                    return false;
                }                
            }

            packetcount = _alarms.size();
            p.init(IBACOM_LYT_ALARMS);
            buffer.reserve(p.full_size());
            file.write((char *)(&packetcount), sizeof(iSize_t));
            for (auto& d: _alarms)
            {
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_type(),      d.alarmtype);
                strcpy(p.p_label1(),    d.group);
                
                p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
                file.write((char *)(buffer.data()), p.full_size());

                if(!file.good())
                {
                    error_io("LAYOUT:save: io error while writing to <%s>", filepath);
                    file.close();
                    return false;
                }                
            }

            packetcount = _weather.size();
            p.init(IBACOM_LYT_WEATHER);
            buffer.reserve(p.full_size());
            file.write((char *)(&packetcount), sizeof(iSize_t));
            for (auto& d: _weather)
            {
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.group);
                
                p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
                file.write((char *)(buffer.data()), p.full_size());

                if(!file.good())
                {
                    error_io("LAYOUT:save: io error while writing to <%s>", filepath);
                    file.close();
                    return false;
                }                
            }

            file.close();
            return true;
        }
        
        bool    Layout::load            (void)
        {
            const char * filepath = paths.db_layout();
            
            std::ifstream file;
            file.open(filepath, std::ios::in | std::ios::binary);

            
            /** Checking if file is good **/
            
            if(!file.is_open())
            {
                error_io("LAYOUT:load: could not open to <%s>", filepath);
                return false;
            }

            if(!file.good())
            {
                error_io("LAYOUT:load: could not read from <%s>", filepath);
                file.close();
                return false;
            }

            iSize_t packetcount;

            std::vector<char> buffer;
            packet::ipacket p;

            file.read((char *)&packetcount, sizeof(iSize_t));
            
            if (file.eof())
            {
                error_io("LAYOUT:load: %s got EOF @ devices !", filepath);
                goto unfinished_load;                            
            }
            
            info_io("LAYOUT:load: loading System.layout from %s [%u packets]", filepath, packetcount);
                        
            for (uint8_t i=0; i<packetcount; i++)
            {
                buffer.reserve(SIZEOF_PACKET_HEADER);
                file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);

                if (file.eof())
                {
                    error_io("LAYOUT:load: %s got EOF at packet %u (header)!", filepath, i);
                    goto unfinished_load;                            
                }
                
                p.parse(buffer.data(), false);
                if (p.command() != IBACOM_LYT_DEVICE)
                {
                    warning_io("LAYOUT:load: %s got invalid packet %s!", filepath, p.label());
                    buffer.clear();
                    continue;
                }
                
                buffer.reserve(p.data_size());
                file.read((char *)(buffer.data()), p.data_size());
                
                if (file.eof())
                {
                    error_io("LAYOUT:load: %s got EOF at packet %u (body)!", filepath, i);
                    goto unfinished_load;                            
                }

                memcpy(p.payload(), buffer.data(), p.data_size());
                
                if (this->is_device(p.p_board(), p.p_devname()) == -1)
                {
                    _devices.push_back( linked_dev_t() );
                    debug_os("Added device %s:%s from file!", p.p_board(), p.p_devname());
                    
                    strcpy(_devices.back().boardname,   p.p_board());
                    strcpy(_devices.back().devname,     p.p_devname());
                    strcpy(_devices.back().area,        p.p_label1());
                    strcpy(_devices.back().location,    p.p_label2());
                    strcpy(_devices.back().type,        p.p_type());
                    
                }
                else
                {
                    warning_os("LAYOUT:load: %s:%s already in list!", p.p_board(), p.p_devname());
                }
                
                buffer.clear();
            }
                        
            file.read((char *)&packetcount, sizeof(iSize_t));
            
            if (file.eof())
            {
                error_io("LAYOUT:load: %s got EOF @ alarms!", filepath);
                goto unfinished_load;
            }

            p.init(IBACOM_LYT_LIGHTS);
            
            for (uint i=0; i<packetcount; i++)
            {
                buffer.reserve(SIZEOF_PACKET_HEADER);
                file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);

                if (file.eof())
                {
                    error_io("LAYOUT:load: %s got EOF at packet %u (header)!", filepath, i);
                    goto unfinished_load;                            
                }
                
                p.parse(buffer.data(), false);
                if (p.command() != IBACOM_LYT_LIGHTS)
                {
                    warning_io("LAYOUT:load: %s got invalid packet %s!", filepath, p.label());
                    buffer.clear();
                    continue;
                }
                
                buffer.reserve(p.data_size());
                file.read((char *)(buffer.data()), p.data_size());
                
                if (file.eof())
                {
                    error_io("LAYOUT:load: %s got EOF at packet %u (body)!", filepath, i);
                    goto unfinished_load;                            
                }

                memcpy(p.payload(), buffer.data(), p.data_size());
                int index = this->is_device(p.p_board(), p.p_devname());
                
                if (index == -1)
                {
                    warning_io("LAYOUT:load: %s has invalid light:device %s:%s!", filepath, p.p_board(), p.p_devname());                    
                }
                else
                {
                    if (is_light(p.p_board(), p.p_devname()) == -1 )
                    {
                        auto dev = _devices.at(index);
                        
                        _lights.push_back( dev_light_t() );
                        
                        strcpy(_lights.back().boardname,    p.p_board());
                        strcpy(_lights.back().devname,      p.p_devname());
                        strcpy(_lights.back().area,         dev.area);
                        strcpy(_lights.back().location,     dev.location);
                        strcpy(_lights.back().type,         dev.type);
                        strcpy(_lights.back().group,        p.p_label1());
                        strcpy(_lights.back().lightype,     p.p_type());
                        
                        debug_os("Added light relay %s:%s from file!", p.p_board(), p.p_devname());
                    }
                    else
                    {
                        warning_os("LAYOUT:load: light relay %s:%s already in list!", p.p_board(), p.p_devname());
                    }
                    
                }
                
                buffer.clear();
            }
            
            file.read((char *)&packetcount, sizeof(iSize_t));
            
            if (file.eof())
            {
                error_io("LAYOUT:load: %s got EOF @ alarms!", filepath);
                goto unfinished_load;
            }

            p.init(IBACOM_LYT_ALARMS);
            
            for (uint i=0; i<packetcount; i++)
            {
                buffer.reserve(SIZEOF_PACKET_HEADER);
                file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);

                if (file.eof())
                {
                    error_io("LAYOUT:load:alarms: %s got EOF at packet %u/%u (header)!", filepath, i, packetcount);
                    goto unfinished_load;                            
                }
                
                p.parse(buffer.data(), false);
                if (p.command() != IBACOM_LYT_ALARMS)
                {
                    warning_io("LAYOUT:load:alarms: %s got invalid packet %s!", filepath, p.label());
                    buffer.clear();
                    continue;
                }
                
                buffer.reserve(p.data_size());
                file.read((char *)(buffer.data()), p.data_size());
                
                if (file.eof())
                {
                    error_io("LAYOUT:load:alarms: %s got EOF at packet %u/%u (body)!", filepath, i, packetcount);
                    goto unfinished_load;                            
                }

                memcpy(p.payload(), buffer.data(), p.data_size());
                int index = this->is_device(p.p_board(), p.p_devname());
                
                if (index == -1)
                {
                    warning_io("LAYOUT:load:alarms: %s has invalid device %s:%s!", filepath, p.p_board(), p.p_devname());                    
                }
                else
                {
                    if (is_alarm(p.p_board(), p.p_devname()) == -1 )
                    {
                        auto dev = _devices.at(index);
                        
                        _alarms.push_back( dev_alarm_t() );
                        
                        strcpy(_alarms.back().boardname,    p.p_board());
                        strcpy(_alarms.back().devname,      p.p_devname());
                        strcpy(_alarms.back().area,         dev.area);
                        strcpy(_alarms.back().location,     dev.location);
                        strcpy(_alarms.back().type,         dev.type);
                        strcpy(_alarms.back().group,        p.p_label1());
                        strcpy(_alarms.back().alarmtype,    p.p_type());
                        
                        debug_os("Added alarm device %s:%s from file!", p.p_board(), p.p_devname());
                    }
                    else
                    {
                        warning_os("LAYOUT:load: alarm %s:%s already in list!", p.p_board(), p.p_devname());
                    }
                    
                }
                
                buffer.clear();
            }
            
            file.read((char *)&packetcount, sizeof(iSize_t));
            
            if (file.eof())
            {
                error_io("LAYOUT:load: %s got EOF @ weather!", filepath);
                goto unfinished_load;
            }

            p.init(IBACOM_LYT_WEATHER);
            
            for (uint i=0; i<packetcount; i++)
            {
                buffer.reserve(SIZEOF_PACKET_HEADER);
                file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);

                if (file.eof())
                {
                    error_io("LAYOUT:load:weather: %s got EOF at packet %u/%u (header)!", filepath, i, packetcount);
                    goto unfinished_load;                            
                }
                
                p.parse(buffer.data(), false);
                if (p.command() != IBACOM_LYT_WEATHER)
                {
                    warning_io("LAYOUT:load:weather: %s got invalid packet %s!", filepath, p.label());
                    buffer.clear();
                    continue;
                }
                
                buffer.reserve(p.data_size());
                file.read((char *)(buffer.data()), p.data_size());
                
                if (file.eof())
                {
                    error_io("LAYOUT:load:weather: %s got EOF at packet %u/%u (body)!", filepath, i, packetcount);
                    goto unfinished_load;                            
                }

                memcpy(p.payload(), buffer.data(), p.data_size());
                int index = this->is_device(p.p_board(), p.p_devname());
                
                if (index == -1)
                {
                    warning_io("LAYOUT:load:weather: %s has invalid device %s:%s!", filepath, p.p_board(), p.p_devname());                    
                }
                else
                {
                    if (is_weather(p.p_board(), p.p_devname()) == -1 )
                    {
                        auto dev = _devices.at(index);
                        
                        _weather.push_back( dev_weather_t() );
                        
                        strcpy(_weather.back().boardname,    p.p_board());
                        strcpy(_weather.back().devname,      p.p_devname());
                        strcpy(_weather.back().area,         dev.area);
                        strcpy(_weather.back().location,     dev.location);
                        strcpy(_weather.back().type,         dev.type);
                        strcpy(_weather.back().group,        p.p_label1());
                        
                        debug_os("Added weather device %s:%s from file!", p.p_board(), p.p_devname());
                    }
                    else
                    {
                        warning_os("LAYOUT:weather: weather device %s:%s already in list!", p.p_board(), p.p_devname());
                    }
                    
                }
                
                buffer.clear();
            }
            file.close();
            return true;
            
            unfinished_load:
                error_io("LAYOUT:load: <%s> failed", filepath);
                file.close();
                return false;
            
        }

        void    Layout::send_devices    (void)
        {
            for (auto& d: _devices)
            {
                packet::netpacket p(IBACOM_LYT_DEVICE);
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.area);
                strcpy(p.p_label2(),    d.location);
                strcpy(p.p_type(),      d.type);
                Server.shell.broadcast(&p);
            }
        }

        void    Layout::send_lights     (void)
        {
            for (auto& d: _lights)
            {
                packet::netpacket p(IBACOM_LYT_LIGHTS);
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.group);
                strcpy(p.p_type(),      d.lightype);
                Server.shell.broadcast(&p);
            }
        }

        void    Layout::send_alarms     (void)
        {
            for (auto& d: _alarms)
            {
                packet::netpacket p(IBACOM_LYT_ALARMS);
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.group);
                strcpy(p.p_type(),      d.alarmtype);
                Server.shell.broadcast(&p);
            }
        }
        
        void    Layout::send_weather    (void)
        {
            for (auto& d: _weather)
            {
                packet::netpacket p(IBACOM_LYT_WEATHER);
                strcpy(p.p_devname(),   d.devname);
                strcpy(p.p_board(),     d.boardname);
                strcpy(p.p_label1(),    d.group);
                Server.shell.broadcast(&p);
            }
        }
        
        void    Layout::send_config     (void)
        {
            this->send_devices();
            this->send_alarms();
            this->send_lights();
            this->send_weather();
        }
 
        packet::netpacket   *   Layout::get_layout_packet   (const char * lgroup, const char * bname, const char * dname)
        {
            if (strcmp(lgroup, "devices") == 0)
            {
                if (this->is_device(bname, dname) != -1)
                {
                    linked_dev_t d = _devices.at(this->is_device(bname, dname));
                    packet::netpacket * p = new packet::netpacket(IBACOM_LYT_DEVICE);
                    strcpy(p->p_devname(),   d.devname);
                    strcpy(p->p_board(),     d.boardname);
                    strcpy(p->p_label1(),    d.area);
                    strcpy(p->p_label2(),    d.location);
                    strcpy(p->p_type(),      d.type);
                    
                    return p;
                }
                
            }
            else if (strcmp(lgroup, "lights") == 0)
            {
                
            }
            else if (strcmp(lgroup, "alarms") == 0)
            {
                
            }
            
            error_os("FATAL:layout:get_packet: invalid board:device %s:%s", bname, dname);
            return nullptr;
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
