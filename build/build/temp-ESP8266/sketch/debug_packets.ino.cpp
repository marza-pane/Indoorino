#include <Arduino.h>
#line 1 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino"
#include "utils.h"

#if defined (ESP8266)
SoftwareSerial  SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);
#endif

#line 7 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino"
void setup();
#line 24 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino"
void loop();
#line 7 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino"
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

