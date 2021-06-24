/*
 * devices.h
 *
 *  Created 3.0.7 on: Nov 6, 2019. Edited 13 Jun 2020 (BOARDS)
 *  Created 3.0.9 on: Nov 10, 2020 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_DEVICES_H_
#define _SRC_DEVICES_H_

#include "definitions.h"

#if defined(ARDUINO) && defined(INDOORINO_DEVS)

#include    "DHT.h"
#include    "OneWire.h" 
#include    "DallasTemperature.h"
#include    "definitions.h"
#include    "ipacket.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       VIRTUAL DEVICE                                              |
//      |___________________________________________________________________|
//

class virtualDevice
{
protected:
    /*
     * STATUS:
     * 0 = no errors
     * 1 = invalid pin
     * 2 = offline
     * 3 = io error
     * 4 = type error
     */
    uint8_t         _status=0;
    uint8_t         _confindex=0;
    uint8_t         _pin=0;
    char            _name[LEN_DEVNAME] {0};

public:
             virtualDevice      (uint8_t);
    virtual ~virtualDevice      ();
    
    virtual void        loop                (void)=0;
    virtual bool        reset               (void)=0;

    uint8_t             status              (void) { return _status; }
    packet::ipacket *   config              (packet::ipacket *);
    const char *        name                (void) { return _name; }
    
//     virtual ipacket *   status              (ipacket *)=0;    
//     virtual ipacket **  probe               (ipacket **)=0;

    void                send_dev_conf       (void);

    virtual void        send_dev_stat       (void)=0;
    virtual void        send_probe          (void)=0;    
    virtual int32_t     value               (void)=0;       // This return the default sensr value
    virtual bool        set                 (uint32_t)=0;   // this set the actuator to input value.

    virtual bool        isSensor            ()=0;
};


//      _____________________________________________________________________
//      |                                                                   |
//      |       SENSOR GENERIC                                              |
//      |___________________________________________________________________|
//

class   virtualSensor   : public  virtualDevice
{

public:
    
             virtualSensor      (uint8_t i):virtualDevice(i) {};
    virtual ~virtualSensor      ();

    virtual void        send_probe          (void)      {}
    virtual int32_t     value               (void)      { return 0; }
    virtual void        loop                (void)      {}
    bool                set                 (uint32_t)  { return (bool)_status; }

    bool                isSensor            (void)      { return true;  }
    //     ipacket     **      probe               (ipacket ** p)  { return p; };
};


//      _____________________________________________________________________
//      |                                                                   |
//      |       ACTUATOR GENERIC                                            |
//      |___________________________________________________________________|
//

class   virtualActuator   : public  virtualDevice
{
    
    /*  ACTUATOR mode is: 0=OFF, 1=ON */    

public:
    
             virtualActuator      (uint8_t index):virtualDevice(index) {};
    virtual ~virtualActuator      ();

    virtual bool        set                 (uint32_t)  { return false; }
    virtual void        loop                (void)      {}
    int32_t             value               (void)      { return (int32_t)_status; }
    void                send_probe          (void)      {}

    bool                isSensor            (void)      { return false; }
    //     ipacket     **      probe               (ipacket ** p)  { return p; };
};


//      _____________________________________________________________________
//      |                                                                   |
//      |       DEVICE LIST BASE                                            |
//      |___________________________________________________________________|
//



class   IndoorinoDeviceList
{
private:
    utils::ObjectList<virtualDevice> _devices;
    
protected:
    
    uint8_t             count           (iCom_t);
    bool                _alloc_type     (uint8_t);
        
public:
             IndoorinoDeviceList();
    virtual ~IndoorinoDeviceList();
        
    void                begin           (void);
    void                reset           (void);
    void                loop            (void);
    bool                add             (packet::ipacket *);
    bool                mod             (packet::ipacket *);
    bool                rem             (const char *);
    bool                setName         (const char *, const char *);
    bool                setPin          (const char *, iPin_t);
    uint8_t             devnum          (void) { return _devices.count(); }


    virtualDevice  *    operator[]      (const char *);
    virtualDevice  *    operator[]      (uint8_t);
};

#endif /* ARDUINO */

#endif /* _SRC_DEVICES_H_ */













#if defined (INDOORINO_NETWORK)
// #include "packeTemplate.h"

// class IndoorinoDevice
// {
//     /*
//      * STATUS:
//      * 0 = no errors
//      * 1 = invalid pin
//      * 2 = offline
//      * 3 = io error
//      */
// protected:
//     uint8_t _pin=0;
//     uint8_t _status=0;
//     char    _name[LEN_DEVNAME]   {0};
//     char    _boardname[LEN_NAME] {0};
// public:
//              IndoorinoDevice(const char *, const char *, uint8_t *);
//              IndoorinoDevice(ipacket *);
//     virtual ~IndoorinoDevice() {};
//     
//     virtual bool            parse           (ipacket *);
//     virtual ipacket *       get_conf        (ipacket *);
//     virtual ipacket *       get_stat        (ipacket *);
//     
//     const char  *   boardname       (void) { return (const char *)_boardname; }
//     const char  *   name            (void) { return (const char *)_name; }
//     uint8_t         pin             (void) { return _pin; }
//     uint8_t         status          (void) { return _status; }
//     
//     void            set_offline     (void) { _status=2; }
// };
// 
// class IndoorinoDeviceTemplate : public IndoorinoDevice
// {
// private:
// public:
//     IndoorinoDeviceTemplate(const char * n, const char * b, uint8_t *p):IndoorinoDevice(n,b,p) {}
//     IndoorinoDeviceTemplate(ipacket *p):IndoorinoDevice(p) {}
//     bool            parse           (ipacket *);
//     ipacket *       get_conf        (ipacket *);
//     ipacket *       get_stat        (ipacket *);
// };
// 
// class IndoorinoDeviceRelay : public IndoorinoDevice
// {
// private:
//     uint8_t         _relay_stat=0;
// public:
//     IndoorinoDeviceRelay(const char * n, const char * b, uint8_t *p):IndoorinoDevice(n,b,p) {}
//     IndoorinoDeviceRelay(ipacket *p):IndoorinoDevice(p) {}
//     bool            parse           (ipacket *);
//     ipacket *       get_conf        (ipacket *);
//     ipacket *       get_stat        (ipacket *);
//     uint8_t         relay_stat      (void) { return _relay_stat; }
// };
// 
// class IndoorinoDeviceList
// {
// private:
// public:
//      IndoorinoDeviceList() {};
//     ~IndoorinoDeviceList() { list.clear(); }
//     
//     ObjectList      <IndoorinoDevice>  list;
//     
//     ibasize_t                   num             (void)              { return list.len(); }
//     void                        clear           (void)              { list.clear(); }
//     IndoorinoDevice     *       operator[]      (const ibasize_t i) { return list[i]; }
//     IndoorinoDevice     *       operator[]      (const char *);
//     bool                        exist           (const char *);
//     bool                        remove          (const char *);
//     bool                        add             (ipacket    *);
// };


#endif /* INDOORINO_NETWORK */
