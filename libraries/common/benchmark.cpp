/*
 * common/benchmark.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: n00b
 */

#include "indoorino.h"

#if defined (DEBUG_BENCHMARK)

#if defined(ARDUINO)
    #include "stddev.h"
#endif
    
#if defined(EEPROM_MODULE)
#include <EEPROM.h>
void        dumpnEEPROM         (iSize_t start, iSize_t size)
{
    char b[8] {0};
    for (iSize_t i=0; i<size; i++)
    {
        uint8_t buf = EEPROM.read(start + i);

        if (isprint(buf)) 
        {
            sprintf(b, "\'%c\'|", buf);
            SerialDebugPrint(b);
        }
        else
        {
            sprintf(b, " %u |", (uint8_t)buf);
            SerialDebugPrint(b);
        }
        if (i > 0 && i % 25 == 0) SerialDebugPrint(">\n>");
    }
}
#endif

const char ip0[] PROGMEM = "192.168.1.12";
const char ip1[] PROGMEM = "0.0.0.0";
const char ip2[] PROGMEM = "127.0.0.1";

    
const char s[] PROGMEM = "**This is a PROGMEM string**";
const char c[] PROGMEM = "-- This %s is %s to %s --";
const __FSH * f; /* statement-expressions are not allowed outside functions */

class Test
{
    char * p=nullptr;
public:
     Test() { p = (char *)calloc(10,10); alert_os("Allocating TEST"); };
    ~Test() { alert_os("De-allocating TEST"); if (p) { free(p); p=nullptr;} };
    
    int x=0;
    int y=0;
    char string[10] {0};
};

namespace benchmark
{
    void        b_debug     (void)
    {
        
        SerialDebugPrint(F("Testing debug\n"));

        SerialDebugPrint(F("\tDEBUG_MEMORY: "));
        debug_mem("pass");
        SerialDebugPrint(F("\tDEBUG_DEVICES: "));
        debug_dev("pass");
        SerialDebugPrint(F("\tDEBUG_IO: "));
        debug_io("pass");
        SerialDebugPrint(F("\tDEBUG_NET: "));
        debug_net("pass");
        
        SerialDebugPrint(F("\nTesting WARNINGS: \n"));
                
        error_mem("error_MEM: pass");
        error_io("error_IO: pass");
        error_os("error_OS: pass");
        error_dev("error_DEV: pass");
        error_board("error_BOARD: pass");
        error_server("error_SERVER: pass");
        error_client("error_CLIENT: pass ");

        warning_mem("warning_MEM: pass");
        warning_io("warning_IO: pass");
        warning_os("warning_OS: pass");
        warning_dev("warning_DEV: pass");
        warning_board("warning_BOARD: pass");
        warning_server("warning_SERVER: pass");
        warning_client("warning_CLIENT: pass");
        
        alert_mem("alert_MEM: pass");
        alert_io("alert_IO: pass");
        alert_os("alert_OS: pass");
        alert_dev("alert_DEV: pass");
        alert_board("alert_BOARD: pass");
        alert_server("alert_SERVER: pass");
        alert_client("alert_CLIENT: pass");
        
        info_mem("info_MEM: pass");
        info_mem("info_MEM: pass");
        info_io("info_IO: pass");
        info_os("info_OS: pass");
        info_dev("info_DEV: pass");
        info_board("info_BOARD: pass");
        info_server("info_SERVER: pass");
        info_client("info_CLIENT: pass");

       
        f = F("**This is a flash string**");

        SerialDebugPrint(F("\n\nTesting Flash Arguments\n"));

        info_os("\tDEBUG:passing NO ARGS");
        info_os("\tDEBUG:passing %s %s", "RAM string", "argument");
    
        SerialDebugPrint(F("\tDEBUG:Passing a PROGMEM argument: "));
        info_os("%s", P2C(s));
        SerialDebugPrint(F("\tDEBUG:Passing a __FSH argument: "));
        info_os("%s", F2C(f));
        SerialDebugPrint(F("\tDEBUG:Passing multiple args: "));
        info_os("%s %s", P2C(s), F2C(f));
        SerialDebugPrint(F("\tDEBUG:formatting FSH with sprintf: "));
        
        char buff[SERIAL_TX_BUFFER_SIZE] {0};
        sprintf_P(buff, c, "string", "compiled", "format");
        info_os("%s", buff);
    }

    void        b_board     (void)
    {

        SerialDebugPrint(F("\n\nBOARD DEFAULT DEFINITIONS (please enable DEBUG_GENERIC)\n"));
    
        info_board("\tBoard   type:   %s", P2C(BOARD_TYPE));
        info_board("\tProject type:   %s", P2C(INDOORINO_TYPE));
        info_board("\tNetwork name:   %s", P2C(BOARD_NAME));
        info_board("\tDefault deices: %u", DEFAULT_DEVNUM);
        
        #if defined (ARDUINO)
            info_board("\tTotal/free RAM: %u/%u", utils::board::available_ram(), RAMEND);
            
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

            info_board("\n\tChip Id       : %08X", ESP.getChipId());

            s = ESP.getCoreVersion();
            s.getBytes(buffer, SERIAL_TX_BUFFER_SIZE);
            info_board("\tCore version  : %s", buffer);
            
            s = ESP.getSdkVersion();
            s.getBytes(buffer, SERIAL_TX_BUFFER_SIZE);
            info_board("\tSDK  version  : %s", buffer);
            
            info_board("\tBoot version  : %u", ESP.getBootVersion());
            info_board("\tBoot mode     : %u", ESP.getBootMode());
            info_board("\tFlash chip Id : %08X", ESP.getFlashChipId());
            info_board("\tCPU frequency : %u MHz\nFile system (SPIFFS)", ESP.getCpuFreqMHz());
            info_board("\tSketch Flash RAM is size: %0.3f/%0.3f MB", flashChipSize,realFlashChipSize);
            info_board("\tFlash frequency  : %f MHz", flashFreq); 
            info_board("\tFlash write mode : %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

            info_board("\tSRAM Total : %f KB", fileTotalKB);
            info_board("\tSRMA Used  : %f KB", fileUsedKB);
            info_board("\tBlock size : %lu B", fs_info.blockSize);
            info_board("\tPage size  : %lu B", fs_info.pageSize);
            info_board("\tMaximum open files   : %lu", fs_info.maxOpenFiles);
            info_board("\tMaximum path length  : %lu", fs_info.maxPathLength);

            Dir dir = SPIFFS.openDir("/");
            info_board("\tSPIFFS directory {/} :");
            while (dir.next())
            {
                s = dir.fileName();
                s.getBytes(buffer, SERIAL_TX_BUFFER_SIZE);
                info_board("\t%s",buffer);
            }

        #endif /* ESP8266 */
            
        #endif /* ARDUINO */
    }

    void        b_utils     (void)
    {
        
        SerialDebugPrint(F("\nUTILS BENCHMARK (please enable DEBUG_GENERIC)"));
        
        char buffer[UINT8_MAX - 1];
        for (uint8_t i=0; i<UINT8_MAX; i++) buffer[i]=i; 

        
        info_os("\n\tDump Array:");
        utils::dump_hex(buffer, UINT8_MAX - 1);
        info_os("\tTime String (2000): %s",     utils::timestring_epoch32(SECONDS_FROM_1970_TO_2000));
        info_os("\tTime String (2010): %s",     utils::timestring_epoch32(SECONDS_FROM_1970_TO_2010));
        info_os("\tTime String (2100): %s",     utils::timestring_epoch32(SECONDS_FROM_1970_TO_2100));

        #if defined (INDOORINO_NETWORK)
        info_os("\tTime (64) String (2000): %s",     utils::timestring_epoch64(SECONDS_FROM_1970_TO_2000));
        info_os("\tTime (64) String (2010): %s",     utils::timestring_epoch64(SECONDS_FROM_1970_TO_2010));
        info_os("\tTime (64) String (2100): %s",     utils::timestring_epoch64(SECONDS_FROM_1970_TO_2100));
        #endif
        
        #if defined(ARDUINO)
        info_os("\tTime String (compiletime): %s", utils::timestring_epoch32(DateTime(F(__DATE__), F(__TIME__)).unixtime()));
        #endif
        info_os("\tTime String (epoch): %s",       utils::timestring_epoch32(utils::epoch_now()));
        info_os("\tTime String (random): %s",      utils::timestring_epoch32(utils::random_signed(0, SECONDS_FROM_1970_TO_2010)));
        
        info_os("\tDelta Time  (secs): %s",      utils::format_seconds(utils::random_signed(0, 3600)));
        info_os("\tDelta Time  (hour): %s",      utils::format_seconds(utils::random_signed(3600, SECONDS_PER_DAY)));
        info_os("\tDelta Time  (days): %s",      utils::format_seconds(utils::random_signed(SECONDS_PER_DAY, SECONDS_PER_DAY * 30)));
        info_os("\tDelta Time  (years):%s",      utils::format_seconds(utils::random_signed(SECONDS_PER_YEAR, SECONDS_PER_YEAR * 20)));

        #if defined (__linux__)
        auto t0 = std::chrono::system_clock::now();
        auto t1 = t0 + std::chrono::seconds(10);
        
        info_os("\tchrono2unixepoch (now):%s", utils::timestring_epoch32(utils::chrono2unixepoch(std::chrono::system_clock::now())));
        info_os("\tTime chrono (now):%s",utils::timestring_chrono(t0));
        info_os("\tTime chrono (now):%s",utils::timestring_chrono(t1));
        info_os("\tDuration Time  (secs):%s", utils::format_duration(std::chrono::duration_cast<std::chrono::seconds>(t1 - t0)));
        #endif
    }
    
    void        b_queue     (void)
    {

        SerialDebugPrint(F("\nUTILS BENCHMARK (please enable DEBUG_GENERIC)\n"));

        SerialDebugPrint("\n\tQueue: pushing - pulling - clear\n");
        utils::Queue<int> que_int;
        for (uint8_t i=0; i<10; i++)
        {
            info_io("pushing and appending %u [%u]", i, que_int.count());
            que_int.push_back(i);
            que_int.push_front(i);
            
        }
        
        while (que_int.count() > 0)
        {
           info_io("popping [%u] %u", que_int.count(), que_int.pop_front());
           info_io("popping [%u] %u", que_int.count(), que_int.pop_back());
        }

        
        SerialDebugPrint("\n\tQueueObject: pushing - pulling - clear\n");
        utils::ObjectQueue<Test> que_pac;
        for (uint8_t i=0; i<20; i++)
        {
            Test * t = new Test();
            t->x=i+2;
            t->y=20-i;
            que_pac.push_back(t);
            info_io("pushing %u:%u [%u:%u]", t->x, t->y, que_pac.is_empty(), que_pac.count());
        }
        Test * r;
        while (que_int.count() > 5)
        {
            r = que_pac.pop_front();
            info_io("popping %u:%u [%u:%u]", r->x, r->y, que_pac.is_empty(), que_pac.count());
            delete r;
            r = que_pac.pop_back();
            info_io("popping %u:%u [%u:%u]", r->x, r->y, que_pac.is_empty(), que_pac.count());
            delete r;
        }
        info_io("len= %u", que_pac.count());
        que_pac.clear();
    }
    
    void        b_list      (void)
    {
        
        SerialDebugPrint("\n\tList: pushing - pulling - clear\n");
        utils::List<int> q;
        for (uint8_t i=0; i<10; i++)
        {
            info_io("pushing %u [%u]", i, q.count());
            q.push(i);
            info_io("append  %u [%u]", i, q.count());
            q.append(i);
            info_io("read h  %d", q[0]);
            info_io("read b  %d", q[-1]);
            
//             #if defined(ARDUINO)
//             debug("\t- Free SRAM is %u", utils::board::available_ram());
//             if (utils::board::available_ram() < 250)    break;
//             #endif
        }
        
        iSize_t s=q.count();
        for (int i=0; i<s; i++)
        {
            info_io("reading [%d]", q[i]);
            info_io("gnidaer [%d]", q[-i]);
        }

        for (uint8_t i=0; i<s; i++)
        {
           info_io("popping %u [%u]", q.pop(), q.count());
        }
        
        q.clear();

        
        SerialDebugPrint("\n\tListObject: pushing - pulling - clear\n");
        utils::ObjectList<Test> f;
        for (uint8_t i=0; i<10; i++)
        {
            Test * t = new Test();
            t->x=i+2;
            t->y=20-i;
            f.push(t);
            info_io("append  %u:%u [%u:%u]", t->x, t->y, f.is_empty(), f.count());
            t = new Test();
            t->x=i+8;
            t->y=28-i;
            f.append(t);
            info_io("pushing %u:%u [%u:%u]", t->x, t->y, f.is_empty(), f.count());
        }

        s=f.count();
        Test * r;
        for (uint8_t i=0; i<s; i++)
        {
            r = f[i];
            info_io("reading [%u] [%u:%u]", i, r->x, r->y);
        }
        for (uint8_t i=0; i<s; i++)
        {
            r = f.pop();
            info_io("popping %u:%u [%u:%u]", r->x, r->y, f.is_empty(), f.count());
            delete r;
        }
        
        for (uint8_t i=0; i<10; i++)
        {
            Test * t = new Test();
            t->x=i+2;
            t->y=20-i;
            f.push(t);
            t = new Test();
            t->x=i+8;
            t->y=28-i;
            f.append(t);
            info_io("pushing[%u:%u]", f.is_empty(), f.count());
        }
        s=f.count();
        for (uint8_t i=0; i<s; i++)
        {
            info_io("removing [%u]", i);
            f.rem(0);
        }
        for (uint8_t i=0; i<10; i++)
        {
            Test * t = new Test();
            t->x=i+2;
            t->y=20-i;
            f.push(t);
            t = new Test();
            t->x=i+8;
            t->y=28-i;
            f.append(t);
            info_io("pushing[%u:%u]", f.is_empty(), f.count());
        }
        s=f.count();
        for (uint8_t i=0; i<s; i++)
        {
            r = f[i];
            info_io("reading [%u] [%u:%u]", i, r->x, r->y);
        }

        debug_mem("Removing specific 10: %u:%u", f[10]->x, f[10]->y);
        f.rem(10);

        debug_mem("Removing specific 3: %u:%u", f[3]->x, f[3]->y);
        f.rem(3);

        debug_mem("Removing specific 5: %u:%u", f[5]->x, f[5]->y);
        f.rem(5);
        
        s=f.count();
        for (uint8_t i=0; i<s; i++)
        {
            r = f[i];
            info_io("reading [%u] [%u:%u]", i, r->x, r->y);
        }
        f.clear();
                
    }

    void        b_packets       (void)
    {
        SerialDebugPrint(F("\n\nPACKETS DEFINITIONS (please enable DEBUG_GENERIC)\n\n"));
        
        packet::variable_fst_ s;
        packet::packet_fst_ p;
        utils::ObjectQueue<packet::ipacket> qpacket;
       
        info_pack("Variable Map:");
        for (iCom_t i=0; i<VARMAP_NUMBER; i++)
        {
            memcpy_P(&s, &packet::VariableMap[i], sizeof(s)); 
            info_pack("\t[%03.3u] : %s [type:size] = [%c:%u]", i, s.name, s.type, s.size);
        }
            
        info_pack("Packet Map:");
        for (iCom_t i=0; i<TOTAL_IBACOM; i++)
        {
            memcpy_P(&p, &packet::PacketMap[i], sizeof(p)); 
            info_pack("\t[%03.3u] : IBACOM %u:%s -- count[%u] : ", i, p.comm, p.label,  p.count);
            for (iCom_t k=0; k<p.count; k++)
            {        
                for (iCom_t j=0; j<VARMAP_NUMBER; j++)
                {
                    if (p.params[k] == j)
                    {
                        memcpy_P(&s, &packet::VariableMap[j], sizeof(s)); 
                        info_pack("\t\t[%03.3u] : %s [type:size] = [%c:%u]", j, s.name, s.type, s.size);
                    }
                }
            }
        }
        
        info_pack("\nNow testing all packets...");
        for (iCom_t i=0; i<TOTAL_IBACOM; i++)
        {
            memcpy_P(&p, &packet::PacketMap[i], sizeof(p));
            qpacket.push_back(new packet::ipacket(p.comm));
    #if defined(ARDUINO)
            info_pack("\t- Free SRAM is %u", utils::board::available_ram());
            if (utils::board::available_ram() < 250)    break;
    #endif
        }    
        
        info_pack("\nMax allocalbe packets for %s : %u/%u\n", P2C(BOARD_TYPE), qpacket.count(), TOTAL_IBACOM);
        SerialDebugPrint("\n");
        while(qpacket.count())
        {
            packet::ipacket * p = qpacket.pop_front();
            info_pack("%s", p->description());
//             dump_packet(p);
            delete p;
        }
        
        info_pack("\n\nNow testing packet methods...");


//         packet::ipacket * q = new packet::ipacket(IBACOM_REPORT);
    #if defined(ARDUINO)
            info_pack("\t- Free SRAM is %u", utils::board::available_ram());
            if (utils::board::available_ram() < 250)    return;
    #endif        
//         debug_io("label: %s", q->label());
//         debug_io("description: %s", q->description());
        
//         debug_io("dump:");
//         q->dump();
//         
//         char head[SIZEOF_PACKET_HEADER];
//         q->header(head,"SOURCE","TARGET");
//         utils::dump_hex(head, SIZEOF_PACKET_HEADER);
//         
//         delete q;
//         
//         q = new packet::ipacket(head);
//         debug_io("dump:");
//         q->dump();       
//         
//         debug_io("\n\nNow testing packet queues...");
//         utils::ObjectQueue<packet::netpacket> qp;
//         
//         packet::netpacket * h = new packet::netpacket;
//         h=q;
//         strcpy(h->source, "SOURCE");
//         strcpy(h->target, "TARGET");
//         debug_io("Pushing %s", h->label());
//         qp.push_back(h);
//         
//         
//         packet::ipacket * k = new packet::ipacket(1001);
//         h = new packet::netpacket;
//         strcpy(h->source, "ORIGIN");
//         strcpy(h->target, "DESTINATION");
//         h = k;
//         debug_io("Pushing %s", h->label());
//         qp.push_back(h);
//         
//         
//         packet::ipacket * u = new packet::ipacket(2522);
//         h = new packet::netpacket;
//         strcpy(h->source, "AAAA");
//         strcpy(h->target, "BBBB");
//         h = u;
//         debug_io("Pushing %s", h->label());
//         qp.push_back(h);
//         
//         packet::netpacket * j= qp.pop_front();
//         debug_io("Popping %s", j->description());
//         delete j;
//         
//         j= qp.pop_front();
//         debug_io("Popping %s", j->description());
//         delete j;
//         
//         j=qp.pop_front();
//         debug_io("Popping %s", j->description());
//         delete j;
        

#if defined(__linux__)        
        utils::ObjectQueue<packet::netpacket> npq; 
        packet::netpacket * g0 = new packet::netpacket();
        npq.push_back(g0);
        npq.clear();
#endif
//         debug_io("Packet test finished!\n\nQueue size is %u", qp.count());
        fflush(stdout);
    }

}

#endif /* DEBUG_BENCHMARK */

// void        benchmark_config    (void)
// {
// #if defined(ARDUINO) && !defined(ESP8266)
//     
// //     delay(2000);
// //     conf.factory();
// //     conf.begin();
// //     dumpnEEPROM(0,500);
// //     sendConfig();
// //     
// //     debug("\n\n\t*** TESTING BOARDCONF ***\n");
// //     
// //     ipacket * ptr0 = new ipacket(IBACOM_CONF_LDR);
// //     ipacket * ptr1 = new ipacket(IBACOM_RESET);
// // 
// //     debug("\n\n\t*** ADD nullptr ***\n");
// //     conf.devAdd(nullptr);
// // 
// //     debug("\n\n\t*** ADD wrong device ***\n");
// //     conf.devAdd(ptr1);
// // 
// //     debug("\n\n\t*** ADD wrong device type ***\n");
// //     ptr1=reallocPacket(ptr1, IBACOM_REPORT);
// //     conf.devAdd(ptr1);
// // 
// //     debug("\n\n\t*** ! ADD valid DHT ***\n");
// // 
// //     ptr1=reallocPacket(ptr1, IBACOM_CONF_DHT22);
// //     strcpy(ptr1->p_devname(), "DHTX");
// //     *ptr1->p_pin1() = 22;
// //     conf.devAdd(ptr1);
// //     
// //     debug("\n\n\t*** ADD busy pin ***\n");
// //     strcpy(ptr0->p_devname(), "LDRX");
// //     *ptr0->p_pin1() = 22;
// //     conf.devAdd(ptr0);
// //     
// //     debug("\n\n\t*** ! ADD valid LDR ***\n");
// //     *ptr0->p_pin1() = 24;
// //     conf.devAdd(ptr0);
// // 
// //     debug("\n\n\t*** MOD LDR wrong name ***\n");
// //     conf.devMod("LDRY", ptr0);
// // 
// //     debug("\n\n\t*** MOD LDR - wrong type ***\n");
// //     *ptr1->p_pin1() = 28;
// //     conf.devMod("LDRX", ptr1);
// // 
// //     debug("\n\n\t*** MOD LDR - used pin ***\n");
// //     *ptr0->p_pin1() = 22;
// //     conf.devMod("LDRX", ptr0);
// // 
// //     debug("\n\n\t*** ! MOD LDR ***\n");
// //     *ptr0->p_pin1() = 26;
// //     conf.devMod("LDRX", ptr0);
// //     
// //     debug("\n\n\t*** ! SETNAME LDRX --> LDR1 name***\n");
// // 
// //     *ptr1->p_pin1() = 22;
// //     conf.devSetName("LDRX", "LDR1");
// // 
// //     debug("\n\n\t*** MOD LDR wrong name ***\n");
// //     *ptr1->p_pin1() = 30;
// //     conf.devMod("LDRX", ptr1);
// // 
// //     debug("\n\n\t*** MOD LDR wrong type ***\n");
// //     *ptr1->p_pin1() = 30;
// //     conf.devMod("LDR1", ptr1);
// //     
// //     debug("\n\n\t*** REM LDR wrong name ***\n");
// //     conf.devRem("LDRY");
// //    
// //     debug("\n\n\t*** Removing LDR ***\n");
// //     conf.devRem("LDR1");
// // 
// //     debug("\n\n\t*** Removing DHT ***\n");
// //     conf.devRem("DHTX");
// //    
// //     delete ptr0;
// //     delete ptr1;
// //     
// //     
// //     debug("\n\n\t*** Resetting to factory ***\n");
// //     
// //     delay(5000);
// //     conf.factory();
// //     dumpnEEPROM(0,500);
// //     sendConfig();
// 
// #endif /* ARDUINO */
// }
