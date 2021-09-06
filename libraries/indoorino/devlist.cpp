/*
 * devices-list.cpp
 *
 *  Created 3.0.7 on: Aug 4, 2021
 *      Author: n00b
 */

/*
 * STATUS:
 * 0 = no errors
 * 1 = invalid pin
 * 2 = offline
 * 3 = io error
 * 4 = type error
 */


#if defined (INDOORINO_SERVER)

#include "../common/indoorino.h"
#include "devices.h"
#include "indoorino-system.h"

namespace indoorino
{

    namespace devs
    {

    //      _________________________________________
    //      |                                       |
    //      |       Board Holder List               |
    //      |_______________________________________|
    //


        void                Boards::clear                   (void)
        {
            for (auto b: _boards)
            {
                if (b != nullptr) delete b;
            }
            _boards.clear();
        }

        int                 Boards::exist                   (const char * name)
        {
            utils::clean_vector<BoardTemplate>(_boards);
            int i=0;
            for (auto b: _boards)
            {
                if (strcmp(b->name(), name) == 0)
                    return i;
                i++;
            }
            return -1;
        }

        BoardTemplate   *   Boards::operator()              (const char * name)
        {

            int i = this->exist(name);
            
            if (i == -1)
            {
                error_board("LIST: invalid call name <%s>!", name);
                return nullptr;            
            }
            
            return _boards.at(i);
        }

        bool                Boards::add                     (packet::ipacket * p)
        {
            utils::clean_vector<BoardTemplate>(_boards);
            
            if (p->command() == IBACOM_CONF_STD || p->command() == IBACOM_BOARD_BOOT)
            {
                // p->p_name() is shared between two types of packet
                
                if (utils::is_devname(p->p_name(), 3, LEN_NAME))
                {
                    if (this->exist(p->p_name()) == -1)
                    {
                        _boards.push_back(new BoardTemplate(p->p_name()));
                        return true;
                    }
                    else error_board("ADD: <%s> already exist", p->p_name());
                }
                else error_board("ADD: invalid name <%s>", p->p_name());
            }
            else warning_board("ADD: invalid packet <%s>", p->label());

            return false;
        }
        
        bool                Boards::remove                  (const char * name)
        {
            int index = this->exist(name);
            
            if (index == -1)
            {
                warning_board("LIST:rem: could not find board %s", name);
                return false;
            }
            
            alert_board("LIST: removing board %s: index %u/%u", name, index, _boards.size());

            if (_boards.at(index) != nullptr)
                delete _boards.at(index);

            _boards.erase(_boards.begin() + index);

            return true;
        }

        void                Boards::read_layout             (void)
        {
            info_board("Reading from layout...");
            utils::clean_vector<BoardTemplate>(_boards);
            
            for (auto brd : _boards)
            {
                /* if not board in layout, delete it */
                if (!System.layout.is_board(brd->name()))
                {
                    info_board("read-layout: popping board %s", brd->name());
                    this->remove(brd->name());
                    this->read_layout();
                    return;                
                }
    //             /* if board in layout, set services on */
    //             else if (!brd->_has_services)
    //             {
    //                 brd->_has_services=true;
    //             }            
            }
            
            
            for (auto& dev : System.layout.devices())
            {
                /* if not board in layout, add it */
                if (this->exist(dev.boardname()) == -1)
                {
                    _boards.push_back(new BoardTemplate(dev.boardname()));
    //                 _boards.back()->_has_services=true;
                    _boards.back()->read_layout();
                }
            }
        }
        
        bool                Boards::save                    (void)
        {
            utils::clean_vector<BoardTemplate>(_boards);

            iSize_t packetcount=0;
            
            for (auto board: _boards)
            {
                packetcount += 4;
                packetcount += 2 * board->_devices.size();
            }
            
            const char * filepath = paths.db_boards();
            char * data;
            
            std::ofstream file;
            file.open(filepath, std::ios::out | std::ios::binary);
            
            if ( !file.is_open() )
            {
                error_board("save: could not open to <%s>", filepath);
                return false;
            }
            
            if ( !file.good() )
            {
                error_board("save: could not write to <%s>", filepath);
                return false;
            }
            
            if ( packetcount == 0 )
            {
                warning_board("save: no data to save!");
                return false;
            }
                
            
            info_os("save: writing System.boards to %s [%u packets]", filepath, packetcount);
            file.write((const char*)&packetcount, sizeof(iSize_t));

            for (auto board: _boards)
            {            
                /* 1) 4 config/status packets (first is IBACOM_CONF_STD) */
                
                data = (char *)calloc(board->_config[0].full_size(), sizeof(char));
                board->_config[0].forge(data, BOARD_NAME, BOARD_NAME);
                file.write(data, board->_config[0].full_size());
                free(data);
                debug_os("save-boards: wrote %s", board->_config[0].description());

                data = (char *)calloc(board->_config[1].full_size(), sizeof(char));
                board->_config[1].forge(data, BOARD_NAME, BOARD_NAME);
                file.write(data, board->_config[1].full_size());
                free(data);
                debug_os("save-boards: wrote %s", board->_config[1].description());

                data = (char *)calloc(board->_status[0].full_size(), sizeof(char));
                board->_status[0].forge(data, BOARD_NAME, BOARD_NAME);
                file.write(data, board->_status[0].full_size());
                free(data);
                debug_os("save-boards: wrote %s", board->_status[0].description());
                
                data = (char *)calloc(board->_status[1].full_size(), sizeof(char));
                board->_status[1].forge(data, BOARD_NAME, BOARD_NAME);
                file.write(data, board->_status[1].full_size());
                free(data);
                debug_os("save-boards: wrote %s", board->_status[1].description());

                /* 2) 2 config/status packets for each device (first is IBACOM_CONF_STD) */
                
                for (auto & device : board->_devices())
                {
                    if (device == nullptr) continue;
                    data = (char *)calloc(device->config().full_size(), sizeof(char));
                    device->config().forge(data, BOARD_NAME, BOARD_NAME);
                    file.write(data, device->config().full_size());
                    free(data);
                    debug_os("save-boards: wrote %s", device->config().description());                
                    if(!file.good())
                    {
                        error_board("save: error saving board:device <%s:%s> to %s", device->boardname(), device->name(), filepath);
                        return false;
                    }
                }

                for (auto & device : board->_devices())
                {
                    if (device == nullptr) continue;
                    data = (char *)calloc(device->status().full_size(), sizeof(char));
                    device->status().forge(data, BOARD_NAME, BOARD_NAME);
                    file.write(data, device->status().full_size());
                    free(data);
                    debug_os("save-boards: wrote %s", device->status().description()); 

                    if(!file.good())
                    {
                        error_board("save: error saving board:device <%s:%s> to %s", device->boardname(), device->name(), filepath);
                        return false;
                    }
                }

                
                if(!file.good())
                {
                    error_board("save: error saving board <%s> to %s", board->name(), filepath);
                    return false;
                }

            }
            
            file.close();
            return true;
        }

        bool                Boards::load                    (void)
        {
            
            const char * filepath = paths.db_boards();
            
            std::ifstream file;
            file.open(filepath, std::ios::in | std::ios::binary);
            
            /** Checking if file is good **/
            
            if(!file.is_open())
            {
                error_board("load: could not open to <%s>", filepath);
                return false;
            }

            if(!file.good())
            {
                error_board("load: could not read to <%s>", filepath);
                file.close();
                return false;
            }

            /** Reading board number (first bytes) **/
            
            iSize_t n;
            iSize_t packetcount;
            file.read((char *)&packetcount, sizeof(iSize_t));
            
            if (packetcount == 0)
            {
                warning_board("load: <%s> has 0 packets", filepath);
                return false;            
            }
            
            packet::netpacket p;
            char head[SIZEOF_PACKET_HEADER];

            info_board("load: loading System.boards from %s [%u packets]", filepath, packetcount);
            
            /** Looping on board number **/
            for (n=0; n<packetcount; n++)
            {
                iSize_t datasize;
                char * payload;
                
                /** Reading first packet. Must be IBACOM_CONF_STD (1001) **/
                
                file.read(head, SIZEOF_PACKET_HEADER);
                p.parse(head, false);
                datasize=p.data_size();
                
                debug_os("load-boards: reading packet <%s>", p.label());
                
                if (file.eof())
                {
                    error_board("load: %s got EOF at [%u] header!", filepath, n);
                    goto unfinished_load;                            
                }
                
                if (datasize > 0)
                {
                    payload = new char [datasize];
                    file.read(payload, datasize);
                    memcpy(p.payload(), payload, datasize);
                    delete[] payload;
                }
                
                if (file.eof())
                {
                    error_board("load: %s got EOF at [%u] body!", filepath, n);
                    goto unfinished_load;                            
                }

                
                if ( (p.command() >= IBACOM_CONF_STD) && (p.command() <= IBACOM_STATUS_DEVSTD) )
                {
                    int index = this->exist(p.p_name());
                    if (index == -1)
                    {
                        /** Adding new board **/
                        if (!this->add(&p)) warning_board("load: could not init board <%s> with <%s>", p.p_name(), p.label());;
                    }
                    else
                    {
                        _boards.at(index)->parse(&p);
                    }
                }
                else warning_board("load: invalid board packet <%s>", p.label());;
        
            }
            
            
            unfinished_load:
            {
            }
            
            file.close();
            if (n == packetcount)
            {
                info_board("load: Successfully read %u packets!", n);
                return true;
            }
            else
            {
                warning_board("load: Incomplete read - %u/%u packets", n, packetcount);
                return false;
            }
        }

        void                Boards::show                    (void)
        {
            std::cout << "\n\t\t *** Showing BOARDS: ***\n" << std::endl;
            
            for (auto b : _boards)
            {
                for (uint i=0; i<LEN_LOCATIONAME; i++) std::cout << "_";
                std::cout << std::endl;
                b->show();
            }
        }
    

        
    //      _________________________________________
    //      |                                       |
    //      |       Device Holder List              |
    //      |_______________________________________|
    //
        
        Devices::Devices()
        {

        }
        
        Devices::~Devices()
        {
            clear();
        }
        
        void                        Devices::clear          (void) 
        {
            for (auto p: _devs)
            {
                if (p != nullptr) delete p;
            }
            _devs.clear();
        }
        
        int                         Devices::exist          (const char *key)
        {
            utils::clean_vector<DeviceTemplate>(_devs);
            int index = 0;
            for (auto p: _devs)
            {
                if (strcmp(p->name(), key) == 0) return index;
                index++;
            }
            return -1;
        }

        DeviceTemplate  *           Devices::operator[]     (const int index)
        {
            iSize_t i=0;
            if (index < 0)  { i = _devs.size() + index; }
            else            { i = index; }
            
            if (i >= _devs.size())
            {
                error_os("FATAL:devices: invalid call for index %d", index);
                return nullptr;
            }
            
            return _devs[i];
        }
        
        DeviceTemplate  *           Devices::operator()     (const char *key)
        {
            int i=this->exist(key);

            if (i == -1)
            {
                error_os("FATAL:devices: invalid call for %s", key);
                return nullptr;
            }
            return _devs[i];
        }

        bool                        Devices::remove         (const char *key)
        {
            utils::clean_vector<DeviceTemplate>(_devs);
            int i=this->exist(key);
            if (i == -1)
            {
                warning_dev("list:rem: can not find %s", key);
                return false;
            }
            if (_devs.at(i) != nullptr)
                delete _devs.at(i);
            _devs.erase(_devs.begin() + i);
            info_dev("list:rem: popped %s", key);

            return true;
        }
        
        bool                        Devices::add            (const char * boardname, const char * devname, iCom_t com, iPin_t pin)
        {
            utils::clean_vector<DeviceTemplate>(_devs);

            if (this->exist(devname) != -1)
            {
                warning_dev("LIST:add: %s:%s already exist!", boardname, devname);
                return false;
            }
            
            _devs.push_back(new DeviceTemplate(boardname, devname, com, pin));
                        
            debug_board("ADD:init %s <%s:%s> on pin %u", lyt::devicetype_Com2Str(com), boardname, devname, pin);
            return true;
        }

        bool                        Devices::add            (packet::ipacket * p)
        {

            iCom_t com = p->command();
            
            if (!packet::is_devconf(com))
            {
                warning_dev("ADD: unknown device %u <%s>", com, p->label());
                return false;
            }
                    
            this->add(p->p_name(), p->p_devname(), p->command(), *p->p_pin1());
            _devs[-1]->parse(p);
            
            alert_board("Now listing devices ###");
            for (auto d: _devs) { std::cout << d->boardname() << ":" << d->name() << std::endl; }
            std::cout << "###\n";
        
            return true;
            
        }

    } /* namespace:devs */

} /* namespace : indoorino */

#endif /* INDOORINO_NETWORK */
