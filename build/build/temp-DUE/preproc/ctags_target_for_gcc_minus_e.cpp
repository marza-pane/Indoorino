# 1 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
# 2 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 2

blinkingLed blinker;
ClockDS3231 rtc;
BoardIO boardio(0x1);
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


        Serial1.begin(115200L);



    blinker.start(10);
        // TODO: what to do with unavailable Serial port
    while (!Serial) { blinker.loop(); }
    blinker.stop();


    debughelper((reinterpret_cast<const __FlashStringHelper *>(("\n<INIT:%s>"))),({ char buf[64]; strcpy((buf), (BOARD_NAME)); buf; }));
    boardio.begin();
    sendReport(1, (reinterpret_cast<const __FlashStringHelper *>(BOARD_NAME)), (reinterpret_cast<const __FlashStringHelper *>(("BOARD STARTS"))));

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
    sendReport(1,(reinterpret_cast<const __FlashStringHelper *>(("SAMPLER"))),(reinterpret_cast<const __FlashStringHelper *>(("BOARD READY"))));

}

void loop() {

    blinker.loop();
    rtc.loop();
    boardio.loop();
//     sd.loop();

    static uint32_t s=0;
    if (millis() > (s + 30000))
    {
//         boardio.sendStatus();
        s=millis();
        debughelper((reinterpret_cast<const __FlashStringHelper *>(("SAMPLER:LOOP"))));
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
