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
    IndoorinoSystem::~IndoorinoSystem()
    {
    }

    void    IndoorinoSystem::begin              (void)
    {
        boards.load();
        
        if (!layout.load() || (layout.devices().size() == 0))
        {
            warning_os("Could not load layout! resetting to default");
            layout.reset();
        }

        layout.check();
        layout.show();
        alarms.begin();

    }

    void    IndoorinoSystem::parse              (packet::netpacket * incoming)
    {
        if (incoming == nullptr || incoming->command() == 0)
        {
            error_os("SYSTEM: parsing invalid packet");
        }
        debug_os("SYSTEM: parsing packet: %s ", incoming->description());

        if (Server.board.has_client(incoming->source))
        {
            if (!boards.exist(incoming->source))
            {
                /** Adding new board **/
                
                if (incoming->command() == IBACOM_CONF_STD)
                {
                    boards.add_board(incoming);
                }
                else
                {
                    /** Requesting configuration **/
                    
                    packet::ipacket c(IBACOM_REQUEST_CONFIG);
                    packet::netpacket p(&c, BOARD_NAME, incoming->source);
                    Server.board.send(&p);
                    return;
                }
            }

            /** Parse incoming packets to board **/
            
            boards(incoming->source).parse(incoming);
        }
        
        else if (Server.shell.has_client(incoming->source))
        {
            // parse from shell
            if (incoming->command() == IBACOM_SYS_REQ)
            {
                parse_request(incoming->p_command());
            }
        }


        layout.parse(incoming);
        alarms.parse(incoming);
        
    }

    void    IndoorinoSystem::loop            (void)
    {
        for (iSize_t i=0; i<boards.size(); i++)
        {
            boards[i].loop();
        }
    }

    bool    IndoorinoSystem::save_state      (void)
    {
        bool flag=true;
        
        if (!boards.save()) flag=false;
        if (!layout.save()) flag=false;
    
        return flag;
    }

    bool    IndoorinoSystem::load_state      (void)
    {
        bool flag=true;
        
        if (!boards.load()) flag=false;
        if (!layout.load()) flag=false;
    
        return flag;
    }

void        IndoorinoSystem::parse_request   (const char * command)
{
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
        
        GET:ALL 
        
        BOARDS: GET
                SAVE
                LOAD
                CLEAR
                REM:<boardname>:<devname>
        
        LAYOUT: GET
                SAVE
                LOAD
                CLEAR
                DEVICE: REM:<boardname>:<devname>
                        ADD:<boardname>:<devname>
        
    */
    
    if (n < 2)
    {
        warning_os("Incomplete sys:command <%s>", command);
        return;
    }
    
    if (strcmp(c[0], "BOARDS") == 0)
    {
        if (strcmp(c[1], "GET") == 0)
        {
            warning_os("Parsing sys: getting boards");
            for (auto & b : boards._blist)
            {
                b.send_config();
                b.send_status();
            }
            boards.show();
        }
        else if (strcmp(c[1], "SAVE") == 0)
        {
            boards.save();
        }
        else if (strcmp(c[1], "LOAD") == 0)
        {
            boards.load();
        }
        else if (strcmp(c[1], "CLEAR") == 0)
        {
            boards.clear();
        }
        else if (strcmp(c[1], "REM") == 0)
        {
            if (n < 3)
            {
                warning_os("Incomplete sys:command <%s> missing arg:<boardname>", command);
                return;
            }
            boards.rem_board(c[2]);
        }
        else
        {
            warning_os("Invalid board sys:command <%s>", command);
            return;            
        }
    }    
    else if ( (strcmp(c[0], "GET") == 0) && (strcmp(c[1], "ALL") == 0) )
    {
        warning_os("Parsing sys: getting ALL");
        for (auto & b : boards._blist)
        {
            b.send_config();
            b.send_status();
        }
        layout.send_config();
        alarms.send_status();
    }
    
    for (uint8_t i=0; i<=n; i++)
    {
        free(c[i]);
    }
    free(c);
}
    
    ////// FROM EHEHEHEHEEERRRRE!!!! adesso implementi le boards database! TODO TODO
    

} /* namespace:indoorino */
















