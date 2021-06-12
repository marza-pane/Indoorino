/*
 * stddev.h
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */

#ifndef _INDOORINO_BOARDIO_H_
#define _INDOORINO_BOARDIO_H_

#if defined (ARDUINO)

#if defined (ARDUINO)
#include "icommon.h"


namespace utils
{
    namespace board
    {
        class BoardIo
        {
        private:
            char    *   _w_buffer=nullptr;
            iSize_t     _w_count=0;
            bool        _w_flag=false;
            
        protected:
            
            utils::ObjectQueue<packet::netpacket> _rxqueue;
            utils::ObjectQueue<packet::netpacket> _txqueue;
            uint32_t _rxcount=0;
            uint32_t _txcount=0;
            uint32_t _unsent=0;
            uint32_t _errors=0;

            void read_packet_serial (void);
            void write_head_serial  (void);
            void write_body_serial  (void);

        public:
            BoardIo();
            ~BoardIo();
            
            void    begin       (void);
            void    loop        (void);
            void    send        (packet::ipacket   * p, bool d=false);
            void    send        (packet::netpacket * p, bool d=false);
            void    clear       (void);
            
            void    sendSerial  (packet::netpacket * p, bool d=false);
            void    sendTcp     (packet::netpacket * p, bool d=false);

            utils::ObjectQueue<packet::netpacket>& incoming() { return _rxqueue; }
            
            uint32_t total_rx() { return _rxcount; }
            uint32_t total_tx() { return _txcount; }
            uint32_t unsent()   { return _unsent; }
            uint32_t errors()   { return _errors; }
            
        };
        
        extern BoardIo     io;

    } /* namespace : board */

} /* namespace : utils */

#endif /* ARDUINO*/


#endif /* ARDUINO */

#endif /* _INDOORINO_BOARDIO_H_ */
