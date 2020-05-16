#include "indoorino.h"

#if defined (ESP8266)
SoftwareSerial  SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);
WiFiUDP Udp;
#endif

#if defined (INDOORINO_SAMPLER)
    ConfSampler     conf;
#elif defined (INDOORINO_ESPSERVER)
    ConfEspServer   conf;
#else
    ConfBase        conf;
#endif /* PROJECTS */

blinkingLed     blinker;
ClockDS3231     rtc; 
BoardIO         boardio(false);

void setup()
{
    Serial.begin(SERIAL_DEFAULT_BAUDRATE);
    SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
    boardio.begin();
    
    Serial.println(F("\n\n\tDEBUG START - all debug will be sent in the debug port"));


//     benchmark_debug();
//     benchmark_board();
//     benchmark_utils();
//     benchmark_packets();

//     conf.begin();
//     benchmark_config();

    debug("\n\n");
    


}

void loop()
{
    delay(500);
    SerialDebugPrint(".");
    Serial.print(".");
}

// Conf:begin: complete!
// 
// NET:address:set:st_address:invalid port 0 for 80.48.0.0
// Conf:init: dev array: [ 178 ]
// 
// ESPserver:begin: complete
// iop:sendConfig:devnum is 0
// Conf:begin Parameter 0:TEST [ TEST ] 
// Conf:begin Parameter 1:ESPSERVER [ ESPSERVER ] 
// Conf:begin Parameter 2:WEMOS [ WEMOS ] 
// Conf:begin: complete!
// 
// NET:address:set:st_address:invalid port 0 for 80.48.0.0
// Conf:init: dev array: [ 178 ]
