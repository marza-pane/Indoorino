/*
 * network/client.cpp
 *
 *  Created on: Jan 09, 2021
 *      Author: n00b
 * 
 */

#if defined(INDOORINO_CLIENT)
#include "../network/client.h"

ShellClient * Client=nullptr;

void on_exit         (int exitcode)
{
    std::cout << debug::TRS << std::endl << std::flush;
    Client->stop();
    printf("\texitcode [%u] : See you in space cowboy\n",exitcode);
    std::exit(EXIT_SUCCESS);
}

ShellClient::ShellClient()
{
    info_client(" *** CLIENT INITIALIZED ***");        
}

ShellClient::~ShellClient()
{ 
    stop();
    _connection.release();
    _rxqueue.clear();
    warning_client(" *** CLIENT TERMINATED ***");
}

bool    ShellClient::is_connected       (void)
{
    if (_connection)
    {
        return (_connection->is_connected());
    }
    return false;
}

bool    ShellClient::begin              (const std::string& name, const std::string& host, const uint16_t port)
{
    _name= std::string(name);
    _host= std::string(host);
    _port= port;
    
    return connect2server();
}

bool    ShellClient::connect2server     (void)
{
    if (_connection) _connection.release();

    try
    {
        boost::asio::ip::tcp::resolver resolver(_asio_context);
        boost::asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(_host, std::to_string(_port));
        _connection = std::make_unique<net::clientConnection>(_asio_context, boost::asio::ip::tcp::socket(_asio_context), _rxqueue, _name);
        _connection->connect2server(endpoint);
        _thread_context = std::thread([this]() { _asio_context.run(); });
        
        time_t timeout  = utils::millis() + 200;
        while ( utils::millis() < timeout )
        {
            if (!_connection->is_connected()) return false;
        }
    }
    catch (std::exception& error)
    {
        error_client("connection 2 server failed!");
        std::cout << "Client Exception: " << error.what() << "\n";
        return false;
    }
    debug_os("CONNECTED!");
    packet::netpacket q(IBACOM_SYS_REQ);
    strcpy(q.p_command(), "GET:ALL");
    strcpy(q.source, _name.c_str());
    strcpy(q.target, DEFAULT_TARGET);

    send(&q);
    return true;
}

void    ShellClient::loop               (void)
{
    static time_t timeout=0;
    static time_t dotting=0;
    
    if (utils::millis() > dotting)
    {
        dotting=utils::millis() + 1000;
        std::cout << ".";
        fflush(stdout);
    }
    
    if (is_connected())
    {
        if (!incoming().is_empty())
        {
            debug_os("New packet! fetch it outside please");
        }
    }
    else
    {
        if (utils::millis() > timeout)
        {
            stop();
            timeout=utils::millis() + 5000;
            connect2server();
        }
    }
}

void    ShellClient::stop               (void)
{
    
    if (_connection && _connection->is_connected()) _connection->stop();

    if (!_asio_context.stopped())
    {
        _asio_context.stop();
        alert_client("ASIO context stopped!");
    }
    
    if (_thread_context.joinable())
    { 
        _thread_context.join();
        alert_client("IO thread finished!");
    }
    
    if (_connection)
    {
        _connection.reset();
        alert_mem("releasing shared connection pointer");
    }
}

void    ShellClient::send               (const char * target, packet::ipacket * p)
{
    if (!_connection->is_connected())
    {
        warning_client("Cant send packet while not connected!");
        return;
    }
    this->send(new packet::netpacket(p, _name.c_str(), target));
}

void    ShellClient::send               (packet::netpacket * p)
{
    if (!_connection->is_connected())
    {
        warning_client("Cant send packet while not connected!");
        return;
    }
    if (p==nullptr || p->command() == 0)
    {
        error_client("send:invalid/nullptr packet");
        return;
    }
    if ( !(utils::is_devname(p->source) && utils::is_devname(p->target)) )
    {
        error_client("send: packet has invalid source <> or target <>", p->source, p->target);
        return;            
    }
    _connection->send(new packet::netpacket(p));
}

#endif /* INDOORINO_CLIENT */
