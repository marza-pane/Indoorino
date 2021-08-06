// #include <Arduino.h>
// #include "definitions.h"
// void setup()
// {
//     
// 
// }
// void loop(){
// 
//     Serial.begin(SERIAL_DEFAULT_BAUDRATE);
//     Serial.print("\n\n\t*** SERIAL PORT START ***\nNow writing pins...\n");
// 
//     for (int i=2; i<64; i++)
//     {
// 
//         SoftwareSerial  SerialDebug(i, i+1, false);
//         Serial.print("INIT SoftwareSerial(");
//         Serial.print(i);
//         Serial.print(", ");
//         Serial.print(i+1);
//         Serial.println("); now begin ...");
// 
//         SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
//         delay(100);
//         SerialDebug.print("==> RX PIN #");
//         SerialDebug.print(i);
//         SerialDebug.print(" - TX PIN #");
//         SerialDebug.println(i+1);
// 
//         Serial.print("WROTE to pin ");
//         Serial.println(i+1);
//         
//         SerialDebug.end();        
//     }
// 
//     delay(2000); //Delay 2 sec.
// }
// 
// // n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.0.0-newlib4.0.0-gnu23-48f7b08/bin/../lib/gcc/xtensa-lx106-elf/10.2.0/../../../../xtensa-lx106-elf/bin/ld: /tmp/arduino_build_365271/libraries/common/utils.cpp.o:(.bss.SerialDebug+0x0): multiple definition of `SerialDebug'; /tmp/arduino_build_365271/sketch/esprouter.ino.cpp.o:(.bss.SerialDebug+0x0): first defined here
// // collect2: error: ld returned 1 exit status
