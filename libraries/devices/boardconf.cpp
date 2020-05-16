/*
 * eepromConf.cpp
 *
 *  Created on: Oct 27, 2019
 *  Edited on May 15 2020
 *      Author: n00b
 */

#include    "indoorino.h"
  
const char _k_ERROR_[] PROGMEM = { "ERROR" };
const char _k_WARNING_[] PROGMEM = { "WARNING" };

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
    debug_dev("\nConf:deleted!");
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
    
    debug("\nConf:factory");
    
    for (uint8_t n=0; n<3; n++)
    {
        debug_dev("\n\tWriting base prop %u : %s", n, d[n]);
        for (uint8_t i=0; i<LEN_NAME; i++)
            staticspace.update( (n * LEN_NAME) + i, d[n][i]);
    }
    staticspace.write( 3 * LEN_NAME, DEFAULT_DEVNUM);

    if(DEFAULT_DEVNUM == 0)
        for (uint8_t i=SIZEOF_STDCONF; i<SIZEOF_STDCONF+100; i++)
            staticspace.update(SIZEOF_STDCONF, 0);
    
    staticspace.update();

#if defined(DEBUG_DEVICES)
    for (uint8_t n=0; n<3; n++)
    {
        debug_dev("\n\tReading base prop %u : ", n);
        for (uint8_t i=0; i<LEN_NAME; i++)
        {
            debug("%c", staticspace.read((n * LEN_NAME) + i));
        }    
    }
#endif
    debug(" DONE! devnum = %u", devnum());
}


void        ConfBase::begin                 (void)
{
    const char * ptr[3] = {BOARD_NAME, INDOORINO_TYPE, BOARD_TYPE};
    char buf[LEN_NAME];
    uint16_t offset =0;
    
    for (uint8_t i=0; i<3; i++)
    {
        debug_dev("\nConf:begin Parameter %u:%s", i, P2C(ptr[i]));
        memset(buf, 0, LEN_NAME);
        for (uint16_t j=0; j<LEN_NAME; j++)
        { 
            buf[j]  = staticspace.read(j + offset);
        }
        offset += LEN_NAME;
        debug_dev(" [ %s ] ",buf);
        if (strcmp_P(buf, ptr[i]) != 0)
        {
            error_dev("%s: failed begin!\n", P2C(_k_ERROR_));
            /* sendReport(3, _id, "%s invalid parameter %u [%s:%s]",P2C(_k_nConfBase_), i, P2C(ptr[i]), buf); */
            this->factory();
            this->begin();
            return;
        }   
        
    }

//     if (devnum() > MAX_ATTACHED_DEVICES)
//     {
//         error_dev("%s%s: %s : %u", P2C(_k_nConfBase_), P2C(_k_begin_), F2C(_k_too_many_), devnum());
//         sendReport(3, _id, F("%s%s%u"), P2C(_k_nConfBase_), P2C(dst_dev_1), devnum());
//         this->factory();
//         this->begin();
//     }
    
    debug("\nConf:begin: complete!\n");
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
        debug_dev("\nConf:init: [%u/%u] <%u> at index %u", m, n, com, index);
        index +=sizeof(ibacomm_t);

        p = reallocPacket(p, com);
        

        if (p->data_size() == 0)
        {
            error_dev("ERROR:init: invalid [%u:%s] at index %u",
                      com, F2C(p->label()), index);

            /* sendReport(2, _id, F("%s"),buf); */
            break;
        }
        
        for (ibasize_t j=0; j<p->data_size(); j++)
        {
            p->append(staticspace.read(index));
            index++;
        }
        
        debug_dev(" - loaded %s on pin %u", p->p_devname(), *p->p_pin());

        _dpos[m+1]=index;
    }
    if (p != nullptr) delete p;
        
    if (m != n)
    {
        error_dev("WARNING:init: truncating array at %u/%u", m, n);

        /* sendReport(2, _id, F("%s"),buf); */
        staticspace.write( 3 * LEN_NAME, m); // update devnum() 
        staticspace.update();
    }

    #if defined(DEBUG_DEVICES)
        debug_dev("\nConf:init: dev array: [ ");
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
        error_dev("%s:devCall: invalid name <%s>", P2C(_k_ERROR_), name);
        return ptr;
    }
}

ipacket  *   ConfBase::device               (ipacket * ptr, uint8_t index)
{        
 
    ibacomm_t com=0;

    if (index < devnum())
    {
        staticspace.get(_dpos[index], com);
        ptr = reallocPacket(ptr, com);

        for (ibasize_t j=0; j<ptr->data_size(); j++)
        {
            ptr->append(staticspace.read(_dpos[index] + sizeof(ibacomm_t) + j));
        }
    }
    else
    {
        error_dev("%s:devCall: invalid index %u", P2C(_k_ERROR_), index);
    }
    return ptr;
}

uint8_t     ConfBase::validate              (ipacket * ptr)
{
    if (ptr == nullptr)
    {
        error_dev("%s:validate: got nullpointer", P2C(_k_ERROR_));
        return 1;        
    }
    
    if (ptr->data_size() == 0)
    {
        error_dev("%s:validate: <%s> has zero size", P2C(_k_WARNING_), F2C(ptr->label()));
        return 2;
    }
    if (!is_type_devconf(ptr))
    {
        error_dev("%s:validate: <%s> is not a device", P2C(_k_WARNING_), F2C(ptr->label()));
        return 3;
    }
    if (!is_string_devname(ptr->p_devname()))
    {
        error_dev("%s:validate: <%s> has invalid devname %s", P2C(_k_WARNING_), F2C(ptr->label()), ptr->p_devname());
        return 4;
    }
    

    return 0;
}

bool        ConfBase::devAdd                (ipacket * ptr)
{
    if (devnum() == MAX_ATTACHED_DEVICES)
    {
        error_dev("%s:devAdd: too many devices",P2C(_k_WARNING_));
        return false;
    }
    
    if (validate(ptr) > 0) return false;
    
    if (devindex(ptr->p_devname()) != UINT8_MAX)
    {
        error_dev("%s:devAdd: %s already defined",P2C(_k_WARNING_), ptr->p_devname());
        return false;
    }
    
    
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
        debug("\nConf:devAdd: added %s on pin %u!", ptr->p_devname(), * ptr->p_pin());
        debug_dev("\n\tTotal devices: %u [ ", dvn);
        for (uint8_t i=0; i<dvn; i++) debug_dev("%u ",_dpos[i]);
        debug_dev("]\n\n");
    #endif /* DEBUG_DEVICES */
    
    sendConfig();
    return true;
}

bool        ConfBase::devMod                (const char * name, ipacket * ptr)
{
    ibasize_t n=devindex(name);
    if (n == UINT8_MAX)
    {
        error_dev("%s:devMod: invalid name <%s>", P2C(_k_WARNING_), name);
        /* sendReport(2, _id, F("%sinvalid device %s"),F2C(constring), name); */
        return false;
    }
    
    ipacket *p=nullptr;
    if (device(p, n)->command() != ptr->command())
    {
        error_dev("%s:devMod: mismatching types [%u - %u]",
                  P2C(_k_WARNING_),
                  device(p, n)->command(),
                  ptr->command());
        if (p) delete p;
        return false;
    }
 
    debug_dev("\nConf:devMod: editing %s", ptr->p_devname());
    if (p) delete p;
    
    ibasize_t ndx=_dpos[n] + sizeof(ibacomm_t);
      
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
    
    if (index == UINT8_MAX)
    {
        error_dev("%s:devRem: invalid name <%s>", P2C(_k_WARNING_), name);
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
    
    
    debug_dev("\nConf:devRem: removing from %u to %u", ndx, ndx + size);
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

        ibasize_t i = devindex(name);
    if (i == UINT8_MAX)
    {
        error_dev("%s:devSetName: invalid name <%s>", P2C(_k_WARNING_), name);
        /* sendReport(2, _id, F("%sdevice %s"), F2C(constring), name); */
        return false;
    }
    ibasize_t ndx=_dpos[i] + sizeof(ibacomm_t);
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

bool        ConfBase_AVR::devAdd            (ipacket * ptr)
{
    
    uint8_t r = validate(ptr);
    if (r > 0)
    {

//     const __FSH * constring =F("conf:devadd: ");
//         switch (r)
//         {
//             case 1:
//             {
//                 sendReport(2, _id, "%sgot NULLPOINTER!", F2C(constring));
//                 break;
//             }
//             case 2:
//             {
//                 sendReport(2, _id, "%s<%s> : size is 0!", F2C(constring), F2C(ptr->label()));
//                 break;
//             }
//             case 3:
//             {
//                 sendReport(2, _id, "%s<%s> : is a not valid device conf", F2C(constring), F2C(ptr->label()));
//                 break;
//             }
//         }
        
        return false;
    }

    uint8_t pin = *ptr->p_pin();
    
    if (devindexBypin(pin) != UINT8_MAX)
    {
        error_dev("%s:devAdd: pin %u alredy in use", P2C(_k_WARNING_) ,pin);
        /* sendReport(2, _id, "%s<%s> : ", F2C(constring), F2C(ptr->label()), pin); */
        return false;
    }
       
    return ConfBase::devAdd(ptr);
}

bool        ConfBase_AVR::devMod            (const char * name, ipacket * ptr)
{
    uint8_t nameindex = devindex(name);
    
    if (validate(ptr) > 0) return false; /* Qui swicchare per devmod */
    
    uint8_t pin = *ptr->p_pin();
    uint8_t index = devindexBypin(pin);
    
    if (index != UINT8_MAX && index != nameindex)
    {
        error_dev("%s:devMod: pin %u already in use ", P2C(_k_WARNING_), pin);
        /* sendReport(2, _id, F("%spin %u alredy in use"),F2C(constring), pin); */
        return false;
    }
    
    return ConfBase::devMod(name, ptr);
    
}

bool        ConfBase_AVR::devSetName            (const char * name, const char * new_name)
{  
    if (!is_string_devname(new_name,1,LEN_DEVNAME))
    {
        error_dev("%s:devSetName: invalid new name <%s>", P2C(_k_WARNING_), new_name);
        /* sendReport(2, _id, F("%sname %s"), F2C(constring), new_name); */
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
    
    /* const __FSH * constring =F("conf:set:pin: "); */
    uint8_t nameindex = devindex(name);
    uint8_t pinindex  = devindexBypin(pin);
    
    debug_dev("\nConfBase:setDevPin: %s will be set to pin %u", name, pin);
    debug_dev("\n\tIndices: name:%u pin:%u", nameindex, pinindex);
        
    if (nameindex == UINT8_MAX)
    {
        error_dev("%s:devSetPin: invalid name <%s>", P2C(_k_WARNING_), name);
        /* sendReport(2, _id, F("%sinvalid device %s"), constring, name); */
        return false;
    }
    
    if (pinindex != UINT8_MAX && nameindex != pinindex)
    {
        error_dev("%s:devSetPin: pin %u already in use", P2C(_k_WARNING_), pin);
        /* sendReport(2, _id, F("%s pin %u already in use"), constring, pin); */
        return false;
    }

    ibasize_t ndx=_dpos[nameindex] + sizeof(ibacomm_t) + LEN_DEVNAME;
    debug_dev("\n\tWill write to %u", ndx);
    
    staticspace.update(ndx, pin);
    return true;

}

//      _____________________________________________________________________
//      |                                                                   |
//      |       ESP BASE CONFIGURATION                                      |
//      |___________________________________________________________________|
//

/*
 *  _______________________________________________
 *  |         |     :     :     :     |           |
 *  | STDCONF | IP0 : IP1 : ... : IPn | LOCALPORT |
 *  |_________|_____:_____:_____:_____|___________|
 *  
 */

void            ConfBase_ESP::begin             (void)
{
    ConfBase::begin();
    
    st_address      addr;
    
    for (uint8_t i=0; i<MAX_NET_TARGETS; i++)
    {
        staticspace.get(SIZEOF_STDCONF + i* sizeof(st_address), addr);
        
        debug_dev("\nESPserver:begin: loaded %u.%u.%u.%u:%u",
                  addr.ip[0],addr.ip[1],addr.ip[2],addr.ip[3],addr.port);
        if (!_address.set(&addr))
        {
            if (i == 0)
            {
                error_dev("\nESPserver:begin: invalid IP TARGET (none)");
                this->factory();
                this->begin();
            }
            else break;
        }
        else _address_num ++;
    }
}

void            ConfBase_ESP::factory           (void)
{
    ConfBase::factory();

    _address.set(P2C(DEFAULT_TARGET_IP), DEFAULT_TARGET_PORT);
    _address_num = 1;

    st_address  adr;
    memcpy(&adr, _address.get(), sizeof(st_address));
    staticspace.put(SIZEOF_STDCONF, adr);
    
    // trailing zeros
    for (ibasize_t i=SIZEOF_STDCONF + sizeof(st_address); i<(MAX_NET_TARGETS  ) * sizeof(st_address); i++) staticspace.update(i,0);
}

bool            ConfBase_ESP::addAddress        (netaddress * apt)
{
    if (_address_num == MAX_NET_TARGETS - 1)
    {
        error_dev("espServer:add: too many targets!");
        return false;
    }
    if (apt==nullptr)
    {
        error_dev("%s:espServer:add: got nullpointer", P2C(_k_WARNING_));
        return false; 
        
    }
    if (!apt->is_valid())
    {
        error_dev("espServer:add: invalid target %s:%u", apt->ip(), apt->port());
        return false;
    }
    
    ibasize_t   ndx = SIZEOF_STDCONF + _address_num * sizeof(st_address);

    st_address  addr;
    memcpy(&addr, apt->get(), sizeof(st_address));
    staticspace.put(ndx, addr);
    staticspace.update();
    _address_num++;
    return true;
}

bool            ConfBase_ESP::remAddress      (netaddress * apt)
{
    if (apt==nullptr)
    {
        error_dev("%s:espServer:rem: got nullpointer", P2C(_k_WARNING_));
        return false; 
    }

    bool flag=true;
    st_address addr;
    ibasize_t   ndx = SIZEOF_STDCONF;
    
    for (uint8_t i=0; i<_address_num; i++)
    {
        flag=true;
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
            debug_dev("\nConf:remAddress: found %s:%u", apt->ip(), apt->port());
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
            _address_num--; 
            return true;
        }
        ndx += sizeof(st_address);
    }
    error_dev("espServer:rem: invalid address %s:%u", apt->ip(), apt->port());
    return false; 
    
}

bool            ConfBase_ESP::remAddress      (const char * addr)
{
    return remAddress(address(addr));
}

netaddress  *   ConfBase_ESP::address          (uint8_t index)
{
    static uint8_t n=UINT8_MAX;
    
    if(index >= _address_num)
    {
        error_dev("%s:address: index OOR",P2C(_k_ERROR_));
        return nullptr;
    }
    
    if(n != index)
    {
        n = index;
        st_address      addr;    
        staticspace.get(SIZEOF_STDCONF + index * sizeof(st_address), addr);
        debug_dev("\nESPserver:address: read %u.%u.%u.%u:%u",
                    addr.ip[0],addr.ip[1],addr.ip[2],addr.ip[3],addr.port);
        _address.set(&addr);
    }
    return &_address;
};

netaddress  *   ConfBase_ESP::address          (const char * addr)
{
    netaddress * p=nullptr;
    for (uint8_t i=0; i<_address_num; i++)
    {
        p = address(i);
        if (strcmp(p->ip(), addr) == 0) break;
    }
    return p;
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
        error_dev("\nSampler:begin: invalid STEP <%u>", par); 
        /* sendReport(3, _id, F("%s:%s STEP <%u>"), P2C(_k_begin_), P2C(_k_invalid_), par); */
        this->factory();
        this->begin();
    }

    staticspace.get(SIZEOF_STDCONF + sizeof(uint32_t), par);
    if (par < DT_MIN_COOL || par > DT_MAX_COOL)
    {
//         sendReport(3, _id, F("%sCOOL TIME <%u>"), P2C(string_foundinvalid), par);
        error_dev("\nSampler:begin: invalid COOL TIME  <%u>", par); 
        this->factory();
        this->begin();
    }
    
    initdev(SIZEOF_STDCONF + 2 * sizeof(uint32_t));
    debug("\n%s:Sampler begin complete", F2C(_id));
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

    for(ibasize_t i=ndx; i<ndx + 100; i++)
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

#elif defined (INDOORINO_ESPSERVER)

//      _____________________________________________________________________
//      |                                                                   |
//      |       eepromConfig - project ESPSERVER                            |
//      |___________________________________________________________________|
//

void            ConfEspServer::begin            (void)
{
    ConfBase_ESP::begin();
        
    uint32_t    par=0;
    uint8_t     num=0;
    ibasize_t   ndx=SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address);

    staticspace.get(ndx, par);
    debug_dev("\n\t--> PORT:%u",par);
    if (par < PORT_SOCKET_FIRST || par > PORT_SOCKET_LAST)
    {
        error_dev("\n%s:ESPserver:begin: invalid PORT:%u",P2C(_k_WARNING_), par);
        this->factory();
        this->begin();
    }
        
    ndx += sizeof(uint32_t);    
    staticspace.get(ndx, par);
    debug_dev("\n\t--> TIMEOUT:%u",par);
    if (par < 100)
    {
        error_dev("\n%s:ESPserver:begin: invalid TIMEOUT:%u",P2C(_k_WARNING_), par);
//         sendReport(3, _id, F("\n%sTIMEOUT <%u>"),P2C(string_foundinvalid), par);
        this->factory();
        this->begin();
    }
    

    ndx += sizeof(uint32_t);    
    staticspace.get(ndx, num);
    debug_dev("\n\t--> ATTEMPTS:%u",num);
    if (num > MAX_RETRY_NUM)
    {
        error_dev("\n%s:ESPserver:begin: invalid ATTEMPTS:%u",P2C(_k_WARNING_), num);
//         sendReport(3, _id, F("\n%sATTEMPTS <%u>"),P2C(string_foundinvalid), num);
        this->factory();
        this->begin();
    }
    
    
    ndx += sizeof(uint8_t);    
    initdev(ndx);
    debug("\nESPserver:begin: complete");
}

void            ConfEspServer::factory          (void)
{
    ConfBase_ESP::factory();

    debug_dev("\nESPserver:factory");

    ibasize_t   ndx=SIZEOF_STDCONF + MAX_NET_TARGETS * sizeof(st_address);
    uint32_t    par=0;
    uint8_t     num=0;
     
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

#elif defined (INDOORINO_CAMERA)

#endif
