#include "utils.h"

#if defined (ESP8266)
SoftwareSerial  SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);
#endif

void setup()
{
    Serial.begin(SERIAL_DEFAULT_BAUDRATE);
    SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
    
    Serial.println(F("\n\n\tDEBUG START - all debug will be sent in the debug port"));

    benchmark_debug();
    benchmark_board();
    benchmark_utils();
    benchmark_packets();
    debug("\n\n");
    
//     delay(5000);
//     resetBoard();
}

void loop()
{
    delay(500);
    SerialDebugPrint(".");
    Serial.print(".");
}
