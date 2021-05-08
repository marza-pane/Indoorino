/*
 * server/connection.h
 *
 *  Created on: Dec 30, 2020
 *      Author: n00b
 * 
 * Thanks to Javidx9 - OneLoneCoder
 * 
 * http://www.onelonecoder.com
 * https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA
 * 
 */

#pragma once

#ifndef INDOORINO_CONNECTION_H_
#define INDOORINO_CONNECTION_H_

#include "netutils.h"

#if defined (__linux__)

#include "../packets/ipacket.h"
#include "../common/queue.h"

#include "../indoorino/schedule.h"

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

/* header of exit function */
void on_exit (int);

namespace net
{

//    ________________________________________________________________________
//    |                                                                      |
//    |    CONNECTION                                                        |
//    |______________________________________________________________________|
//

    class connection : public std::enable_shared_from_this<connection>
    {
    private:


    protected:
                
        iPid_t  _pid = 0;
        
        boost::asio::io_context&        _asio_context;
        boost::asio::ip::tcp::socket    _socket;

        utils::ObjectQueue<packet::bufpacket>       _txqueue;
        utils::ObjectQueue<packet::netpacket>   &   _rxqueue;
        
        std::vector<uint8_t>                        _readbuffer;

        std::mutex              _io_mutex;
        char                    _ipstring[INET_ADDRSTRLEN] {"0.0.0.0"};
                
        iSize_t                 _size=0;
        char                    _iv [N_BLOCK] {0};
        Safe                    _aes;
        std::vector<double>     _ping_time;
        
    public:

                 connection(
                     boost::asio::io_context&, 
                     boost::asio::ip::tcp::socket, 
                     utils::ObjectQueue<packet::netpacket>&);
                 
        virtual ~connection();
        
        void            stop                (void);
        bool            send                (packet::netpacket *p, bool direct=false);
        iPid_t          id                  () const { return _pid; }
        bool            is_connected        () const { return _socket.is_open(); }
        void            set_aeskey          (const char * b) { _aes.set_key(b); }
        char    *       ipstring            (void);
        void            ping                (void);
        
        const std::vector<double>& ping_time() { return _ping_time; }
    protected:
        
        void            read_header         (void);
        void            read_body           (void);
        void            write_packet        (void);
        void            send_checksum       (packet::netpacket *);

        virtual void    on_client_ready     (void)=0;
        virtual void    on_packet           (packet::netpacket *);
    };

//    ________________________________________________________________________
//    |                                                                      |
//    |    SERVER CONNECTION                                                 |
//    |______________________________________________________________________|
//

    class serverConnection : public connection
    {
    protected:
        void            on_packet           (packet::netpacket *);
        void            on_client_ready     (void);

    protected:
        std::vector<std::string>    _namelist;
        std::thread                 _timeout_thread;

    public:
        serverConnection(
            std::chrono::milliseconds,
            boost::asio::io_context&,
            boost::asio::ip::tcp::socket,
            utils::ObjectQueue<packet::netpacket>&);
        ~serverConnection();
        
        void            connect2client      (void);
        void            broadcast           (const char *, packet::ipacket *);
        void            broadcast           (packet::netpacket *);
        bool            has_client          (const char *);
    };

//    ________________________________________________________________________
//    |                                                                      |
//    |    CLIENT CONNECTION  --- this class is used only by Client ---      |
//    |______________________________________________________________________|
//
    
    class clientConnection : public connection
    {
    private:
        std::string         _name;
    protected:
        void            on_packet           (packet::netpacket *);
        void            on_client_ready     (void);

    public:
        clientConnection(
            boost::asio::io_context&, 
            boost::asio::ip::tcp::socket, 
            utils::ObjectQueue<packet::netpacket>&,
            const std::string&);
        ~clientConnection() {};
        void            connect2server      (const boost::asio::ip::tcp::resolver::results_type& );
    };
    
//    ________________________________________________________________________
//    |                                                                      |
//    |    SERVER TEMPLATE                                                   |
//    |______________________________________________________________________|
//
    
    class serverTemplate
    {
    protected:
        
        std::chrono::milliseconds       _timeout;
        boost::asio::io_context         _asio_context;
        std::thread                     _thread_context;
        boost::asio::ip::tcp::acceptor  _acceptor;
    
        utils::ObjectQueue<packet::netpacket>&          _rxqueue;
        std::deque<std::shared_ptr<serverConnection>>   _clientlist;


    public:
                 serverTemplate(utils::ObjectQueue<packet::netpacket>&, uint16_t);
        virtual ~serverTemplate();
        
        bool            begin               (void);
        void            WaitForClient       (void);
        void            loop                (void);
        void            stop                (void);
        
        bool            has_client          (const char *);
        void            broadcast           (packet::ipacket *);
        void            broadcast           (packet::netpacket *);
        void            send                (packet::netpacket *);
        void            send                (const char *, packet::ipacket *);
        
        bool            is_connected() const { return _acceptor.is_open(); }
    
    public:
        utils::ObjectQueue<packet::netpacket>& incoming () { return _rxqueue; }

    protected:
        
        virtual bool    on_new_client       (std::shared_ptr<serverConnection>)=0;
        virtual void    on_lost_client      (std::shared_ptr<serverConnection>)=0;
        
        friend class    IndoorinoServer;

    };

//    ________________________________________________________________________
//    |                                                                      |
//    |    BOARD SERVER                                                      |
//    |______________________________________________________________________|
//

    class serverBoards : public serverTemplate
    {
    private:
        std::thread _thread_ping;
    public:
         serverBoards(utils::ObjectQueue<packet::netpacket>&, uint16_t);
        ~serverBoards();

        void        sync_board          (const char *);
        void        sync_board          (void);
        void        stop                (void);

    protected:
	
        bool        on_new_client       (std::shared_ptr<serverConnection>);
        void        on_lost_client      (std::shared_ptr<serverConnection>);
    };

//    ________________________________________________________________________
//    |                                                                      |
//    |    SHELL SERVER                                                      |
//    |______________________________________________________________________|
//

    class serverShell : public serverTemplate
    {
        
    public:

         serverShell(utils::ObjectQueue<packet::netpacket>&, uint16_t);
        ~serverShell();

    protected:
	
        bool        on_new_client       (std::shared_ptr<serverConnection>);
        void        on_lost_client      (std::shared_ptr<serverConnection>);
    };


} /* namespace net */




#endif /* __linux__ */

#endif /* INDOORINO_CONNECTION_H_ */











//         serverConnection    *   get_connection  (const char * name)
//         {
//             for(const auto& client: _clientlist)
//             {
//                 if (client->has_board(name))
//                 {
//                     info_server("found connection for <%s> id:[%u] address:<%s>",p->source, client->id(), client->ipstring());
//                 }
//             }
//         }

//     public:
//         
//         #if defined (INDOORINO_SERVER)
//         void connect2client(uint32_t uid = 0);
// 
//         
//         void        begin           (void);
//         void        stop            (void);
//         void        listen          (void);
// 
//         bool        is_connected    () const { return m_socket.is_open(); }
// 
//     public:
//         void        send            (netpacket * p);
// 
//     private:
// 
//         void write_header();
//         void write_body();
// 
//         void read_header();
//         void read_body();
// 
//         void parse();
// 
//         
//     };
//  
// //    _______________________________________________________________________
// //    |                                                                     |
// //    |    SERVER INTERFACE                                                 |
// //    |_____________________________________________________________________|
// //
// 
//     #if defined (INDOORINO_SERVER)
//     class server_interface
//     {
//     public:
// 
//         server_interface(uint16_t);
// 
//         virtual ~server_interface();
//         
//         bool            begin           (void);
//         void            stop            (void);
//         void            listen          (void);
// 
//         void            send            (const char *, packet::ipacket *);
//         void            send            (std::shared_ptr<connection> c, netpacket * p);
//         void            broadcast       (netpacket * p, std::shared_ptr<connection> c = nullptr);
// 
//         void            loop            (size_t nMaxMessages = -1, bool bWait = false);
// 
//     protected:
// 
//         virtual bool    on_new_client   (std::shared_ptr<connection> c);
//         virtual void    on_loss_client  (std::shared_ptr<connection> c);
//         virtual void    parse           (sharedCpacket *)=0;
// 
//     protected:
// 
//         utils::ObjectQueue<sharedCpacket>       _rxqueue;
//         std::deque<std::shared_ptr<connection>> _qcon;
// 
//         asio::io_context                         m_asioContext;
//         asio::ip::tcp::acceptor                  m_asioAcceptor;
//         std::thread                              m_threadContext;
//         
//         iPid_t _idcounter = 10000;
//     };
//     
//     #endif /* INDOORINO_SERVER */
// //    _______________________________________________________________________
// //    |                                                                     |
// //    |    CLIENT INTERFACE                                                 |
// //    |_____________________________________________________________________|
// //
//     
//     #if defined (INDOORINO_CLIENT)
//     class client_interface
//     {
// 		private:
// 			utils::ObjectQueue<sharedCpacket> _rxqueue;
// 		protected:
// 			asio::io_context             m_context;
// 			std::thread                  thrContext;
// 			std::unique_ptr<connection>  m_connection;
// 			
// 		public:
// 			client_interface();
// 			virtual ~client_interface();
// 
//         bool            begin           (const std::string& host, const uint16_t port);
//         void            stop            ();
//         bool            is_connected    ();
//         
//         void            send            (netpacket * p);
//         void            send            (packet::ipacket * p, const char * target);
//         
//         utils::ObjectQueue<sharedCpacket>&       Incoming()      { return _rxqueue; }
// 
//     };
//     #endif /* INDOORINO_CLIENT */
        


