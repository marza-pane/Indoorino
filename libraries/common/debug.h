/*
 * common/debug.h
 *
 *  Created on: Oct 19, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_DEBUGTOOLS_H_
#define SOURCE_DEBUGTOOLS_H_
/*
    --- Debug flags are:
    
    DEBUG_GENERIC
    DEBUG_MEMORY
    DEBUG_IO
    DEBUG_OS
    DEBUG_SERIAL
    DEBUG_PARSER
    DEBUG_TCP
    DEBUG_DEVICES
    DEBUG_NETWORK
    DEBUG_SERVER
    DEBUG_CLIENT
    DEBUG_MODULE
    DEBUG_PACKET
    DEBUG_PACKETDUMP
    DEBUG_CHKSUM
*/

#include "definitions.h"

#if defined(DEBUG_PACKETDUMP)
    #ifndef DEBUG_PACKET
        #define DEBUG_PACKET
    #endif
#endif

namespace debug
{
    const char ERROR    [] PROGMEM = { "ERROR:"   };
    const char WARNING  [] PROGMEM = { "WARNING:" };
    const char ALERT    [] PROGMEM = { "ALERT:"   };
    const char INFO     [] PROGMEM = { "INFO:"    };
    const char DEBUG    [] PROGMEM = { "DEBUG:"   };


    const char MEM      [] PROGMEM = { "MEM:"     };
    const char IO       [] PROGMEM = { "IO:"      };
    const char OS       [] PROGMEM = { "OS:"      };
    const char DEV      [] PROGMEM = { "DEV:"     };
    const char NET      [] PROGMEM = { "NET:"     };
    const char AES      [] PROGMEM = { "AES:"     };
    const char TCP      [] PROGMEM = { "TCP:"     };
    const char PACK     [] PROGMEM = { "PACK:"    };
    const char BOARD    [] PROGMEM = { "BOARD:"   };
    const char SERVER   [] PROGMEM = { "SERVER:"  };
    const char CLIENT   [] PROGMEM = { "CLIENT:"  };
    const char CHKSUM   [] PROGMEM = { "CHECKSUM:"};
    const char MODULE   [] PROGMEM = { "MODULE:"  };
    const char DBGSRL   [] PROGMEM = { "SERIAL:"  };

#if defined (__linux__)
    const char RED      [] = {"\033[1;31m"};
    const char GRN      [] = {"\033[1;32m"};
    const char YLW      [] = {"\033[1;33m"};
    const char PUR      [] = {"\033[1;35m"};
    const char AZR      [] = {"\033[1;36m"};
    const char TRS      [] = {"\033[0m\n"};
#endif /* COLORS */

    
    void _helper (uint8_t, const char *, const __FSH *, ...);
    
} /* namespace : debug */

/*  Followinf 4 functions are REALLY verbose */

#if defined(DEBUG_MEMORY)
    #define debug_mem(S,...) debug::_helper(0, debug::MEM, F(S), ## __VA_ARGS__) 

#else
    #define debug_mem(S,...) {}
#endif

#if defined(DEBUG_SERIAL)
    #define debug_serial(S,...) debug::_helper(0, debug::DBGSRL, F(S), ## __VA_ARGS__)
#else
    #define debug_serial(S,...)    {}
#endif

#if defined(DEBUG_PARSER)
    #define debug_parser(S,...) debug::_helper(0, debug::IO, F(S), ## __VA_ARGS__)
#else
    #define debug_parser(S,...)    {}
#endif

#if defined(DEBUG_TCP)
    #define debug_tcp(S,...) debug::_helper(0, debug::TCP, F(S), ## __VA_ARGS__)
#else
    #define debug_tcp(S,...)    {}
#endif

/* ^^^^^ ^^^^^ ^^^^^ ^^^^^ ^^^^^ ^^^^^ ^^^^^ */

#if defined(DEBUG_IO)
    #define debug_io(S,...) debug::_helper(0, debug::IO, F(S), ## __VA_ARGS__)
#else
    #define debug_io(S,...)    {}
#endif

#if defined(DEBUG_OS)
    #define debug_os(S,...) debug::_helper(0, debug::OS, F(S), ## __VA_ARGS__)
#else
    #define debug_os(S,...)    {}
#endif


#if defined(DEBUG_DEVICES)
    #define debug_dev(S,...) debug::_helper(0, debug::DEV, F(S), ## __VA_ARGS__)
#else
    #define debug_dev(S,...) {}
#endif

#if defined(DEBUG_PACKET)
    #define debug_pack(S,...) debug::_helper(0, debug::PACK, F(S), ## __VA_ARGS__)
#else
    #define debug_pack(S,...)    {}
#endif

#if defined(DEBUG_BOARD)
    #define debug_board(S,...) debug::_helper(0, debug::BOARD, F(S), ## __VA_ARGS__)
#else
    #define debug_board(S,...)    {}
#endif

#if defined(DEBUG_NETWORK)
    #define debug_net(S,...) debug::_helper(0, debug::NET, F(S), ## __VA_ARGS__)
#else
    #define debug_net(S,...)    {}
#endif

#if defined(DEBUG_CHKSUM)
    #define debug_chksum(S,...) debug::_helper(0, debug::CHKSUM, F(S), ## __VA_ARGS__)
#else
    #define debug_chksum(S,...)    {}
#endif

#if defined(DEBUG_SERVER)
    #define debug_server(S,...) debug::_helper(0, debug::SERVER, F(S), ## __VA_ARGS__)
#else
    #define debug_server(S,...)    {}
#endif

#if defined(DEBUG_CLIENT)
    #define debug_client(S,...) debug::_helper(0, debug::CLIENT, F(S), ## __VA_ARGS__)
#else
    #define debug_client(S,...)    {}
#endif

#if defined(DEBUG_MODULE)
    #define debug_module(S,...) debug::_helper(0, debug::MODULE, F(S), ## __VA_ARGS__)
#else
    #define debug_module(S,...)    {}
#endif

/*
 * Available debug functions:
    
    debug()
    debug_mem()
    debug_serial()
    debug_parser()
    debug_io()
    debug_os()
    debug_dev()
    debug_pack()
    debug_board()
    debug_net()
    debug_chksum()
    debug_client()
    debug_module()
 * 
 */

//      _____________________________________________________________________
//      |                                                                   |
//      |       Debug and errors macros                                     |
//      |___________________________________________________________________|
//

#define error_mem(S,...)        debug::_helper(4, debug::MEM,   F(S), ## __VA_ARGS__)
#define error_io(S,...)         debug::_helper(4, debug::IO,    F(S), ## __VA_ARGS__)
#define error_os(S,...)         debug::_helper(4, debug::OS,    F(S), ## __VA_ARGS__)
#define error_dev(S,...)        debug::_helper(4, debug::DEV,   F(S), ## __VA_ARGS__)
#define error_net(S,...)        debug::_helper(4, debug::NET,   F(S), ## __VA_ARGS__)
#define error_aes(S,...)        debug::_helper(4, debug::AES,   F(S), ## __VA_ARGS__)
#define error_pack(S,...)       debug::_helper(4, debug::PACK,  F(S), ## __VA_ARGS__)
#define error_board(S,...)      debug::_helper(4, debug::BOARD, F(S), ## __VA_ARGS__)
#define error_server(S,...)     debug::_helper(4, debug::SERVER,F(S), ## __VA_ARGS__)
#define error_client(S,...)     debug::_helper(4, debug::CLIENT,F(S), ## __VA_ARGS__)


#define warning_mem(S,...)      debug::_helper(3, debug::MEM,   F(S), ## __VA_ARGS__)
#define warning_io(S,...)       debug::_helper(3, debug::IO,    F(S), ## __VA_ARGS__)
#define warning_os(S,...)       debug::_helper(3, debug::OS,    F(S), ## __VA_ARGS__)
#define warning_dev(S,...)      debug::_helper(3, debug::DEV,   F(S), ## __VA_ARGS__)
#define warning_net(S,...)      debug::_helper(3, debug::NET,   F(S), ## __VA_ARGS__)
#define warning_aes(S,...)      debug::_helper(3, debug::AES,   F(S), ## __VA_ARGS__)
#define warning_pack(S,...)     debug::_helper(3, debug::PACK,  F(S), ## __VA_ARGS__)
#define warning_board(S,...)    debug::_helper(3, debug::BOARD, F(S), ## __VA_ARGS__)
#define warning_server(S,...)   debug::_helper(3, debug::SERVER,F(S), ## __VA_ARGS__)
#define warning_client(S,...)   debug::_helper(3, debug::CLIENT,F(S), ## __VA_ARGS__)

#define alert_mem(S,...)        debug::_helper(2, debug::MEM,   F(S), ## __VA_ARGS__)
#define alert_io(S,...)         debug::_helper(2, debug::IO,    F(S), ## __VA_ARGS__)
#define alert_os(S,...)         debug::_helper(2, debug::OS,    F(S), ## __VA_ARGS__)
#define alert_dev(S,...)        debug::_helper(2, debug::DEV,   F(S), ## __VA_ARGS__)
#define alert_net(S,...)        debug::_helper(2, debug::NET,   F(S), ## __VA_ARGS__)
#define alert_aes(S,...)        debug::_helper(2, debug::AES,   F(S), ## __VA_ARGS__)
#define alert_pack(S,...)       debug::_helper(2, debug::PACK,  F(S), ## __VA_ARGS__)
#define alert_board(S,...)      debug::_helper(2, debug::BOARD, F(S), ## __VA_ARGS__)
#define alert_server(S,...)     debug::_helper(2, debug::SERVER,F(S), ## __VA_ARGS__)
#define alert_client(S,...)     debug::_helper(2, debug::CLIENT,F(S), ## __VA_ARGS__)

#define info_mem(S,...)         debug::_helper(1, debug::MEM,   F(S), ## __VA_ARGS__)
#define info_io(S,...)          debug::_helper(1, debug::IO,    F(S), ## __VA_ARGS__)
#define info_os(S,...)          debug::_helper(1, debug::OS,    F(S), ## __VA_ARGS__)
#define info_dev(S,...)         debug::_helper(1, debug::DEV,   F(S), ## __VA_ARGS__)
#define info_net(S,...)         debug::_helper(1, debug::NET,   F(S), ## __VA_ARGS__)
#define info_aes(S,...)         debug::_helper(1, debug::AES,   F(S), ## __VA_ARGS__)
#define info_pack(S,...)        debug::_helper(1, debug::PACK,  F(S), ## __VA_ARGS__)
#define info_board(S,...)       debug::_helper(1, debug::BOARD, F(S), ## __VA_ARGS__)
#define info_server(S,...)      debug::_helper(1, debug::SERVER,F(S), ## __VA_ARGS__)
#define info_client(S,...)      debug::_helper(1, debug::CLIENT,F(S), ## __VA_ARGS__)




#endif /*SOURCE_DEBUGTOOLS_H_*/
