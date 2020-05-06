/*
 * stddev.h
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */

#ifndef STDDEV_H_
#define STDDEV_H_

#if defined (ARDUINO) || defined (ESP8266)

#include    <Arduino.h>
#include    "DHT.h"
#include    "Wire.h"
#include    "OneWire.h" 
#include    "DallasTemperature.h"

#include    "packetUtils.h"

#define     ERROR_BLINK_LONG    2000
#define     ERROR_BLINK_SHORT   200
#define     CLOCK_REFRESH_RATE  100
#define     DEVICE_INIT_RETRY_DELAY 30000

class blinkingLed
{
public:
    blinkingLed         () {};
    void    begin       (uint8_t pin=LED_BUILTIN);
    void    start       (uint8_t rept);             // 1=DEVICE, 2=SD, 3=RTC, 4=SYSTEM
    void    stop        (void);
    void    loop        (void);
    bool    status      (void)	 { return _status; };

private:
    bool        _status     =false;
    uint8_t     _rept       =0;
    uint8_t     _count      =0;
    uint8_t     _pin        =0;
    uint32_t    _last       =0;
    uint32_t    _lastblink  =0;
};

extern blinkingLed blinker;

//      _____________________________________________________________________
//      |                                                                   |
//      |       Real time clock                                             |
//      |___________________________________________________________________|
//

class       rtclock
{
protected:
    const __FSH *   _id;   

public:

    rtclock                 () {};
    virtual ~rtclock        () {};
    
    virtual void            begin       ()=0;
    virtual void            loop        ()=0;
    virtual uint32_t        epoch       ()=0;
    virtual bool            status		()=0;
    virtual void            set			(DateTime)=0;
    
    void                    set         (uint32_t synctime) { set(DateTime(synctime)); };
};

class       millisClock :   public rtclock
{
public:

    millisClock     ();
    ~millisClock    ();
    void            begin       (void);
    void            loop        (void) {};
    void            set		    (DateTime synctime);
    uint32_t        epoch       (void);
    bool            status		(void){ return true;};

protected:
    
    RTC_Millis  *   _rtcmillis=NULL;    
};

#if defined (RTC_MODULE)
    class   ClockDS3231 :   public rtclock
    {
    public:

        ClockDS3231     ();
        ~ClockDS3231    ();
        
        void            begin       (void);
        void            loop        (void);
        void            set         (DateTime synctime);
        uint32_t        epoch       (void)              { return _current_time; };
        bool            status      (void)              { return _flag; };

    protected:
        
        RTC_DS3231	*	_rtclock=NULL;
        RTC_Millis  *   _rtcmillis=NULL;
        
        void            request_time    (void);
        
        uint32_t        _last_retry=0;
        uint32_t        _last_update=0;
        uint32_t        _current_time=0;
        
        bool        _flag=false;        
    };
    
    extern ClockDS3231 rtc;

#else

    extern millisClock rtc;

#endif /* RTC MODULE */
    
#endif /* ARDUINO */

#endif /* STDDEV_H_ */
  
