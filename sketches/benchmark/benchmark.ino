/*

#include "common.h"
#include "netutils.h"

// SoftwareSerial  SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);

// TODO provare UNO con inline SoftwareSerial SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false); 

void setup()
{
    utils::board::debug_init();
    Serial.begin(SERIAL_DEFAULT_BAUDRATE);
    Serial.print(F("\n\n\t*** BEGIN! ***"));
    
    SerialDebugPrint(F("\nThis is the first\n"));
//     benchmark::b_debug();
//     benchmark::b_board();
//     benchmark::b_utils();
//     benchmark::b_queue();
//     benchmark::b_list();
    benchmark::b_packets();
    
}
void loop()
{
    SerialDebug.print(".");
    Serial.print(".");
    delay(500);
}

*/
