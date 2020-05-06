#include "indoorino.h"

blinkingLed     blinker;
ClockDS3231     rtc; 
BoardIO         boardio(true);
// ConfSampler     conf;
// deviceList      devices;      
// sdProbe         sd;

uint32_t lastprobe=0;

// void probe(void)
// {
//     uint8_t probenum=5; // max probe per sensor
//     
//     ipacket   **  dev = (ipacket **) calloc (probenum, sizeof(ipacket *));
//     for (uint8_t i=0; i<probenum; i++)      { dev[i]=nullptr; }
// 
// 
//     for (uint8_t i=0; i<conf.devnum(); i++)
//     {
//         dev = devices(i)->probe(dev);
//         debug("\nProbing device %u/%u",i, conf.devnum());
//         for (uint8_t j=0; j<probenum; j++)
//         {
//             if (dev[j] != nullptr)
//             {
//                 sd.add(dev[j]);
//                 boardio.send(dev[j]);
//                 delete dev[j];
//                 dev[j]=nullptr;
//             }
//             else
//             {
//                 debug("\nPROBE: NULLPRT ");
//             }
//         }            
//     }
//     
//     for (uint8_t i=0; i<probenum; i++)
//     {
//         if (dev[i] == nullptr)     { delete dev[i]; }
//     }
//     
//     free(dev);
//     
// }

void setup()
{

    delay(500); // we wait server to boot
    blinker.begin();
    
    #if defined(DEBUG_FLAG) /* Default debug port */                                                                                                                                                                      
        SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
    #endif
        
    #ifndef ESP8266
    blinker.start(10);
        // TODO: what to do with unavailable Serial port
    while (!Serial)  { blinker.loop(); }
    blinker.stop();
    #endif
    
    debug("\n<INIT:%s>",P2C(BOARD_NAME));        
    boardio.begin();
    sendReport(1, FPSTR(BOARD_NAME), F("BOARD STARTS"));    
    
    rtc.begin();
//     debug("\n<INIT:CONF>\n");
//     conf.begin();
//     boardio.sendConfig();
// 
//     debug("\n<INIT:SENSORS>\n");
//     devices.begin();
// 
// 
//     debug("\n<INIT:SD>\n");
//     sd.begin();
//     lastprobe=rtc.epoch();
//    
//     
//     boardio.sendStatus();
//     debug("BOARD:START");
    sendReport(1,F("SAMPLER"),F("BOARD READY"));

}

void loop() {
    
    blinker.loop();
    rtc.loop();
    boardio.loop();
//     sd.loop();
      
    static uint32_t s=0;
    if (millis() > (s + REFRESH_RATE_1))
    {
//         boardio.sendStatus();
        s=millis();
        debug("SAMPLER:LOOP");
    }

//     static uint32_t z=0;
//     if (millis() > (z + 1000))
//     {
//         boardio.sendReport(1,F("SAMPLER"),F("LOOP"));
//         z=millis();
//     }
    
//     if (rtc.epoch() > lastprobe + conf.step())
//     {
//         lastprobe=rtc.epoch();
//         devices("FAN1")->set(0);
//         delay(100);
//         probe();
//         boardio.sendStatus();
//     }
//     else if (rtc.epoch() > lastprobe + conf.step() - conf.cool())
//     {
//         devices("FAN1")->set(1);
//     }    
}
