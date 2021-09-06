/*
 * boardio.cpp
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */


#if defined (ARDUINO)
#include "boardio.h"
#if defined (ESP8266)
#include "espclient.h"
#endif
//             uint32_t _rxcount=0;
//             uint32_t _txcount=0;
//             uint32_t _unsent=0;
//             uint32_t _errors=0;
namespace utils
{
    namespace board
    {
        BoardIo::BoardIo() 
        {}
        BoardIo::~BoardIo() 
        {
            clear();
        }

        void        BoardIo::begin                  (void)
        {
            Serial.begin(SERIAL_DEFAULT_BAUDRATE);
            Serial.setTimeout(0);
        }
        
        void        BoardIo::loop                   (void)
        {
            iSize_t current_ram = utils::board::available_ram();
            
//             static iSize_t last_ram = 0;
//             if (current_ram != last_ram)
//             {
//                 char sign='-';
//                 if (current_ram > last_ram) { sign='+'; }
//                 alert_mem("SRAM [%c] change [%u] ==> [%u] delta = [%d]",
//                           sign, last_ram, current_ram, abs(current_ram - last_ram));
//                 last_ram = current_ram;
//             }
            
            if (current_ram < SRAM_FATAL)
            {
                utils::board::reset();
            }

            if (current_ram < SRAM_LIMIT)
            {
                if (!_txqueue.is_empty() || !_rxqueue.is_empty())
                {
                    warning_mem("LOW SRAM:Freeing serial buffers");
                    this->clear();
                }
            }
            
            
            read_packet_serial();           
            
            write_head_serial();
        }
        
        void        BoardIo::clear                  (void)
        { 
            _unsent=_txqueue.count();

            _rxqueue.clear();
            _txqueue.clear();
        }
            
        void        BoardIo::send                   (packet::ipacket * p, bool direct)
        {
            char src[LEN_NAME] {0};
            char trg[LEN_NAME] {0};
            
            strcpy_P(src, BOARD_NAME);
            strcpy_P(trg, DEFAULT_TARGET);
            
            packet::netpacket np(p, src, trg);
            this->send(&np, direct);
        }
        
        void        BoardIo::send                   (packet::netpacket *p, bool direct)
        {
        #if defined (ESP8266)
            sendTcp(p, direct);
        #else
            sendSerial(p, direct);
        #endif
        }

        void        BoardIo::sendSerial             (packet::netpacket *p, bool direct)
        {
            if (p == nullptr || p->command() == 0)
            {
                warning_io("serial:send: invalid packet!");
                return;
            }
            if (direct) { _txqueue.push_front(new packet::netpacket(p)); }
            else        { _txqueue.push_back(new packet::netpacket(p));  }
        }
        
        void        BoardIo::sendTcp                (packet::netpacket *p, bool direct)
        {
        #if defined (ESP8266)
            client.send(p, direct);
        #endif
        }

        void        BoardIo::write_head_serial      (void)
        {
            
            if (_txqueue.is_empty()) { return; }
            
            if (Serial.availableForWrite() < SIZEOF_PACKET_HEADER) { return; }
                       
            char b_head[SIZEOF_PACKET_HEADER];

            #if defined(DEBUG_IO)
            uint32_t m = micros();
            #endif
            
            _txqueue.front()->header(b_head);
            
            Serial.write((uint8_t *)b_head, SIZEOF_PACKET_HEADER);

            #if defined(DEBUG_IO)
            debug_io("Serial write-header time is %u usec", micros() - m);
            #endif

            
            write_body_serial();
        }
        
        void        BoardIo::write_body_serial      (void)
        {
            packet::netpacket * p = _txqueue.pop_front();
            
            const iSize_t size = p->data_size();

            #if defined(DEBUG_IO)
            uint32_t m = micros();
            #endif
            
            if (size > 0)
            {
                debug_io("writing serial %s packet body!", p->label());
                iSize_t total=0;
                iEpoch_t timeout = millis() + TIMEOUT_SERIAL_WRITE;
                while (total < size)
                {
                    if (millis() > timeout)
                    {
                        warning_io("serial:timout: could not write <%s> body", p->label());
                        break;
                    }
                    
                    debug_io("Checking if AFW = %d < %u [ min(%u,%u)]",
                             Serial.availableForWrite(),
                             MIN_INTEGER_M_(SERIAL_TX_BUFFER_SIZE - 5, (size - total)),
                             SERIAL_TX_BUFFER_SIZE - 5, size - total);
                    
                    if ( Serial.availableForWrite() < (MIN_INTEGER_M_( (SERIAL_TX_BUFFER_SIZE - 5), (size - total) )) )
                    { 
                        continue;
                    }

                    if ( (size - total) > (SERIAL_TX_BUFFER_SIZE - 5) )
                    {
                        
                        debug_io("writing MAX [%u] bytes", SERIAL_TX_BUFFER_SIZE - 5);
                        total += Serial.write((uint8_t *)p->payload() + total, (SERIAL_TX_BUFFER_SIZE - 5) );
                    }
                    else
                    {
                        debug_io("writing [%u] bytes", (size - total));
                        total += Serial.write((uint8_t *)p->payload() + total, (size - total));                        
                    }                    
                }
            }
            
            else  debug_io("wrote bodyless serial packet!");

            #if defined(DEBUG_IO)
            debug_io("Serial write-body time is %u usec", micros() - m);
            #endif

            delete p;
        }
        
        void        BoardIo::read_packet_serial     (void)
        {
            
//             error_io(".");
            if (Serial.available() <= 0) { return; }
            uint8_t checkbyte = Serial.read();
//             error_io(":");
            if (checkbyte != UINT8_MAX)
            {
                SerialDebug.print(checkbyte, HEX);
                return;
            }
//             error_io("@");
            
            iSize_t total=0;
            uint8_t b_head[SIZEOF_PACKET_HEADER] {0};
            iEpoch_t timeout = millis() + TIMEOUT_SERIAL_READ;
            
            
            memset(b_head, 0, SIZEOF_PACKET_HEADER);
            b_head[total] = checkbyte;
            total++;
            
            
            #if defined(DEBUG_SERIAL)
            uint32_t m = micros();
            #endif
            while (total < SIZEOF_PACKET_HEADER)
            {
                if (Serial.available())
                {
                    uint8_t c= Serial.read();
                    b_head[total] = c;
                    total++;
                }
                
                if(millis() > timeout)
                { 
                    error_io("serial read:header timeout!");
                    break;
                }
            }
            #if defined(DEBUG_SERIAL)
            debug_serial("read:header time is %u usec", micros() - m);
            utils::dump_hex((char*)b_head, total);
            m = micros();
            #endif

            packet::netpacket p((char*)b_head, false);

            #if defined(DEBUG_SERIAL)
            debug_serial("packet allocation time is %u usec", micros() - m);
            m = micros();
            #endif

            iSize_t size = p.data_size();

            if (size > 0)
            {
                total = 0;
                while (total < size)
                {
                    if (Serial.available())
                    {
                        uint8_t c= Serial.read();
                        p.payload()[total] = c;
                        total++;
                    }
                    if(millis() > timeout)
                    { 
                        error_io("serial read:body timeout!");
                        break;
                    }
                }
            }
            #if defined(DEBUG_SERIAL)
            debug_serial("read:body time is %u usec", micros() - m);
            utils::dump_hex(p.payload(), p.data_size());
            #endif
            
            
            if (p.command() != 0)
            {
                debug_io("serial: read %s", p.description());
                _rxqueue.push_back(new packet::netpacket(&p));
                #if !defined(ESP8266)
                while (millis() < timeout)
                {
                    read_packet_serial();
                }
                #endif
            }
            else
            {
                warning_io("serial: got invalid packet! %s", p.description());
            }
        }
        
        BoardIo     io;

    } /* namespace : board */

    
} /* namespace : utils */




namespace utils
{
    namespace board
    {
        void        send_allarm                 (const uint8_t, const uint8_t)
        {}
        
        void        send_warning                (const uint8_t, const uint8_t)
        {}

//      _____________________________________________________________________
//      |                                                                   |
//      |       Send Config                                                 |
//      |___________________________________________________________________|
//

        void        send_config                 (void)
        {
            packet::ipacket p(IBACOM_CONF_STD);    

            strcpy_P(p.p_name(), BOARD_NAME);
            strcpy_P(p.p_type(), INDOORINO_TYPE);
            strcpy_P(p.p_board(), BOARD_TYPE);
            strcpy_P(p.p_desc1(), INDOORINO_RESOURCE_DESC);
            
            
            *p.p_devnum() = conf.devnum();            
            
            debug_dev("send_config: STD");
            io.send(&p);

        #if defined (INDOORINO_SAMPLER)

            p.init(IBACOM_CONF_SAMPLER);
            
            strcpy_P(p.p_name(), BOARD_NAME);
            
            * p.p_stephour1() = conf.step_config();
            * p.p_stephour2() = conf.step_status();
            * p.p_stephour3() = conf.step_probe();
            
            #if defined (DEBUG_PACKET)
                debug("\npacket:sendConfig:Sending CONF_SAMPLER\n");
                dump_packet(p);
            #endif
            io.send(&p);
        
            
        #elif defined (INDOORINO_CONTROLLER)

        #elif defined (ESP8266)
            
//             uint32_t n {0};
            
            p.init(IBACOM_CONF_ESP);    

            strcpy_P(p.p_name(), BOARD_NAME);

            WiFi.localIP().toString().toCharArray(p.p_ip1(), INET_ADDRSTRLEN);
            conf.remote_ip().toString().toCharArray(p.p_ip2(), INET_ADDRSTRLEN);
            
            *p.p_port1() = (uint32_t)client.clientport();
            *p.p_port2() = (uint32_t)conf.remote_port();

//          memcpy(p->p_aeskey1(), (char *)secure.get_key(), LEN_AES_MASTERKEY);
//          memcpy(p->p_timeout1(), (uint32_t)conf.timeout_client(), sizeof(uint32_t));

            *p.p_timeout2() = (uint32_t)conf.timeout_packet();            
            *p.p_count1()   = (uint8_t)conf.attemps_packet();

            debug_dev("send_config: ESP");

            io.send(&p);
        
            
        #endif /* PROJECTS */
            
        #if defined (INDOORINO_DEVS)
        debug_dev("packet:sendConfig: [%u] devices", conf.devnum());    
        for (uint8_t i=0; i<conf.devnum(); i++)
        {                 
            io.send(conf.device(&p,i));
        }
        #endif /* INDOORINO_DEVS */

        }

//      _____________________________________________________________________
//      |                                                                   |
//      |       Send Status                                                 |
//      |___________________________________________________________________|
//
        
        void        send_status                 (void)
        {
            packet::ipacket p(IBACOM_STATUS_STD);
            strcpy_P(p.p_name(), BOARD_NAME);

            *p.p_epoch()    = (uint32_t)rtc.epoch();
            *p.p_freemem()  = (uint32_t)available_ram();
            *p.p_looptime() = (uint32_t)rtc.looptime();

            *p.p_errors1()  = (uint32_t)io.errors();
            *p.p_errors2()  = (uint32_t)io.unsent();;
            *p.p_errors3()  = 0;
            *p.p_errors4()  = 0;
                
            debug_dev("send_status STD");
            
            io.send(&p);
            
        #if defined (INDOORINO_SAMPLER)

//             extern  uint32_t lastprobe;
// 
//             ptr=reallocPacket(ptr, IBACOM_STATUS_SAMPLER);
//             strcpy(ptr->p_name(), buf);
// 
//             uint32_t a=lastprobe + conf.step() - rtc.epoch();
//             memcpy(ptr->p_stepday1(), &a, sizeof(uint32_t)); 
// 
//             uint32_t b=sd.probenum();
//             memcpy(ptr->p_value(), &b, sizeof(uint32_t)); 
// 
//             boardio.tx.send(ptr);
//                     
//             debug("packet:sendStatus:devnum is %u",conf.devnum());
//             
//             for (uint8_t i=0; i<conf.devnum(); i++)
//             {
//                 ptr = devices(i)->status(ptr);
//                 boardio.tx.send(ptr);
//             }

        #elif defined (INDOORINO_CONTROLLER)


        #elif defined (ESP8266)

            uint32_t buf {0};
            p.init(IBACOM_STATUS_ESP);
            strcpy_P(p.p_name(), BOARD_NAME);

            *p.p_errors1()     =   (uint32_t)client.missing();
            *p.p_errors2()     =   (uint32_t)client.unsent();
            *p.p_errors3()     =   (uint32_t)network.disconnections();
            *p.p_errors4()     =   (uint32_t)client.overflows();
            *p.p_tcptx()       =   (uint32_t)client.total_tx();
            *p.p_tcprx()       =   (uint32_t)client.total_rx();
            *p.p_serialtx()    =   (uint32_t)io.total_tx();
            *p.p_serialrx()    =   (uint32_t)io.total_tx();

            debug_dev("send_status: ESP");
            
            io.send(&p);
            
        #endif /* PROJECTS */
                        
            #if defined (INDOORINO_DEVS)
            if (devices.devnum() > 0)
            {
                debug_dev("packet:sendStatus: [%u] devices",devices.devnum());
                        
                for (uint8_t i=0; i<devices.devnum(); i++)
                {
                    devices[i]->send_dev_stat();
                }
            }
            #endif /* INDOORINO_DEVS */
        }


//      _____________________________________________________________________
//      |                                                                   |
//      |       Send boot signal                                            |
//      |___________________________________________________________________|
//        
        void        send_boot_signal            (void)
        {
            packet::ipacket p(IBACOM_BOARD_BOOT);
            strcpy_P(p.p_name(), BOARD_NAME);
            io.send(&p);
        }
        

//      _____________________________________________________________________
//      |                                                                   |
//      |       Send AES setup                                              |
//      |___________________________________________________________________|
//        
        void        send_aes_setup              (const char * key)
        {
        #if defined (ESP8266)
            alert_os("Setting new AES key");
            packet::ipacket p(IBACOM_CONF_AES);
            memcpy(p.p_aeskey1(), key, LEN_AES_MASTERKEY);
            strcpy(p.p_command(), "SET");
            #if defined(DEBUG_AES)
            utils::dump_hex(key, LEN_AES_MASTERKEY);
            #endif
            io.send(&p, true);
        #endif
        }
        
            
    } /* namespace : board */
    
} /* namespace : utils */







#endif /* ARDUINO*/

