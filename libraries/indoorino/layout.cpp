/*
 * layout.cpp
 *
 *  Created 3.1.0 on: Apr 19, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#include "../common/utils.h"
#include "../network/server.h"
#include "indoorino-system.h"

#if defined (INDOORINO_SERVER)

#include "layout.h"
#include "paths.h"

namespace indoorino
{
    namespace lyt
    {
            
        LayoutKey::LayoutKey(
            const char *b,
            const char *d,
            const char *a,
            const char *l,
            const char *c,
            const char *t)
        {
            strcpy(_boardname, b);
            strcpy(_devname, d);
            strcpy(_area, a);
            strcpy(_location, l);
            strcpy(_conf, c);

            for (auto & devtype : indoorino::lyt::LAYOUT_DEVTYPES)
            {
                if (strcmp(devtype.label, t) == 0)
                {
                    memcpy(&_devtype, &devtype, sizeof(devtype));
                    return;
                }
            }

            warning_io("LAYOUT:LayoutKey: unmatched type <%s> for %s:%s", t, b, d);
            memcpy(&_devtype, indoorino::lyt::LAYOUT_DEVTYPES, sizeof(LayoutDevKey)); /* the [0] is missing becuase useless */
        }
        
        LayoutKey::LayoutKey(
            const char *b,
            const char *d,
            const char *a,
            const char *l,
            const char *c,
            iCom_t t )
        {
            strcpy(_boardname, b);
            strcpy(_devname, d);
            strcpy(_area, a);
            strcpy(_location, l);
            strcpy(_conf, c);
                        
            for (auto & devtype : indoorino::lyt::LAYOUT_DEVTYPES)
            {
                if (t == devtype.command)
                {
                    memcpy(&_devtype, &devtype, sizeof(devtype));
                    return;
                }
            }
            
            warning_io("LAYOUT:LayoutKey: unmatched type %u for %s:%s", t, b, d);
            memcpy(&_devtype, indoorino::lyt::LAYOUT_DEVTYPES, sizeof(LayoutDevKey)); /* the [0] is missing becuase useless */
        }

        iCom_t devicetype_Str2Com (const char * name)
        {
            for (auto & entry : LAYOUT_DEVTYPES)
                if (strcmp(name, entry.label) == 0)
                    return entry.command;
            return IBACOM_CONF_DEVSTD;
        }

        const char * devicetype_Com2Str (iCom_t comm)
        {
            for (uint i=0; i<LYT_NUM_DEVTYPES; i++)
                if (comm == LAYOUT_DEVTYPES[i].command)
                    return (const char *)LAYOUT_DEVTYPES[i].label;
            return (const char *)LAYOUT_DEVTYPES[0].label;                
        }
        
        //      _________________________________________
        //      |                                       |
        //      |       Layout home map                 |
        //      |_______________________________________|
        //
                
    
        bool    HomeMap::add_area           (const char * n)
        {
            if (exist(n) == -1)
            {
                _areas.push_back(HomeArea(n));
                debug_os("LAYOUT:HOMEMAP:add: added area %s", n);
                return true;
            }
            else warning_os("LAYOUT:HOMEMAP:add: area %s already exist", n);

            return false;
        }
                    
        bool    HomeMap::rem_area           (const char * n)
        {
            int i = exist(n);
            if (i != -1)
            {
                _areas.erase(_areas.begin() + i);
                debug_os("LAYOUT:HOMEMAP:rem: removed area %s", n);
                return true;
            }
            else warning_os("LAYOUT:HOMEMAP:rem: area %s do not exist", n);
            
            return false;                
        }
        
        bool    HomeMap::add_location       (const char * a, const char * l)
        {
            int i = exist(a);
            
            if (i != -1)
            {
                if (_areas.at(i).exist(l) == -1)
                {
                    _areas[i]._locations.push_back(std::string(l));
                    debug_os("LAYOUT:HOMEMAP:add: added location %s:%s", a, l);
                    return true;
                }
                else warning_os("LAYOUT:HOMEMAP:add: location %s:%s already exist", a, l);
            }
            else warning_os("LAYOUT:HOMEMAP:add: can not add location %s to unknown area %s", l, a);

            return false;
        }

        bool    HomeMap::rem_location       (const char * a, const char * l)
        {
            int i = exist(a);
            int j;
            
            if (i != -1)
            {
                j = _areas.at(i).exist(l);
                if (j != -1)
                {
                    _areas[i]._locations.erase(_areas[i]().begin() + j);
                    debug_os("LAYOUT:HOMEMAP:rem: removed location %s:%s", a, l);
                    return true;
                }
                else warning_os("LAYOUT:HOMEMAP:rem: location %s do not exist in area %s", l, a);
            }
            else warning_os("LAYOUT:HOMEMAP:rem: area %s do not exist", a);
            
            return false;
        }
        
        void    HomeMap::reset              (void)
        {
            
            _areas.clear();
            alert_os("LAYOUT:HOMEMAP:reset: restoring defaualt values");
            for (auto &a : indoorino::lyt::LAYOUT_AREAS)
            {
                add_area(a);
                
                if (strcmp(a, "home") == 0)
                {
                    for (auto &l : indoorino::lyt::LAYOUT_AREA_HOME)
                    {
                        debug_os("LAYOUT:HOMEMAP: adding location %s:%s", a, l);
                        add_location(a, l);
                    }                    
                }
                else if (strcmp(a, "perimeter") == 0)
                {
                    for (auto &l : indoorino::lyt::LAYOUT_AREA_PERIMETER)
                    {
                        debug_os("LAYOUT:HOMEMAP: adding location %s:%s", a, l);
                        add_location(a, l);
                    }                    
                }
                else if (strcmp(a, "garden") == 0)
                {
                    for (auto &l : indoorino::lyt::LAYOUT_AREA_GARDEN)
                    {
                        debug_os("LAYOUT:HOMEMAP: adding location %s:%s", a, l);
                        add_location(a, l);
                    }                    
                }
                else if (strcmp(a, "sheds") == 0)
                {
                    for (auto &l : indoorino::lyt::LAYOUT_AREA_SHEDS)
                    {
                        debug_os("LAYOUT:HOMEMAP: adding location %s:%s", a, l);
                        add_location(a, l);
                    }                    
                }
            }
        }
        
        bool    HomeMap::save               (void) { return false;} // TODO
        
        bool    HomeMap::load               (void) { return false;} // TODO

        void    HomeMap::send2client        (void)
        {
                        
            for (auto &a : _areas)
            {
                for (auto &l : a._locations)
                {
                    packet::ipacket p(IBACOM_LYT_MAP);
                    strcpy(p.p_label1(), a._name);
                    strcpy(p.p_label2(), l.c_str());
                    Server.shell.broadcast(&p);
                }
            }
        }
    
        //      _________________________________________
        //      |                                       |
        //      |       Layout main class               |
        //      |_______________________________________|
        //        
    
        std::vector<LayoutKey>& Layout::devices()
        {
            static std::vector<LayoutKey> buffer;
            
            buffer.clear();
            
            for (auto &s : services())
            {
                for (auto &d : s.devices())
                {
//                     if ( [d, buffer] {
//                         for (auto &b : buffer)
//                         {
//                             if ( (strcmp(d.boardname(), b.boardname()) == 0) && (strcmp(d.devname(), b.devname()) == 0) )
//                             {
//                                 return true;
//                             }
//                         }
//                         return false;
//                     } ) continue;
//                     
                    bool flag=false;
                    for (auto &b : buffer)
                    {
                        if ( (strcmp(b.boardname(), d.boardname()) == 0) && (strcmp(b.devname(), d.devname()) == 0) )
                        {
                            flag=true;
                            break;
                        }
                    }
                    if (flag) continue;
                    
                    buffer.push_back(d);
                }
            }
            return buffer;
        }
        
        int Layout::is_service              (const char * n)
        {
            int i=0;
            for ( auto & s : _services )
            {
                if ( strcmp(s.name(), n) == 0 )
                {
                    return i;
                }
            }
            return -1;
        }
        
        bool Layout::is_board               (const char * b)
        {
            for ( auto & s : _services )
            {
                for ( auto & dev : s.devices())
                {
                    if (strcmp(dev.boardname(), b) == 0)
                    {
                        return true;
                    }
                }
            }
            return false;                
        }
        
        bool Layout::is_device              (const char * b, const char * d)
        {
            for ( auto & s : _services )
            {
                for ( auto & dev : s.devices())
                {
                    if ( (strcmp(dev.boardname(), b) == 0) && (strcmp(dev.devname(), d) == 0) )
                    {
                        return true;
                    }
                }
            }
            return false;
        }
    
        bool Layout::clear_service          (const char *n)
        {
            int i=is_service(n);
            
            if (i != -1)
            {
                alert_os("LAYOUT: clearing service %s [%d devices]", n, _services.at(i).devices().size());
                _services.at(i).devices().clear();
                // Update System
                return true;
            }
            
            warning_os("LAYOUT:clear: invalid service %s:%s", n);
            return false;
        }

        bool Layout::rem_service            (const char *n)
        {
            int index=this->is_service(n);
            if (index == -1)
            {
                _services.erase(_services.begin() + index);
                alert_os("LAYOUT:REM:service: removed %s", n);
                return true;
            }
            warning_os("LAYOUT:REM:service: invalid service %s", n);
            return false;
        }
        
        bool Layout::rem_board              (const char *b)
        {
            if (is_board(b))
            {
                alert_os("LAYOUT:REM:board: removing %s ", b);
                for (auto & service : _services)
                {
                    int index = 0;
                    for ( auto & dev : service.devices())
                    {
                        if (strcmp(dev.boardname(), b) == 0)
                        {
                            info_os("LAYOUT:REM:board: found %s in %s (@ %d)", b, service.name(), index);
                            _services.erase(_services.begin() + index);
                        }
                        index++;
                    }
                }
                // Update System
                return true;
            }
            warning_os("LAYOUT:REM:board invalid board %s", b);
            return false;
            
        }
        
        bool Layout::rem_device             (const char *b, const char *d)
        {
            if (is_device(b, d))
            {
                alert_os("LAYOUT:REM:device: removing %s:%s ", b, d);
                for (auto & service : _services)
                {
                    int index = service.exist(b, d);
                    if (index != -1)
                    {
                        info_os("LAYOUT:REM:device: found %s in %s (@ %d)", d, service.name(), index);
                        service.devices().erase(service.devices().begin() + index);
                    }                                        
                }
                // Update System
                return true;
            }
            warning_os("LAYOUT:REM:device invalid device %s:%s", b, d);
            return false;
        }
        
        bool Layout::rem_deviceFromService  (const char *b, const char *d, const char *s)
        {
            if (is_device(b, d))
            {
                for (auto & service : _services)
                {
                    if (strcmp(service.name(), s) == 0)
                    {
                        int index = service.exist(b,d );
                        if (index != -1)
                        {
                            service.devices().erase(service.devices().begin() + index);
                            alert_os("LAYOUT:REM:fromService %s:%s - from service: %s", b, d, s);
                            // Update System
                            return true;
                        }                                        
                    }
                }
                warning_os("LAYOUT:REM:fromService device %s:%s do not belong to %s", b, d, s);
                return false;
            }
            warning_os("LAYOUT:REM:fromService invalid device %s:%s", b, d);
            return false;
        }
        
        
        
        bool    Layout::save                (void)
        {
            const char * filepath = paths.db_layout();
            
            std::ofstream file;
            file.open(filepath, std::ios::out | std::ios::binary);
            
            /** Checking if file is good **/

            if( (!file.is_open()) || (!file.good()) )
            {
                if (!file.is_open()) error_io("LAYOUT:save: could not open to <%s>", filepath);
                else error_io("LAYOUT:save: could not write to <%s>", filepath);
                return false;
            }
            
            iSize_t count = _services.size();
            iSize_t count_dev = 0;
            iSize_t i = 0;
            
            if( count == 0 )
            {
                warning_os("LAYOUT:save: no data to save!");
                return false;
            }
            
            info_os("LAYOUT:save: writing System.layout to %s [%u packets]", filepath, count);
            file.write((char *)(&count), sizeof(iSize_t));

            for (auto & service : _services)
            {
                file.write((char *)(service.type()), LEN_NAME);
                file.write((char *)(service.name()), LEN_NAME);
                file.write((char *)(service.desc()), LEN_LABEL);
                file.write((char *)(service.area()), LEN_LOCATIONAME);
                file.write((char *)(service.location()), LEN_LOCATIONAME);
                
                count_dev = service.devices().size();
                file.write((char *)(&count_dev), sizeof(iSize_t));
                
                for (auto & device : service.devices())
                {
                    iCom_t c=device.type().command;
                    file.write((char *)(device.boardname()),    LEN_NAME);
                    file.write((char *)(device.devname()),      LEN_DEVNAME);
                    file.write((char *)(device.area()),         LEN_LOCATIONAME);
                    file.write((char *)(device.location()),     LEN_LOCATIONAME);
                    file.write((char *)(device.conf()),         LEN_LABEL);
                    file.write((char *)&c, sizeof(iCom_t));            
                }
                
                if (!file.good())
                {
                    error_io("LAYOUT:save: io error while writing [%u] to <%s>", i, filepath);
                    file.seekp(0);
                    file.write((char *)(&i), sizeof(iSize_t));
                    file.close();
                    return false;
                }
                
                i++;
            }
            
            file.close();
            info_io("LAYOUT:save: succesfully wrote %u layout services to %s", i, filepath);
            return true;
        }
        
        bool    Layout::load                (void)
        {
            const char * filepath = paths.db_layout();
            
            std::ifstream file;
            file.open(filepath, std::ios::in | std::ios::binary);

            
            /** Checking if file is good **/
            
            if( (!file.is_open()) || (!file.good()) )
            {
                if (!file.is_open()) error_io("LAYOUT:load: could not open to <%s>", filepath);
                else error_io("LAYOUT:save: could not read to <%s>", filepath);
                return false;
            }

            std::vector<char> buffer;
            iSize_t count;
            iSize_t count_dev = 0;
            iSize_t i=0;
            
            file.read((char *)&count, sizeof(iSize_t));
                        
            info_io("LAYOUT:load: loading System.layout from %s [%u services]", filepath, count);
            _services.clear();
            
            for (i=0; i<count; i++)
            {
                char type[LEN_NAME];
                char name[LEN_NAME];
                char desc[LEN_LABEL];
                char area[LEN_LOCATIONAME];
                char location[LEN_LOCATIONAME];
                
                file.read((char *)(type), LEN_NAME);
                file.read((char *)(name), LEN_NAME);
                file.read((char *)(desc), LEN_LABEL);
                file.read((char *)(area), LEN_LOCATIONAME);
                file.read((char *)(location), LEN_LOCATIONAME);
                file.read((char *)(&count_dev), sizeof(iSize_t));
                               
                _services.push_back(Service(type, name, desc, area, location));
                
                for (iSize_t j=0; j<count_dev; j++)
                {
                    char devname[LEN_DEVNAME];
                    char devconf[LEN_LABEL];
                    iCom_t command = 0;

                    file.read((char *)(name), LEN_NAME);
                    file.read((char *)(devname), LEN_DEVNAME);
                    file.read((char *)(area), LEN_LOCATIONAME);
                    file.read((char *)(location), LEN_LOCATIONAME);
                    file.read((char *)(devconf), LEN_LABEL);
                    file.read((char *)(&command), sizeof(iCom_t));
                    
                    _services.back().devices().push_back(
                        indoorino::lyt::LayoutKey(
                            name, devname, area, location, devconf, command
                        )
                    );
                    
                    
                }
            }
            
            file.close();
            info_io("LAYOUT:load: succesfully read %u layout services to %s", i, filepath);
            return true;
        }
        
        void    Layout::parse               (packet::ipacket * incoming)
        {
            if (incoming->command() == IBACOM_LYT_CONF)
            {

                int  n=0;
                char ** c=nullptr;
                
                {
                    if (strlen(incoming->p_command()) > 0)
                    {
                        n = 1;
                    }
                    
                    char *  _chp_;
                    char *  _cptr_;
                    
                    _chp_=(char *)malloc(strlen(incoming->p_command()) + 1);
                    strcpy(_chp_, incoming->p_command());
                    
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
                 
                 * CLEAR    AREA            <area>
                 * REM      LOCATION        <area>          <location>

                 * CLEAR    SERVICE         <service>
                 * REM      SERVICE         <service>
                 * REM      DEVICE          <boardname>     <devname>     
                 * REM      BOARD           <boardname>     
                 * REM      FROMSERVICE     <boardname>     <devname>       <service>
                 * 
                 */
                
                if (n == 1)
                {
                    if (strcmp(c[0], "GET") == 0)
                    {
                        alert_os("LAYOUT:parse: broadcasting configuration to clients");
                    }
                    else if (strcmp(c[0], "LOAD") == 0)
                    {
                        alert_os("LAYOUT:parse: loading from file");
                        this->load();
                        // here add system command

                    }
                    else if (strcmp(c[0], "RESET") == 0)
                    {
                        alert_os("LAYOUT:parse: restoring defaualt layout");
                        // this->reset();
                        // here add system command
                    }
                    else if (strcmp(c[0], "SAVE") == 0)
                    {
                        alert_os("LAYOUT:parse: saving to file");
                        this->save();
                    }
                    
                
                }
                
                else if (n==3)
                {
                    // * CLEAR    AREA            <area>
                    if ( (strcmp(c[0], "CLEAR") == 0) && (strcmp(c[1], "AREA") == 0))
                    {
                        _home_map.rem_area(c[2]);
                        // Update System
                    }
                    // * REM      BOARD           <boardname>     
                    else if ( (strcmp(c[0], "REM") == 0) && (strcmp(c[1], "BOARD") == 0))
                    {
                        this->rem_board(c[2]);
                    }
                    // * CLEAR    SERVICE         <service>
                    else if ( (strcmp(c[0], "CLEAR") == 0) && (strcmp(c[1], "SERVICE") == 0))
                    {
                        this->clear_service(c[2]);
                    }
                    // * REM      SERVICE         <service>
                    else if ( (strcmp(c[0], "REM") == 0) && (strcmp(c[1], "SERVICE") == 0))
                    {
                        this->rem_service(c[2]);
                    } 
                    else
                        warning_os("LAYOUT:parse: invalid command %s", incoming->p_command());
                }
                
                else if (n == 4)
                {
                    //* REM      LOCATION        <area>          <location>
                    if ( (strcmp(c[0], "REM") == 0) && (strcmp(c[1], "LOCATION") == 0))
                    {
                        _home_map.rem_location(c[2], c[3]);
                    }
                    
                    //* REM      DEVICE          <boardname>     <devname>       
                    else if ( (strcmp(c[0], "REM") == 0) && (strcmp(c[1], "DEVICE") == 0))
                    {
                        this->rem_device(c[2], c[3]);
                    }
                    else
                        warning_os("LAYOUT:parse: invalid command %s", incoming->p_command());
                }
                
                else if (n == 5)
                {
                    // * REM      FROMSERVICE     <boardname>     <devname>       <service>
                    if ( (strcmp(c[0], "REM") == 0) && (strcmp(c[1], "FROMSERVICE") == 0))
                    {
                        this->rem_deviceFromService(c[2], c[3], c[4]);
                    }
                    else
                        warning_os("LAYOUT:parse: invalid command %s", incoming->p_command());
                }
                
                for (uint8_t j=0; j<n; j++) free(c[j]);
                free(c);
                
            }
            
            else if (incoming->command() == IBACOM_LYT_DEVICE)
            {
                
//     devname','board', 'label1', 'label2', 'label3', 'type', 'desc1')), #area, location, usability, type (to lookup in layout::LayoutDevKey), and service
                
                int i=is_service(incoming->p_desc1());
                
                if (i != -1)
                {
                    int j = _services.at(i).exist(incoming->p_board(), incoming->p_devname());
                    if (j == -1)
                    {
                        alert_os("LAYOUT:adding new device %s:%s for service %s", incoming->p_board(), incoming->p_devname(), incoming->p_desc1());
                        
                        _services.at(i).devices().push_back( 
                            indoorino::lyt::LayoutKey(
                                incoming->p_board(), incoming->p_devname(), 
                                incoming->p_label1(), incoming->p_label2(),
                                incoming->p_label3(), incoming->p_type()
                            )
                        );
                        
                        
                        // Update System
                        int k = System.is_service(incoming->p_desc1());
                        if (k != -1)
                        {
                            System.services[k]->read_layout();
                        }
                        System.boards.read_layout();
                    }
                }
                else
                {
                    warning_os("LAYOUT:parse: no service <%s> for <%s:%s>", incoming->p_desc1(), incoming->p_label1(), incoming->p_label2());
                }
                
            }
            
            else if (incoming->command() == IBACOM_LYT_SERVICE)
            {
                /* Parsing p_command()
                 * 
                 * ADD
                 * CLEAR
                 * REM
                 * 
                 */
                
                if (strcmp(incoming->p_command(), "ADD") == 0)
                {
                    
                    int index = is_service(incoming->p_desc1());
                    if (index == -1)
                    {
                        for (auto & t : indoorino::lyt::LAYOUT_SERVICES_TYPE)
                        {   // check if it is a valid type
                            if (strcmp(incoming->p_desc2(), t) == 0)
                            {
                                // WARNING: Layout layer has services init(type, name) but packets and all other dependecies has services init(name, type)
                                _services.push_back(indoorino::lyt::Service(incoming->p_desc2(), incoming->p_desc1(), incoming->p_label1(), incoming->p_label2(), incoming->p_label3()));

                                // Update System
                                System.services.read_layout();
                                return;
                            }
                        }
                        warning_os("LAYOUT:PARSE:ADD-SERVICE: invalid type <%s>", incoming->p_desc2());
                    }
                    warning_os("LAYOUT:PARSE:ADD-SERVICE: <%s> already exist", incoming->p_desc1());
                    
                }
                else if (strcmp(incoming->p_command(), "CLEAR") == 0)
                {
                    int index = is_service(incoming->p_desc1());
                    if (index != -1)
                    {
                        _services.at(index).devices().clear();
                        // Update System
                        System.services.read_layout();
                    }
                    else warning_os("LAYOUT:PARSE:CLEAR-SERVICE: invalid service name <%s>", incoming->p_desc1());
                }
                else if (strcmp(incoming->p_command(), "REM") == 0)
                {
                    int index=is_service(incoming->p_desc1());
                    if (index != -1)
                    {
                        alert_os("LAYOUT:parse: removing service <%s:%s> - <%s>", incoming->p_label1(), incoming->p_label2(), incoming->p_desc1());
                        _services.erase(_services.begin() + index);
                        // Update System
                        System.services.read_layout();
                    }
                    else warning_os("LAYOUT:PARSE:CLEAR-SERVICE: invalid service name <%s>", incoming->p_desc1());
                }
            }
            
        }
        
        void    Layout::send2client         (void)
        {
            _home_map.send2client();
            
            for (auto &s : _services)
            {
                packet::ipacket p(IBACOM_LYT_SERVICE);
                strcpy(p.p_desc1(), s.name());
                strcpy(p.p_desc2(), s.type());
                strcpy(p.p_label1(), s.desc());
                strcpy(p.p_label2(), s.area());
                strcpy(p.p_label3(), s.location());
                
                Server.shell.broadcast(&p);
              
                for (auto &d : s.devices())
                {
                    p.init(IBACOM_LYT_DEVICE);

                    strcpy(p.p_board(),     d.boardname());
                    strcpy(p.p_devname(),   d.devname());
                    strcpy(p.p_label1(),    d.area());
                    strcpy(p.p_label2(),    d.location());
                    strcpy(p.p_label3(),    d.conf());
                    strcpy(p.p_desc1(),     s.name());
                    strcpy(p.p_type(),      d.type().label);

                    Server.shell.broadcast(&p);

                }
            }
        }
        
        void    Layout::reset               (void)
        {
            _services.clear();
            warning_os("LAYOUT:reset: restoring defaualt layout");
            for (auto &s : LAYOUT_DEFAULT_SERVICES)
            {
                if ( strlen(s.name) && strlen(s.area) && strlen(s.location) && (is_service(s.name) == -1) )
                {
                    _services.push_back(indoorino::lyt::Service(s.type, s.name, s.description, s.area, s.location));
                    debug_os("LAYOUT:Adding service %s:%s:%s", _services.back().name(), _services.back().area(), _services.back().location()); 
                    for (auto &d : s.devices)
                    {
                        if ( strlen(d.boardname) && strlen(d.devname) && (_services.back().exist(d.boardname, d.devname) == -1) )
                        {
                            debug_os("LAYOUT:Adding device %s:%s to service %s", d.boardname, d.devname, _services.back().name());
                            _services.back().devices().push_back(indoorino::lyt::LayoutKey(
                                d.boardname, d.devname, d.area, d.location, d.devconf, d.devtype
                            ));
                        }
                    }
                }
            }
        }
        
        void    Layout::show                (void)
        {
            std::cout << "\n\n\t\t*** HOME LAYOUT ***" << std::endl;
            
            std::cout << "\n\tAREAS\t\t\tLOCATIONS\n\t";
            for (uint i=0; i<LEN_LOCATIONAME; i++) std::cout << "_";
            std::cout << std::endl;
            
            for (auto &a : _home_map())
            {
                std::cout << "\n\t" << a.name();
                if (a().size())
                {
                    for (uint i=0; i<MAX_INTEGER_M_(LEN_NAME - strlen(a.name()), 0); i++) std::cout << " ";
                    std::cout << ":" << a().at(0);
                }
                
                if (a().size() > 1)
                {
                    for (uint i=1; i< a().size(); i++)
                    {
                        std::cout << "\n\t";
                        for (uint j=0; j<LEN_NAME; j++) std::cout << " ";
                        std::cout << ":" << a().at(i);                        
                    }
                }
                std::cout << std::endl << "\t";
                for (uint i=0; i<LEN_LOCATIONAME; i++) std::cout << "_";
                std::cout << std::endl;
            }

            std::cout << std::endl;
            std::cout << "\n\n\t\t*** SERVICES LAYOUT ***" << std::endl;
            
            for (auto &s : _services)
            {
                std::cout << "\t";
                for (uint i=0; i<LEN_LOCATIONAME * 2; i++) std::cout << "_";
                std::cout << "\n\n\t" << s.name();
                
                if (s.devices().size())
                {
                    for (uint i=0; i<MAX_INTEGER_M_(LEN_LOCATIONAME - strlen(s.name()), 0); i++) std::cout << " ";
                    std::cout << s.devices().at(0).boardname();
                    std::cout << ":" << s.devices().at(0).devname();
                    std::cout << " [" << s.devices().at(0).type().label;
                    std::cout << ":" << s.devices().at(0).conf();
                    std::cout << "]";
                }
                
                std::cout << "\n\ttype : " << s.type();
                
                if (s.devices().size() > 1)
                {
                    for (uint i=0; i<MAX_INTEGER_M_(LEN_LOCATIONAME - (strlen(s.type()) + 7), 0); i++) std::cout << " ";
                    std::cout << s.devices().at(0).boardname();
                    std::cout << ":" << s.devices().at(0).devname();
                    std::cout << " [" << s.devices().at(0).type().label;
                    std::cout << ":" << s.devices().at(0).conf();
                    std::cout << "]";
                }
                
                std::cout << "\n\t" << s.desc();
                
                if (s.devices().size() > 2)
                {
                    for (uint i=0; i<MAX_INTEGER_M_(LEN_LOCATIONAME - strlen(s.desc()), 0); i++) std::cout << " ";
                    std::cout << s.devices().at(1).boardname();
                    std::cout << ":" << s.devices().at(1).devname();
                    std::cout << " [" << s.devices().at(1).type().label;
                    std::cout << ":" << s.devices().at(1).conf();
                    std::cout << "]";
                }
                
                std::cout << "\n\t[" << s.area();
                std::cout << ":" << s.location();
                std::cout << "]" ;
                
                if (s.devices().size() > 3)
                {
                    for (uint i=0; i<MAX_INTEGER_M_(LEN_LOCATIONAME - (strlen(s.area()) + strlen(s.location()) + 3), 0); i++) std::cout << " ";
                    std::cout << s.devices().at(2).boardname();
                    std::cout << ":" << s.devices().at(2).devname();
                    std::cout << " [" << s.devices().at(2).type().label;
                    std::cout << ":" << s.devices().at(2).conf();
                    std::cout << "]";
                }
                
                if (s.devices().size() > 4)
                {
                    for (uint i=3; i< s.devices().size(); i++)
                    {
                        std::cout << "\n\t";
                        for (uint j=0; j<LEN_LOCATIONAME; j++) std::cout << " ";
                        std::cout << s.devices().at(i).boardname();
                        std::cout << ":" << s.devices().at(i).devname();
                        std::cout << " [" << s.devices().at(i).type().label;
                        std::cout << ":" << s.devices().at(i).conf();
                        std::cout << "]";
                        
                    }
                }
                
                std::cout << std::endl;
            }
            std::cout << "\n\t";
            for (uint i=0; i<LEN_LOCATIONAME * 2; i++) std::cout << "_";
            std::cout << "\n" << std::endl;
        }

        int     Layout::check               (void)
        {
            int r=0;

            for (auto &s : _services)
            {
                
                for (auto &e : indoorino::lyt::LAYOUT_SERVICES_TYPE)
                {
                    if (strcmp(e, s.type()) == 0)
                    {
                        goto valid_service_type;
                    }
                }
                
                warning_os("LAYOUT:check: invalid service type %s", s.type());
                r++;
                
                valid_service_type:

                
                if (_home_map.exist(s.area()) == -1)
                {
                    warning_os("LAYOUT:check: invalid service area %s", s.area());
                    r++;
                }

                if (_home_map[s.area()].exist(s.location()) == -1)
                {
                    warning_os("LAYOUT:check: invalid service location %s:%s", s.area(), s.location());
                    r++;
                }
                
                for (auto &d : s.devices())
                {
                    if (!utils::is_devname(d.boardname(), 3, LEN_NAME))
                    {
                        warning_os("LAYOUT:check: <%s> invalid board name %s", s.name(), d.boardname());
                        r++;                        
                    }
                
                    if (!utils::is_devname(d.devname(), 3, LEN_DEVNAME))
                    {
                        warning_os("LAYOUT:check: <%s> invalid device name %s:%s", s.name(), d.boardname(), d.devname());
                        r++;                        
                    }

                    if (_home_map.exist(d.area()) == -1)
                    {
                        warning_os("LAYOUT:check: %s:%s invalid device area %s", d.boardname(), d.devname(),  d.area());
                        r++;
                    }

                    if (_home_map[d.area()].exist(d.location()) == -1)
                    {
                        warning_os("LAYOUT:check: %s:%s invalid device location %s:%s", d.boardname(), d.devname(), d.area(), d.location());
                        r++;
                    }
                    
                
                    if (strcmp(s.area(), d.area()) != 0)
                    {
                        alert_os("LAYOUT:check: mismatching service/device area %s/%s", s.area(), d.area());
                        r++;
                    }

                    if (strcmp(s.location(), d.location()) != 0)
                    {
                        alert_os("LAYOUT:check: %s:%s mismatching service/device location %s/%s", d.boardname(), d.devname(), s.location(), d.location());
                        r++;
                    }

                    if (d.type().command == IBACOM_CONF_DEVSTD)
                    {
                        warning_os("LAYOUT:check: %s:%s:%s invalid device type", s.name(), d.boardname(), d.devname() );
                        r++;                        
                    }

                }
                
            }
            
            return r;
        }


    } /* namespace:layout */

} /* namespace:indoorino */
    
#endif /* INDOORINO_SERVER */







































//         bool    check_in_areas              (const char * entry)
//         {
//             for (uint8_t i=0; i<LYT_NUM_AREAS; i++)
//             {
//                 if ( strcmp(global_area[i], entry) == 0 )
//                 {
//                     return true;
//                 }
//             }
//             return false;
//         }
//         
//         bool    check_in_area_home          (const char * entry)
//         {
//             for (uint8_t i=0; i<LYT_NUM_L_HOME; i++)
//             {
//                 if ( strcmp(global_home[i], entry) == 0 )
//                 {
//                     return true;
//                 }
//             }
//             return false;
//         }
//         
//         bool    check_in_area_perimeter     (const char * entry)
//         {
//             for (uint8_t i=0; i<LYT_NUM_L_PERIMETER; i++)
//             {
//                 if ( strcmp(global_perimeter[i], entry) == 0 )
//                 {
//                     return true;
//                 }
//             }
//             return false;
//         }
//         
//         bool    check_in_devtypes           (const char * entry)
//         {
//             for (uint8_t i=0; i<LYT_NUM_DEVTYPES; i++)
//             {
//                 if ( strcmp(global_devtypes[i], entry) == 0 )
//                 {
//                     return true;
//                 }
//             }
//             return false;
//         }
// 
//         void    Layout::clear       (void)
//         {            
//             _devices.clear();
//             _lights.clear();
//             _alarms.clear();
//             _weather.clear();
//         }
//         
//         void    Layout::reset       (void)
//         {
//             alert_os("layout: resetting to default!");
//             
//             this->clear();
//             
//             for (int i=0; i<LYT_DEFAULT_NUM_DEVICES; i++)
//             {
//                 _devices.push_back(linked_dev_t(DefaultDeviceLayout[i]));
//             }
//             
//             for (int i=0; i<LYT_DEFAULT_NUM_LIGHTS; i++)
//             {
//                 _lights.push_back(dev_light_t(DefaultLightsLayout[i]));
//             }
//             
//             for (int i=0; i<LYT_DEFAULT_NUM_ALARMS; i++)
//             {
//                 _alarms.push_back(dev_alarm_t(DefaultAlarmsLayout[i]));
//             }
//             
//             for (int i=0; i<LYT_DEFAULT_DEV_WEATHER; i++)
//             {
//                 _weather.push_back(dev_weather_t(DefaultWeatherStationLayout[i]));
//             }
// 
//             std::vector<board_t>        _devices;
//             std::vector<dev_light_t>    _lights;
//             std::vector<dev_alarm_t>    _alarms;
//         }
//         
//         int     Layout::check       (void)
//         {
//             int r = 0;
//             
//             for (auto& b: _devices)
//             {
//                 if (!utils::is_devname(b.boardname, 4, LEN_NAME))
//                 {
//                     warning_os("layout:boards: invalid board name <%s>", b.boardname);
//                     r += 11;
//                 }
// 
//                 if (!utils::is_devname(b.devname, 4, LEN_DEVNAME))
//                 {
//                     warning_os("layout:boards: invalid device name <%s>", b.devname);
//                     r += 12;
//                 }
//                 
//                 
//                 if (!check_in_devtypes(b.type))
//                 {
//                     warning_os("layout:boards: invalid type <%s>", b.type);
//                     r += 13;
//                 }
// 
//                 area location
//                 
//                 if (!check_in_areas(b.area))
//                 {
//                     warning_os("layout:boards: invalid area <%s> on device %s:%s",b.area, b.boardname, b.devname);
//                     r += 14;
//                 }
//                 
//                 
//                 if ( strcmp(b.area, "home") == 0 )
//                 {
//                     
//                     if (!check_in_area_home(b.location))
//                     {
//                         warning_os("layout:boards: invalid location <%s> for area <%s> on device %s:%s",b.location, b.area, b.boardname, b.devname);
//                         r += 15;
//                     }
//                 }
//                 else if ( strcmp(b.area, "perimeter") == 0 )
//                 {
//                     if (!check_in_area_perimeter(b.location))
//                     {
//                         warning_os("layout:boards: invalid location <%s> for area <%s> on device %s:%s",b.location, b.area, b.boardname, b.devname);
//                         r += 15;
//                     }                    
//                 }
// 
//             }
//             
//             
//             for (auto& d: _alarms)
//             {
//                 uint8_t c = this->is_device(d.boardname, d.devname);
//                 
//                 if (c == -1)
//                 {
//                     warning_os("layout:alarms: invalid board/device name <%s:%s>", d.boardname, d.devname);
//                     r += 20;
//                 }
//                 for (uint i=0; i<LYT_NUM_ALARMS; i++)
//                 {
//                     if ( strcmp(d.alarmtype, global_alarms[i]) == 0) goto success_alarmtype;
//                 }
//                 warning_os("layout:alarms: <%s:%s> invalid alarm type <%s>", d.boardname, d.devname, d.alarmtype);
//                 r += 21;
//                 
//                 success_alarmtype: {}
//                                 
//                 for (uint i=0; i<LYT_NUM_ALARMS_GROUP; i++)
//                 {
//                     if ( strcmp(d.group, global_alarms_group[i]) == 0) goto success_alarmgroup;
//                 }
//                 warning_os("layout:alarms: <%s:%s> invalid alarm group <%s>", d.boardname, d.devname, d.group);
//                 r += 22;
//                 
//                 success_alarmgroup: {}
//                 
//                 
//             }
//             
//             for (auto& d: _lights)
//             {
//                 uint8_t c = this->is_device(d.boardname, d.devname);
//                 if (c == -1)
//                 {
//                     warning_os("layout:lights: invalid board/device name <%s:%s>", d.boardname, d.devname);
//                     r += 30;
//                 }                
//             }
// 
//             for (auto& d: _weather)
//             {
//                 uint8_t c = this->is_device(d.boardname, d.devname);
//                 if (c == -1)
//                 {
//                     warning_os("layout:weather: invalid board/device name <%s:%s>", d.boardname, d.devname);
//                     r += 40;
//                 }                
//             }
//             
//             if (r == 0) info_os("layout: check completed succesfully");
//             return r;
//         }
//         
//         int     Layout::is_device   (const char * bname, const char * dname)
//         {
//             for (uint i=0; i<_devices.size(); i++)
//             {
//                 if ( (strcmp(_devices[i].devname, dname) == 0) && (strcmp(_devices[i].boardname, bname) == 0) )
//                 { return i; }
//             }
//             return -1;
//         }
//         
//         int     Layout::is_light    (const char * bname, const char * dname)
//         {
//             for (uint i=0; i<_lights.size(); i++)
//             {
//                 if ( (strcmp(_lights[i].devname, dname) == 0) && (strcmp(_lights[i].boardname, bname) == 0) )
//                 { return i; }
//             }
//             return -1;
//         }
//         
//         int     Layout::is_alarm    (const char * bname, const char * dname)
//         {
//             for (uint i=0; i<_alarms.size(); i++)
//             {
//                 if ( (strcmp(_alarms[i].devname, dname) == 0) && (strcmp(_alarms[i].boardname, bname) == 0) )
//                 { return i; }
//             }
//             return -1;
//         }
//         
//         int     Layout::is_weather  (const char * bname, const char * dname)
//         {
//             for (uint i=0; i<_weather.size(); i++)
//             {
//                 if ( (strcmp(_weather[i].devname, dname) == 0) && (strcmp(_weather[i].boardname, bname) == 0) )
//                 { return i; }
//             }
//             return -1;
//         }
//         
//         void    Layout::parse       (packet::netpacket * p)
//         {
//             if (p->command() == IBACOM_LYT_CONF)
//             {
// 
//                 int  n=0;
//                 char ** c=nullptr;
//                 
//                 {
//                     if (strlen(p->p_command()) > 0)
//                     {
//                         n = 1;
//                     }
//                     
//                     char *  _chp_;
//                     char *  _cptr_;
//                     
//                     _chp_=(char *)malloc(strlen(p->p_command()) + 1);
//                     strcpy(_chp_, p->p_command());
//                     
//                     _cptr_ = strchr(_chp_, ':');
//                     while (_cptr_ != NULL)
//                     {
//                         n++;
//                         _cptr_=strchr(_cptr_ + 1,':');
//                     }
//                     
//                     char * index = _chp_;
//                     
//                     c = (char**)calloc(n, sizeof(char*));
// 
//                     for (uint8_t i=0; i<n; i++)
//                     {
//                         c[i] = (char *)calloc(LEN_NAME, sizeof(char));
//                         _cptr_ = strchr(index, ':');
// 
//                         if (_cptr_==NULL)  { strcpy(c[i], index); }
//                         else            
//                         { 
//                             strncpy(c[i], index, _cptr_ - index);
//                             index = _cptr_ + 1;
//                         }
// 
//                         debug_io("-->Chunk %u : %s",i, c[i]);
//                     }
//                     
//                     free (_chp_);
//                 }
//                 
//                 /*
//                  * GET
//                  * LOAD
//                  * SAVE
//                  * RESET
//                  * CLEAR    BOARDS
//                  *          LIGHTS
//                  *          ALARMS
//                  *          WEATHER
//                  * REM      <group>     <boardname>     <devname>
//                  * 
//                  */
//                 
//                 if (n == 1)
//                 {
//                     if (strcmp(c[0], "GET") == 0)
//                     {
//                         alert_os("layout:parse:broadcasting to clients");
//                         this->send_config();
//                     }
//                     else if (strcmp(c[0], "LOAD") == 0)
//                     {
//                         alert_os("layout:parse:loading from file");
//                         this->load();
//                         System.alarms.clear();
//                         System.alarms.load_layout();
//                     }
//                     else if (strcmp(c[0], "RESET") == 0)
//                     {
//                         alert_os("layout:parse:restoring defaualt layout");
//                         this->reset();
//                         System.alarms.clear();
//                         System.alarms.load_layout();
//                     }
//                     else if (strcmp(c[0], "SAVE") == 0)
//                     {
//                         alert_os("layout:parse:saving to file");
//                         this->save();
//                     }
//                     
//                 
//                 }
//                 else if (n == 2)
//                 {
//                     if (strcmp(c[0], "CLEAR") == 0)
//                     {
//                         if (strcmp(c[1], "DEVICES") == 0)
//                         {
//                             alert_os("layout:parse:clearing boards layout");
//                             _devices.clear();
//                         }
//                         else if (strcmp(c[1], "LIGHTS") == 0)
//                         {
//                             alert_os("layout:parse:clearing lights layout");
//                             _lights.clear();
//                         }
//                         else if (strcmp(c[1], "ALARMS") == 0)
//                         {
//                             alert_os("layout:parse:clearing lights layout");
//                             _alarms.clear();
//                             System.alarms.clear();
//                         }
//                         else if (strcmp(c[1], "WEATHER") == 0)
//                         {
//                             alert_os("layout:parse:clearing weather layout");
//                             _weather.clear();
//                         }
//                         else
//                         {
//                             warning_os("layout:parse: invalid CLEAR command %s", p->p_command());
//                         }
// 
//                     }
//                     else
//                     {
//                         warning_os("layout:parse: invalid command %s", p->p_command());
//                     }
//                     
//                 }
//                 else if ( n == 4 && strcmp(c[0], "REM") == 0 )
//                 {
//                     int index;
//                     
//                     if (strcmp(c[1], "DEVICES") == 0)
//                     {
//                         index = is_device(c[2], c[3]);
//                         if ( index == -1 )
//                         {
//                             warning_os("layout:parse:rem:board invalid board:device %s:%s", c[2], c[3]);
//                         }
//                         else
//                         {
//                             _devices.erase(_devices.begin() + index);
//                             alert_os("layout:parse:removing global device %s:%s", c[2], c[3]);
//                         }
//                     }
//                     else if (strcmp(c[1], "LIGHTS") == 0)
//                     {
//                         index = is_light(c[2], c[3]);
//                         if ( index == -1 )
//                         {
//                             warning_os("layout:parse:rem:lights invalid board:device %s:%s", c[2], c[3]);
//                         }
//                         else
//                         {
//                             alert_os("layout:parse:removing light device %s:%s", c[2], c[3]);
//                             _lights.erase(_lights.begin() + index);
//                         }
//                     }
//                     else if (strcmp(c[1], "ALARMS") == 0)
//                     {
//                         index = is_alarm(c[2], c[3]);
//                         if ( index == -1 )
//                         {
//                             warning_os("layout:parse:rem:alarms invalid board:device %s:%s", c[2], c[3]);
//                         }
//                         else
//                         {
//                             alert_os("layout:parse:removing alarm device %s:%s", c[2], c[3]);
//                             _alarms.erase(_alarms.begin() + index);
//                             System.alarms.clear();
//                             System.alarms.load_layout();
//                         }
//                     }
//                     else if (strcmp(c[1], "WEATHER") == 0)
//                     {
//                         index = is_weather(c[2], c[3]);
//                         if ( index == -1 )
//                         {
//                             warning_os("layout:parse:rem:weather invalid board:device %s:%s", c[2], c[3]);
//                         }
//                         else
//                         {
//                             alert_os("layout:parse:removing weather device %s:%s", c[2], c[3]);
//                             _weather.erase(_weather.begin() + index);
//                         }
//                     }
//                     else
//                     {
//                         warning_os("layout:parse: invalid command %s", p->p_command());
//                     } 
//                 }
//                 else
//                 {
//                     warning_os("layout:parse: invalid command %s", p->p_command());
//                 }
//                                 
//                 for (uint8_t i=0; i<n; i++)
//                 {
//                     free(c[i]);
//                 }
//                 free(c);
//                 
//             }
//             else if (p->command() == IBACOM_LYT_DEVICE)
//             {
//                 int index = is_device(p->p_board(), p->p_devname());
//             
//                 if (index == -1)
//                 {
//                     linked_dev_t d;
//                     strcpy(d.boardname, p->p_board());
//                     strcpy(d.devname,   p->p_devname());
//                     strcpy(d.type,      p->p_type());
//                     strcpy(d.area,      p->p_label1());
//                     strcpy(d.location,  p->p_label2());
//                     alert_os("layout:parse:adding new global device %s:%s", p->p_board(), p->p_devname());
// 
//                     _devices.push_back(d);
//                 }
//                 else
//                 {
//                     linked_dev_t &d = _devices.at(index);
//                     alert_os("layout:parse:editing GLOBAL device %s:%s", p->p_board(), p->p_devname());
// 
//                     strcpy(d.type,      p->p_type());
//                     strcpy(d.area,      p->p_label1());
//                     strcpy(d.location,  p->p_label2());
//                     
//                 }
//             }
//             else if (p->command() == IBACOM_LYT_LIGHTS)
//             {
//                 int index = is_light(p->p_board(), p->p_devname());
//             
//                 if (index == -1)
//                 {
//                     dev_light_t d;
//                     strcpy(d.boardname, p->p_board());
//                     strcpy(d.devname,   p->p_devname());
//                     strcpy(d.lightype,  p->p_type());
//                     strcpy(d.group,     p->p_label1());
//                     alert_os("layout:parse:adding new LIGHT device %s:%s", p->p_board(), p->p_devname());
// 
//                     _lights.push_back(d);
//                 }
//                 else
//                 {
//                     dev_light_t &d = _lights.at(index);
//                     alert_os("layout:parse:editing LIGHT device %s:%s", p->p_board(), p->p_devname());
// 
//                     strcpy(d.lightype,  p->p_type());
//                     strcpy(d.group,     p->p_label1());
//                 }
//             }
// 
//             else if (p->command() == IBACOM_LYT_ALARMS)
//             {
//                 int index = is_alarm(p->p_board(), p->p_devname());
//             
//                 if (index == -1)
//                 {
//                     dev_alarm_t d;
//                     strcpy(d.boardname, p->p_board());
//                     strcpy(d.devname,   p->p_devname());
//                     strcpy(d.alarmtype, p->p_type());
//                     strcpy(d.group,     p->p_label1());
//                     alert_os("layout:parse:adding new ALARM device %s:%s", p->p_board(), p->p_devname());
// 
//                     _alarms.push_back(d);
//                     System.alarms.load_layout();
// 
//                 }                
//                 else
//                 {
//                     dev_alarm_t &d = _alarms.at(index);
//                     alert_os("layout:parse:editing ALARM device %s:%s", p->p_board(), p->p_devname());
// 
//                     strcpy(d.alarmtype, p->p_type());
//                     strcpy(d.group,     p->p_label1());
//                 }
//             }
// 
//             else if (p->command() == IBACOM_LYT_WEATHER)
//             {
//                 int index = is_weather(p->p_board(), p->p_devname());
//             
//                 if (index == -1)
//                 {
//                     dev_weather_t d;
//                     strcpy(d.boardname, p->p_board());
//                     strcpy(d.devname,   p->p_devname());
//                     strcpy(d.group,     p->p_label1());
//                     alert_os("layout:parse:adding new WEATHER device %s:%s", p->p_board(), p->p_devname());
// 
//                     _weather.push_back(d);
//                 }                
//                 else
//                 {
//                     dev_weather_t &d = _weather.at(index);
//                     alert_os("layout:parse:editing WEATHER device %s:%s", p->p_board(), p->p_devname());
// 
//                     strcpy(d.group,     p->p_label1());
//                 }
//             }
//             
//             return;
//         }
//         
//         void    Layout::show            (void)
//         {
//             std::cout << "\tLayout devices:" << std::endl;
//             for (auto& b: _devices)
//             {
//                 printf("%s:%s (%s) @ %s:%s",b.boardname, b.devname, b.type, b.area, b.location);
//                 std::cout << std::endl;
//             }
//             std::cout << "\tLayout lights:" << std::endl;
//             for (auto& b: _lights)
//             {
//                 printf("%s:%s ==> %s:%s",b.boardname, b.devname, b.group, b.lightype);
//                 std::cout << std::endl;
//             }
//             std::cout << "\tLayout alarms:" << std::endl;
//             for (auto& b: _alarms)
//             {
//                 printf("%s:%s ==> %s:%s",b.boardname, b.devname, b.group, b.alarmtype);
//                 std::cout << std::endl;
//             }
//             std::cout << "\tLayout weather station:" << std::endl;
//             for (auto& b: _weather)
//             {
//                 printf("%s:%s ==> %s",b.boardname, b.devname, b.group);
//                 std::cout << std::endl;
//             }
//         }
//    
//         bool    Layout::save            (void)
//         {
//             const char * filepath = paths.db_layout();
//             
//             std::ofstream file;
//             file.open(filepath, std::ios::out | std::ios::binary);
//             
//             /** Checking if file is good **/
// 
//             if(!file.is_open())
//             {
//                 error_io("save-boards: could not open to <%s>", filepath);
//                 return false;
//             }
//             
//             if(!file.good())
//             {
//                 error_io("save-boards: could not write to <%s>", filepath);
//                 return false;
//             }
// 
//             iSize_t packetcount = _devices.size();
//             iSize_t size_p = _devices.size();
//             iSize_t size_l = _lights.size();
//             iSize_t size_a = _alarms.size();
//             
//             info_os("LAYOUT:save: writing System.layout to %s [%u packets]", filepath, packetcount);
// 
//             packet::ipacket p(IBACOM_LYT_DEVICE);
//             std::vector<char> buffer;
//             buffer.reserve(p.full_size());
// 
//             file.write((char *)(&packetcount), sizeof(iSize_t));
//             for (auto& d: _devices)
//             {
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.area);
//                 strcpy(p.p_label2(),    d.location);
//                 strcpy(p.p_type(),      d.type);
//                 
//                 p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
//                 file.write((char *)(buffer.data()), p.full_size());
// 
//                 if(!file.good())
//                 {
//                     error_io("LAYOUT:save: io error while writing to <%s>", filepath);
//                     file.close();
//                     return false;
//                 }                
//             }
// 
//             packetcount = _lights.size();
//             p.init(IBACOM_LYT_LIGHTS);
//             buffer.reserve(p.full_size());
//             file.write((char *)(&packetcount), sizeof(iSize_t));
//             for (auto& d: _lights)
//             {
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.group);
//                 strcpy(p.p_type(),      d.lightype);
//                 
//                 p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
//                 file.write((char *)(buffer.data()), p.full_size());
// 
//                 if(!file.good())
//                 {
//                     error_io("LAYOUT:save: io error while writing to <%s>", filepath);
//                     file.close();
//                     return false;
//                 }                
//             }
// 
//             packetcount = _alarms.size();
//             p.init(IBACOM_LYT_ALARMS);
//             buffer.reserve(p.full_size());
//             file.write((char *)(&packetcount), sizeof(iSize_t));
//             for (auto& d: _alarms)
//             {
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_type(),      d.alarmtype);
//                 strcpy(p.p_label1(),    d.group);
//                 
//                 p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
//                 file.write((char *)(buffer.data()), p.full_size());
// 
//                 if(!file.good())
//                 {
//                     error_io("LAYOUT:save: io error while writing to <%s>", filepath);
//                     file.close();
//                     return false;
//                 }                
//             }
// 
//             packetcount = _weather.size();
//             p.init(IBACOM_LYT_WEATHER);
//             buffer.reserve(p.full_size());
//             file.write((char *)(&packetcount), sizeof(iSize_t));
//             for (auto& d: _weather)
//             {
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.group);
//                 
//                 p.forge(buffer.data(), BOARD_NAME, BOARD_NAME);
//                 file.write((char *)(buffer.data()), p.full_size());
// 
//                 if(!file.good())
//                 {
//                     error_io("LAYOUT:save: io error while writing to <%s>", filepath);
//                     file.close();
//                     return false;
//                 }                
//             }
// 
//             file.close();
//             return true;
//         }
//         
//         bool    Layout::load            (void)
//         {
//             const char * filepath = paths.db_layout();
//             
//             std::ifstream file;
//             file.open(filepath, std::ios::in | std::ios::binary);
// 
//             
//             /** Checking if file is good **/
//             
//             if(!file.is_open())
//             {
//                 error_io("LAYOUT:load: could not open to <%s>", filepath);
//                 return false;
//             }
// 
//             if(!file.good())
//             {
//                 error_io("LAYOUT:load: could not read from <%s>", filepath);
//                 file.close();
//                 return false;
//             }
// 
//             iSize_t packetcount;
// 
//             std::vector<char> buffer;
//             packet::ipacket p;
// 
//             file.read((char *)&packetcount, sizeof(iSize_t));
//             
//             if (file.eof())
//             {
//                 error_io("LAYOUT:load: %s got EOF @ devices !", filepath);
//                 goto unfinished_load;                            
//             }
//             
//             info_io("LAYOUT:load: loading System.layout from %s [%u packets]", filepath, packetcount);
//                         
//             for (uint8_t i=0; i<packetcount; i++)
//             {
//                 buffer.reserve(SIZEOF_PACKET_HEADER);
//                 file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);
// 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load: %s got EOF at packet %u (header)!", filepath, i);
//                     goto unfinished_load;                            
//                 }
//                 
//                 p.parse(buffer.data(), false);
//                 if (p.command() != IBACOM_LYT_DEVICE)
//                 {
//                     warning_io("LAYOUT:load: %s got invalid packet %s!", filepath, p.label());
//                     buffer.clear();
//                     continue;
//                 }
//                 
//                 buffer.reserve(p.data_size());
//                 file.read((char *)(buffer.data()), p.data_size());
//                 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load: %s got EOF at packet %u (body)!", filepath, i);
//                     goto unfinished_load;                            
//                 }
// 
//                 memcpy(p.payload(), buffer.data(), p.data_size());
//                 
//                 if (this->is_device(p.p_board(), p.p_devname()) == -1)
//                 {
//                     _devices.push_back( linked_dev_t() );
//                     debug_os("Added device %s:%s from file!", p.p_board(), p.p_devname());
//                     
//                     strcpy(_devices.back().boardname,   p.p_board());
//                     strcpy(_devices.back().devname,     p.p_devname());
//                     strcpy(_devices.back().area,        p.p_label1());
//                     strcpy(_devices.back().location,    p.p_label2());
//                     strcpy(_devices.back().type,        p.p_type());
//                     
//                 }
//                 else
//                 {
//                     warning_os("LAYOUT:load: %s:%s already in list!", p.p_board(), p.p_devname());
//                 }
//                 
//                 buffer.clear();
//             }
//                         
//             file.read((char *)&packetcount, sizeof(iSize_t));
//             
//             if (file.eof())
//             {
//                 error_io("LAYOUT:load: %s got EOF @ alarms!", filepath);
//                 goto unfinished_load;
//             }
// 
//             p.init(IBACOM_LYT_LIGHTS);
//             
//             for (uint i=0; i<packetcount; i++)
//             {
//                 buffer.reserve(SIZEOF_PACKET_HEADER);
//                 file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);
// 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load: %s got EOF at packet %u (header)!", filepath, i);
//                     goto unfinished_load;                            
//                 }
//                 
//                 p.parse(buffer.data(), false);
//                 if (p.command() != IBACOM_LYT_LIGHTS)
//                 {
//                     warning_io("LAYOUT:load: %s got invalid packet %s!", filepath, p.label());
//                     buffer.clear();
//                     continue;
//                 }
//                 
//                 buffer.reserve(p.data_size());
//                 file.read((char *)(buffer.data()), p.data_size());
//                 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load: %s got EOF at packet %u (body)!", filepath, i);
//                     goto unfinished_load;                            
//                 }
// 
//                 memcpy(p.payload(), buffer.data(), p.data_size());
//                 int index = this->is_device(p.p_board(), p.p_devname());
//                 
//                 if (index == -1)
//                 {
//                     warning_io("LAYOUT:load: %s has invalid light:device %s:%s!", filepath, p.p_board(), p.p_devname());                    
//                 }
//                 else
//                 {
//                     if (is_light(p.p_board(), p.p_devname()) == -1 )
//                     {
//                         auto dev = _devices.at(index);
//                         
//                         _lights.push_back( dev_light_t() );
//                         
//                         strcpy(_lights.back().boardname,    p.p_board());
//                         strcpy(_lights.back().devname,      p.p_devname());
//                         strcpy(_lights.back().area,         dev.area);
//                         strcpy(_lights.back().location,     dev.location);
//                         strcpy(_lights.back().type,         dev.type);
//                         strcpy(_lights.back().group,        p.p_label1());
//                         strcpy(_lights.back().lightype,     p.p_type());
//                         
//                         debug_os("Added light relay %s:%s from file!", p.p_board(), p.p_devname());
//                     }
//                     else
//                     {
//                         warning_os("LAYOUT:load: light relay %s:%s already in list!", p.p_board(), p.p_devname());
//                     }
//                     
//                 }
//                 
//                 buffer.clear();
//             }
//             
//             file.read((char *)&packetcount, sizeof(iSize_t));
//             
//             if (file.eof())
//             {
//                 error_io("LAYOUT:load: %s got EOF @ alarms!", filepath);
//                 goto unfinished_load;
//             }
// 
//             p.init(IBACOM_LYT_ALARMS);
//             
//             for (uint i=0; i<packetcount; i++)
//             {
//                 buffer.reserve(SIZEOF_PACKET_HEADER);
//                 file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);
// 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load:alarms: %s got EOF at packet %u/%u (header)!", filepath, i, packetcount);
//                     goto unfinished_load;                            
//                 }
//                 
//                 p.parse(buffer.data(), false);
//                 if (p.command() != IBACOM_LYT_ALARMS)
//                 {
//                     warning_io("LAYOUT:load:alarms: %s got invalid packet %s!", filepath, p.label());
//                     buffer.clear();
//                     continue;
//                 }
//                 
//                 buffer.reserve(p.data_size());
//                 file.read((char *)(buffer.data()), p.data_size());
//                 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load:alarms: %s got EOF at packet %u/%u (body)!", filepath, i, packetcount);
//                     goto unfinished_load;                            
//                 }
// 
//                 memcpy(p.payload(), buffer.data(), p.data_size());
//                 int index = this->is_device(p.p_board(), p.p_devname());
//                 
//                 if (index == -1)
//                 {
//                     warning_io("LAYOUT:load:alarms: %s has invalid device %s:%s!", filepath, p.p_board(), p.p_devname());                    
//                 }
//                 else
//                 {
//                     if (is_alarm(p.p_board(), p.p_devname()) == -1 )
//                     {
//                         auto dev = _devices.at(index);
//                         
//                         _alarms.push_back( dev_alarm_t() );
//                         
//                         strcpy(_alarms.back().boardname,    p.p_board());
//                         strcpy(_alarms.back().devname,      p.p_devname());
//                         strcpy(_alarms.back().area,         dev.area);
//                         strcpy(_alarms.back().location,     dev.location);
//                         strcpy(_alarms.back().type,         dev.type);
//                         strcpy(_alarms.back().group,        p.p_label1());
//                         strcpy(_alarms.back().alarmtype,    p.p_type());
//                         
//                         debug_os("Added alarm device %s:%s from file!", p.p_board(), p.p_devname());
//                     }
//                     else
//                     {
//                         warning_os("LAYOUT:load: alarm %s:%s already in list!", p.p_board(), p.p_devname());
//                     }
//                     
//                 }
//                 
//                 buffer.clear();
//             }
//             
//             file.read((char *)&packetcount, sizeof(iSize_t));
//             
//             if (file.eof())
//             {
//                 error_io("LAYOUT:load: %s got EOF @ weather!", filepath);
//                 goto unfinished_load;
//             }
// 
//             p.init(IBACOM_LYT_WEATHER);
//             
//             for (uint i=0; i<packetcount; i++)
//             {
//                 buffer.reserve(SIZEOF_PACKET_HEADER);
//                 file.read((char *)(buffer.data()), SIZEOF_PACKET_HEADER);
// 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load:weather: %s got EOF at packet %u/%u (header)!", filepath, i, packetcount);
//                     goto unfinished_load;                            
//                 }
//                 
//                 p.parse(buffer.data(), false);
//                 if (p.command() != IBACOM_LYT_WEATHER)
//                 {
//                     warning_io("LAYOUT:load:weather: %s got invalid packet %s!", filepath, p.label());
//                     buffer.clear();
//                     continue;
//                 }
//                 
//                 buffer.reserve(p.data_size());
//                 file.read((char *)(buffer.data()), p.data_size());
//                 
//                 if (file.eof())
//                 {
//                     error_io("LAYOUT:load:weather: %s got EOF at packet %u/%u (body)!", filepath, i, packetcount);
//                     goto unfinished_load;                            
//                 }
// 
//                 memcpy(p.payload(), buffer.data(), p.data_size());
//                 int index = this->is_device(p.p_board(), p.p_devname());
//                 
//                 if (index == -1)
//                 {
//                     warning_io("LAYOUT:load:weather: %s has invalid device %s:%s!", filepath, p.p_board(), p.p_devname());                    
//                 }
//                 else
//                 {
//                     if (is_weather(p.p_board(), p.p_devname()) == -1 )
//                     {
//                         auto dev = _devices.at(index);
//                         
//                         _weather.push_back( dev_weather_t() );
//                         
//                         strcpy(_weather.back().boardname,    p.p_board());
//                         strcpy(_weather.back().devname,      p.p_devname());
//                         strcpy(_weather.back().area,         dev.area);
//                         strcpy(_weather.back().location,     dev.location);
//                         strcpy(_weather.back().type,         dev.type);
//                         strcpy(_weather.back().group,        p.p_label1());
//                         
//                         debug_os("Added weather device %s:%s from file!", p.p_board(), p.p_devname());
//                     }
//                     else
//                     {
//                         warning_os("LAYOUT:weather: weather device %s:%s already in list!", p.p_board(), p.p_devname());
//                     }
//                     
//                 }
//                 
//                 buffer.clear();
//             }
//             file.close();
//             return true;
//             
//             unfinished_load:
//                 error_io("LAYOUT:load: <%s> failed", filepath);
//                 file.close();
//                 return false;
//             
//         }
// 
//         void    Layout::send_devices    (void)
//         {
//             for (auto& d: _devices)
//             {
//                 packet::netpacket p(IBACOM_LYT_DEVICE);
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.area);
//                 strcpy(p.p_label2(),    d.location);
//                 strcpy(p.p_type(),      d.type);
//                 Server.shell.broadcast(&p);
//             }
//         }
// 
//         void    Layout::send_lights     (void)
//         {
//             for (auto& d: _lights)
//             {
//                 packet::netpacket p(IBACOM_LYT_LIGHTS);
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.group);
//                 strcpy(p.p_type(),      d.lightype);
//                 Server.shell.broadcast(&p);
//             }
//         }
// 
//         void    Layout::send_alarms     (void)
//         {
//             for (auto& d: _alarms)
//             {
//                 packet::netpacket p(IBACOM_LYT_ALARMS);
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.group);
//                 strcpy(p.p_type(),      d.alarmtype);
//                 Server.shell.broadcast(&p);
//             }
//         }
//         
//         void    Layout::send_weather    (void)
//         {
//             for (auto& d: _weather)
//             {
//                 packet::netpacket p(IBACOM_LYT_WEATHER);
//                 strcpy(p.p_devname(),   d.devname);
//                 strcpy(p.p_board(),     d.boardname);
//                 strcpy(p.p_label1(),    d.group);
//                 Server.shell.broadcast(&p);
//             }
//         }
//         
//         void    Layout::send_config     (void)
//         {
//             this->send_devices();
//             this->send_alarms();
//             this->send_lights();
//             this->send_weather();
//         }
//  
//         packet::netpacket   *   Layout::get_layout_packet   (const char * lgroup, const char * bname, const char * dname)
//         {
//             if (strcmp(lgroup, "devices") == 0)
//             {
//                 if (this->is_device(bname, dname) != -1)
//                 {
//                     linked_dev_t d = _devices.at(this->is_device(bname, dname));
//                     packet::netpacket * p = new packet::netpacket(IBACOM_LYT_DEVICE);
//                     strcpy(p->p_devname(),   d.devname);
//                     strcpy(p->p_board(),     d.boardname);
//                     strcpy(p->p_label1(),    d.area);
//                     strcpy(p->p_label2(),    d.location);
//                     strcpy(p->p_type(),      d.type);
//                     
//                     return p;
//                 }
//                 
//             }
//             else if (strcmp(lgroup, "lights") == 0)
//             {
//                 
//             }
//             else if (strcmp(lgroup, "alarms") == 0)
//             {
//                 
//             }
//             
//             error_os("FATAL:layout:get_packet: invalid board:device %s:%s", bname, dname);
//             return nullptr;
//         }
        
