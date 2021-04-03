#include "common.h"

NetworkConnection   network;
IndoorinoEspClient  client;
ConfRouter          conf;

void setup()
{
    blinker.begin();
    utils::board::debug_init();
    utils::board::io.begin();
    
    rtc.begin();
    conf.begin();
    conf.factory();
    network.begin();
    
    /* client.begin() is run in loop */
}
void loop()
{
    static iEpoch_t timeout=0;
    static iEpoch_t timeout_status=0;
    
    blinker.loop();
    rtc.loop();
    
    network.loop();
    client.loop();
    utils::board::io.loop();
    
    if (millis() > timeout)
    {
        SerialDebug.print(".");
//         debug_os("[RAM: %u KB] ", utils::board::available_ram());
        timeout = millis() + 1000;
    }
    
    while (!client.incoming().is_empty())
    {
        packet::netpacket * p = client.incoming().pop_front();
//         debug_client("NAME COMPARE: <%s>|<%s>",p->target, P2C(BOARD_NAME));
        if (strcmp_P(p->target, BOARD_NAME) == 0)
        {
            p->eval();
        }
        else
        {
            utils::board::io.sendSerial(p);
        }
        delete p;
    }
    
    if (!client.status())
    {
        return;
    }
    
    if (millis() > timeout_status)
    {
        utils::board::send_status();
        timeout_status = millis() + RATE_UPDATE;
    }
}
