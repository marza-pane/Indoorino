/*
 * benchmark.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: n00b
 */

#include "indoorino.h"
// #if defined(ARDUINO)
//     #include "RTClib.h"
//     #include "boardconf.h"

// 
// #endif

const char ip0[] PROGMEM = "192.168.1.12";
const char ip1[] PROGMEM = "0.0.0.0";
const char ip2[] PROGMEM = "127.0.0.1";

    
const char s[] PROGMEM = "**This is a PROGMEM string**";
const char c[] PROGMEM = "-- This %s is %s to %s --";
const __FSH * f; /* statement-expressions are not allowed outside functions */

void        benchmark_debug     (void)
{
    
    SerialDebugPrint(F("\nTRYING DEBUG FLAG"));
    {
        SerialDebugPrint(F("\n\tDEBUG_GENERIC: "));
        debug("pass");
        SerialDebugPrint(F("\n\tDEBUG_DEVICES: "));
        debug_dev("pass");
        SerialDebugPrint(F("\n\tDEBUG_MEMORY: "));
        debug_mem("pass");
        SerialDebugPrint(F("\n\tDEBUG_IO: "));
        debug_io("pass");
        SerialDebugPrint(F("\n\tDEBUG_NET: "));
        debug_net("pass");
        SerialDebugPrint(F("\n\tWARNING_MEM: "));
        error_mem("INFO: pass");
        SerialDebugPrint(F("\n\tWARNING_IO: "));
        error_io("INFO: pass");
        SerialDebugPrint(F("\n\tWARNING_DEV: "));
        error_dev("INFO: pass");
    }
    
    f = F("**This is a flash string**");

    SerialDebugPrint(F("\n\nTRYING DEBUG ARGS"));
    {
        debug("\n\tDEBUG:passing NO ARGS");
        debug("\n\tDEBUG:passing %s %s", "RAM string", "argument");
    
        SerialDebugPrint(F("\n\tDEBUG:Passing a PROGMEM argument: "));
        debug("%s", P2C(s));
        SerialDebugPrint(F("\n\tDEBUG:Passing a __FSH argument: "));
        debug("%s", F2C(f));
        SerialDebugPrint(F("\n\tDEBUG:Passing multiple args: "));
        debug("%s %s %s", P2C(ip0), P2C(ip1), P2C(ip2));
        SerialDebugPrint(F("\n\tDEBUG:formatting FSH with sprintf: "));
        
        char buff[SERIAL_TX_BUFFER_SIZE] {0};
        sprintf_P(buff, c, "string", "compiled", "format");
        debug("%s", buff);
        
        
 
    }
}

void        benchmark_esp       (void)
{
    #if defined(ESP8266)
    String s;
    unsigned char buffer[SERIAL_TX_BUFFER_SIZE];
    
    SPIFFS.begin();
    FSInfo fs_info;
    SPIFFS.info(fs_info);

    float fileTotalKB = (float)fs_info.totalBytes / 1024.0; 
    float fileUsedKB = (float)fs_info.usedBytes / 1024.0; 
  
    float flashChipSize = (float)ESP.getFlashChipSize() / 1024.0 / 1024.0;
    float realFlashChipSize = (float)ESP.getFlashChipRealSize() / 1024.0 / 1024.0;
    float flashFreq = (float)ESP.getFlashChipSpeed() / 1000.0 / 1000.0;
    FlashMode_t ideMode = ESP.getFlashChipMode();
  

    SerialDebugPrint(F("\n\nESP BOARD PROPERTIES\nFirmware:"));

    debug("\n\tChip Id       : %08X", ESP.getChipId());

    s = ESP.getCoreVersion();
    s.getBytes(buffer, SERIAL_TX_BUFFER_SIZE);
    debug("\n\tCore version  : %s", buffer);
    
    s = ESP.getSdkVersion();
    s.getBytes(buffer, SERIAL_TX_BUFFER_SIZE);
    debug("\n\tSDK  version  : %s", buffer);
    
    debug("\n\tBoot version  : %u", ESP.getBootVersion());
    debug("\n\tBoot mode     : %u", ESP.getBootMode());
    debug("\n\tFlash chip Id : %08X", ESP.getFlashChipId());
    debug("\n\tCPU frequency : %u MHz\nFile system (SPIFFS)", ESP.getCpuFreqMHz());
    debug("\n\tSketch Flash RAM is size: %0.3f/%0.3f MB", flashChipSize,realFlashChipSize);
    debug("\n\tFlash frequency  : %f MHz", flashFreq); 
    debug("\n\tFlash write mode : %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

    debug("\n\tSRAM Total : %f KB", fileTotalKB);
    debug("\n\tSRMA Used  : %f KB", fileUsedKB);
    debug("\n\tBlock size : %lu B", fs_info.blockSize);
    debug("\n\tPage size  : %lu B", fs_info.pageSize);
    debug("\n\tMaximum open files   : %lu", fs_info.maxOpenFiles);
    debug("\n\tMaximum path length  : %lu", fs_info.maxPathLength);

    Dir dir = SPIFFS.openDir("/");
    debug("\n\tSPIFFS directory {/} :");
    while (dir.next())
    {
        s = dir.fileName();
        s.getBytes(buffer, SERIAL_TX_BUFFER_SIZE);
        debug("%s",buffer);
    }
    #endif
}

void        benchmark_board     (void)
{

    SerialDebugPrint(F("\n\nBOARD DEFAULT DEFINITIONS (please enable DEBUG_GENERIC)"));
   
    debug("\n\tBoard   type:   %s", P2C(BOARD_TYPE));
    debug("\n\tProject type:   %s", P2C(INDOORINO_TYPE));
    debug("\n\tNetwork name:   %s", P2C(BOARD_NAME));
    debug("\n\tDefault deices: %u", DEFAULT_DEVNUM);
    debug("\n\tTotal/free RAM: %u/%u", freeRam(), RAMEND);

    #if defined SD_MODULE
    debug("\n\tSD card [PIN:SPEED]    %u:%u", SD_PIN, SD_SPEED_MHZ);
    debug("\n\tSD configuration path: %s", P2C(SD_CONF_PATH));
    debug("\n\tSD probe path:         %s", P2C(SD_PROBE_PATH));
    #endif
    
    benchmark_esp();
}

void        benchmark_utils     (void)
{
    
    SerialDebugPrint(F("\n\nUTILS BENCHMARK (please enable DEBUG_GENERIC)"));
    
    char buffer[UINT8_MAX - 1];
    for (uint8_t i=0; i<UINT8_MAX; i++) buffer[i]=i; 
    
    debug("\n\tDump Array:\n");
    dump_array(buffer, UINT8_MAX - 1);
    time_string(buffer, SECONDS_FROM_1970_TO_2000);
    debug("\n\tTime String (2000): %s", buffer);
    time_string(buffer, SECONDS_FROM_1970_TO_2010);
    debug("\n\tTime String (2010): %s", buffer);
    time_string(buffer, SECONDS_FROM_1970_TO_2100);
    debug("\n\tTime String (2100): %s", buffer);

#if defined(ARDUINO)
    time_string(buffer, DateTime(F(__DATE__), F(__TIME__)).unixtime());
    debug("\n\tTime String (compiletime): %s", buffer);
#endif
    
    time_string(buffer, 1588283820);
    debug("\n\tTime String (random): %s", buffer);
    
    debug("\n\tIP (%s):", P2C(ip0));
    if (is_string_ip(P2C(ip0)))
    {
        debug(" VALID!");
    }
    else
    {
        debug(" INVALID!");
    }
    debug("\n\tIP (%s):", P2C(ip1));
    if (is_string_ip(P2C(ip1)))
    {
        debug(" VALID!");
    }

    else
    {
        debug(" INVALID!");
    }
    debug("\n\tIP (%s):", P2C(ip2));
    if (is_string_ip(P2C(ip2)))
    {
        debug(" VALID!");
    }
    else
    {
        debug(" INVALID!");
    }
    
}

void        benchmark_packets   (void)
{

    SerialDebugPrint(F("\n\nPACKETS DEFINITIONS (please enable DEBUG_GENERIC)"));
    
    varmap_template s;
    packetmap_template p;
    ipacket ** l;

    l = (ipacket **)calloc(TOTAL_IBACOM, sizeof(ipacket *));
    
    
    debug("\n\nVariable Map:");
    for (ibavar_t i=0; i<VARMAP_NUMBER; i++)
    {
        memcpy_P(&s, &VarTable[i], sizeof(s)); 
        debug("\n\t[%03.3u] : %s [type:size] = [%c:%u]", i, s.name, s.type, s.size);
    }
        
    debug("\n\nPacket Map:");
    for (ibacomm_t i=0; i<TOTAL_IBACOM; i++)
    {
        memcpy_P(&p, &PackeTable[i], sizeof(p)); 
        debug("\n\t[%03.3u] : IBACOM %u:%s ", i, p.comm, p.label);
        debug("-- fields[%u] : ", p.fields);
        for (ibavar_t k=0; k<p.fields; k++)
        {        
            for (ibavar_t j=0; j<VARMAP_NUMBER; j++)
            {
                if (p.var[k] == j)
                {
                    memcpy_P(&s, &VarTable[j], sizeof(s)); 
                    debug("\n\t\t[%03.3u] : %s [type:size] = [%c:%u]", j, s.name, s.type, s.size);
                }
            }
        }
    }
    
    ibacomm_t max_packet=0;
    debug("\n\nNow testing all packets...");
    for (ibacomm_t i=0; i<TOTAL_IBACOM; i++)
    {
        memcpy_P(&p, &PackeTable[i], sizeof(p)); 
        l[i] = new ipacket(p.comm);
        debug("\t- Free SRAM is %u", freeRam());
        max_packet++;
#if defined(ARDUINO)
        if (freeRam() < 250)    break;
#endif
    }    
    
    debug("\nMax allocalbe packets for %s : %u/%u\n", P2C(BOARD_TYPE), max_packet, TOTAL_IBACOM);

    for (ibacomm_t i=0; i<max_packet; i++)
    {
        dump_packet(l[i]);
        delete l[i];
    }
    
    debug("\nPacket test finished!\n\n");
    fflush(stdout);
    
}

void        benchmark_config    (void)
{
#if defined(ARDUINO)
    sendConfig();
    
    debug("\n\n\t*** TESTING BOARDCONF ***\n");
    
    ipacket * ptr0 = new ipacket(IBACOM_CONF_LDR);
    ipacket * ptr1 = new ipacket(IBACOM_RESET);

    debug("\n\n\t*** Adding nullptr ***\n");
    conf.devAdd(nullptr);

    debug("\n\n\t*** ADD wrong device ***\n");
    conf.devAdd(ptr1);

    debug("\n\n\t*** ADD wrong device type ***\n");
    ptr1 = reallocPacket(ptr1, IBACOM_REPORT);
    conf.devAdd(ptr1);

    debug("\n\n\t*** Adding DHTX ***\n");

    ptr1 = reallocPacket(ptr1, IBACOM_CONF_DHT22);
    strcpy(ptr1->p_devname(), "DHTX");
    *ptr1->p_pin() = 22;
    conf.devAdd(ptr1);
    
    debug("\n\n\t*** ADD LDRX busy pin***\n");
    strcpy(ptr0->p_devname(), "LDRX");
    *ptr0->p_pin() = 22;
    conf.devAdd(ptr0);
    
    debug("\n\n\t*** Adding LDRX ***\n");
    
    *ptr0->p_pin() = 24;
    conf.devAdd(ptr0);

    debug("\n\n\t*** MOD LDRY wrong name***\n");
    conf.devMod("LDRY", ptr0);

    debug("\n\n\t*** MOD LDRX - wrong type***\n");
    *ptr1->p_pin() = 28;
    conf.devMod("LDRX", ptr1);

    debug("\n\n\t*** MOD LDRX - used pin***\n");
    *ptr0->p_pin() = 22;
    conf.devMod("LDRX", ptr0);

    debug("\n\n\t*** Editing LDRX ***\n");
    *ptr0->p_pin() = 26;
    conf.devMod("LDRX", ptr0);
    
    debug("\n\n\t*** Editing LDRX - LDR1 name***\n");

    *ptr1->p_pin() = 22;
    conf.devSetName("LDRX", "LDR1");

    debug("\n\n\t*** MOD LDRX wrong name ***\n");
    *ptr1->p_pin() = 30;
    conf.devMod("LDRX", ptr1);

    debug("\n\n\t*** MOD LDR1 wrong type ***\n");
    *ptr1->p_pin() = 30;
    conf.devMod("LDR1", ptr1);
    
    debug("\n\n\t*** REM LDRY wrong name ***\n");
    conf.devRem("LDRY");
   
    debug("\n\n\t*** Removing LDR1 ***\n");
    conf.devRem("LDR1");

    debug("\n\n\t*** Removing DHTX ***\n");
    conf.devRem("DHTX");
   
    delete ptr0;
    delete ptr1;
    
#if defined (ESP8266)
    
    debug("\n\n\t*** TESTING ESP ADDRESSES***\n");
    debug("\n\t*** Adding 192.168.1.14:5678 ***\n");

    netaddress n("192.168.1.14", 5678);
    conf.addAddress(&n);
    sendConfig();
    
    debug("\n\n\t*** Adding 192.168.1.16:9678 ***\n");

    n.set("192.168.1.16", 9678);
    conf.addAddress(&n);
    sendConfig();

    debug("\n\n\t*** Removing 192.168.1.14***\n");

    conf.remAddress("192.168.1.14");
    sendConfig();

    debug("\n\n\t*** Removing 192.168.1.16:9678***\n");

    conf.remAddress(&n);
    sendConfig();

#endif
    
    debug("\n\n\t*** Resetting to factory ***\n");
    
    delay(5000);
    conf.factory();
    sendConfig();

#endif
}




//         debug("\n[%03.3u]Packet %u:%s (size=%u/%u)",i,
//               l[i]->command(), F2C(l[i]->label()), l[i]->data_size(), l[i]->full_size());
//         
//         if(l[i]->p_message() != nullptr) debug("\n\t--> message");
//         if(l[i]->p_command() != nullptr) debug("\n\t--> command");
//         if(l[i]->p_name() != nullptr) debug("\n\t--> name");
//         if(l[i]->p_type() != nullptr) debug("\n\t--> type");
//         if(l[i]->p_board() != nullptr) debug("\n\t--> board");
//         if(l[i]->p_devname() != nullptr) debug("\n\t--> devname");
//         if(l[i]->p_epoch() != nullptr) debug("\n\t--> epoch");
//         if(l[i]->p_value() != nullptr) debug("\n\t--> value");
//         if(l[i]->p_freemem() != nullptr) debug("\n\t--> freemem");
//         if(l[i]->p_looptime() != nullptr) debug("\n\t--> looptime");
//         if(l[i]->p_errors() != nullptr) debug("\n\t--> errors");
//         if(l[i]->p_totalrx() != nullptr) debug("\n\t--> totalrx");
//         if(l[i]->p_totaltx() != nullptr) debug("\n\t--> totaltx");
//         if(l[i]->p_serialrx() != nullptr) debug("\n\t--> serialrx");
//         if(l[i]->p_serialtx() != nullptr) debug("\n\t--> serialrx");
//         if(l[i]->p_param1() != nullptr) debug("\n\t--> param1");
//         if(l[i]->p_param2() != nullptr) debug("\n\t--> param1");
//         if(l[i]->p_param3() != nullptr) debug("\n\t--> param1");
//         if(l[i]->p_param4() != nullptr) debug("\n\t--> param1");
//         if(l[i]->p_temperature() != nullptr) debug("\n\t--> temperature");
//         if(l[i]->p_humidity() != nullptr) debug("\n\t--> humidity");
//         if(l[i]->p_stepday1() != nullptr) debug("\n\t--> stepday1");
//         if(l[i]->p_stephour1() != nullptr) debug("\n\t--> stephour1");
//         if(l[i]->p_timeout() != nullptr) debug("\n\t--> timeout");
//         if(l[i]->p_port() != nullptr) debug("\n\t--> port");
//         if(l[i]->p_level() != nullptr) debug("\n\t--> level");
//         if(l[i]->p_status() != nullptr) debug("\n\t--> status");
//         if(l[i]->p_devnum() != nullptr) debug("\n\t--> devnum");
//         if(l[i]->p_pin() != nullptr) debug("\n\t--> pin");
//         if(l[i]->p_ip() != nullptr) debug("\n\t--> ip");
