#include "indoorino.h"

ConfController  conf;
IndoorinoDeviceList devices;

#define DOTTING_PERIOD 1000

void setup()
{

    delay(1000); // we wait server to boot
    
    utils::board::debug_init();
    utils::board::io.begin();
        
    blinker.begin();
    
    rtc.begin();
//     conf.factory();
    conf.begin(); /* must be run only once! */
    
// // //     sendReport(1, FPSTR(BOARD_NAME), F("Board %s:%s booting"), P2C(BOARD_NAME), P2C(INDOORINO_TYPE));    
        
    
    utils::board::send_boot_signal();
    
    utils::board::send_config();

    devices.begin();
    
    utils::board::send_status();
}

void loop()
{
    static iEpoch_t timeout_stat=0;
    static iEpoch_t timeout_conf=millis() + RATE_BEACON;

    blinker.loop();
    rtc.loop();
    utils::board::io.loop();
    devices.loop();
    
    if (!utils::board::io.incoming().is_empty())
    {
        packet::netpacket * p = utils::board::io.incoming().pop_front();
        debug_pack("Got %s from serial", p->label());
        
        
        if (strcmp_P(p->target, BOARD_NAME) == 0) p->eval();
        else if (strcmp_P(p->target, "ALL") == 0) p->eval();
        
        delete p;
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

    static iEpoch_t timeout_dot=0;
    if (millis() > timeout_dot)
    {
        timeout_dot=millis() + DOTTING_PERIOD;
        SerialDebug.print(".");
//         info_mem("free heap @ <mainloop> [%u] KB", utils::board::available_ram());
    }
}

