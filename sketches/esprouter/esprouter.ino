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
//     conf.factory();
    network.begin();
    
    utils::board::send_boot_signal();

    /* client.begin() and utils::board::send_boot_signal() are called in loop() */
}
void loop()
{
    static iEpoch_t timeout_stat=0;
    static iEpoch_t timeout_conf=0;
    
    blinker.loop();
    rtc.loop();
    
    network.loop();
    client.loop();
    utils::board::io.loop();
    
    static iEpoch_t timeout=0;
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
    
    /* This send status every RATE_UPDATE secs. */
    if (millis() > timeout_stat)
    {
        utils::board::send_status();
        timeout_stat = millis() + RATE_UPDATE;
    }
    
    /* This send config every RATE_BEACON secs. */
    if (millis() > timeout_conf)
    {
        utils::board::send_config();
        timeout_conf = millis() + RATE_BEACON;
    }
}
