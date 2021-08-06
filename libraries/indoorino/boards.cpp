/*
 * system.h
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#include "indoorino-system.h"
#include "../network/server.h"


namespace indoorino
{


//      _________________________________________
//      |                                       |
//      |       Board List                      |
//      |_______________________________________|


    int             Boards::exist               (const char * name)
    {
        clean_nullp();
        int i=0;
        for (auto b: _boards)
        {
            if (strcmp(b->name(), name) == 0)
                return i;
            i++;
        }
        return -1;
    }

    BoardTemplate   *   Boards::operator()      (const char * name)
    {

        int i = this->exist(name);
        
        if (i == -1)
        {
            error_board("LIST: invalid call name <%s>!", name);
            return nullptr;            
        }
        
        return _boards.at(i);
    }

    bool            Boards::add                 (packet::ipacket * p)
    {
        
        if (p->command() == IBACOM_CONF_STD)
        {
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
        else error_board("ADD: invalid packet <%s>", p->label());

        return false;
    }
    
    bool            Boards::remove              (const char * name)
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

    void            Boards::update              (void)
    {
        info_board("Reading from layout...");
        
        for (auto brd : _boards)
        {
            /* if not board in layout, delete it */
            if (!System.layout.is_board(brd->name()))
            {
                info_board("read-layout: popping board %s", brd->name());
                this->remove(brd->name());
                this->update();
                return;                
            }
            /* if board in layout, set services on */
            else if (!brd->_has_services)
            {
                brd->_has_services=true;
            }            
        }
        
        
        for (auto& dev : System.layout.devices())
        {
            /* if not board in layout, add it */
            if (this->exist(dev.boardname()) == -1)
            {
                _boards.push_back(new BoardTemplate(dev.boardname()));
                _boards.back()->_has_services=true;
                _boards.back()->sysupdate();
            }
        }
    }
    
    void            Boards::clean_nullp         (void)
    {
        iSize_t i=0;
        for (auto board: _boards)
        {
            if (board == nullptr)
            {
                error_board("LIST: found nullptr!");
                _boards.erase(_boards.begin() + i);
                clean_nullp();
                return;
            }
            i++;
        }
    }

    bool            Boards::save                (void)
    {
        clean_nullp();

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
        
        if(!file.is_open())
        {
            error_board("save: could not open to <%s>", filepath);
            return false;
        }
        
        if(!file.good())
        {
            error_board("save: could not write to <%s>", filepath);
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

    bool            Boards::load                (void)
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

    void            Boards::show                (void)
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
//      |       Board Template                  |
//      |_______________________________________|


    BoardTemplate::BoardTemplate(const char * bname)
    {
        _config[0].init(IBACOM_CONF_STD);
        _config[1].init(IBACOM_CONF_STD);
        _status[0].init(IBACOM_STATUS_STD);
        _status[1].init(IBACOM_STATUS_STD);
                
        strcpy(_config[0].p_name(), bname);
        strcpy(_config[1].p_name(), bname);
        strcpy(_status[0].p_name(), bname);
        strcpy(_status[1].p_name(), bname);
        alert_board("NEW BOARD: added %s to System.boards", this->name());
    }
    
    BoardTemplate::~BoardTemplate()
    {                
        _devices.clear();
        _rxqueue.clear();
        _txqueue.clear();
    }

    void            BoardTemplate::loop         (void)
    {
        for (auto d: _devices())
        {
            d->loop();
        }
    }

    void            BoardTemplate::parse        (packet::netpacket * incoming)
    {
        if (!incoming) 
        {
            error_board("%s: parsing nullptr", name());
            return;
        }
        
        debug_board("%s: parsing %s", name(), incoming->description());
        
        /* packet is a BOARD CONFIG */
        if (packet::is_boardconf(incoming->command()))
        {
            if (incoming->command() != _config[1].command())
            {
                warning_board("<%s> changed type [%u] ==> [%u]", name(), _config[1].command(), incoming->command());
                _config[1].init(incoming->command());
            }
            if (memcmp(_config[1].payload(), incoming->payload(), incoming->data_size()) != 0)
            {
                alert_board("Updating config for <%s>", name());
                memcpy(_config[1].payload(), incoming->payload(), incoming->data_size());
            }
        }
        
        /* packet is a BOARD STATUS */
        else if (packet::is_boardstat(incoming->command()))
        {
            if (incoming->command() != _status[1].command())
            {
                warning_board("<%s> changed type [%u] ==> [%u]", name(), _status[1].command(), incoming->command());
                _status[1].init(incoming->command());
            }            
            memcpy(_status[1].payload(), incoming->payload(), incoming->data_size());
        }
        
        /* packet is a DEVICE CONFIG */
        else if (packet::is_devconf(incoming->command()))
        {
            if (_devices.exist(incoming->p_devname()) == -1)
            {
                _devices.add(incoming);
                return;
            }
            _devices(incoming->p_devname())->parse(incoming);
        }
        
        /* packet is a DEVICE CONFIG */
        else if (packet::is_devstat(incoming->command()))
        {
            if (_devices.exist(incoming->p_devname()) == -1)
            {
                warning_board("PARSE:set-status: device %s:%s does not exist!", incoming->p_name(), incoming->p_devname());
                /* How not send 100 req packets?? */                
                return;
            }
            _devices(incoming->p_devname())->parse(incoming);
        }
        
        /* parsing PACKET */
        else
        {
            switch(incoming->command())
            {
                case (IBACOM_CONF_STD):
                {
                    if (memcmp(_config[0].payload(), incoming->payload(), incoming->data_size()) != 0)
                    {
                        warning_board("Updating config for <%s>", name());
                        incoming->dump();
                        memcpy(_config[0].payload(), incoming->payload(), incoming->data_size());
                    }
                    break;
                }
                case (IBACOM_STATUS_STD):
                {
                    memcpy(_status[0].payload(), incoming->payload(), incoming->data_size());
                    break;
                }
                default:
                {
                    
                }
            }
        }        
    }
    
    void            BoardTemplate::send_config  (void)
    {
        alert_board("Sending sys conf! (%u devs)", _devices.size());
        Server.shell.broadcast(&_config[0]);
        Server.shell.broadcast(&_config[1]);

        for (auto d: _devices()) Server.shell.broadcast(&d->config());
    }

    void            BoardTemplate::send_status  (void)
    {
        alert_board("Sending sys stat! (%u devs)", _devices.size());
        Server.shell.broadcast(&_status[0]);
        Server.shell.broadcast(&_status[1]);
    
        for (auto d: _devices()) Server.shell.broadcast(&d->status());

    }
    
    void            BoardTemplate::sysupdate    (void)
    {
        info_board("Reading devices...");
        
        int i=0;
        
        for (auto & d : _devices())
        {
            /* if not board in layout, delete it */
            if (!System.layout.is_device(name(), d->name()))
            {
                _devices._devs.erase(_devices._devs.begin() + i);
                info_board("read-layout: popping device %s:%s", name(), d->name());
            }
            /* if board in layout, set services on */
            else if (d->_services.size())
            {
                // Loo through System.layout for matching services : d->add_service();
                // oppure togli sta merda e fai un link decente tra devs e services
            }
            i++;
        }
        
        for (auto& service : System.layout.services())
        {
            for (auto& d : service.devices())
            {
                if ( (strcmp(name(), d.boardname()) == 0) && (_devices.exist(d.devname()) == -1) )
                {
                    if (!_devices.add(d.boardname(), d.devname(), d.type().command)) continue; // add something
                }
            }
        }
    }
    
    void            BoardTemplate::show         (void)
    {
        
        std::cout << "\nBoard           : [" << name() << "]" << std::endl;
        std::cout << "\tenabled : " << _enabled << std::endl;
        std::cout << "\tonline  : " << _is_online << std::endl;
        std::cout << "\tservice : " << _has_services<< std::endl;
        std::cout << "\tcurrent lag : " << _lag.count() << " [sec.]" << std::endl;
        
        std::cout << "\n\t\tBOARD CONFIG: " << std::endl;
        _config[0].dump();
        _config[1].dump();
        std::cout << "\n\t\tBOARD STATUS: " << std::endl;
        _status[0].dump();
        _status[1].dump();
        std::cout << std::endl;
        
        for (auto d : _devices())
        {
            std::cout << "\t";
            for (uint i=0; i<LEN_NAME; i++) std::cout << "_";
            std::cout << std::endl;
            d->show();    
        }
    }
} /* namespace:indoorino */
