# 1 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
# 2 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 2

blinkingLed blinker;
ClockDS3231 rtc;
BoardIO boardio(true);
ConfSampler conf;
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


        SerialDebug.begin(115200L);



    blinker.start(10);
        // TODO: what to do with unavailable Serial port
    while (!Serial) { blinker.loop(); }
    blinker.stop();


    debughelper((reinterpret_cast<const __FlashStringHelper *>(
# 66 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
   (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 66 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
   "\n<INIT:%s>"
# 66 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
   ); &__c[0];}))
# 66 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
   )),({ char buf[64]; strcpy_P(buf, BOARD_NAME); buf; }));
    boardio.begin();
    rtc.begin();
    sendReport(1, (reinterpret_cast<const __FlashStringHelper *>(BOARD_NAME)), (reinterpret_cast<const __FlashStringHelper *>(
# 69 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
                                    (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 69 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
                                    "BOARD STARTS"
# 69 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
                                    ); &__c[0];}))
# 69 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
                                    )));

    debughelper((reinterpret_cast<const __FlashStringHelper *>(
# 71 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
   (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 71 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
   "\n<INIT:CONF>\n"
# 71 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
   ); &__c[0];}))
# 71 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
   )));
    conf.begin();
    sendConfig();

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
    sendReport(1,(reinterpret_cast<const __FlashStringHelper *>(
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
                "SAMPLER"
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
                ); &__c[0];}))
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
                )),(reinterpret_cast<const __FlashStringHelper *>(
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
                             (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
                             "BOARD READY"
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
                             ); &__c[0];}))
# 86 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
                             )));

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
        debughelper((reinterpret_cast<const __FlashStringHelper *>(
# 102 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
       (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 102 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
       "SAMPLER:LOOP"
# 102 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino" 3
       ); &__c[0];}))
# 102 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/sampler/sampler.ino"
       )));
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
