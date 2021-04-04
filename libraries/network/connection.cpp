/*
 * server/connection.cpp
 *
 *  Created on: Jan 04, 2021
 *      Author: n00b
 * 
 * Thanks to Javidx9 - OneLoneCoder
 * 
 * http://www.onelonecoder.com
 * https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA
 * 
 */


#include "connection.h"

#if defined (__linux__)

namespace net
{
    char * connection::ipstring (void)
    {
        static char bm[LEN_NETADDRESS];
        if (is_connected())
        {
            boost::system::error_code ec;
            boost::asio::ip::tcp::endpoint ep = _socket.remote_endpoint(ec);
            
            if (!ec)
            {
                std::string s = ep.address().to_string();
                strcpy(bm, s.c_str());
            }
            else        strcpy(bm, "#.#.#.#");
        }
        else
        {
            strcpy(bm, "?.?.?.?");
        }
        return (char *)bm;
    }

//    ________________________________________________________________________
//    |                                                                      |
//    |    CONNECTION TEMPLATE                                               |
//    |______________________________________________________________________|
//

    connection::connection(
        boost::asio::io_context& context,
        boost::asio::ip::tcp::socket socket,
        utils::ObjectQueue<packet::netpacket>& incoming):
        _asio_context(context),
        _socket(std::move(socket)),
        _rxqueue(incoming)
    {
        _pid=rand();
        _aes.begin();
        _readbuffer.resize(SIZEOF_NET_HEADER);
    }

    connection::~connection()
    {
        stop();
        debug_net("deleting connection [%u]", _pid);
        if (!_txqueue.is_empty())
        {
            warning_net("connection ended with unsent [%u] packet(s)", _txqueue.count());
            while (_txqueue.count())
            {
                packet::ipacket * p = _txqueue.pop_front();
                if (p)
                {
                    std::cout << p->description() << std::endl;
                    delete p;
                }
                else std::cout << "(nullptr)" << std::endl;
            }
        }
    }
          
                    /*   S T O P   */
    void            connection::stop                        (void)
    { 
        
        if (_socket.is_open())
        {
            alert_net("Un-binding client [%u] on socket %s",this->id(), this->ipstring());
            boost::asio::post(_asio_context, [this]()
            {
                boost::system::error_code ec;
                _socket.close(ec);
                if (ec)
                {
                    error_net("ASIO:socket can not close!");
                }                
            });
        }
    }
    

                    /*  R E A D  H E A D  */
    void            connection::read_header                 (void)
    {
        if (!is_connected())        { error_server("WP:reading offline"); return; }
        
        
        boost::asio::async_read(
            _socket, boost::asio::buffer(_readbuffer, SIZEOF_NET_HEADER),
					[this](std::error_code errorcode, std::size_t length)
            {						
                if (!errorcode)
                {
                    char b_header[SIZEOF_NET_HEADER] {0};
                    for (iSize_t i=0; i<SIZEOF_NET_HEADER; i++) b_header[i] = _readbuffer.at(i);

                    #if defined(DEBUG_TCP)
                    debug_tcp("network header downloaded!");
                    utils::dump_hex(b_header, SIZEOF_NET_HEADER);
                    #endif
                    if (memcmp(b_header, PREAMBLE, SIZEOF_PREAMBLE) != 0)
                    {
                            error_net("Invalid header preamble");
                            utils::dump_hex(b_header, SIZEOF_NET_HEADER);
                            stop();
                    } 
                    else
                    {
                        memcpy(&_size, b_header + SIZEOF_PREAMBLE, sizeof(iSize_t));
                        if (_size == 0 || _size > MAX_SIZE_PACKET)
                        {
                            error_net("read_body: invalid size [%u>%u]", _size, MAX_SIZE_PACKET);
                            _size=0;
                            return;
                        }
                        memcpy(_iv, b_header + SIZEOF_PREAMBLE + sizeof(iSize_t), N_BLOCK);
                        read_body();
                    }
                }
                else
                {
                    error_net("ASIO read:head error");
                    std::cout << errorcode.message() << std::endl;
                    _socket.close();
                }
            });
    }

                    /* R E A D   B O D Y */    
    void            connection::read_body                   (void)
    {
        
        char * buffer = (char *)calloc(_size, sizeof(char));
        
        boost::asio::async_read(_socket, boost::asio::buffer(buffer, _size),
					[this, buffer](std::error_code errorcode, std::size_t length)
            {						
                if (!errorcode)
                {
                    char data[MAX_SIZE_PACKET] {0};
                    iSize_t s_cipher = _aes.decrypt(data, _iv, buffer, _size);    
                    free(buffer);
                    
                    #if defined(DEBUG_TCP)
                    debug_tcp("PACKET READ");
                    utils::dump_hex(data, s_cipher);
                    debug_tcp("** CIPHER DECRYPY KEY **");
                    utils::dump_hex(_aes.masterkey(), LEN_AES_MASTERKEY);        
                    #endif
                    
                    packet::netpacket * p=new packet::netpacket(data, true);

                    if (p->command() == 0)
                    {
                        if (utils::is_devname(p->source))   { error_net("dowloaded data returned an invalid packet from <%s>!", p->source); }
                        else                                { error_net("dowloaded data returned an invalid packet"); }

                        /* Probably now encryption key has been set, so we close to connection */                        
                        utils::dump_hex(data, s_cipher);
                        delete p;
                        stop();
                        return;
                    }
                    
                    /** Packet has been received! **/
                    this->on_packet(p);
                    read_header();
                }
                else
                {
                    error_net("ASIO read:body error");
                    std::cout << errorcode.message() << std::endl;
                    _socket.close();
                }

            });
        

    }
    
                    /*   S E N D   */
    void            connection::send                        (packet::netpacket * p, bool direct)
    {
        if (p==nullptr || p->command() == 0)
        {
            if (p->command() == 0)  error_net("send: empty packet!");
            else                    error_net("send: nullptr!");
            return;
        }
        debug_tcp("Appending %s to connection send list", p->description());
        
        packet::bufpacket * q = new packet::bufpacket(p);
        
        boost::asio::post(_asio_context,
        [this, q, direct]()
        {
            bool flag = _txqueue.is_empty();
                        
            if (direct) { _txqueue.push_front(q); }

            else        { _txqueue.push_back(q); }

            if (flag)
            {
                debug_tcp("Ready to write!");
                write_packet();
            }
        });
    }
    
                    /* W R I T E P A C K E T */
    void            connection::write_packet                (void)
    {
//         static std::vector<uint8_t> buffer;
        
//         std::cout << " * 1" << std::flush;
        
        if (!is_connected())        { error_net("WP:reading offline"); return; }
        
        if (_txqueue.is_empty())    { error_net("WP:empty queue"); return; }
        

        packet::bufpacket * p = _txqueue.pop_front();

        std::unique_lock<std::mutex> lock (_io_mutex, std::defer_lock);
        
//         std::cout << " 2" << std::flush;
        
        if ( p==nullptr || p->command()==0)
        {
            error_net("WP: nullptr packet found!");
            if (!_txqueue.is_empty())
            {
                write_packet();
            }
            return;            
        }
        
        debug_io("tcp:write: [%u] writing <%s>",_pid, p->label());
                

//         std::cout << " 3" << std::flush;
        
        char    b_iv[N_BLOCK] {0};
        iSize_t s_cipher=0;
        iSize_t size=p->full_size();
        iSize_t csize=_aes.cipherlen(size);
        
        if (csize > MAX_SIZE_PACKET)
        {
            warning_net("packet <%s> size [%u] exceed max size [%u]", p->label(), csize, MAX_SIZE_PACKET);
            delete p;
            if (!_txqueue.is_empty())
            {
                write_packet();
            }
            return;
        }
        debug_tcp("write: allocating cipher [%u] clear [%u]", csize, size);

        std::vector<uint8_t> cleardata;
        std::vector<uint8_t> cipher;
        
//         std::cout << " 4" << std::flush;

        cleardata.resize(size);
        cipher.resize(csize);
//         std::cout << " 5" << std::flush;

        p->header((char *) cleardata.data());
        if (p->data_size() > 0)
        {
            memcpy(cleardata.data() + SIZEOF_PACKET_HEADER, p->payload(), p->data_size());
        }

//         std::cout << " 6" << std::flush;

        s_cipher = _aes.encrypt((char *)cipher.data(), b_iv,(char *) cleardata.data(), size);
        debug_tcp("writing: cipher SIZE [%u]", s_cipher);

//         std::cout << " 7" << std::flush;
        
        p->data.resize(SIZEOF_NET_HEADER + s_cipher);
//         buffer.clear();
//         buffer.resize(SIZEOF_NET_HEADER + s_cipher);
        
//         std::cout << " 8" << std::flush;

        memcpy(p->data.data(), PREAMBLE, SIZEOF_PREAMBLE);
        memcpy(p->data.data() + SIZEOF_PREAMBLE, &s_cipher, sizeof(iSize_t));
        memcpy(p->data.data() + SIZEOF_PREAMBLE + sizeof(iSize_t), b_iv, N_BLOCK);
        memcpy(p->data.data() + SIZEOF_NET_HEADER, cipher.data(), s_cipher);
        
//         std::cout << " 9" << std::flush;
        #if defined(DEBUG_TCP)
        debug_tcp("write: dump raw <%s> packet:", p->label());
        utils::dump_hex((const char *)cleardata.data(), p->full_size());
        debug_tcp("write: dump encrypted packet:");
        utils::dump_hex((const char *)p->data.data(), p->data.size());
        debug_tcp("** CIPHER ENCRYPT KEY **");
        utils::dump_hex(_aes.masterkey(), LEN_AES_MASTERKEY);
        #endif
        
//         if (p->command() == IBACOM_RESET)
//         {
//             error_net("PARSING %s", p->description());
//             delete p;
//             return;
//         }
        
        

//         std::cout << " 10" << std::flush;
        
        boost::asio::async_write(_socket,
            boost::asio::const_buffer(p->data.data(), p->data.size()),
                [this, p](std::error_code errorcode, std::size_t length)
        {
            if (!errorcode)
            {
//                 std::cout << " 11 * " << std::flush;
//                 _txqueue.pop_front();
                delete p;
                if (!_txqueue.is_empty())
                {
                    write_packet();
                }
            }
            else
            {
                delete p;
                error_net("ASIO write error");
                std::cout << errorcode.message() << std::endl;
                _socket.close();
            }
        });
    }
                    
                    /* S E N D C H E C K S U M */
    void            connection::send_checksum               (packet::netpacket * p)
    {
        uint8_t hash[SHA_DIGEST_LENGTH];
        p->checksum(hash);

        packet::netpacket ack(IBACOM_ACK);
        memcpy(ack.p_hash1(), hash, SHA_DIGEST_LENGTH);
        strcpy(ack.target, p->source);
        strcpy(ack.source, p->target);
        this->send(&ack, true);
    }

        
                    /* O N  P A C K E T */
    void            connection::on_packet                   (packet::netpacket * p)
    {
        debug_net("connection [%u]: on packet <%s>!", _pid, p->label());
        
        /* These are hashless packets */
        if (p->command() == IBACOM_ACK)
        {
            // TODO: scroll/match a hash list
            delete p;
            return;
        }
        
        if (p->command() == IBACOM_CONF_AES)
        {
            if ( strcmp(p->p_command(), "SET") == 0 )
            {
                _aes.set_key(p->p_aeskey1());
                this->on_client_ready();
            }
            else
            {
                warning_net("Unrecognized AES conf command <%s>", p->p_command());
            }
            delete p;
            return;
        }
        
        send_checksum(p);
        _rxqueue.push_back(p);
    }
    
    
//    ________________________________________________________________________
//    |                                                                      |
//    |    SERVER CONNECTION                                                 |
//    |______________________________________________________________________|
//
    
    
    serverConnection::serverConnection(
        std::chrono::milliseconds timeout,
        boost::asio::io_context& context,
        boost::asio::ip::tcp::socket socket,
        utils::ObjectQueue<packet::netpacket>& incoming):connection(
            context,
            std::move(socket),
            incoming)
    {
        debug_net("New connection [%u]", _pid);
//         _timeout_thread = std::thread(
//             [this, timeout]
//             {
//                 std::this_thread::sleep_for(timeout);
//                 stop();
//             });
//         _timeout_thread.detach();
        
    }
    
    serverConnection::~serverConnection()
    {
        _namelist.clear();
    }
    
                    /* C O N N E C T 2 C L I E N T */
    void            serverConnection::connect2client        (void)
    {
        if (_socket.is_open())
        {
            read_header();
        }
        else    { error_net("New connection on close socket!"); }
    };
    
                    /*   B R O A D C A S T   */
    void            serverConnection::broadcast             (const char * source, packet::ipacket *p)
    {
        for(const auto& target: _namelist)
        {
//             std::cout << "Broadcasting netpacket" << p->description() << "to " << target << std::endl;
            this->send(new packet::netpacket(p, source, target.c_str()));
        }
    }
    
    void            serverConnection::broadcast             (packet::netpacket *p)
    {
        if (!utils::is_devname(p->source)) { strcpy(p->source, BOARD_NAME); }
        
        for(const auto& target: _namelist)
        {
            strcpy(p->target, target.c_str());
            std::cout << "Broadcasting netpacket" << p->description() << "to " << target << std::endl;
            this->send(p);
        }
        
    }
        
                    /*  H A S     C L I E N T  */
    bool            serverConnection::has_client            (const char * boardname)
    {
        for(const auto& value: _namelist)
        {
            if (value.compare(0, LEN_NAME, boardname) == 0) { return true; }
        }
        return false;
    }

                    /*   O N    P A C K E T   */
    void            serverConnection::on_packet             (packet::netpacket * p)
    {
        if (utils::is_devname(p->source) && !has_client(p->source))
        {
            _namelist.push_back(std::string(p->source));
            info_net("connection [%u]: adding target name <%s>", _pid, p->source);            
        }
        connection::on_packet(p);
    }
    
                    /* O N  C L I E N T  R E A D Y */
    void            serverConnection::on_client_ready       (void)
    {
        iEpoch_t t=utils::epoch_now();
        packet::netpacket p(IBACOM_UNIX_EPOCH);
        memcpy(p.p_epoch(), &t, sizeof(iEpoch_t));
        std::cout << "Updating board RTC with : " << utils::time_string(*p.p_epoch()) << std::endl;

        broadcast(&p);
        
        uint8_t  u8 = 1;
        uint32_t u32 = utils::epoch_now();

        p.init(IBACOM_REPORT);
        strcpy(p.p_name(), "INDOORINO SERVER");
        strcpy(p.p_message(), "Welcome to indoorino network!");
        memcpy(p.p_epoch(), &u32, sizeof(uint32_t));
        memcpy(p.p_level(), &u8, sizeof(uint8_t));
        
        broadcast(&p);
    }

//    ________________________________________________________________________
//    |                                                                      |
//    |    CLIENT CONNECTION                                                 |
//    |______________________________________________________________________|
//

    clientConnection::clientConnection(
        boost::asio::io_context& context,
        boost::asio::ip::tcp::socket socket,
        utils::ObjectQueue<packet::netpacket>& incoming,
        const std::string& name):
            connection(context,std::move(socket),incoming),
            _name(name)
    {
        debug_net("Client [%s:%u] initialized", _name.c_str(), _pid);        
    }
    
    
                    /*   O N    P A C K E T   */
    void            clientConnection::on_packet             (packet::netpacket * p)
    {
        connection::on_packet(p);
    }
    
                    /* C O N N E C T 2 S E R V E R */
    void            clientConnection::connect2server        (const boost::asio::ip::tcp::resolver::results_type& target)
    {
               
        boost::asio::async_connect(_socket, target,
            [this](std::error_code errorcode, boost::asio::ip::tcp::endpoint endpoint)
            {
                if (!errorcode)
                {
                    std::cout << "Connected to " << endpoint << std::endl;
                    read_header();

                    char key[LEN_AES_MASTERKEY] {0};
                    _aes.generate(key);
                    packet::netpacket p(IBACOM_CONF_AES);
                    memcpy(p.p_aeskey1(), key, LEN_AES_MASTERKEY);
                    strcpy(p.p_command(), "SET");
                    strcpy(p.source, _name.c_str());
                    strcpy(p.target, DEFAULT_TARGET);
                    send(&p, true);
                    
                    std::thread th_key(
                        [this, key] {
                            while (!_txqueue.is_empty()) {}
                            _aes.set_key(key);
                        });
                    th_key.detach();
                }
                else
                {
                    std::cout << "ERROR:connection with server " << endpoint << " failed..." << std::endl;
                    stop();
                }
            });
    }
    
                    /* O N  C L I E N T  R E A D Y */
    void            clientConnection::on_client_ready       (void)
    {              
//         packet::netpacket q(IBACOM_REPORT);
//         strcpy(q.p_name(), "INDOORINO SERVER");
//         strcpy(q.p_message(), "Welcome to indoorino network!");
//         *q.p_epoch() = utils::epoch_now();
//         *q.p_level() = 1;
//         strcpy(q.source, _name.c_str());
//         strcpy(q.target, DEFAULT_TARGET);

    }
    
//    ________________________________________________________________________
//    |                                                                      |
//    |    SERVER TEMPLATE                                                   |
//    |______________________________________________________________________|
//    

    serverTemplate::serverTemplate(
        utils::ObjectQueue<packet::netpacket>& incoming,
        uint16_t port): 
            _acceptor(_asio_context, 
                      boost::asio::ip::tcp::endpoint(
                          boost::asio::ip::tcp::v4(), port)),
            _rxqueue(incoming)
    {
        info_server("Initializing server on port [%u]!", port);
        
        _timeout = std::chrono::milliseconds(TIMEOUT_CLIENT);
    };

    serverTemplate::~serverTemplate()
    {
        stop();
    }

 
                    /*     B E G I N    */
    bool            serverTemplate::begin                   (void)
    {
        try
        {
            WaitForClient();
            _thread_context = std::thread([this]() { _asio_context.run(); });
        }
        catch (std::exception& errorcode)
        {
            error_server("ASIO init failed!");
            std::cout << errorcode.what() << std::endl;
            return false;
        }

        info_server("Started!");
        return true;
    }

    
                    /* W A I T F O R C L I E N T */
    void            serverTemplate::WaitForClient           (void)
    {
//         static std::mutex _acceptor_mutex;

        _acceptor.async_accept(
            [this](std::error_code errorcode, boost::asio::ip::tcp::socket socket)
            {
                // Triggered by incoming connection request
                if (!errorcode)
                {
                    
                    std::cout << "SERVER:New connection at:" << socket.remote_endpoint() << std::endl;
                    
//                     _acceptor_mutex.lock();
                    std::shared_ptr<serverConnection> newconn = std::make_shared<serverConnection>(
                        _timeout, 
                        _asio_context,
                        std::move(socket),
                        _rxqueue);

                    if (on_new_client(newconn))
                    {								
                        _clientlist.push_back(std::move(newconn));
                        _clientlist.back()->connect2client();                        
                    }
                    else
                    {
                    }
//                     _acceptor_mutex.unlock();
                }
                else
                {
                    error_server("New connection failed");
                    std::cout << errorcode.message() << std::endl;
                }

                WaitForClient();
            });
    }

                    /*     S T O P     */
    void            serverTemplate::stop                    (void)
    {
        _acceptor.cancel();
        if (!_asio_context.stopped())   { _asio_context.stop(); alert_server("ASIO context stopped!"); }
        if (_thread_context.joinable()) { _thread_context.join(); alert_server("IO thread finished!"); }
        _clientlist.clear();
    }
    
                    /*     L O O P     */
    void            serverTemplate::loop                    (void)
    {
        for (auto& client: _clientlist)
        {
            if ( !(client && client->is_connected()) )
            {
                on_lost_client(client);
                client.reset();
                _clientlist.erase(std::remove(_clientlist.begin(), _clientlist.end(), client), _clientlist.end());
                loop();
                return;
            }
        }
    }

                    /*  B R O A D C A S T   */
    void            serverTemplate::broadcast               (packet::ipacket * p)
    {
        for(const auto& client: _clientlist)
        {
            client->broadcast(BOARD_NAME, p);
        }
    }

    void            serverTemplate::broadcast               (packet::netpacket * p)
    {            
        for(const auto& client: _clientlist)
        {
            client->broadcast(p);
        }
    }

                    /*     S E N D     */
    void            serverTemplate::send                    (packet::netpacket * p)
    {
        uint8_t count = 0;
        for (auto& client: _clientlist)
        {
            if (client->has_client(p->target))
            {
                debug_io("Found target <%s> in connection list", p->target);//will be debug_tcp
                
                if (client && client->is_connected())
                {
                    debug_io("Target is online");
                    client->send(p);
                    count++;
                    continue;                    
                }
                else
                {
                    on_lost_client(client);
					client.reset();
					_clientlist.erase(std::remove(_clientlist.begin(), _clientlist.end(), client), _clientlist.end());
                    break;
                }
            }
        }
        
        if (count == 0)
            warning_net("Could not find connections for <%s> sending <%s>", p->target, p->label());
    }
    
//     void            serverTemplate::send                    (const char * target, packet::ipacket * p)
//     {
//         this->send(new packet::netpacket(p, BOARD_NAME, target));
//     }
    
   
} /* namespace net */


#endif /* (__linux__) */