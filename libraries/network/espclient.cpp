/*
 * espclient.cpp
 *
 *  Created on: Sep 04, 2020
 *      Author: n00b
 */

#if defined(ESP8266)
#include "../common/icommon.h"

/* 
 * Questo è il nome della boaard attaccata al seriale.
 * Andrebbe messa dentro utils::board::io.serialBoardName() con un #if defined(ESP8266)
 */

char childname[LEN_NAME] {0};

//      ________________________________________________________________________
//      |                                                                      |
//      |    WiFi Keep Alive Connection loop                                   |
//      |______________________________________________________________________|
//

NetworkConnection::NetworkConnection()
{
    _last = millis();
    WiFi.mode(WIFI_STA);
    strcpy(childname, "ALL");
}

void                    NetworkConnection::begin        (void)
{    
   
    alert_net("Connecting %s to %s... ",P2C(INDOORINO_TYPE), conf.ssid());
    WiFi.begin(conf.ssid(), conf.psk());

//     char ssid[LEN_SSID] {0};
//     char psk[LEN_PSK] {0};
//     strcpy(ssid, P2C(DEFAULT_SSID));
//     strcpy(psk, P2C(DEFAULT_PSK));
//     alert_net("Connecting %s to %s... ",P2C(INDOORINO_TYPE), ssid);
//     WiFi.begin(ssid, psk);
    
    _reqtime=millis() + TIME_NETWORK_CONNECT;
}

void                    NetworkConnection::loop         (void)
{
    if (_status)
    {
        /* If connected to Wifi, just check for connection status every TIME_CONNECTION_CHECK */
        if (millis() - _last < TIME_NETWORK_CHECK)
        {
            _last = millis();
            if (!WiFi.isConnected())
            {
                warning_net("Connection Lost! Reconnecting...");
                _status=false;
                _counter++;
                _reqtime=millis() + TIME_NETWORK_CONNECT;
                
                blinker.start(1);
                client.stop();
                WiFi.reconnect();
            }
        }
    }
    else
    {
        if (millis() < _reqtime)
        {
            if (WiFi.isConnected())
            {
                char ip[INET_ADDRSTRLEN];

                _status=true;
                _last=millis();
                blinker.stop();
                
                WiFi.localIP().toString().toCharArray(ip, INET_ADDRSTRLEN);    
                info_net("Connected to %s. IP %s (channel %u)!",conf.ssid() , ip, WiFi.channel()); // P2C(DEFAULT_SSID)
//                 sendReport(1, BOARD_NAME, F("Connected to %s. IP%s"),conf.ssid(), ip);  
                client.begin();
            }
        }
        else
        {
                warning_net("Reconnection timed out! Retrying...");
                _status=false;
                _reqtime=millis() + TIME_NETWORK_CONNECT;
                begin();
        }
        
    }
}

//      _________________________________________________________________________
//      |                                                                       |
//      |    Indoorino TCP client                                               |
//      |_______________________________________________________________________|
//

IndoorinoEspClient::IndoorinoEspClient() 
{
    _serveraddress.fromString(P2C(DEFAULT_SERVER_IP));
    _serverport=DEFAULT_BOARD_PORT;
}

IndoorinoEspClient::~IndoorinoEspClient()
{
    stop();
}

                /*      B E G I N      */
bool            IndoorinoEspClient::begin           (void)
{
//     static bool=true;
    
    if (network.status())
    {
        stop();
        int result = _client.connect(_serveraddress, _serverport);
        char ip[INET_ADDRSTRLEN] {0};
        if (result)
        {
            char rip[INET_ADDRSTRLEN] {0};
            _client.localIP().toString().toCharArray(ip, INET_ADDRSTRLEN);
            _client.remoteIP().toString().toCharArray(rip, INET_ADDRSTRLEN);

            info_client("ESP %s:%u connected to %s:%u",
                        ip,  _client.localPort(),
                        rip, _client.remotePort());
            
            _client.keepAlive();
            debug_net("client status:\n\tconnected:[%d]\n\tstatus:[%d]\n\tavailable:[%d]",
                      _client.connected(), _client.status(), _client.available());
            
            if (_client.status() !=4)   { warning_client("Client unusual state [%d]", client.status()); }

//             _client.setTimeout(TIMEOUT_CLIENT);
//             _client.setTimeout(conf.timeout_client());
            
            /*     this should set new AES key;     */
            _aes.begin();            
            char key[LEN_AES_MASTERKEY] {0};
            _aes.generate(key);
            utils::board::send_aes_setup(key);
            if (this->write_packet())
            {
                _aes.set_key(key);
            }
            else
            {
                warning_client("Could not send new key!");
            }
            utils::wait(200);
            
            utils::board::send_config();
//             if (bootflag)
//             {                utils::board::send_boot_signal();
// 
//                 bootflag=false
//             }

            // Ask update to child board via serial 
            packet::netpacket p(IBACOM_REQUEST_CONFIG);
            strcpy_P(p.source, BOARD_NAME);
            strcpy(p.target, childname);
            utils::board::io.sendSerial(&p);
            
//             utils::board::io.sendSerial(
//                 new packet::netpacket(
//                     new packet::ipacket(IBACOM_REQUEST_STATUS), 
//                     P2C(BOARD_NAME),
//                     child_board_name));
            
            return true;
        }
        else
        {
            _serveraddress.toString().toCharArray(ip, INET_ADDRSTRLEN);
            error_client("Can not connect to %s:%u [error %d]",ip, _serverport, result); }
    }
    return false;
}


                /*       S T O P       */
void            IndoorinoEspClient::stop            (void)
{
//     debug_client("stop: c.connected=%d, c.status=%d", _client.connected(), _client.status());
    if (_client.connected() || _client.status())
    {
        char ip[INET_ADDRSTRLEN] {0};
        _client.remoteIP().toString().toCharArray(ip, INET_ADDRSTRLEN);
        alert_client("closing server connection %s:%u", ip, _client.remotePort());
        
        _client.flush();
        utils::wait(100);
        _client.stop();
    }
    // _aes.begin(); // is it necessary?
    _status=false;

}

                /*   R E A D    P A C K E T   */
bool            IndoorinoEspClient::read_packet     (void)
{
    
    char        b_head[SIZEOF_NET_HEADER] {0};
        
    if (!read_data(b_head, SIZEOF_NET_HEADER)) { return false; }

    iSize_t     size;
    bool        flag_invalidata=false;
    char    *   b_cipher;
    char    *   b_clear;

    debug_mem("@peak BEFORE read_packet: %u KB", utils::board::available_ram() - SIZEOF_NET_HEADER);    
    debug_tcp("SIZEOF_NET_HEADER [%u] bytes available", SIZEOF_NET_HEADER);
    
    if (memcmp_P(b_head, PREAMBLE, SIZEOF_PREAMBLE) != 0)
    {
        if (!flag_invalidata)
        {
            flag_invalidata=true;
            error_net("Invalid header preamble");
        }
        utils::dump_hex(b_head, SIZEOF_NET_HEADER);
        return false;
    } 
    else { flag_invalidata=false; }
    debug_tcp("preable pass!");
    

    memcpy(&size, b_head + SIZEOF_PREAMBLE, sizeof(iSize_t));
    if (size > MAX_SIZE_PACKET)
    {
        error_net("read_body: invalid size [%u>%u]", size, MAX_SIZE_PACKET);
        return false;
    }
    

    b_cipher=(char *)calloc(size, sizeof(char));
    debug_tcp("downloading body [%u] Bytes", size);
    
    iEpoch_t timeout = millis() + TIMEOUT_CLIENT_READ;
    while (! _client.available())
    {
        if (millis() > timeout)
        {
            error_net("Read body init timeout!");
            return false;
        }
    }
    
    if (!read_data(b_cipher, size))
    { 
        error_client("could not read data!");
        return false;
    }
    
    char b_iv[N_BLOCK];
    b_clear=(char *)calloc(size, sizeof(char)); /* HERE I alloc <size> bytes hoping <size> of encrypted data is bigger than clear one */
    memcpy(b_iv, b_head + SIZEOF_PREAMBLE + sizeof(iSize_t), N_BLOCK);
    
    debug_tcp("decrypting data");

    _aes.decrypt(b_clear, b_iv, b_cipher, size);
    
    #if defined(DEBUG_TCP)
        debug_tcp("PACKET READ");
        utils::dump_hex(b_clear, size);
        debug_tcp("** CIPHER DECRYP KEY **");
        utils::dump_hex(_aes.masterkey(), LEN_AES_MASTERKEY);        
    #endif
    
        packet::netpacket * p = new packet::netpacket(b_clear, true);
    
    debug_mem("@peak AFTER read_packet: %u KB", utils::board::available_ram());
    
    if (p->command() == 0)
    {
        warning_net("dowloaded data returned an invalid packet!");
        utils::dump_hex(b_clear, 100);
        delete p;
        free(b_clear);
        free(b_cipher);
        return false;
    }
    
    on_packet(p);
    free(b_clear);
    free(b_cipher);
    return true;
}

                /*       S E N D       */
void            IndoorinoEspClient::send            (packet::netpacket * p, bool direct)
{
    if (p == nullptr || p->command() == 0)
    {
        warning_net("send: invalid packet!");
        return;
    }
    
    packet::netpacket * r = new packet::netpacket(p);
    
    if (direct)     { _txqueue.push_front(r); }
    else            { _txqueue.push_back(r);  }
    debug_mem("@peak BEFORE send_packet: %u KB", utils::board::available_ram());

}

                /*       S E N D       */
void            IndoorinoEspClient::send            (packet::ipacket * p, bool direct)
{
    packet::netpacket r(p);
    
    strcpy_P(r.source, BOARD_NAME);
    strcpy_P(r.target, DEFAULT_TARGET);
        
    this->send(&r, direct);
}           

                /*    R E A D    D A T A    */
bool            IndoorinoEspClient::read_data       (char * buffer, iSize_t size)
{
    iSize_t available = _client.available();
    if (available == 0) { return false; }
    
    debug_tcp("downloading %u Bytes", size);
    
    iEpoch_t    timeout = millis() + TIMEOUT_CLIENT_READ;
    iSize_t     total = 0;
    debug_tcp("downloading %u Bytes", size);
    while (total < size)
    {
        iSize_t r = _client.read((uint8_t *)(buffer + total), size - total);
        if (r > 0)
        {
            total += r;
            debug_tcp("done [%u]", total);
        }
        else 
        {
            error_net("read: incomplete data!");
            utils::dump_hex(buffer, total);
            return false;
        }
        if (millis() > timeout)
        {
            warning_net("read: data timed out!");
            utils::dump_hex(buffer, total);
            return false;            
        }
    }
    return true;
}

                /*    O N    P A C K E T    */
void            IndoorinoEspClient::on_packet       (packet::netpacket * p)
{
    
    debug_net("on_packet from <%s> to <%s>", p->source, p->target);
    p->dump();
    
    uint8_t hash[SHA_DIGEST_LENGTH];
    if (p->command() == IBACOM_ACK)
    {
        memcpy(hash, p->p_hash1(), SHA_DIGEST_LENGTH);
//         if (!match(hash)) warning_client("Unmatched hash");        
        delete p;
        return;
    }
    
    p->checksum(hash);
    
    packet::ipacket ack(IBACOM_ACK);
    memcpy(ack.p_hash1(), hash, SHA_DIGEST_LENGTH);
    this->send(&ack, true);

    _rxqueue.push_back(p);
    
//     here you can: delete packet and make this function do all the job
//     or add p to rxqueue and handle packet outside (in the main loop)
}
                /*   W R I T E   P A C K E T   */
bool            IndoorinoEspClient::write_packet    (void)
{
    
    if (_txqueue.is_empty())
    {
        return false;
    }
    
    debug_mem("@peak BEFORE write_packet: %u KB", utils::board::available_ram());
    packet::netpacket * p=_txqueue.front();
    
    iSize_t size=p->full_size();
    if (_client.availableForWrite() <= size)
    {
        warning_client("tcp write buffer full");
        utils::wait(BUSY_NETWORK);
        _client.flush();
    }
    if (_client.availableForWrite() <= size)
    {
        stop();
        _overflows++;
        return false;
    }
    
    
    char    b_header[SIZEOF_NET_HEADER] {0};
    char *  b_cipher;
    char *  b_clear;
    char    b_iv[N_BLOCK] {0};
    
    /* WARNING: we assume that total cipher size is sum of aes lib suggested size PLUS clear size */
    b_clear = (char *)calloc(size, sizeof(char));
    b_cipher= (char *)calloc(_aes.cipherlen(size), sizeof(char));
    p->header(b_clear);
    memcpy(b_clear + SIZEOF_PACKET_HEADER, p->payload(), p->data_size());

    iSize_t s_cipher = _aes.encrypt(b_cipher, b_iv, b_clear, size);
    
    memcpy_P(b_header, PREAMBLE, SIZEOF_PREAMBLE);
    memcpy(b_header + SIZEOF_PREAMBLE, &s_cipher, sizeof(iSize_t));
    memcpy(b_header + SIZEOF_PREAMBLE + sizeof(iSize_t), b_iv, N_BLOCK);
    
    #if defined DEBUG_TCP
    debug_tcp("write: dump network header:");
    utils::dump_hex(b_header, SIZEOF_NET_HEADER);    
    debug_tcp("write: dump clear buffer:");
    utils::dump_hex(b_clear, size);
    debug_tcp("write: dump encrypted buffer:");
    utils::dump_hex(b_cipher, size);
    debug_tcp("** CIPHER ENCRYPT KEY **");
    utils::dump_hex(_aes.masterkey(), LEN_AES_MASTERKEY);
    #endif

    free(b_clear);
    
    do
    {
        if (! (_client.write(b_header, SIZEOF_NET_HEADER) == SIZEOF_NET_HEADER))    { break; }
        
//         if (utils::random_signed(0, 30) > 22) { s_cipher -= 5; } // This is an ugly und hateful way to trick errors

        if (! (_client.write((uint8_t*)b_cipher, s_cipher) == s_cipher))            { break; }
        
        _total_tx++;
        free(b_cipher);
        delete _txqueue.pop_front();
        
        debug_client("Wrote %u bytes", SIZEOF_NET_HEADER + s_cipher);
        return true;
    }
    while (0);
    
    free(b_cipher);
    error_client("commit: unable to send data!");
    return false;

}

                /*       L O O P       */
void            IndoorinoEspClient::loop            (void)
{
    if (utils::board::available_ram() < SRAM_LIMIT)
    {
        _unsent=_txqueue.count();

        _rxqueue.clear();
        _txqueue.clear();
        
        warning_mem("LOW SRAM:Freeing serial buffers");
    }

    static iEpoch_t timeout=0;

    
//     static iEpoch_t dubg_tm=0;
//     if (millis() > dubg_tm)
//     {
//         info_net("Connected:[%d] status:[%d] available:[%d]",_client.connected(), _client.status(), _client.available());
//         dubg_tm = millis() + 750;
//     }
    
    while (!utils::board::io.incoming().is_empty())
    {
        packet::netpacket * p = utils::board::io.incoming().pop_front();
        debug_io("Rerouting packet %s", p->description());
//         p->dump();
        strcpy(childname, p->source);
        this->send(p);
        delete p;
    }
    
    /* CHECK CONNECTION */
    
    if (!network.status()) return;
        
    if (!_client.connected())
    {
        if (_status)
        {
            info_client("Disconnected !");
            stop();
        }
        if (millis() > timeout)
        {
            timeout = millis() + TIME_BIND_CONNECT;
            begin();
        }
        return;
    } 
    else if (!_status)
    {
        _status=true;
    }
    
    /* READ INCOMING PACKETS */
    
    if (read_packet()) {/*implement something*/}
        
    /* SEND OUTGOING PACKETS */
    
    /* TODO here implement start stop for hash */

    if (write_packet())
    {
        debug_mem("@peak AFTER write_packet: %u KB", utils::board::available_ram());        
    }
 
    
}

                /*      M A T C H       */ 
bool            IndoorinoEspClient::match           (const char * hash) //TODO
{
    if (hash) return true;
    return false;
}

#endif /* ESP8266 */



























    
//     for (uint16_t i=0; i<_sendlist.len(); i++)
//     {
//         if (_sendlist[i]->expired())
//         {
//             _unmatched++;
//             _sendlist.rem(i);
//             loop();
//             return;
//         }
//         if (_sendlist[i]->ready())
//         {
//             ibasize_t size = _sendlist[i]->packet->full_size();
//             debug_mem("Allocating tcp buffer %u", size);
//             char * data = (char *)calloc(size, sizeof(char));
//             _sendlist[i]->forge(data);
//             if (!commit(data, size))
//             {
//                 ibaepoch_t ima = millis();
//                 if (ima <= faillast)
//                 {
//                     faillast = ima + rtc.looptime();
//                     failcount++;
//                     if (failcount == 100)
//                     {
//                         failcount=0;
//                         error_net("Could not send! Reconnecting...");
//                         stop();
//                         return;
//                     }
//                 }
//                 warning_net("network busy! waiting %u ms.", BUSY_NETWORK);
//                 wait(BUSY_NETWORK);
//                 _unsent++;                
//             }
//             free(data);
//         }
//     }
