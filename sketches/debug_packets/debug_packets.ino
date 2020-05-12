#include "indoorino.h"

#if defined (ESP8266)
SoftwareSerial  SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);
#endif

blinkingLed     blinker;
ClockDS3231     rtc; 
BoardIO         boardio(false);

void setup()
{
    Serial.begin(SERIAL_DEFAULT_BAUDRATE);
    SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
    boardio.begin();
    
    Serial.println(F("\n\n\tDEBUG START - all debug will be sent in the debug port"));

    benchmark_debug();
    benchmark_board();
    benchmark_utils();
    benchmark_packets();
//     benchmark_config();

    debug("\n\n");
    


}

void loop()
{
    delay(500);
    SerialDebugPrint(".");
    Serial.print(".");
}
