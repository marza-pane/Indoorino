/*
 * espServerUtils.cpp
 *
 *  Created on: May 11, 2020
 *      Author: n00b
 */

#include "../indoorino/indoorino.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    DEBUG TOOLS   (keeps connection alive)                            |
//    |______________________________________________________________________|
//

connectionLoop::connectionLoop ()
{
    _last = millis();
    WiFi.mode(WIFI_STA);
}

void                connectionLoop::begin       (void)
{
    char ssid[64];
    char psk[64];
    
    strcpy(ssid, P2C(DEFAULT_SSID));
    strcpy(psk, P2C(DEFAULT_PSK));
    
    debug("\n%s Connecting to : %s (%s)... ",P2C(INDOORINO_TYPE), ssid, psk);
    WiFi.begin(ssid, psk);
    _reqtime=millis() + AUTH_TIMEOUT;

}

void                connectionLoop::loop        (void)
{
    if (_status)
    {
        /* If connected to Wifi, just check for connection status every RECONNECT_TIMEOUT */
        if (millis() - _last < RECONNECT_TIMEOUT)
        {
            _last = millis();
            if (!WiFi.isConnected())
            {
                debug("\nConnection Lost! Reconnecting...");
                _status=false;
                _reqtime=millis() + AUTH_TIMEOUT;
                Udp.stop();
                blinker.start(1);
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
                char ip[LEN_IPADDRESS];

                _status=true;
                _last=millis();
                
                Udp.begin(DEFAULT_LOCALPORT);
                blinker.stop();
                
                WiFi.localIP().toString().toCharArray(ip,LEN_IPADDRESS);    
                debug("\n ### Connected (channel %u)! ###",WiFi.channel());
                debug("\n ### UDP server address %s:%u ###\n", ip, DEFAULT_LOCALPORT);
                sendReport(1, BOARD_NAME, F("ESP connected to %s address :%s:%u"),
                                   P2C(DEFAULT_SSID),ip,DEFAULT_LOCALPORT);  
            }
        }
        else
        {
                debug("\nReconnection timed out! Retrying...");
                _status=false;
                _reqtime=millis() + AUTH_TIMEOUT;
                WiFi.reconnect();            
        }
        
    }
}
