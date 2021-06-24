/*
 * ipacket.cpp
 *
 *  Created on: Dec 27, 2020
 *      Author: n00b
 */

#include "ipacket.h"

char b_description[SERIAL_TX_BUFFER_SIZE];
char b_label[LEN_LABEL];
//      _____________________________________________________________________
//      |                                                                   |
//      |       Generic ipacket: base implementation                        |
//      |___________________________________________________________________|
//

namespace packet
{    
    ipacket::ipacket()
    {
        _pid=rand();
    }

    ipacket::ipacket(iCom_t command):ipacket()
    {
        this->init(command);
    }

    ipacket::ipacket(ipacket * source)
    {
        memcpy(&_pid, &source->_pid, sizeof(iPid_t));
        init(source->command());
        
        iSize_t s=data_size();
        if (s > 0)
            memcpy(_payload, source->payload(), s);
    }

    ipacket::ipacket(const ipacket & source)
    {
        memcpy(&_pid, &source._pid, sizeof(iPid_t));
        init(source.command());
        iSize_t s=data_size();
        if (s > 0)
            memcpy(_payload, source.payload(), s);
    }

    ipacket::ipacket(char * head, bool wbody)
    {
        parse(head, wbody);
    }
    
    ipacket::~ipacket()
    {
        debug_mem("deleting [%u] %s", _pid, label());
        if (_payload != nullptr)
        {
            free(_payload);
            _payload = nullptr;
        };
    }

    bool            ipacket::init               (iCom_t com)
    {
        iCom_t c;
        /* Roll pmap::Map till matching com */
        for (iCom_t i=0; i<TOTAL_IBACOM; i++)
        {
            memcpy_P(&c, &PacketMap[i], sizeof(c)); /* read just the first 2 bytes */ 
            if (c == com)
            {
                _pid=rand();
                if (command() == com)
                {
                    this->clear();
                }
                else
                {
                    _flash_index=i;
                    iSize_t size=this->data_size();
                    
                    #if defined(ARDUINO)
                    utils::board::can_alloc(size);
                    #endif                                       
                    if (_payload != nullptr)
                    {  
                        free(_payload);
                        _payload=nullptr;
                    }
                    if (size > 0)
                    {
                        _payload = (char *)calloc(size, sizeof(char));
                        debug_mem("ipacket:%s [%u]: allocate %u bytes", label(), _pid, size);
                    }
                }
                return true;
            }
        }
        _flash_index=0;
        if (_payload != nullptr)
        {
            free(_payload);
            _payload = nullptr;
        }
        error_pack("init: invalid command [%u]", com);
        return false;
    }

    iSize_t         ipacket::data_size          (void) const
    {
        iSize_t size=0;
        
        packet_fst_ p;
        variable_fst_ s;
        memcpy_P(&p, &PacketMap[_flash_index], sizeof(p));
        for (uint8_t  n=0; n<p.count; n++)
        {
            memcpy_P(&s, &VariableMap[p.params[n]], sizeof(s));
            size += s.size;
        }
        return size;
    }

    iSize_t         ipacket::full_size          (void) const
    { 
        return (data_size() + SIZEOF_PACKET_HEADER);
    }

    void            ipacket::clear              (void)
    {
        if (_payload != nullptr)
            memset(_payload, 0, this->data_size());
    }

    char        *   ipacket::label              (void) const
    {
        memset(b_label, 0, LEN_LABEL);
        strcpy_P(b_label, (char*)&PacketMap[_flash_index] + sizeof(iCom_t) + 1);
        return (char *)b_label;
    }

    iCom_t          ipacket::command            (void) const
    {
        iCom_t c;
        memcpy_P(&c, &PacketMap[_flash_index], sizeof(c)); /* read just the first 2 bytes */ 
        return c;    
    }

    char        *   ipacket::description        (void) const
    {
        iSize_t size=data_size();
        memset(b_description, 0, SERIAL_TX_BUFFER_SIZE);
    
        /* <LABEL:COMMAND:PID:SIZE:FULL> */
        
        #if defined (INDOORINO_NETWORK)
            sprintf(b_description, "<%s:%u:%u:%u:%lu>",label(), command(), _pid, size, size + SIZEOF_PACKET_HEADER);
        #else
            sprintf(b_description, "<%s:%u:%lu:%u:%u>",label(), command(), _pid, size, size + SIZEOF_PACKET_HEADER);
        #endif
        return b_description;
    }
    
    void            ipacket::header             (char *buffer, const char *source, const char *target)
    {
        memset(buffer, 0, SIZEOF_PACKET_HEADER);
        
        iSize_t i=0;
        iCom_t c=command();
        memcpy_P(buffer, PREAMBLE, SIZEOF_PREAMBLE);    i +=SIZEOF_PREAMBLE;
        memcpy(buffer + i, &c, sizeof(iCom_t));         i +=sizeof(iCom_t);
        
        if (utils::is_devname(source)) { strcpy(buffer + i, source); }
        else { error_pack("header: invalid source <%s>", source); }
        i +=LEN_NAME;

        if (utils::is_devname(target)) { strcpy(buffer + i, target); }
        else {  error_pack("header: invalid target <%s>", target); }
        i +=LEN_NAME;
        
        memcpy(buffer + i, &_pid, sizeof(iPid_t));
        // i +=sizeof(iPid_t);
    }
    
    void            ipacket::forge              (char *buffer, const char *source, const char *target)
    {
        iSize_t s_data=data_size();
        iSize_t s_full=s_data + SIZEOF_PACKET_HEADER;
        
        char head[SIZEOF_PACKET_HEADER];
        header(head, source, target);
        
        memset(buffer, 0, s_full);
        memcpy(buffer, head, SIZEOF_PACKET_HEADER);
        memcpy(buffer + SIZEOF_PACKET_HEADER, _payload, s_data);
    }
    
    bool            ipacket::parse              (char * head, bool wbody)
    {
        /*  Read header and initialize packet. Header shape:
         *  [ PREAMBLE ][ COMMAND ][ SOURCE ][ DESTINATION ][ pid ]
         */
        
        char buffer[LEN_NAME] {0};
        iCom_t command {0};
        iSize_t i {0};
        
        if (memcmp_P(head, PREAMBLE, SIZEOF_PREAMBLE) != 0)
        {
            warning_pack("parse:invalid preamble");
            goto throw_header_error;
        }
        
        i += SIZEOF_PREAMBLE;
        memcpy(&command, head + i, sizeof(iCom_t));
        i += sizeof(iCom_t);
                
        strcpy(buffer, head + i);       // Copying source
        i += LEN_NAME;

        if (!utils::is_devname(buffer))
        {
            warning_pack("parse:invalid source");
            goto throw_header_error;            
        }

        memset(buffer, 0, LEN_NAME);
        strcpy(buffer, head + i);       // Copying target
        i += LEN_NAME;

        if (!utils::is_devname(buffer))
        {
            warning_pack("parse:invalid target");
            goto throw_header_error;
        }
        
        if (!init(command))             // init packet
        {
            goto throw_header_error;
        }

        memcpy(&_pid, head + i, sizeof(iPid_t));
        
        if (wbody)
        {
            i += sizeof(iPid_t);
            memcpy(_payload, head + i, data_size());
        }
        
        return true;
        
        throw_header_error:
            error_pack("parse:invalid header");
            utils::dump_hex(head, SIZEOF_PACKET_HEADER);
            return false;
    }

    void            ipacket::checksum           (uint8_t *buffer)
    {
        iSize_t size = data_size();
        char * data = (char *)calloc(size, sizeof(char));
        memset(buffer, 0, SHA_DIGEST_LENGTH);
        memcpy(data, _payload, size);
        
        #if defined (ESP8266)
            sha1(data, size, buffer);
        #elif defined (INDOORINO_CLIENT)
            SHA1((unsigned char *)data, size, (unsigned char *)buffer);       
        #endif

        free(data);
    }
    
    void            ipacket::dump               (void)
    {
        #if defined(DEBUG_PACKETDUMP)
        debug_pack("%s", description());

        iSize_t i=0;
        packet_fst_ p;
        variable_fst_ s;
        char buffer[SERIAL_TX_BUFFER_SIZE];
        memcpy_P(&p, &PacketMap[_flash_index], sizeof(p));
        for (uint8_t  n=0; n<p.count; n++)
        {
            memcpy_P(&s, &VariableMap[p.params[n]], sizeof(s));
            switch (s.type)
            {
                case 'c':
                {
                    if (utils::is_readable(_payload + i, 1, SERIAL_TX_BUFFER_SIZE))
                    {
                        memset(buffer, 0, SERIAL_TX_BUFFER_SIZE);
                        strncpy(buffer, _payload + i, s.size);                        
                        debug_pack("\t --> %s: <%s>",s.name, buffer);
                    }
                    else
                    {
                        if (memvcmp(_payload + i, 0, s.size))
                        {
                            debug_pack("\t --> %s: EMPTY", s.name);
                        }
                        else
                        {
                            debug_pack("\t --> %s:", s.name);
                            utils::dump_hex(_payload + i, s.size);
                        }                        
                    }
                    break;
                }
                case 's':
                {
                    uint8_t c=0;
                    memcpy(&c, _payload + i, sizeof(uint8_t));
                    debug_pack("\t --> %s: [ %u ]",s.name, c);
                    break;
                }
                case 'u':
                {
                    uint32_t c=0;
                    memcpy(&c, _payload + i, sizeof(uint32_t));
                    debug_pack("\t --> %s: [ %u ]",s.name, c);
                    break;
                }
                case 'n':
                {
                    int32_t c=0;
                    memcpy(&c, _payload + i, sizeof(int32_t));
                    debug_pack("\t --> %s: [ %d ]",s.name, c);
                    break;
                }
                case 'h':
                {
                    debug_pack("\t --> %s: HASHES", s.name);
                    #if defined(DEBUG_PACKET)
                    utils::dump_hex(_payload + i, s.size);
                    #endif
                    break;
                }
                default:
                {
                    error_pack("VariableMap on dump(): invalid type <%c>", s.type);
                }
            }
            i += s.size;
        }

    #endif
    }

    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Full ipacket: has source and destination buffer             |
//      |___________________________________________________________________|
//
    

    netpacket::netpacket(char * head, bool wbody):ipacket(head, wbody)
    {
        strcpy(this->source, head + SIZEOF_PREAMBLE + sizeof(iCom_t));
        strcpy(this->target, head + SIZEOF_PREAMBLE + sizeof(iCom_t) + LEN_NAME);
    }
    
    netpacket::netpacket(const netpacket &p):ipacket((ipacket&)p)
    {
        strcpy(this->source, p.source);
        strcpy(this->target, p.target);
    }

    netpacket::netpacket(ipacket * p, const char * s, const char * t):ipacket(p)
    {
        strcpy(this->source, s);
        strcpy(this->target, t);
    }
        
    netpacket::netpacket(netpacket * p):ipacket( (ipacket *)p )
    {
        strcpy(this->source, p->source);
        strcpy(this->target, p->target);
    }

    void            netpacket::header           (char * buffer)
    {
        ipacket::header(buffer, this->source, this->target);
    }

    void            netpacket::forge            (char * buffer)
    {
        ipacket::forge(buffer, this->source, this->target);
    }

    schedPacket::schedPacket(netpacket * p):netpacket((netpacket *)p) { };


    
    
    
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Transient ipacket: has timer and counter                    |
//      |___________________________________________________________________|
//

#if defined(__linux__)

    bufpacket::~bufpacket()
    { 
        if (txdata != nullptr)
        {
//             std::cout << "FREE!" << std::endl;
            free(txdata);
            txdata = nullptr;
        }
    }
    
#endif /* __linux__ */

} /* namespace packet */





