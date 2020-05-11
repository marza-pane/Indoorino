# 1 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino"
# 2 "/home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino" 2


SoftwareSerial SerialDebug(D5, D6, false);


void setup()
{
    Serial.begin(115200L);
    SerialDebug.begin(115200L);

    Serial.println(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "debug_packets.ino" "." "12" "." "21" "\", \"aSM\", @progbits, 1 #"))) = ("\n\n\tDEBUG START - all debug will be sent in the debug port"); &__c[0];}))))));

    benchmark_debug();
    benchmark_board();
    benchmark_utils();
    benchmark_packets();
    debughelper(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "debug_packets.ino" "." "18" "." "22" "\", \"aSM\", @progbits, 1 #"))) = ("\n\n"); &__c[0];}))))));

//     delay(5000);
//     resetBoard();
}

void loop()
{
    delay(500);
    SerialDebug.print(".");
    Serial.print(".");
}
