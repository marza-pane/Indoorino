/*
 * indoorino-system.cpp
 *
 *  Created on: Jan 24, 2021
 *      Author: n00b
 */

#include "indoorino-system.h"
#include "../network/server.h"

namespace indoorino 
{
    IndoorinoSystem::IndoorinoSystem()
    {
    }
    
    IndoorinoSystem::~IndoorinoSystem()
    {
        on_exit();
    }

    void    IndoorinoSystem::on_exit            (void)
    {
        for (auto &s: services())
        {
            s->stop();
        }
    }
    
    void    IndoorinoSystem::begin              (void)
    {
        
        layout.home_map().reset(); 
        
        if (!layout.load() || (layout.services().size() == 0))
        {
            warning_os("Could not load layout! resetting to default");
            layout.reset();
        }
        layout.reset();

        layout.check();
//         layout.show();
        
        boards.begin();
        boards.load();
//         boards.show();
        
        services.begin();
//         growroom.begin();
    }
    
    bool    IndoorinoSystem::save_state      (void)
    {
        bool flag=true;
        
        if (!boards.save())     flag=false;
        if (!layout.save())     flag=false;
        if (!services.save())   flag=false;
    
        return flag;
    }

    
    bool    IndoorinoSystem::load_state      (void)
    {
        bool flag=true;
        
        if (!boards.load())     flag=false;
        if (!layout.load())     flag=false;
        if (!services.load())   flag=false;
    
        return flag;
    }    

    bool    IndoorinoSystem::is_device      (const char *bname, const char *dname)
    {
        if (boards.exist(bname) != -1)
        {
            if (boards(bname)->devices().exist(dname) != -1) return true;
        }
        return false;
    }

    devs::DeviceTemplate *  IndoorinoSystem::get_device         (const char *bname, const char *dname)
    {
        if (boards.exist(bname) == -1) return nullptr;
        
        return boards(bname)->devices()(dname);
        
    }

    void    IndoorinoSystem::parse          (packet::netpacket * incoming)
    {
        if (incoming == nullptr || incoming->command() == 0)
        {
            error_os("SYSTEM: parsing invalid packet");
            return;
        }
        debug_os("SYSTEM: parsing packet: %s ", incoming->description());
        
            /** Parse incoming packets to board **/


        
        /** Parse incoming packets FROM board **/
        
        if (Server.board.has_client(incoming->source))
        {
            if (boards.exist(incoming->source) == - 1)
            {
                /** Adding new board **/
                
                boards.add(incoming); // try to add board via packet
            }

            /** Feeding incoming packets to virtual boards **/
            
            boards(incoming->source)->parse(incoming);
        }
        
        /** Parse incoming packets FROM shell **/
        
        else if (Server.shell.has_client(incoming->source))
        {
                    
            /** Parse system request  **/
            if (incoming->command() == IBACOM_SYS_REQ)
            {
                parse_request(incoming);
            }
            
            /** Parse layout configuration **/
            if (incoming->command() >= IBACOM_LYT_CONF && incoming->command() <= IBACOM_LYT_SERVICE)
            {
                layout.parse(incoming);
            }
        }

        /** Services parse all packets **/
        for (auto s : services())
        {
            s->parse(incoming);
        }


    }


    void        IndoorinoSystem::parse_request   (packet::netpacket * incoming)
    {
        
        const char * command = incoming->p_command();
        
        if (strlen(command) == 0)
        {
            warning_os("parse:sys:request: empty command!");
            return;
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
        
        warning_os("Parsing sys:command <%s> (%d chunks)", command, n);
        if (!c)
        {
            error_os("Parsing sys: buffer returned nullptr!");
            return;
        }
        
        if (n == 0) return;

        /*  Command must be one of the following strings
            
            GET:    ALL
                    PACKET (value1 = packet command)
            
            BOARDS: GET
                    SAVE
                    LOAD
                    CLEAR
                    REM:<boardname>:<devname>
            
            LAYOUT: is managed in layput.cpp by IBACOM_LYT_CONF
            
            PROBE:  <boardname>:<devname>:<desc> (value1 = first epoch, value2 = last epoch)

        */
        
        if (n < 2)
        {
            warning_os("Incomplete sys:command <%s>", command);
            return;
        }
        
        /*** BOARDS ***/
        
        if ( strcmp(c[0], "BOARDS") == 0 )
        {
            if (strcmp(c[1], "GET") == 0)
            {
                warning_os("Parsing sys: getting boards");
                for (auto & b : boards())
                {
                    b->send_config();
                    b->send_status();
                }
                boards.show();
            }
            else if (strcmp(c[1], "SAVE") == 0)
            {
                info_os("sys:command: Saving board list!");
                boards.save();
            }
            else if (strcmp(c[1], "LOAD") == 0)
            {
                info_os("sys:command: Loading board list!");
                boards.load();
            }
            else if (strcmp(c[1], "CLEAR") == 0)
            {
                info_os("sys:command: Clearing board list!");
                boards.clear();
            }
            else if (strcmp(c[1], "REM") == 0)
            {
                if (n < 3)
                {
                    warning_os("Incomplete sys:command <%s> missing arg:<boardname>", command);
                    return;
                }
                info_os("sys:command: Removing board %s!", c[2]);
                boards.remove(c[2]);
                return; // remove
            }
            else
            {
                warning_os("Invalid board sys:command <%s>", command);
                return;
            }
        }    

        /*** ALARMS ***/
        
//         if ( strcmp(c[0], "ALARMS") == 0 )
//         {
//             if (strcmp(c[1], "GET") == 0)
//             {
//                 info_os("Parsing sys: getting alarms");
//                 alarms.send_status();
//             }
//             else if (strcmp(c[1], "UPDATE") == 0)
//             {
//                 info_os("sys:command: updating alarms!");
//                 alarms.load_layout();
//             }
//         }
//         
//         
//         if ( strcmp(c[0], "PROBE") == 0)
//         {
//             database.query();
//         }
        
        else if ( strcmp(c[0], "GET") == 0 )
        {
            
            if ( strcmp(c[1], "ALL") == 0)
            {
                warning_os("Parsing sys: getting ALL");
                layout.send2client();
                for (auto & b : boards())
                {
                    b->send_config();
                    b->send_status();
                }
                for (auto & s : services())
                {
                    s->send_status();
                }
            }
            else if ( strcmp(c[1], "PACKET") == 0)
            {
                int32_t pcom;
                memcpy(&pcom, incoming->p_value1(), sizeof(int32_t));
                warning_os("Parsing sys: Generating test packet %d", pcom);
                if ( packet::payloadsize(pcom > 0) != -1 )
                {                
                    packet::ipacket q(pcom);
                    warning_os("Parsing sys: Generating test packet %s", q.label());                
                    std::cout << "\t*** TEST START *** " << std::endl;

                    std::cout << "Packet description " << q.description() << std::endl;
                    std::cout << "Payload size       <" << q.data_size() << ">" << std::endl;
                    std::cout << "Full size          <" << q.full_size() << ">" << std::endl;
                    
                    std::cout << "\t*** TEST END *** " << std::endl;
                    Server.shell.broadcast(&q);
                }
            }
        }
        
        for (uint8_t i=0; i<n; i++)
        {
            free(c[i]);
        }
        free(c);
    }
        
    ////// FROM EHEHEHEHEEERRRRE!!!! adesso implementi le boards database! TODO TODO
    

} /* namespace:indoorino */
















