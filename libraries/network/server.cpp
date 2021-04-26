/*
 * network/server.cpp
 *
 *  Created on: Jan 04, 2021
 *      Author: n00b
 * 
 */

#if defined(INDOORINO_SERVER)
#include "../network/server.h"
#include "../indoorino/indoorino-system.h"

static bool net_thread_flag=true;

void on_exit         (int exitcode)
{
//     net_thread_flag = false;

    printf("\n");
    fflush(stdout);
    Server.stop();
    // System.stop(); oppure System.save()

    printf("\n%s\t exitcode [%d] on process [%d] - See you in space cowboy\n\n", debug::TRS, exitcode, getpid());

    (exitcode != 0) ? (std::exit(EXIT_FAILURE)) : (std::exit(EXIT_SUCCESS));
}


IndoorinoServer::IndoorinoServer():
    board(_rxqueue, DEFAULT_BOARD_PORT),
    shell(_rxqueue, DEFAULT_SHELL_PORT)
{
    
}

IndoorinoServer::~IndoorinoServer()
{
    stop();
}

void        IndoorinoServer::begin           (void)
{
   
    _thr_netcheck=std::thread(
    [this]
    {
        static bool isOnline=false;
        while (net_thread_flag)
        {
            std::cout << ".";
            if (net::netstatus() != isOnline)
            {
                if (net::netstatus())
                {
                    info_net("Connected to network! Starting Server");
                    isOnline=true;
                    board.begin();
                    shell.begin();
                }
                else
                {
                    warning_net("Network is down! Server will start on reconnection");
                    isOnline=false;
                    board.stop();
                    shell.stop();   
                    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_BIND_CONNECT));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(TIME_NETWORK_CHECK));
        }
        warning_net("netstaus thread terminated!");
        return;
    });

    
}

void        IndoorinoServer::loop            (void)
{        
    if (!net::netstatus())  { return; }
    
    board.loop();
    shell.loop();

    _rxqueue.wait();

    while (!_rxqueue.is_empty())
    {
        packet::netpacket * p = _rxqueue.pop_front();
        on_packet(p);
//         delete p;
    }

}    

void        IndoorinoServer::stop           (void)
{
    alert_server("halting server...");
    
    if (net_thread_flag)
    {
        net_thread_flag = false;
        board.stop();
        shell.stop();
        if (_thr_netcheck.joinable()) { _thr_netcheck.join(); }
    }
    
}


void        IndoorinoServer::on_packet              (packet::netpacket * incoming)
{   
            
    info_server("new packet from <%s> : %s ",incoming->source, incoming->description());
    incoming->dump();
    
    /** Server Logic **/
    
    if (board.has_client(incoming->source))
    {
        /** Packets from boards **/
        
        if (!System.boards.exist(incoming->source))
        {
            /** Adding new board **/
            
            if (incoming->command() == IBACOM_CONF_STD)
            {
                System.boards.add_board(incoming);
            }
            else
            {
                /** Requesting configuration **/
                
                packet::ipacket c(IBACOM_REQUEST_CONFIG);
                packet::netpacket p(&c, BOARD_NAME, incoming->source);
                Server.board.send(&p);
            }
        }
        else
        {
            /** Parse incoming packets to board **/
            
            System.boards(incoming->source).parse(incoming);
        }
        
//         if (incoming->command() != IBACOM_CONF_DHT22)
        shell.broadcast(incoming);        
    }

    if (board.has_client(incoming->target))
    {
        /** Packets from Shell targeting Boards **/
        
        info_net("Re-routing: %s ==> %s ==> %s", incoming->source, incoming->description(), incoming->target);
        Server.board.send(incoming);
    }
        
        
    /** Packets from Shell targeting Server **/

    if (strcmp(incoming->target, BOARD_NAME) == 0)
    {
        switch (incoming->command())
        {
            case IBACOM_REQUEST_CONFIG:
            case IBACOM_REQUEST_STATUS:
            {
                board.broadcast(incoming);
                break;
            }
            
            case IBACOM_REPORT:
            {
                incoming->eval();
                break;
            }

            case IBACOM_SRV_REQ:
            {
                parse_request(incoming->p_command());
                break;
            }
            
            default: System.parse(incoming); // this dumps packet on output
        }
        
    }
    
//     boost::asio::post(_asio_context,
//         [incoming]()
//         {
//             delete incoming;
//         });
    
}

void        IndoorinoServer::parse_request          (const char * command)
{
    if (strlen(command) == 0)
    {
        warning_os("parse:request: empty command!");
        return;
    }
    
    debug_server("Parsing command <%s>", command);
    
    int  n=0;
    char ** c;
    
    {
        char    b[SERIAL_TX_BUFFER_SIZE] {0};
        char *  t=nullptr;
        char *  p=strchr((char *)command,':');
    
        // Counting chunks
        
        while (p != NULL)
        {
            n++;
            p=strchr(p + 1,':');
        }
        
        debug_os("Found %u command chunks", n + 1);
        
        // Allocating
        strcpy(b, command);
        t = b;
        
        c = (char**)calloc(n + 1, sizeof(char*));
        
        // Assigning values
        for (uint8_t i=0; i<=n; i++)
        {
            c[i] = (char *)calloc(LEN_NAME, sizeof(char));
            p = strchr(t,':');

            if(p==NULL) { strcpy(c[i], t); }
            else        { strncpy(c[i], t, p - t); t=p + 1; }

            debug_os("Chunk %u : %s",i, c[i]);
        }
    }    
    
    /*  Command must be one of the following strings
        
        RESET:  BOARDS
                BOARDSERVER
                SHELLSERVER
                <boardname>
        
        SAVE:   ALL
                USERS
                BOARDS
                PROBES
        
        LOAD:   ALL
                USERS
                BOARDS
                PROBES
        
        CLEAR:  ALL
                BOARDS
                <boardname>
                PACKETS
                PROBES
                
        UPDATE: ALL
                BOARDS
                <boardname>

        SYNC:   ALL
                <boardname>
    */
    
    if (strcmp(c[0], "RESET") == 0)
    {
        if (strcmp(c[1], "BOARDS") == 0)
        {
            packet::ipacket p(IBACOM_RESET);
            this->board.broadcast(&p);
        }
        else if (strcmp(c[1], "BOARDSERVER") == 0)
        {}
        else if (strcmp(c[1], "SHELLSERVER") == 0)
        {}
        else
        {
            
            if (this->board.has_client(c[1]))
            {
                packet::ipacket p(IBACOM_RESET);
                packet::netpacket t(&p, BOARD_NAME, c[1]);
                this->board.send(&t);
            }
            else { warning_board("parse:command:%s: could not find board <%s>", c[0], c[1]); }
            
            /* look for c[1] in database.boards.exist */
        }            
    }
    
    else if (strcmp(c[0], "SAVE") == 0)
    
    {
        if (strcmp(c[1], "ALL") == 0)
        {
//             _access.save_keys(database.paths.passwd());
//             database.save_boards();
        }
        else if (strcmp(c[1], "USERS") == 0)
        {
//             _access.save_keys(database.paths.passwd());
        }
        else if (strcmp(c[1], "BOARDS") == 0)
        {
//             database.save_boards();
        }
    }
        
    else if (strcmp(c[0], "LOAD") == 0)
    
    {
        if (strcmp(c[1], "ALL") == 0)
        {
//             _access.load_keys(database.paths.passwd());
//             database.load_boards();
        }
        else if (strcmp(c[1], "USERS") == 0)
        {
//             _access.load_keys(database.paths.passwd());
        }
        else if (strcmp(c[1], "BOARDS") == 0)
        {
//             database.load_boards();
        }
    }
    
    else if (strcmp(c[0], "CLEAR") == 0)
    
    {
        if (strcmp(c[1], "ALL") == 0)
        {
            System.boards.clear();
        }
        else if (strcmp(c[1], "BOARDS") == 0)
        {
            System.boards.clear();
        }
    } 
    else if (strcmp(c[0], "UPDATE") == 0)
    {
        if (strcmp(c[1], "ALL") == 0)
        {
            packet::ipacket p(IBACOM_REQUEST_CONFIG);
            packet::ipacket t(IBACOM_REQUEST_STATUS);
            
            this->board.broadcast(&p);
            this->board.broadcast(&t);            
        }
        
        else if (strcmp(c[1], "BOARDS") == 0)
        {
            packet::ipacket p(IBACOM_REQUEST_CONFIG);
            packet::ipacket t(IBACOM_REQUEST_STATUS);
            
            this->board.broadcast(&p);
            this->board.broadcast(&t);            
        }
        else
        {
            if (this->board.has_client(c[1]))
            {
                packet::ipacket p(IBACOM_REQUEST_CONFIG);
                packet::netpacket t(&p, BOARD_NAME, c[1]);
                this->board.send(&t);            
            
                t.init(IBACOM_REQUEST_STATUS);
                this->board.send(&t);            
            }
            else { warning_board("parse:command:%s: could not find board <%s>", c[0], c[1]); }
        }            
    }
    else if (strcmp(c[0], "SYNC") == 0)
    {
        if (strcmp(c[1], "ALL") == 0)
        {
            this->board.sync_board();
        }

        else
        {
            if (this->board.has_client(c[1]))
            {
                this->board.sync_board(c[1]);
            }
            else { alert_board("parse:command:%s: could not find board <%s>", c[0], c[1]); }
        }            
    }
    else
    {
        warning_server("parse: invalid request <%s> !", command);
    }
    
    
    for (uint8_t i=0; i<=n; i++)
    {
        free(c[i]);
    }
    free(c);
}



#endif /* INDOORINO_SERVER */

