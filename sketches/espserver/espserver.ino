#include "indoorino.h"
#include "netutils.h"

#if defined(DEBUG_FLAG) /* Default debug port */                                                                                                                                                                      
SoftwareSerial  SerialDebug(DEBUG_PIN_RX, DEBUG_PIN_TX, false);
#endif

blinkingLed     blinker;
BoardIO         boardio(false);
millisClock     rtc; 
ConfEspServer   conf;
WiFiUDP         Udp;

connectionLoop  connection;
PacketList      packetlist;

char boardname[LEN_NAME];

uint32_t total_errors=0;
uint32_t total_rx_ser=0;
uint32_t total_rx_udp=0;
uint32_t total_tx_ser=0;

 // total_tx_udp is <boardio._total_tx> (protected)


void setup()
{

    strcpy(boardname,P2C(BOARD_NAME));
    Serial.begin(SERIAL_DEFAULT_BAUDRATE);
    Serial.setTimeout(0);
    blinker.begin();
    
    #if defined(DEBUG_FLAG)
        /* Default debug port */                                                                                                                                                                      
        SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
    #endif
        
    #ifndef ESP8266
    blinker.start(10);
    while (!Serial)  { blinker.loop(); }
    blinker.stop();
    #endif
    
    debug("\n\n<INIT:%s>\n",P2C(BOARD_NAME));        
    delay(2000);
    
    boardio.begin();       
    rtc.begin();
    
    connection.begin();
    debug("\n<INIT:CONF>\n");
    conf.begin();

//     sendConfig();
    sendReport(1, boardname, F("SERVER STARTS"));
}

void loop()
{
    blinker.loop();
    boardio.loop();
    rtc.loop();
    
    static uint32_t z=0;
    if (millis() > (z + 500))
    {
        SerialDebugPrint(".");
        z=millis();
    }
    
    
    static uint32_t s=0;
    if (millis() > (s + REFRESH_RATE_1))
    {
        sendStatus();
        s=millis();
    }

//    ________________________________________________________________________
//    |                                                                      |
//    |    PARSING SERIAL                                                    |
//    |______________________________________________________________________|
//

//     if (Serial.available())
//     {
//         debug("\nserver:SERIAL: new data!");
//         uint32_t t = millis() + DEFAULT_PACKET_TIMEOUT;
//         ipacket * rx_ser = nullptr;
//         while (Serial.available())
//         {            
//             boardio.append((uint8_t)Serial.read());
//             rx_ser = boardio.packet();
//             if (rx_ser != nullptr)
//             {
//                 debug("\nserver:SERIAL: new packet");
//                 boardio.send(rx_ser, boardio.buffername());
// // // //                 packetlist.push(rx_ser, boardio.buffername());
//                 total_rx_ser++;
//                 return;
//             }
// // // //             debug_io(" .");
//             if (millis() > t)
//             {
//                 io_error("\nserver:SERIAL: packet timout!");
//                 boardio.refresh();
//                 return;
//             }
// 
//         }
//     }
        


// // // //     if (Serial.available())
// // // //     {
// // // //         uint32_t t = millis() + DEFAULT_PACKET_TIMEOUT;
// // // //         ipacket * rx_ser = nullptr;
// // // //         debug("\nserver:SERIAL: new data!\n");
// // // //         while (rx_ser == nullptr)
// // // //         {
// // // // //             debug(" .");
// // // //             if (millis() > t)
// // // //             {
// // // // //                 debug("BREAK!");
// // // //                 boardio.refresh();
// // // //                 return;
// // // //             }
// // // //             
// // // //             boardio.append((uint8_t)Serial.read());
// // // //             rx_ser = boardio.packet();
// // // //         }
// // // //         debug("\nserver:SERIAL: new packet");
// // // //         packetlist.push(rx_ser, boardio.buffername());
// // // //     }

//    ________________________________________________________________________
//    |                                                                      |
//    |    checking connection                                               |
//    |______________________________________________________________________|
//

    connection.loop();
    if (!connection.status())
    {
        return;
    }

//    ________________________________________________________________________
//    |                                                                      |
//    |    PARSING UDP                                                       |
//    |______________________________________________________________________|
//
    
    packetlist.loop();
    
    while (Udp.parsePacket())
    {
        // Parsing is different from serial because udp packets are in fact indoorino packets
        debug_net("\nserver:UDP: new data!");
        ipacket * rx_udp = nullptr;
        while (Udp.available())
        {
            boardio.rx()->append(Udp.read());
            rx_udp = boardio.rx()->packet();
            if (rx_udp != nullptr)
            {
                debug_net("\nserver:UDP: new packet");
                total_rx_udp++;
                if (rx_udp->command() == IBACOM_ACK)
                {
                    /* IT'S AN ACK PACKET! (finally some delicious checksums) */ 

                    uint32_t checksum;
                    memcpy(&checksum, rx_udp->p_value(), sizeof(uint32_t));
                    debug_net(" CHECKSUM !", checksum);
                    
                    char ip[LEN_IPADDRESS] {0};
                    strcpy(ip, Udp.remoteIP().toString().c_str());

                    if (!packetlist.match(ip, checksum)) 
                    {
                        error_io("\nserver:ERROR: unknown checksum %u from %s", checksum, ip);
                        total_errors++;
                    }
                    
                }
                else if (strcmp(boardio.rx()->buffername(),boardname) != 0)
                {
                    /* IT'S NOT FOR THE SERVER :( (nobody sends messages to servers) */
                    
                    if (rx_udp->command() == IBACOM_UNIX_EPOCH)
                    {
                        rtc.set(*rx_udp->p_epoch());
                    }

                    debug_net(" -- re-routing through Serial [%s : %s]",
                              boardio.rx()->buffername(), boardname);
                    boardio.tx()->sendSerial(rx_udp, boardio.rx()->buffername());
                    total_tx_ser++;
                }
                else
                {
                    /* YAY IT'S FOR THE SERVER :) (finally someone sent a message to server) */
                    debug_net(" -- parsing packet %s:%s", boardname, F2C(rx_udp->label()));                    
//                     if (rx_udp->check())       { rx_udp->eval(); }
                }
            }
        }
    }
    
    
    
   
}
