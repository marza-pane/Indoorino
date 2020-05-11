/*
 * eepromConf.cpp
 *
 *  Created on: Oct 27, 2019
 *      Author: n00b
 */

#include    "indoorino.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       BOARD BASE CONFIGURATION                                    |
//      |___________________________________________________________________|
//

ConfBase::ConfBase()
{
    staticspace.begin();
#if defined(EEPROM_MODULE)
    _id=F("EECONF");
#elif defined(SD_MODULE)
    _id=F("SDCONF");
#else
    _id=F("PGMCONF");
#endif
    memset(_dpos, 0, MAX_ATTACHED_DEVICES * sizeof(ibasize_t));
    
}

ConfBase::~ConfBase()
{
    debug_dev("\nDeleting ConfBase");
}


uint8_t     ConfBase::devnum                (void)
{
    return staticspace.read(3 * LEN_NAME); 
}


void        ConfBase::factory               (void)
{
    char        d[3][LEN_NAME];

    memset(d[0], 0, LEN_NAME);
    memset(d[1], 0, LEN_NAME);
    memset(d[2], 0, LEN_NAME);
    
    strcpy_P(d[0], BOARD_NAME);
    strcpy_P(d[1], INDOORINO_TYPE);
    strcpy_P(d[2], BOARD_TYPE);
    
    for (uint8_t n=0; n<3; n++)
    {
        debug("\nWriting base prop %u : %s", n, d[n]);
        for (uint8_t i=0; i<LEN_NAME; i++)
            staticspace.update( (n * LEN_NAME) + i, d[n][i]);
    }
    staticspace.write( 3 * LEN_NAME, DEFAULT_DEVNUM);
    staticspace.update();
    
    for (uint8_t n=0; n<3; n++)
    {
        debug("\nReading base prop %u : ", n);
        for (uint8_t i=0; i<LEN_NAME; i++)
        {
            Serial.print(staticspace.read((n * LEN_NAME) + i));
        }    
    }
    debug("\nDEVNUM updated is %u", devnum());
}


void        ConfBase::begin                 (void)
{
    const char * ptr[3] = {BOARD_NAME, INDOORINO_TYPE, BOARD_TYPE};
    char buf[LEN_NAME];
    uint16_t offset =0;
    
    for (uint8_t i=0; i<3; i++)
    {
        debug_dev("\nConfBase:begin: Parameter %u:%s",i, P2C(ptr[i]));
        memset(buf, 0, LEN_NAME);
        for (uint16_t j=0; j<LEN_NAME; j++)
        { 
            buf[j]  = staticspace.read(j + offset);
        }
        offset += LEN_NAME;
        debug_dev(" [ %s ] ",buf);
        if (strcmp_P(buf, ptr[i]) != 0)
        {
            debug_dev(">> FAILED!\n");
            sendReport(3, _id, "ConfBase:begin: invalid parameter %u [%s:%s]", i, P2C(ptr[i]), buf);
            delay(200);
            this->factory();
            this->begin();
        }   
        
    }

    if (devnum() > MAX_ATTACHED_DEVICES)
    {
        debug_dev("\nConfBase:begin: too many devices : %u (max %u)", devnum(),MAX_ATTACHED_DEVICES);
        sendReport(3, _id, F("Too many attached devices <%u>"), devnum());
        this->factory();
        delay(200);
        this->begin();
    }
    
    debug("\nConfBase:begin: complete!\n");
}


void        ConfBase::initdev               (ibasize_t start)
{
    _dpos[0]=start;    

    uint8_t         n=devnum();
    uint8_t         m=0;
    ibacomm_t       com=0;    
    ibasize_t       index=_dpos[0];
    ipacket *       p=nullptr;
    
    for (m=0; m<n; m++)
    {
        
        staticspace.get(index, com);
        debug_dev("\nConfBase:initdev: [%u/%u] <%u> at index %u", m, n, com, index);
        index +=sizeof(ibacomm_t);

        p = new ipacket(com);

        if (p->data_size() == 0)
        {
            sendReport(3, _id, F("Invalid device [%u:%s] at position %u"),com, F2C(p->label()));
            break;
        }
        
        for (ibasize_t j=0; j<p->data_size(); j++)
        {
            p->append(staticspace.read(index));
            index++;
        }
        
        debug_dev(" - loaded %s on pin %u",p->p_devname(), *p->p_pin());

        _dpos[m+1]=index;
        delete p;
    }
        
    if (m != n)
    {
        debug_dev("\nConfBase:initdev: truncating array at %u/%u", m, n);
        staticspace.write( 3 * LEN_NAME, m); // update devnum() 
        staticspace.update();
    }

    #if defined(DEBUG_DEVICES)
        debug_dev("\nConfBase:initdev: position array: [ ");
        for(uint8_t i=0; i<devnum() + 1; i++)
        {
            debug_dev("%u ",_dpos[i]);
        }
        debug_dev("]\n");
    #endif /* DEBUG_DEVICES */
}

uint8_t     ConfBase::devindex              (const char * name)
{
    char            n_buff[LEN_DEVNAME];
    
    for (uint8_t i=0; i<devnum(); i++)
    {
        memset(n_buff,0,LEN_DEVNAME);
        staticspace.get(_dpos[i] + sizeof(ibacomm_t), n_buff);
        if (strcmp(name, n_buff) == 0)
        {
            return i;
        }
    }
            
    return UINT8_MAX;
    
}

ibacomm_t   ConfBase::comByindex            (uint8_t index)
{
    ibacomm_t com=0;
    staticspace.get(_dpos[index], com);
    return com;
}

ipacket  *   ConfBase::device               (ipacket * ptr, const char * name)
{
    uint8_t index=devindex(name);
    
    if (index != UINT8_MAX)
    {
       return device(ptr, index);
    }
    else
    {
        error_dev("ERROR:ConfBase:device: invalid call for device %s", name);
        return ptr;
    }
}

ipacket  *   ConfBase::device               (ipacket * ptr, uint8_t index)
{        
 
    ibacomm_t com=0;

    if (index < devnum())
    {
        staticspace.get(_dpos[index], com);
        debug_dev("\nAllocating dev %u ...", com);
        ptr = reallocPacket(ptr, com);

        debug_dev("reading...");
        for (ibasize_t j=0; j<ptr->data_size(); j++)
        {
            ptr->append(staticspace.read(_dpos[index] + sizeof(ibacomm_t) + j));
        }
        debug_dev("done!");

    }
    else
    {
        debug_dev("\nERROR:ConfBase:device: invalid call for device at [%u]", index);
    }
    return ptr;
}

uint8_t     ConfBase::validate              (ipacket * ptr)
{
    if (ptr == nullptr)
    {
        error_dev("WARNING:validate: got nullpointer");
        return 1;        
    }
    
    if (ptr->data_size() == 0)
    {
        error_dev("WARNING:validate: got nullpointer");
        sendReport(2, _id, F("Got invalid packet <%s> for conf"),F2C(ptr->label()));
        return 2;
    }

    return 0;
}

bool        ConfBase::devAdd                (ipacket * ptr)
{
    uint8_t     dvn=devnum();
    ibasize_t   ndx=_dpos[dvn];
    
    staticspace.put(ndx, ptr->command());
    ndx+=sizeof(ibacomm_t);
        
    for (ibasize_t j=0; j<ptr->data_size(); j++)
    {
        staticspace.update(ndx, ptr->payload()[j]);
        ndx++;
    }
    
    dvn++;
    staticspace.write( 3 * LEN_NAME, dvn);
    staticspace.update();

    _dpos[devnum()] = ndx;
    
    #if defined(DEBUG_DEVICES)
        debug("\n\n\tconf:devAdd:added %s on pin %u!", ptr->p_devname(), * ptr->p_pin());
        debug_dev("\n\tTotal devices: %u [ ", dvn);
        for (uint8_t i=0; i<dvn; i++) debug_dev("%u ",_dpos[i]);
        debug_dev("]\n\n");
    #endif /* DEBUG_DEVICES */
    
    sendConfig();
    return true;
}

bool        ConfBase::devMod                (const char * name, ipacket * ptr)
{
    ibasize_t ndx=_dpos[devindex(name)] + sizeof(ibacomm_t);
      
    for (ibasize_t j=0; j<ptr->data_size(); j++)
    {        
        staticspace.update(ndx, ptr->payload()[j]);
        ndx++;
    }
    sendConfig();
    return true;
}

bool        ConfBase::devRem                (const char * name)
{
    uint8_t     dvn=devnum();
    uint8_t     index=devindex(name);
    
    if (dvn == 0 || index == UINT8_MAX)
    {
        if (dvn == 0)   { sendReport(2, _id, F("No device to remove"));       }
        else            { sendReport(2, _id, F("Device %s not found"), name); }
        return false;
    }
    
    ibasize_t   ndx=_dpos[index];
    
    uint8_t   b=0;
    ibacomm_t com=0;
    ibasize_t last=_dpos[dvn];
    staticspace.get(ndx, com);
    ibasize_t size=sizeof(ibacomm_t) + packet_size(com);
    
    dvn--;
    staticspace.write( 3 * LEN_NAME, dvn);
    
    
    debug_dev("\nRemoving from %u to %u",ndx, ndx + size);
    for (ibasize_t i=ndx; i<last; i++)
    {
        b = staticspace.read(i + size);
        staticspace.update(i, b);
    }

    initdev(_dpos[0]);
    sendConfig();
    return true;
}

bool        ConfBase::devSetName            (const char * name, const char *new_name)
{
    ibasize_t ndx=_dpos[devindex(name)] + sizeof(ibacomm_t);
    for (ibasize_t j=0; j<LEN_DEVNAME; j++)
    {
        staticspace.update(ndx, new_name[j]);
        ndx++;
    }
    sendConfig();
    return true;
}

//      _____________________________________________________________________
//      |                                                                   |
//      |       AVR BASE CONFIGURATION                                      |
//      |___________________________________________________________________|
//

uint8_t     ConfBase_AVR::validate            (ipacket * ptr)
{
    
    uint8_t r = ConfBase::validate(ptr);
    
    if (r > 0) return r;

    if (!is_type_devconf(ptr))
    {
        return 3;
    }
    return 0;
}

bool        ConfBase_AVR::devAdd            (ipacket * ptr)
{
    const __FSH * constring =F("conf:devadd: ");
    uint8_t r = this->validate(ptr);
    if (r > 0)
    {
        switch (r)
        {
            case 1:
            {
                sendReport(2, _id, "%sgot NULLPOINTER!", constring);
                break;
            }
            case 2:
            {
                sendReport(2, _id, "%s<%s> : size is 0!", F2C(constring), F2C(ptr->label()));
                break;
            }
            case 3:
            {
                sendReport(2, _id, "%s<%s> : is a not valid device conf", F2C(constring), F2C(ptr->label()));
                break;
            }
        }
        return false;
    }

    uint8_t pin = *ptr->p_pin();
    
    if (devindexBypin(pin) != UINT8_MAX)
    {
        sendReport(2, _id, "%s<%s> : pin %u alredy in use", F2C(constring), F2C(ptr->label()),pin);
        return false;
    }
    
    return ConfBase::devAdd(ptr);
}



bool        ConfBase_AVR::devMod            (const char * name, ipacket * ptr)
{
    const __FSH * constring =F("conf:devmod: ");
    uint8_t nameindex = devindex(name);

    if (nameindex == UINT8_MAX)
    {
        sendReport(2, _id, F("%sinvalid device %s"),constring, name);
        return false;
    }
    
    if (validate(ptr)) return false; /* Qui swicchare per devmod */
    
    uint8_t pin = *ptr->p_pin();
    uint8_t index = devindexBypin(pin);
    
    if (index != UINT8_MAX && index != nameindex)
    {
        sendReport(2, _id, F("%spin %u alredy in use"),constring, pin);
        return false;
    }
    
    return ConfBase::devMod(name, ptr);
    
}

bool        ConfBase_AVR::devSetName            (const char * name, const char * new_name)
{
    const __FSH * constring =F("conf:set:devname: invalid ");
    uint8_t index = devindex(name);
    if (index == UINT8_MAX)
    {
        sendReport(2, _id, F("%sdevice %s"), constring, name);
        return false;
    }
    
    if (!is_string_devname(new_name,1,LEN_DEVNAME))
    {
        sendReport(2, _id, F("%sname %s"), constring, new_name);
        return false;
    }
    
    return ConfBase::devSetName(name, new_name);
}

uint8_t     ConfBase_AVR::devindexBypin     (uint8_t pin)
{
    uint8_t p=0;
    for (uint8_t i=0; i<devnum(); i++)
    {
        p = staticspace.read(_dpos[i] + sizeof(ibacomm_t) + LEN_DEVNAME);
        if (p == pin)   return i;
    }
    
    return UINT8_MAX;
}

bool        ConfBase_AVR::devSetPin         (const char * name, uint8_t pin)
{
    
    const __FSH * constring =F("conf:set:pin: ");
    uint8_t nameindex = devindex(name);
    uint8_t pinindex  = devindexBypin(pin);
    
    debug_dev("\nConfBase:setDevPin: %s will be set to pin %u", name, pin);
    debug_dev("\n\tIndices: name:%u pin:%u", nameindex, pinindex);
        
    if (nameindex == UINT8_MAX)
    {
        sendReport(2, _id, F("%sinvalid device %s"), constring, name);
        return false;
    }
    
    if (pinindex != UINT8_MAX && nameindex != pinindex)
    {
        sendReport(2, _id, F("%s pin %u already in use"), constring, pin);
        return false;
    }

    ibasize_t ndx=_dpos[nameindex] + sizeof(ibacomm_t) + LEN_DEVNAME;
    debug_dev("\n\tWill write to %u", ndx);
    
    staticspace.update(ndx, pin);
    return true;

}

const char string_foundinvalid[] PROGMEM = "Found invalid ";

//      _____________________________________________________________________
//      |                                                                   |
//      |       AVR BASE CONFIGURATION                                      |
//      |___________________________________________________________________|
//



uint8_t     ConfBase_ESP::validate            (ipacket * ptr)
{
    return ConfBase::validate(ptr);
}


#if defined (INDOORINO_SAMPLER)

//      _____________________________________________________________________
//      |                                                                   |
//      |       eepromConfig - project SAMPLER                              |
//      |___________________________________________________________________|
//


void        ConfSampler::begin              (void)
{
    ConfBase::begin();
    
    uint32_t  par=0;
    staticspace.get(SIZEOF_STDCONF, par);
    if (par < DT_MIN_STEP || par > DT_MAX_STEP)
    {
        sendReport(3, _id, F("%sSTEP <%u>"), P2C(string_foundinvalid), par);
        delay(200);
        this->factory();
        this->begin();
    }

    staticspace.get(SIZEOF_STDCONF + sizeof(uint32_t), par);
    if (par < DT_MIN_COOL || par > DT_MAX_COOL)
    {
        sendReport(3, _id, F("%sCOOL TIME <%u>"), P2C(string_foundinvalid), par);
        delay(200);
        this->factory();
        this->begin();
    }
    
    initdev(SIZEOF_STDCONF + 2 * sizeof(uint32_t));
    debug("\nEEPROM:Sampler begin complete");
}

void        ConfSampler::factory            (void)
{ 
        
    ConfBase::factory();
    
    ibasize_t   ndx=SIZEOF_STDCONF;
    uint32_t    par=0;

    par =DT_DEF_STEP;
    staticspace.put(ndx, par);
    ndx +=sizeof(uint32_t);

    par =DT_DEF_COOL;
    staticspace.put(ndx, par);
    ndx +=sizeof(uint32_t);
        
//     const ibacomm_t     stdcom[DEFAULT_DEVNUM]                  = DEFAULT_DEV_COMS;
//     const uint8_t       stdpin[DEFAULT_DEVNUM]                  = DEFAULT_DEV_PINS;
//     const char  *       stdname[DEFAULT_DEVNUM]                 = DEFAULT_DEV_NAME;

    
    default_dev_conf_template       dev;
    for (uint8_t i=0; i<DEFAULT_DEVNUM; i++)
    {
        memcpy_P(&dev, &DEFAULT_DEVCONF[i], sizeof(dev));

        ipacket     *   data    = new ipacket(dev.type);

        staticspace.put(ndx, dev.type);
        ndx+=sizeof(ibacomm_t);

        debug_dev("\nWriting sensor %u: %u - ", i, dev.type);
        debug_dev("%s - on pin %u",dev.name, dev.pin);
        
        strcpy(data->p_devname(), dev.name);
        *data->p_pin() = dev.pin;
        if (data->p_param1() != nullptr) *data->p_param1() = 100;
        if (data->p_param3() != nullptr) *data->p_param3() = 100;
        
        for (ibasize_t j=0; j<data->data_size(); j++)
            {
                staticspace.update(ndx, data->payload()[j]);
                ndx++;
            }
            debug_dev("\nWrote sensor %u:%s - no pin %u\n", i, 
                     data->p_devname(),
                     *data->p_pin());
        delete data;
    }

    for(ibasize_t i=0; i<100; i++)
    {
        // trailing zeros
        staticspace.update(ndx + i, 0);
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

#elif defined (INDOORINO_ESPSERVER)

//      _____________________________________________________________________
//      |                                                                   |
//      |       eepromConfig - project ESPSERVER                            |
//      |___________________________________________________________________|
//
    
/*
 *  _______________________________________________
 *  |         |     :     :     :     |           |
 *  | STDCONF | IP0 : IP1 : ... : IPn | LOCALPORT |
 *  |_________|_____:_____:_____:_____|___________|
 *  
 */

    
void        ConfEspServer::begin            (void)
{
    ConfBase::begin();
        
    uint32_t    par=0;
    uint8_t     num=0;
    ibasize_t   ndx=SIZEOF_STDCONF;
    
    
    st_address      addr;
    
    for (uint8_t i=0; i<MAX_NET_TARGETS; i++)
    {
        staticspace.get(ndx + i* sizeof(st_address), addr);
        
        if (!_address.set(&addr))
        {
            if (i == 0)
            {
                error_dev("%sIP TARGET (none)",P2C(string_foundinvalid));
                delay(200);
                this->factory();
                this->begin();
            }
            else break;
        }
        else _address_num ++;
    }

    ndx += MAX_NET_TARGETS * sizeof(st_address);
    
    staticspace.get(ndx, par);
    if (par < PORT_SOCKET_FIRST || par > PORT_SOCKET_LAST)
    {
        error_dev("%sPORT <%u>",P2C(string_foundinvalid), par);
        delay(200);
        this->factory();
        this->begin();
    }
        
    ndx += sizeof(uint32_t);    
    staticspace.get(ndx, par);
    if (par < 100)
    {
        error_dev("%sTIMEOUT <%u>",P2C(string_foundinvalid), par);
//         sendReport(3, _id, F("\n%sTIMEOUT <%u>"),P2C(string_foundinvalid), par);
        delay(200);
        this->factory();
        this->begin();
    }
    

    ndx += sizeof(uint32_t);    
    staticspace.get(ndx, num);
    if (num > MAX_RETRY_NUM)
    {
        error_dev("%sATTEMPTS <%u>",P2C(string_foundinvalid), par);
//         sendReport(3, _id, F("\n%sATTEMPTS <%u>"),P2C(string_foundinvalid), par);
        delay(200);
        this->factory();
        this->begin();
    }
    
    
    ndx += sizeof(uint8_t);    
    initdev(ndx);
    debug("\nEEPROM:EspServer begin complete");
}

void        ConfEspServer::factory          (void)
{
    ConfBase::factory();

//     ( 1003,     'conf_esp',         'esp conf',             ('ip', 'port', 'timeout', 'level')),

    ibasize_t   ndx=SIZEOF_STDCONF;
    uint32_t    par=0;
    uint8_t     num=0;
    
    _address.set(P2C(DEFAULT_TARGET_IP), DEFAULT_TARGET_PORT);
    _address_num = 1;
    staticspace.put(ndx, * _address.get());
    
    ndx += MAX_NET_TARGETS * sizeof(st_address);
    
    par =DEFAULT_LOCALPORT;
    staticspace.put(ndx, par);
    ndx +=sizeof(uint32_t);

    par =DEFAULT_PACKET_TIMEOUT;
    staticspace.put(ndx, par);
    ndx +=sizeof(uint32_t);

    num=DEFAULT_RETRY_NUM;
    staticspace.put(ndx, num);
    ndx +=sizeof(uint8_t);
    
    
    staticspace.update();
    debug_dev("\nConfBase:begin: FACTORY");

//     ipacket * ptr = new ipacket(IBACOM_ESP_ADDRESS);
//     
//     strcpy_P(ptr->p_devname(), DEFAULT_TARGET_NAME);
//     strcpy_P(ptr->p_ip(), DEFAULT_TARGET_IP);
//     par = DEFAULT_TARGET_PORT;
//     memcpy(ptr->p_port(), &par, sizeof(uint32_t));
//     debug_dev("\nConfBase:begin: PTRPORT:%u --> %u",ptr->data_size(),par);
//     
//     staticspace.put(ndx, IBACOM_ESP_ADDRESS);
//     ndx+=sizeof(ibacomm_t);
//     for (ibasize_t j=0; j<ptr->data_size(); j++)
//     {
//         staticspace.write(ndx, ptr->payload()[j]);
//         ndx++;
//     }
//     staticspace.update();
//     memcpy(&par, ptr->p_port(), sizeof(uint32_t));
//     debug_dev("\nWrote target %s:%u\n", ptr->p_ip(), par);
//     delete ptr;    
}

uint32_t        ConfEspServer::localport        (void)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address), par);
    return par;
}

uint32_t        ConfEspServer::timeout          (void)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address) + sizeof(uint32_t), par);
    return par;
}

uint8_t         ConfEspServer::attempts         (void)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address) + 2 * sizeof(uint32_t), par);
    return par;
}

void            ConfEspServer::localport        (uint32_t value)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address), par);
    if (par != value)
    {
        staticspace.put(SIZEOF_STDCONF, value);        
        staticspace.update();
    }
}

void            ConfEspServer::timeout          (uint32_t value)
{
    uint32_t par;
    
    staticspace.get(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address) + sizeof(uint32_t), par);
    if (par != value)
    {
        staticspace.put(SIZEOF_STDCONF + sizeof(uint32_t), value);        
        staticspace.update();
    }
}

void            ConfEspServer::attempts         (uint8_t value)
{
    uint32_t par;
    staticspace.get(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address) + 2 * sizeof(uint32_t), par);
    if (par != value)
    {
        staticspace.put(SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address) + 2 * sizeof(uint32_t), value);        
        staticspace.update();
    }
}

void            ConfEspServer::addAddress      (netaddress * apt)
{
    if (_address_num == MAX_NET_TARGETS - 1)
    {
        error_dev("conf:espServer: too many targets!");
        return;
    }
    
    ibasize_t   ndx = SIZEOF_STDCONF + _address_num * sizeof(st_address);

    st_address  addr;
    memcpy(&addr, apt->get(), sizeof(st_address));
    staticspace.put(ndx, addr);
    staticspace.update();
    _address_num++;
}

void            ConfEspServer::remAddress      (netaddress * apt)
{


    bool flag=true;
    st_address addr;
    ibasize_t   ndx = SIZEOF_STDCONF;
    
    for (uint8_t i=0; i<_address_num; i++)
    {
        staticspace.get(ndx, addr);
        for (uint8_t j=0; j<4; j++)
        {
            if (apt->ip(j) !=addr.ip[j])
            {
                flag = false;
                break;
            }
        }
        
        if (flag && apt->port() == addr.port)
        {
            uint8_t byte=0;
            debug("\nconf:remAddress: found %s:%u", apt->ip(), apt->port());
            for (ibasize_t j=ndx; j<SIZEOF_STDCONF + (_address_num - 1) * sizeof(st_address); j++)
            {
                byte = staticspace.read(j + sizeof(st_address));
                staticspace.update(j, byte);
                ndx++;
            }
            for (ibasize_t j=ndx; j<ndx + sizeof(st_address); j++)
            {
                staticspace.update(j, 0);
            }
            return;
        }
        ndx += sizeof(st_address);
    }
    
}

netaddress  *   ConfEspServer::address         (uint8_t index)
{
        uint32_t            _address_num     =0;
};
   
#elif defined (INDOORINO_CAMERA)

#endif
