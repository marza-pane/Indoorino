/*
 * system.h
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#include "indoorino-system.h"
//      _________________________________________
//      |                                       |
//      |       Board Template                  |
//      |_______________________________________|

static indoorino::BoardTemplate invalid_board("");


namespace indoorino
{

//      _________________________________________
//      |                                       |
//      |       Board Template                  |
//      |_______________________________________|

    BoardTemplate::BoardTemplate(const char * boardname)
    {
        if (utils::is_devname(boardname))
        {
            strcpy(_config[0].p_name(), boardname);
            _is_valid=true;
            return;
        }
        strcpy(_config[0].p_name(), "UNKNOWN");
        alert_board("*** NEW BOARD: %s ***", name());

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
        if (p) { debug_board("Board <%s> parsing %s", name(), p->description()); }
        
        /* PACKET is a board CONFIG */
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
        
        /* PACKET is a board STATUS */
        else if (packet::is_boardstat(p->command()))
        {
            if (p->command() != _status[1].command())
            {
                warning_board("<%s> changed type [%u] ==> [%u]", name(), _status[1].command(), p->command());
                _status[1].init(p->command());
            }            
            memcpy(_status[1].payload(), p->payload(), p->data_size());
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

//      _________________________________________
//      |                                       |
//      |       Board List                      |
//      |_______________________________________|

    bool            BoardList::exist            (const char *name)
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
        error_os("FATAL:boardlist:invalid call name <%s>!", name);
        return invalid_board;
    }

    void            BoardList::add_board   (packet::netpacket * p)
    {
        _blist.push_back(BoardTemplate(p->p_name()));
        _blist.back().parse(p);
    }
    
    bool            BoardList::rem_board   (const char *)
    {
        return false;
    }

    bool            BoardList::save         (void)
    {
        const char * filepath = paths.db_boards();
        
        std::ofstream file;
        file.open(filepath, std::ios::out | std::ios::binary);
        
        char * data;
        

        for (auto& board: _blist)
        {
            
            /* 1) 4 config/status packets (first is IBACOM_CONF_STD) */
            
            data = (char *)calloc(board._config[0].full_size(), sizeof(char));
            board._config[0].header(data);
            memcpy(data + SIZEOF_PACKET_HEADER, board._config[0].payload(), board._config[0].data_size());
            file.write(data, board._config[0].full_size());
            free(data);

            data = (char *)calloc(board._config[1].full_size(), sizeof(char));
            board._config[1].header(data);
            memcpy(data + SIZEOF_PACKET_HEADER, board._config[1].payload(), board._config[1].data_size());
            file.write(data, board._config[1].full_size());
            free(data);

            data = (char *)calloc(board._status[0].full_size(), sizeof(char));
            board._status[0].header(data);
            memcpy(data + SIZEOF_PACKET_HEADER, board._status[0].payload(), board._status[0].data_size());
            file.write(data, board._status[0].full_size());
            free(data);
            
            data = (char *)calloc(board._status[1].full_size(), sizeof(char));
            board._status[1].header(data);
            memcpy(data + SIZEOF_PACKET_HEADER, board._status[1].payload(), board._status[1].data_size());
            file.write(data, board._status[1].full_size());
            free(data);

            /* 2) 2 config/status packets for each device (first is IBACOM_CONF_STD) */
            
//             for (iSize_t j=0; j< board.device.count(); j++)
//             {
//                 device=board.device[i];
//                 
//                 buffer.reserve(board.config().full_size());
//                 board.config().header(buffer.data());
//                 memcpy(buffer.data() + SIZEOF_PACKET_HEADER, board.config().payload(), board.config().data_size());
//                 file.write(data, board.config().full_size());
//                 buffer.clear();
// 
//                 buffer.reserve(board.status().full_size());
//                 board.status().header(buffer.data());
//                 memcpy(buffer.data() + SIZEOF_PACKET_HEADER, board.status().payload(), board.status().data_size());
//                 file.write(data, board.status().full_size());
//                 buffer.clear();
//                                     
//             }
        }
        
        file.close();
        return true;
    }
        
    bool            BoardList::load         (void)
    {
        
        const char * filepath = paths.db_boards();
        
        std::ofstream file;
        file.open(filepath, std::ios::in | std::ios::binary);
        std::vector<uint8_t> buffer;
        char b_head[SIZEOF_PACKET_HEADER];
//         if (file.read(b_head, SIZEOF_PACKET_HEADER))
//         {
//             
//         }
        file.close();
        return true;
    }

    
    
    
    
    
    
    
    
} /* namespace:indoorino */
