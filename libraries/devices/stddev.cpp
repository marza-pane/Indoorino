/*
 * stddev.cpp
 *
 *  Created on: Oct 26, 2019
 *      Author: n00b
 */

#include "stddev.h"

#define FLASHEPOCH  DateTime(F(__DATE__), F(__TIME__)).unixtime()

#if defined (ARDUINO) || defined (ESP8266)

//      _____________________________________________________________________
//      |                                                                   |
//      |       On board LED blinking                                       |
//      |___________________________________________________________________|
// 



void        blinkingLed::begin          (uint8_t pin)
{	
	_pin = pin; pinMode(_pin, OUTPUT);
	for (uint8_t i=0; i<3; i++)
	{
		digitalWrite(_pin, HIGH);
		delay(ERROR_BLINK_SHORT);
		digitalWrite(_pin, LOW);
		delay(ERROR_BLINK_SHORT);
	}
}
	
void        blinkingLed::start          (uint8_t rept)
{	
    _rept = rept;
    _count = 0;
}

void        blinkingLed::stop           (void)
{
	start(0);
}

void        blinkingLed::loop	 	    (void)
{
	uint32_t ima = millis();
	
	if (ima > _last + ERROR_BLINK_LONG && _rept > 0)
	{
		if (_count == _rept)
		{
			_last = ima;
			_count = 0;
			digitalWrite(_pin, LOW);
		}
		
		if (ima > _lastblink + ERROR_BLINK_SHORT)
		{
			if (!_status)
			{
				digitalWrite(_pin, HIGH);
				_status = true;
			}
			else
			{
				digitalWrite(_pin, LOW);
				_status = false;
				_count++;
			}
			_lastblink = ima;
		}
	}
}


//      _____________________________________________________________________
//      |                                                                   |
//      |       Real time clock                                             |
//      |___________________________________________________________________|
//


millisClock::millisClock()
{
    _id=F("MILLIS");    
}

millisClock::~millisClock()
{
    if (_rtcmillis != NULL) { delete _rtcmillis; }
}

void        millisClock::begin            (void)
{    
    char buf[LEN_DATETIMESTR];
    _rtcmillis = new RTC_Millis;
    _rtcmillis->begin(DateTime(F(__DATE__), F(__TIME__)));
    time_string(buf, FLASHEPOCH);
    debug("\nclock:%s:begin at %s", F2C(_id));
    time_string(buf, epoch());
    debug("\nclock: current datetime: %s", buf);
}

void        millisClock::set              (DateTime synctime)
{
    _rtcmillis->adjust(synctime);
}

uint32_t    millisClock::epoch            (void)
{
    if (_rtcmillis != NULL) { return _rtcmillis->now().unixtime(); }
    else                    { return 0; }
}

 
#if defined (RTC_MODULE)

ClockDS3231::ClockDS3231()
{
    _id=F("RTCLOCK DS3231");
    _rtclock = new RTC_DS3231;
    _current_time=FLASHEPOCH;
}

ClockDS3231::~ClockDS3231()
{
    if (_rtclock != NULL) { delete _rtclock; }
}



void            ClockDS3231::begin          (void)
{
    _flag=false;
    debug("\nclock:%s:begin", F2C(_id));
    
    char buf[LEN_DATETIMESTR];
    uint32_t startime=max(FLASHEPOCH, _current_time);
    startime = FLASHEPOCH;
    if ( _rtclock->begin())
    {
        if (_rtclock->lostPower())
        {
            _rtclock->adjust(startime);
            sendReport(3, _id, F("begin: power loss. Please check clock battery"));
        }
        if (!_rtclock->now().isValid())
        {
            _rtclock->adjust(startime);
            sendReport(3, _id, F("begin: invalid epoch. Please check connection"));
        }
        else
        {
            _flag=true;
            request_time();
            
            time_string(buf, _current_time);
            debug("\nclock: current datetime: %s", buf);
            return;
        }
    }
    else
    {
        sendReport(3, _id, F("begin: can not init DS3231 class"));
    }
    
    time_string(buf, _current_time);
    debug("\nclock: can not init DS3231");
    debug("\nclock: MILLIS start:\nclock: current datetime: %s", buf);
    _rtcmillis = new RTC_Millis;
    _rtcmillis->begin(DateTime(_current_time));
    blinker.start(3);
}

void            ClockDS3231::request_time   (void)
{
    DateTime now = _rtclock->now();
    if (now.isValid())
    {
        _current_time =  now.unixtime();
        if (!_flag)
        {
            _flag=true;
            if (_rtcmillis != NULL) { delete _rtcmillis; }
            _rtcmillis=NULL;
            blinker.stop();
            sendReport(1,_id, F("epoch: RTC found!"));
        }
    }
    else
    {
        if (_flag)
        {
            _flag=false;
            _rtcmillis = new RTC_Millis;
            _rtcmillis->begin(DateTime(_current_time));
            blinker.start(3);
            sendReport(3,_id, F("epoch: RTC signal lost!"));
        }
        _current_time = _rtcmillis->now().unixtime();
    }
}

void            ClockDS3231::loop           (void)
{
    if (millis() > _last_update + CLOCK_REFRESH_RATE)
    {
        _last_update = millis();
        request_time();
        
        if (!_flag && millis() > _last_retry + DEVICE_INIT_RETRY_DELAY)
        {
            _last_retry = millis();
            begin();
        }
    }
}

void            ClockDS3231::set            (DateTime synctime)
{
    if (_flag)  { _rtclock->adjust(synctime);   }
    else        { _rtcmillis->adjust(synctime); }
}


#endif // RTC MODULE
    
#endif // ARDUINO

