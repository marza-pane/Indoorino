/*
 * system.h
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#include "indoorino-system.h"
#include "../network/server.h"

//      _________________________________________
//      |                                       |
//      |       Board Template                  |
//      |_______________________________________|

static indoorino::BoardTemplate invalid_board(nullptr);

//         strcpy(_config[0].source, BOARD_NAME);
//         strcpy(_config[1].source, BOARD_NAME);
//         strcpy(_status[0].source, BOARD_NAME);
//         strcpy(_status[1].source, BOARD_NAME);
// 
//         strcpy(_config[0].target, BOARD_NAME);
//         strcpy(_config[1].target, BOARD_NAME);
//         strcpy(_status[0].target, BOARD_NAME);
//         strcpy(_status[1].target, BOARD_NAME);

namespace indoorino
{

//      _________________________________________
//      |                                       |
//      |       Board Template                  |
//      |_______________________________________|

    BoardTemplate::BoardTemplate(packet::ipacket * p)
    {
        
        if (p)
        {
            if (p->command() == IBACOM_CONF_STD)
            {
                _is_valid=true;
                memcpy(_config[0].payload(), p->payload(), _config[0].data_size());
                alert_board("NEW BOARD:added %s to System.boards", name());
            }
            else error_board("NEW BOARD: invalid packet <%s>", p->label());
        }
        else error_board("NEW BOARD: nullptr init packet");

        if (!_is_valid) return;
        
        if ( strcmp(p->p_type(), "CONTROLLER") == 0 )
        {
            _config[1].init(IBACOM_CONF_STD);
            memcpy(_config[1].payload(), _config[0].payload(), _config[1].data_size());
            strcpy(_status[1].p_name(), p->p_name());
            strcpy(_status[0].p_name(), p->p_name());
        }
    }
    
    BoardTemplate::~BoardTemplate()
    {                
        _devices.clear();
        _rxqueue.clear();
        _txqueue.clear();
    }

    void            BoardTemplate::loop         (void)
    {
        
    }

    void            BoardTemplate::parse        (packet::netpacket * p)
    {
        if (!p) 
        {
            error_board("%s: parsing nullptr", name());
            return;
        }
        
        debug_board("%s: parsing %s", name(), p->description());
        
        /* packet is a BOARD CONFIG */
        if (packet::is_boardconf(p->command()))
        {
            if (p->command() != _config[1].command())
            {
                warning_board("<%s> changed type [%u] ==> [%u]", name(), _config[1].command(), p->command());
                _config[1].init(p->command());
            }
            if (memcmp(_config[1].payload(), p->payload(), p->data_size()) != 0)
            {
                alert_board("Updating config for <%s>", name());
                memcpy(_config[1].payload(), p->payload(), p->data_size());
            }
        }
        
        /* packet is a BOARD STATUS */
        else if (packet::is_boardstat(p->command()))
        {
            if (p->command() != _status[1].command())
            {
                warning_board("<%s> changed type [%u] ==> [%u]", name(), _status[1].command(), p->command());
                _status[1].init(p->command());
            }            
            memcpy(_status[1].payload(), p->payload(), p->data_size());
        }
        
        /* packet is a DEVICE CONFIG */
        else if (packet::is_devconf(p->command()))
        {
            if (!_devices.exist(p->p_devname()))
            {
                _devices.add(p);
                return;
            }
            _devices(p->p_devname()).parse(p);
        }
        
        /* packet is a DEVICE CONFIG */
        else if (packet::is_devstat(p->command()))
        {
            if (!_devices.exist(p->p_devname()))
            {
                warning_board("PARSE: device %s:%s does not exist!", p->p_name(), p->p_devname());
                /* How not send 100 req packets?? */
                
//                 packet::ipacket c(IBACOM_REQUEST_CONFIG);
//                 packet::netpacket p(&c, BOARD_NAME, incoming->source);
//                 Server.board.send(&p);
                
                return;
            }
            _devices(p->p_devname()).parse(p);
        }
        
        /* parsing PACKET */
        else
        {
            switch(p->command())
            {
                case (IBACOM_CONF_STD):
                {
                    if (memcmp(_config[0].payload(), p->payload(), p->data_size()) != 0)
                    {
                        debug_board("Updating project config for <%s>", name());
                        memcpy(_config[0].payload(), p->payload(), p->data_size());
                    }
                    break;
                }
                case (IBACOM_STATUS_STD):
                {
                    memcpy(_status[0].payload(), p->payload(), p->data_size());
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
        alert_board("Sending sys conf! (%u devs)", _devices.count());
        Server.shell.broadcast(&_config[0]);
        Server.shell.broadcast(&_config[1]);

        for (auto & dev: _devices._list) Server.shell.broadcast(&dev.config());
    }

    void            BoardTemplate::send_status  (void)
    {
        alert_board("Sending sys stat! (%u devs)", _devices.count());
        Server.shell.broadcast(&_status[0]);
        Server.shell.broadcast(&_status[1]);
    
        for (auto & dev: _devices._list) Server.shell.broadcast(&dev.status());

    }
    


//      _________________________________________
//      |                                       |
//      |       Board List                      |
//      |_______________________________________|

    int             BoardList::get_index        (const char * name)
    {
        for (uint i=0; i<_blist.size(); i++)
        {
            if (strcmp(_blist[i].name(), name) == 0)
            {
                return i;
            }
        }
        warning_board("LIST:get-index: invalid name <%s>", name);
        return -1;
    }

    bool            BoardList::exist            (const char * name)
    {
        for (auto& b: _blist)
        {
            if (strcmp(b.name(), name) == 0)
            {
                return true;
            }
        }
        return false;
    }

    BoardTemplate&  BoardList::operator()       (const char * name)
    {       
        for (auto& b: _blist)
        {
            if (strcmp(b.name(), name) == 0)
            {
                return b;
            }
        }
        error_board("LIST: invalid call name <%s>!", name);
        return invalid_board;
    }

    void            BoardList::add_board   (packet::netpacket * p)
    {
        _blist.push_back(BoardTemplate(p));
    }
    
    bool            BoardList::rem_board   (const char * name)
    {
        int index = this->get_index(name);
        
        if (index == -1) return false;
        
        alert_board("LIST: removing board %s: index %u/%u", name, index, _blist.size());
        _blist.erase(_blist.begin() + index);
        
        return true;        
    }

    bool            BoardList::save         (void)
    {

        iSize_t packetcount=0;
        
        for (auto& board: _blist)
        {
            packetcount += 4;
            packetcount += 2 * board.devices().count();
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
        
        info_board("save: saving System.boards to %s [%u packets]", filepath, packetcount);
        file.write((const char*)&packetcount, sizeof(iSize_t));

        for (auto& board: _blist)
        {
            
            /* 1) 4 config/status packets (first is IBACOM_CONF_STD) */
            
            data = (char *)calloc(board._config[0].full_size(), sizeof(char));
            board._config[0].forge(data, BOARD_NAME, BOARD_NAME);
            file.write(data, board._config[0].full_size());
            free(data);
            debug_os("save-boards: wrote %s", board._config[0].label());

            data = (char *)calloc(board._config[1].full_size(), sizeof(char));
            board._config[1].forge(data, BOARD_NAME, BOARD_NAME);
            file.write(data, board._config[1].full_size());
            free(data);
            debug_os("save-boards: wrote %s", board._config[1].label());

            data = (char *)calloc(board._status[0].full_size(), sizeof(char));
            board._status[0].forge(data, BOARD_NAME, BOARD_NAME);
            file.write(data, board._status[0].full_size());
            free(data);
            debug_os("save-boards: wrote %s", board._status[0].label());
            
            data = (char *)calloc(board._status[1].full_size(), sizeof(char));
            board._status[1].forge(data, BOARD_NAME, BOARD_NAME);
            file.write(data, board._status[1].full_size());
            free(data);
            debug_os("save-boards: wrote %s", board._status[1].label());

            /* 2) 2 config/status packets for each device (first is IBACOM_CONF_STD) */
            
            for (iSize_t j=0; j< board.devices().count(); j++)
            {
                auto device=board.devices()[j];

                data = (char *)calloc(device.config().full_size(), sizeof(char));
                device.config().forge(data, BOARD_NAME, BOARD_NAME);
                file.write(data, device.config().full_size());
                free(data);
                debug_os("save-boards: wrote %s", device.config().label());

                if(!file.good())
                {
                    error_board("save: error saving board:device <%s:%s> to %s", board.name(), device.name(), filepath);
                    return false;
                }
            }

            for (iSize_t j=0; j< board.devices().count(); j++)
            {
                auto device=board.devices()[j];

                data = (char *)calloc(device.status().full_size(), sizeof(char));
                device.status().forge(data, BOARD_NAME, BOARD_NAME);
                file.write(data, device.status().full_size());
                free(data);
                debug_os("save-boards: wrote %s", device.status().label());

                if(!file.good())
                {
                    error_board("save: error saving board:device <%s:%s> to %s", board.name(), device.name(), filepath);
                    return false;
                }
            }

            
            if(!file.good())
            {
                error_board("save: error saving board <%s> to %s", board.name(), filepath);
                return false;
            }

        }
        
        file.close();
        return true;
    }

    bool            BoardList::load         (void)
    {
        
        const char * filepath = paths.db_boards();
        
        std::ifstream file;
        file.open(filepath, std::ios::in | std::ios::binary);
//         std::vector<uint8_t> buffer;
        
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
                if (!this->exist(p.p_name()))
                {
                    /** Adding new board **/
                    
                    if (p.command() == IBACOM_CONF_STD)
                    {
                        this->add_board(&p);
                    }
                    else warning_board("load: could not init board <%s> with <%s>", p.p_name(), p.label());;
                }
                else
                {
                    _blist.at(get_index(p.p_name())).parse(&p);
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

    void            BoardList::show         (void)
    {
        std::cout << "<BoardList::show> TO IMPLEMENT" << std::endl;
    }
    
    
    
    
    
    
    
} /* namespace:indoorino */
