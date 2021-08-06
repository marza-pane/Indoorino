#include <Arduino.h>
#include "utils.h"
#include <EEPROM.h>

#define BUFFER_SIZE 300
int address = 0;
char buffer[BUFFER_SIZE];

void setup()
{
    utils::board::debug_init();
    memset(buffer, 0, BUFFER_SIZE);
   
    for (uint32_t i=0; i<BUFFER_SIZE; i++)
    {
        buffer[i] = EEPROM.read(i);
    }
    utils::dump_hex(buffer, BUFFER_SIZE);

}

void loop()
{
   
    delay(2000); //Delay 2 sec.
}

// n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.0.0-newlib4.0.0-gnu23-48f7b08/bin/../lib/gcc/xtensa-lx106-elf/10.2.0/../../../../xtensa-lx106-elf/bin/ld: /tmp/arduino_build_365271/libraries/common/utils.cpp.o:(.bss.SerialDebug+0x0): multiple definition of `SerialDebug'; /tmp/arduino_build_365271/sketch/esprouter.ino.cpp.o:(.bss.SerialDebug+0x0): first defined here
// collect2: error: ld returned 1 exit status
