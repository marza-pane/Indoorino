/*
 * eepromConf.cpp
 *
 *  Created on: Oct 27, 2019
 *  Edited on May 15 2020
 *      Author: n00b
 */

#if defined (ARDUINO)
#include "common.h"


//      _____________________________________________________________________
//      |                                                                   |
//      |       BOARD DEFAULT CONFIGURATION                                 |
//      |___________________________________________________________________|
//

Conf_Board::Conf_Board()
{
    
}

Conf_Board::~Conf_Board()
{
    debug_dev("conf:deleted!");
}

void        Conf_Board::begin               (void)
{
    staticspace.begin();
    
    const char * ptr[3] = {BOARD_NAME, INDOORINO_TYPE, BOARD_TYPE};
    char buf[LEN_NAME];
    uint16_t offset =0;
    
    for (uint8_t i=0; i<3; i++)
    {
        debug_dev("conf:std: parameter %u: <%s>", i, P2C(ptr[i]));
        memset(buf, 0, LEN_NAME);
        for (uint16_t j=0; j<LEN_NAME; j++)
        { 
            buf[j]  = staticspace.read(j + offset);
        }
        offset += LEN_NAME;
        debug_dev(" [ %s ] ",buf);
        if (strcmp_P(buf, ptr[i]) != 0)
        {
            error_dev("conf:begin: failed");
            Conf_Board::factory();
            this->begin();
            return;
        }   
        
    }

    if (devnum() > MAX_ATTACHED_DEVICES)
    {
        error_dev("conf:begin: too many devices %u", devnum());
//         sendReport(3, _id, F("%s%s%u"), P2C(_k_niConfBoard_), P2C(dst_dev_1), devnum());
        Conf_Board::factory();
        this->begin();
    }
    
    info_dev("conf:loaded board configuration!");
}

void        Conf_Board::factory             (void)
{
    char        d[3][LEN_NAME];

    memset(d[0], 0, LEN_NAME);
    memset(d[1], 0, LEN_NAME);
    memset(d[2], 0, LEN_NAME);
    
    strcpy_P(d[0], BOARD_NAME);
    strcpy_P(d[1], INDOORINO_TYPE);
    strcpy_P(d[2], BOARD_TYPE);
    
    alert_dev("conf:factory reset");
    
    for (uint8_t n=0; n<3; n++)
    {
        debug_dev(" >>>\tWriting base prop %u : %s", n, (char *)d[n]);
        for (uint8_t i=0; i<LEN_NAME; i++)
        { staticspace.update( (n * LEN_NAME) + i, d[n][i]); }
    }
    staticspace.write( 3 * LEN_NAME, DEFAULT_DEVNUM);

    if(DEFAULT_DEVNUM == 0)
        for (uint8_t i=SIZEOF_STDCONF; i<SIZEOF_STDCONF+100; i++)
            staticspace.update(SIZEOF_STDCONF, 0);
    
    utils::wait(500);
    staticspace.update();  
    utils::wait(500);

#if defined(DEBUG_DEVICES)
    for (uint8_t n=0; n<3; n++)
    {
        debug_dev(">>>\tReading base prop %u: ", n);
        SerialDebugPrint(n);
        for (uint8_t i=0; i<LEN_NAME; i++)
        {
            SerialDebugPrint((char)staticspace.read((n * LEN_NAME) + i));
        }    
    }
#endif
    debug_dev("Done resetting conf! (devnum = %u)", devnum());
}

//      _____________________________________________________________________
//      |                                                                   |
//      |       AVR BASE CONFIGURATION                                      |
//      |___________________________________________________________________|
//


void        Conf_AVR::initdev               (iSize_t start)
{
    _dpos[0]=start;    

    uint8_t         n=devnum();
    uint8_t         m=0;
    iCom_t          com=0;    
    iSize_t         index=_dpos[0];
    
    packet::ipacket * p=new packet::ipacket();
    
    for (m=0; m<n; m++)
    {
        
        staticspace.get(index, com);
        debug_dev("conf:init: [%u/%u] <%u> at index %u >>>", m, n, com, index);
        index +=sizeof(iCom_t);

        p->init(com);
        if (p->command() == 0)
        {
            error_mem("initdev: init failed");
            break;
        }
        
        if (p->data_size() == 0)
        {
            error_dev("conf:init: invalid [%s] at index %u", F2C(p->label()), index);
            break;
        }
        
        for (iSize_t j=0; j<p->data_size(); j++)
        {
            p->payload()[j] = staticspace.read(index);
            index++;
        }
        
        debug_dev(">>> Loaded %s on pin %u", p->p_devname(), *p->p_pin1());

        _dpos[m+1]=index;
    }
    
    delete p;
        
    if (m != n)
    {
        warning_dev("init: truncating array at %u/%u", m, n);

        /* sendReport(2, _id, F("%s"),buf); */
        staticspace.write( 3 * LEN_NAME, m); // update devnum() 
        staticspace.update();
    }

    #if defined(DEBUG_DEVICES)
        debug_dev("conf:init: dev array:");
        SerialDebugPrint("[ ");
        for(uint8_t i=0; i<devnum() + 1; i++)
        {
            SerialDebugPrint(_dpos[i]);
            SerialDebugPrint(" ");
        }
        SerialDebugPrint("]\n\n");
    #endif /* DEBUG_DEVICES */
}

void        Conf_AVR::device_name           (char * buffer, int8_t index)
{
    memset(buffer, 0 ,LEN_DEVNAME);
    if (index >= 0 && index < devnum())
    {
        for (uint8_t i=0; i<LEN_DEVNAME; i++)
        {
            char c = staticspace.read(_dpos[index] + sizeof(iCom_t) + LEN_NAME + i);
            memcpy(buffer + i, &c, sizeof(char));
        }
        return;
    }
    sprintf(buffer, "ERROR%u", index);
    error_dev("device_name: invalid index %u", index);
}

int8_t      Conf_AVR::indexFromName         (const char * name)
{
    char            n_buff[LEN_DEVNAME];
    
    if (!name) return -1;
    
    for (uint8_t i=0; i<devnum(); i++)
    {
        memset(n_buff, 0 ,LEN_DEVNAME);
        staticspace.get(_dpos[i] + sizeof(iCom_t) + LEN_NAME, n_buff);
        if (strcmp(name, n_buff) == 0)
        {
            return i;
        }
    }
            
    return -1;
    
}

int8_t      Conf_AVR::indexFromPin          (int8_t pin)
{
    int8_t p=0;
    for (int8_t i=0; i<devnum(); i++)
    {
        p = int8_t(staticspace.read(_dpos[i] + sizeof(iCom_t) + LEN_NAME + LEN_DEVNAME));
        if (p == pin)   return i;
    }
    
    return -1;
}

iCom_t      Conf_AVR::device_command        (int8_t index)
{   
    if (index >= 0 && index < devnum())
    {
        iCom_t com=0;
        staticspace.get(_dpos[index], com);
        return com;
    }
    
    error_dev("device_command: invalid index [%d]", index);
    return 0;
}

int8_t      Conf_AVR::device_pin            (int8_t index)
{
    if (index >= 0 && index < devnum())
    {
        uint8_t p=0;
        staticspace.get(_dpos[index]  + sizeof(iCom_t) + LEN_NAME + LEN_DEVNAME, p);
        return p;
    }
    
    error_dev("device_pin: invalid index %u", index);
    return 0;    
}

packet::ipacket  *   Conf_AVR::device       (packet::ipacket * ptr, const char * name)
{
    int8_t index=indexFromName(name);
    
    if (index == -1)
    {
        error_dev("devCall: invalid name <%s>", name);
        return ptr;
    }
    return device(ptr, index);
}

packet::ipacket  *   Conf_AVR::device       (packet::ipacket * ptr, int8_t index)
{        
 
    iCom_t com=0;

    if (index < devnum())
    {
        staticspace.get(_dpos[index], com);
        if (!ptr->init(com))
        {
            error_mem("deviceCall invalid command [%u]", com);
            return ptr;
        }
        
        for (iSize_t j=0; j<ptr->data_size(); j++)
        {
            ptr->payload()[j] = staticspace.read(_dpos[index] + sizeof(iCom_t) + j);
        }
    }
    else
    {
        error_dev("deviceCall: invalid index [%u]", index);
    }
    return ptr;
}

uint8_t     Conf_AVR::validate              (packet::ipacket * ptr)
{
    if (ptr == nullptr)
    {
        error_dev("validate: got nullpointer");
        return 1;        
    }
    
    if (ptr->data_size() == 0)
    {
        warning_dev("validate: <%s> has zero size", F2C(ptr->label()));
        return 2;
    }
 // TODO: if (!ptr->is_devconf)
    if (false)
    {
        warning_dev("validate: <%s> is not a device", F2C(ptr->label()));
        return 3;
    }
    if (!utils::is_devname(ptr->p_devname()))
    {
        warning_dev("validate: <%s> has invalid devname %s", F2C(ptr->label()), ptr->p_devname());
        return 4;
    }
    uint8_t pin = *ptr->p_pin1();
    if (!utils::board::is_pin(pin))
    {
        warning_dev("validate: <%s> has invalid pin %u", F2C(ptr->label()), pin);
        return 4;
    }
    

    return 0;
}

bool        Conf_AVR::devAdd                (packet::ipacket * ptr)
{
    if (devnum() >= MAX_ATTACHED_DEVICES)
    {
        error_dev("devAdd: too many devices");
        return false;
    }
    
    if (validate(ptr) > 0) return false;

    char name[LEN_DEVNAME] {0};
    strcpy(name, ptr->p_devname());
    
    if (indexFromName(name) != -1)
    {
        warning_dev("devAdd: %s already defined", name);
        /* Here you can pass it to devMod */
        devMod(name, ptr);
        return false;
    }

    uint8_t pin = *ptr->p_pin1();
    
    if (indexFromPin(pin) != -1)
    {
        warning_dev("devAdd: pin %u alredy in use", pin);
        /* sendReport(2, _id, "%s<%s> : ", F2C(constring), F2C(ptr->label()), pin); */
        return false;
    }

    uint8_t     dvn=devnum();
    iSize_t   ndx=_dpos[dvn];
    
    staticspace.put(ndx, ptr->command());
    ndx+=sizeof(iCom_t);
        
    for (iSize_t j=0; j<ptr->data_size(); j++)
    {
        staticspace.update(ndx, ptr->payload()[j]);
        ndx++;
    }
    
    dvn++;
    staticspace.write( 3 * LEN_NAME, dvn);
    staticspace.update();

    _dpos[devnum()] = ndx;
    
    #if defined(DEBUG_DEVICES)
        debug_dev("conf:devAdd: added %s on pin %u!", name, pin);
        debug_dev("Total devices: %u ", dvn);
        SerialDebugPrint("[ ");
        for (uint8_t i=0; i<dvn; i++)
        {
            SerialDebugPrint(_dpos[i]);
            SerialDebugPrint(" ");
        }
        SerialDebugPrint("]\n\n");
    #endif /* DEBUG_DEVICES */
    
    //sendConfig();
    return true;
}

bool        Conf_AVR::devMod                (const char * name, packet::ipacket * ptr)
{
    int8_t n=indexFromName(name);

    if (n == -1)
    {
        warning_dev("devMod: invalid name <%s>", name);
        /* sendReport(2, _id, F("%sinvalid device %s"),F2C(constring), name); */
        return false;
    }
    
    if (validate(ptr) != 0) return false;

    uint8_t pin = *ptr->p_pin1();
    int8_t  px  = indexFromPin(pin);
    
    if (device_command(n) != ptr->command())
    {
        warning_dev("devMod: mismatching types [%u - %u]", device_command(n), ptr->command());
        return false;
    }
 
    
    if (px != -1 && px != n)
    {
        warning_dev("devMod: pin %u already in use ", pin);
        /* sendReport(2, _id, F("%spin %u alredy in use"),F2C(constring), pin); */
        return false;
    }
 
 //     uint8_t nameindex = devindex(name);
//     
//     if (validate(ptr) > 0) return false; /* Qui swicchare per devmod */
//     
//     uint8_t pin = *ptr->p_pin();
//     uint8_t index = devindexBypin(pin);

 
    debug_dev("conf:devMod: editing %s", ptr->p_devname());
    
    iSize_t ndx=_dpos[n] + sizeof(iCom_t);
      
    for (iSize_t j=0; j<ptr->data_size(); j++)
    {        
        staticspace.update(ndx, ptr->payload()[j]);
        ndx++;
    }
    //sendConfig();
    return true;
}

bool        Conf_AVR::devRem                (const char * name)
{
    uint8_t     dvn=devnum();
    int8_t      index=indexFromName(name);
    
    if (index == -1)
    {
        error_dev("devRem: invalid name <%s>", name);
        return false;
    }
    

    iSize_t ndx=_dpos[index];
    iCom_t command=0;
    staticspace.get(ndx, command);
    iSize_t size=sizeof(iCom_t) + packet::payloadsize(command);
    
    dvn--;
    staticspace.write( 3 * LEN_NAME, dvn);
    
    
    debug_dev("conf:devRem: removing from %u to %u", ndx, ndx + size);
    uint8_t   byte=0;
    iSize_t last=_dpos[dvn];
    for (iSize_t i=ndx; i<last; i++)
    {
        byte = staticspace.read(i + size);
        staticspace.update(i, byte);
    }

    initdev(_dpos[0]);
    //sendConfig();
    return true;
}

bool        Conf_AVR::devSetName            (const char * name, const char *new_name)
{

    if (!utils::is_devname(new_name,1,LEN_DEVNAME))
    {
        warning_dev("devSetName: invalid new name <%s>", new_name);
        /* sendReport(2, _id, F("%sname %s"), F2C(constring), new_name); */
        return false;
    }
    
    int8_t i = indexFromName(name);
    if (i == -1)
    {
        warning_dev("devSetName: invalid name <%s>", name);
        /* sendReport(2, _id, F("%sdevice %s"), F2C(constring), name); */
        return false;
    }
    
    info_dev("Changing <%s> name [%u] ==> [%u]", name, name, new_name);
    iSize_t ndx=_dpos[i] + sizeof(iCom_t) + LEN_NAME;
    for (iSize_t j=0; j<LEN_DEVNAME; j++)
    {
        staticspace.update(ndx, new_name[j]);
        ndx++;
    }
    //sendConfig();
    return true;
}

bool        Conf_AVR::devSetPin             (const char * name, uint8_t pin)
{
    
    /* const __FSH * constring =F("conf:set:pin: "); */
    int8_t nameindex = indexFromName(name);
    int8_t pinindex  = indexFromPin(pin);
    
    debug_dev("conf:setDevPin: %s will be set to pin %u", name, pin);
    debug_dev("Indices: name:%u pin:%u", nameindex, pinindex);
        
    if (nameindex == -1)
    {
        warning_dev("devSetPin: invalid name <%s>", name);
        /* sendReport(2, _id, F("%sinvalid device %s"), constring, name); */
        return false;
    }
    
    if (pinindex != -1 && nameindex != pinindex)
    {
        warning_dev("devSetPin: pin %u already in use", pin);
        /* sendReport(2, _id, F("%s pin %u already in use"), constring, pin); */
        return false;
    }

    iSize_t ndx=_dpos[nameindex] + sizeof(iCom_t) + LEN_NAME + LEN_DEVNAME;
    info_dev("Changing <%s> pin [%u] ==> [%u]", name, ndx, pin);
    
    staticspace.update(ndx, pin);
    return true;

}

//      _____________________________________________________________________
//      |                                                                   |
//      |       ESP ROUTER CONFIGURATION                                    |
//      |___________________________________________________________________|
//

#if defined(ESP8266)

void            ConfRouter::begin             (void)
{
    Conf_Board::begin();

    /*
    *  ___________________________________________________________________________________________
    *  |         | WiFi |  WiFi | remote | remote | timeout | timeout | attemps |
    *  | STDCONF | SSID |  PSK  |   IP   |  PORT  | client  | packet  | packet  |
    *  |____49___|__32__|__64___|___4____|___2____|____4____|____4____|____1____|
    *  
    */

    char        buf[LEN_PSK] {0};    
    bool        flag=false;
    iSize_t     ndx=SIZEOF_STDCONF;
    iEpoch_t    timeout=0;
    uint16_t    port=0;
    
    do
    {
        staticspace.read(ndx, buf, LEN_SSID);
        if (strlen(buf) < 6)
        {
            error_dev("conf:begin: invalid SSID %s", buf);
            flag=true;
            break;
        }
        memcpy(_ssid, buf, LEN_SSID);
        debug_dev("conf:esp SSID: <%s>", _ssid);
        ndx += LEN_SSID;
        
        staticspace.read(ndx, buf, LEN_PSK);
        if (strlen(buf) < 6)
        {
            error_dev("conf:begin: invalid PSK %s", buf);
            flag=true;
            break;
        }
        memcpy(_psk, buf, LEN_PSK);        
        debug_dev("conf:esp password: <%s>", _psk);
        ndx += LEN_PSK;
                        
        memset(buf, 0, LEN_PSK);
        uint8_t ipbuf[4];
        
        #if defined(DEBUG_DEVICES)
        char * bpt = buf;
        bpt += sprintf(bpt, "%s", F2C(F("conf:esp remote IP ["))); 
        #endif
        for (uint8_t i=0; i<4; i++)
        {
            ipbuf[i] = staticspace.read(ndx);
            #if defined(DEBUG_DEVICES)
            bpt += sprintf(bpt, "%u", ipbuf[i]);
            if (i<3) { bpt += sprintf(bpt, "."); }
            #endif
            ndx++;
        }
        #if defined(DEBUG_DEVICES)
        bpt += sprintf(bpt, "]");
        debug_dev("%s",buf);
        #endif

        IPAddress ip(ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]);
        if (!ip.isSet())
        {
            memset(buf, 0, LEN_PSK);
            ip.toString().toCharArray(buf,INET_ADDRSTRLEN);    
            error_dev("conf:begin: invalid remote IP %s", buf);
            flag=true;
            break;
        }
        _remote_address = ip;
        
        staticspace.get(ndx, port);
        debug_dev("conf:esp remote port: %u", port);
        _remote_port=port;
        ndx +=sizeof(uint16_t);   
                
//         staticspace.get(ndx, timeout);
//         debug_dev("conf:esp client timeout: %u", timeout);
//         _timeout_client=timeout;
//         ndx +=sizeof(iEpoch_t);
        
        staticspace.get(ndx, timeout);
        debug_dev("conf:esp packet timeout: %u", timeout);
        _timeout_packet=timeout;
        ndx +=sizeof(iEpoch_t);
        
        _attemps_packet=staticspace.read(ndx);
        debug_dev("conf:esp send attemps: %u", _attemps_packet);
        
    } while (false);
    
    if (flag)
    {
        this->factory();
        utils::board::reset();
        return;
    }
    
    info_dev("conf:loaded ESP router parameters!");
}

void            ConfRouter::factory           (void)
{

    alert_dev("esp:conf:factory reset");
    
    iSize_t   ndx=SIZEOF_STDCONF;
    
    char        buf_ssid[LEN_SSID]          {0};
    char        buf_psk[LEN_PSK]            {0};
    uint16_t    port=0;
    IPAddress   ip;
    iEpoch_t  timeout=0;
    uint8_t     attempts=0;
    
    memcpy_P(buf_ssid, DEFAULT_SSID, LEN_SSID);
    memcpy_P(buf_psk, DEFAULT_PSK, LEN_PSK);
    
    debug_dev("conf:esp:factory: ssid:%s", buf_ssid);
    staticspace.write(ndx, buf_ssid, LEN_SSID);
    ndx += LEN_SSID;
    
    debug_dev("conf:esp:factory: psk:%s", buf_psk);
    staticspace.write(ndx, buf_psk, LEN_PSK);
    ndx += LEN_PSK;
        
    ip.fromString((P2C(DEFAULT_SERVER_IP)));
    #if defined(DEBUG_DEVICES)
    char buf_ip[INET_ADDRSTRLEN] {0};
    ip.toString().toCharArray(buf_ip, INET_ADDRSTRLEN);
    debug_dev("conf:esp:factory: remote IP:%s", buf_ip);
    #endif   
    
    for (uint8_t i=0; i<4; i++)
    {
        staticspace.write(ndx, ip[i]);
        ndx++;
    }
    
    debug_dev("conf:esp:factory: remote port :%u", DEFAULT_BOARD_PORT);
    port=DEFAULT_BOARD_PORT;
    staticspace.put(ndx, port);
    ndx +=sizeof(uint16_t);   

/*    debug_dev("conf:esp:factory: timeout client: %u", TIMEOUT_CLIENT);
    timeout=TIMEOUT_CLIENT;
    staticspace.put(ndx, timeout);
    ndx +=sizeof(iEpoch_t);*/   

    debug_dev("conf:esp:factory: timeout packet: %u", TIMEOUT_SEND_PACKET);
    timeout=TIMEOUT_SEND_PACKET;
    staticspace.put(ndx, timeout);
    ndx +=sizeof(iEpoch_t);   

    debug_dev("conf:esp:factory: attempts packet: %u", RETRY_SEND_PACKET);
    attempts=RETRY_SEND_PACKET;
    staticspace.put(ndx, attempts);
    ndx +=sizeof(uint8_t);   

    for (uint8_t i=0; i<10; i++) { staticspace.write(ndx, 0); ndx++; }

    utils::wait(500);
    staticspace.update();  
    utils::wait(500);
    
}

void            ConfRouter::local_port          (const uint16_t)
{
    // TODO
    client.stop();
    utils::wait(500);
}

void            ConfRouter::remote_ip           (const IPAddress&)
{
    
}
void            ConfRouter::remote_port         (const uint16_t)
{
    
}
void            ConfRouter::timeout_packet      (const iEpoch_t)
{
    
}
void            ConfRouter::attemps_packet      (const uint8_t)
{
    
}

#endif /* ESP8266 */


//      _____________________________________________________________________
//      |                                                                   |
//      |       PROJECT CONFIGURATIONS                                      |
//      |___________________________________________________________________|
//


#if defined (INDOORINO_SAMPLER)

void        ConfSampler::begin              (void)
{
    Conf_Board::begin();
    
    uint32_t  par=0;
    staticspace.get(SIZEOF_STDCONF, par);
    if (par < DT_MIN_STEP || par > DT_MAX_STEP)
    {
        error_dev("Sampler:begin: invalid STEP <%u>", par); 
        /* sendReport(3, _id, F("%s:%s STEP <%u>"), P2C(_k_begin_), P2C(_k_invalid_), par); */
        this->factory();
        this->begin();
    }

    staticspace.get(SIZEOF_STDCONF + sizeof(uint32_t), par);
    if (par < DT_MIN_COOL || par > DT_MAX_COOL)
    {
//         sendReport(3, _id, F("%sCOOL TIME <%u>"), P2C(string_foundinvalid), par);
        error_dev("Sampler:begin: invalid COOL TIME  <%u>", par); 
        this->factory();
        this->begin();
    }
    
    initdev(SIZEOF_STDCONF + 2 * sizeof(uint32_t));
    debug_dev("%s:Sampler begin complete", F2C(_id));
}

void        ConfSampler::factory            (void)
{ 
        
    Conf_Board::factory();
    
    iSize_t   ndx=SIZEOF_STDCONF;
    uint32_t    par=0;

    par =DT_DEF_STEP;
    staticspace.put(ndx, par);
    ndx +=sizeof(uint32_t);

    par =DT_DEF_COOL;
    staticspace.put(ndx, par);
    ndx +=sizeof(uint32_t);
    
    default_dev_conf_template       dev;
    for (uint8_t i=0; i<DEFAULT_DEVNUM; i++)
    {
        memcpy_P(&dev, &DEFAULT_DEVCONF[i], sizeof(dev));

        packet::ipacket * data  = new packet::ipacket(dev.type);

        staticspace.put(ndx, dev.type);
        ndx+=sizeof(iCom_t);

        debug_dev("Writing sensor %u: %u - %s - on pin %u", i, dev.type,dev.name, dev.pin);
        
        strcpy(data->p_devname(), dev.name);
        strcpy(data->p_name(), P2C(BOARD_NAME));
        *data->p_pin() = dev.pin;
        
        if (data->p_param1() != nullptr) *data->p_param1() = 1;
        if (data->p_param3() != nullptr) *data->p_param3() = 1;
        
        for (iSize_t j=0; j<data->data_size(); j++)
            {
                staticspace.update(ndx, data->payload()[j]);
                ndx++;
            }
            debug_dev("Wrote sensor %u:%s - no pin %u\n", i, 
                     data->p_devname(),
                     *data->p_pin());
        delete data;
    }

    for(iSize_t i=ndx; i<ndx + 100; i++)
    {
        // trailing zeros
        staticspace.update(i, 0);
    }      
}

uint32_t    ConfSampler::step               (void)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF, par);
    return par;
}

uint32_t    ConfSampler::cool               (void)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + sizeof(uint32_t), par);
    return par;
}

void        ConfSampler::step               (uint32_t value)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF, par);
    if (par != value)
    {
        staticspace.put(SIZEOF_STDCONF, value);        
    }
    staticspace.update();

}

void        ConfSampler::cool               (uint32_t value)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + sizeof(uint32_t), par);
    if (par != value)
    {
        staticspace.put(SIZEOF_STDCONF + sizeof(uint32_t), value);        
    }
    staticspace.update();

}


#elif defined (INDOORINO_CONTROLLER)

void        ConfController::begin           (void)
{
    Conf_Board::begin();
    
//     uint32_t  par=0;
//     staticspace.get(SIZEOF_STDCONF, par);
    
    info_dev("conf:loading [%u] devices!", devnum());
    initdev(SIZEOF_STDCONF);
}

void        ConfController::factory         (void)
{ 
        
    Conf_Board::factory();
    
    device_conf_template dev;
    iSize_t ndx=SIZEOF_STDCONF;  
    packet::ipacket data;
    
    for (uint8_t i=0; i<DEFAULT_DEVNUM; i++)
    {
        memcpy_P(&dev, &DEFAULT_DEVCONF[i], sizeof(dev));

        data.init(dev.type);

        staticspace.put(ndx, dev.type);
        ndx+=sizeof(iCom_t);

        debug_dev("Writing device %u: %u - %s - on pin %u", i, dev.type, dev.name, dev.pin);
        
        strcpy(data.p_name(), P2C(BOARD_NAME));
        strcpy(data.p_devname(), dev.name);
        memcpy(data.p_pin1(), &dev.pin, sizeof(iPin_t));
        
        for (iSize_t j=0; j<data.data_size(); j++)
            {
                staticspace.update(ndx, data.payload()[j]);
                ndx++;
            }
            debug_dev("Wrote sensor %u:%s - on pin %u", i, 
                     data.p_devname(),
                     *data.p_pin1());
    }

    for(iSize_t i=ndx; i<ndx + 100; i++)
    {
        // trailing zeros
        staticspace.update(i, 0);
    }      
}

#elif defined (INDOORINO_CAMERA)


#endif /* PROJECTS */

#endif /* ARDUINO */


















































/*
'H'|'O'|'U'|'S'|'E'|'.'|'B'|'E'|'A'|'M'|'S'| 0 | 0 | 0 | 0 | 0 |
'C'|'O'|'N'|'T'|'R'|'O'|'L'|'L'|'E'|'R'| 0 | 0 | 0 | 0 | 0 | 0 |
'M'|'E'|'G'|'A'|'2'|'5'|'6'|'0'| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
8 |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'1'| 0 | 0 | 0 | 28 |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'2'| 0 | 0 | 0 | 30 |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'3'| 0 | 0 | 0 |' ' |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'4'| 0 | 0 | 0 |'"' |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'5'| 0 | 0 | 0 |'$' |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'6'| 0 | 0 | 0 |'&' |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'7'| 0 | 0 | 0 |'(' |
164 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |'B'|'E'|'A'|'M'|'8'| 0 | 0 | 0 |'*' |
> 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |>
> 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |>
*/











// bool        iConfBoard_AVR::devAdd            (ipacket * ptr)
// {
//     
//     uint8_t r = validate(ptr);
//     if (r > 0)
//     {
// 
// //     const __FSH * constring =F("conf:devadd: ");
// //         switch (r)
// //         {
// //             case 1:
// //             {
// //                 sendReport(2, _id, "%sgot NULLPOINTER!", F2C(constring));
// //                 break;
// //             }
// //             case 2:
// //             {
// //                 sendReport(2, _id, "%s<%s> : size is 0!", F2C(constring), F2C(ptr->label()));
// //                 break;
// //             }
// //             case 3:
// //             {
// //                 sendReport(2, _id, "%s<%s> : is a not valid device conf", F2C(constring), F2C(ptr->label()));
// //                 break;
// //             }
// //         }
//         
//         return false;
//     }
// 
//     uint8_t pin = *ptr->p_pin();
//     
//     if (devindexBypin(pin) != UINT8_MAX)
//     {
//         error_dev("%s:devAdd: pin %u alredy in use", P2C(_k_WARNING_) ,pin);
//         /* sendReport(2, _id, "%s<%s> : ", F2C(constring), F2C(ptr->label()), pin); */
//         return false;
//     }
//        
//     return Conf_AVR::devAdd(ptr);
// }
// 
// bool        iConfBoard_AVR::devMod            (const char * name, ipacket * ptr)
// {
//     uint8_t nameindex = devindex(name);
//     
//     if (validate(ptr) > 0) return false; /* Qui swicchare per devmod */
//     
//     uint8_t pin = *ptr->p_pin();
//     uint8_t index = devindexBypin(pin);
//     
//     if (index != UINT8_MAX && index != nameindex)
//     {
//         error_dev("%s:devMod: pin %u already in use ", P2C(_k_WARNING_), pin);
//         /* sendReport(2, _id, F("%spin %u alredy in use"),F2C(constring), pin); */
//         return false;
//     }
//     
//     return Conf_AVR::devMod(name, ptr);
//     
// }
// 
// bool        iConfBoard_AVR::devSetName        (const char * name, const char * new_name)
// {  
//     if (!is_string_devname(new_name,1,LEN_DEVNAME))
//     {
//         error_dev("%s:devSetName: invalid new name <%s>", P2C(_k_WARNING_), new_name);
//         /* sendReport(2, _id, F("%sname %s"), F2C(constring), new_name); */
//         return false;
//     }
//     
//     return iConfBoard::devSetName(name, new_name);
// }
// 
// uint8_t     iConfBoard_AVR::devindexBypin     (uint8_t pin)
// {
//     uint8_t p=0;
//     for (uint8_t i=0; i<devnum(); i++)
//     {
//         p = staticspace.read(_dpos[i] + sizeof(iCom_t) + LEN_DEVNAME);
//         if (p == pin)   return i;
//     }
//     
//     return UINT8_MAX;
// }
// 
