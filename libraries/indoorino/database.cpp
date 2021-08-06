// /*
//  * database.cpp
//  *
//  *  Created 3.1.0 on: May 8, 2021 (SERVER)
//  * 
//  *      Author: n00b
//  */
// 
// #if defined (INDOORINO_NETWORK)
// 
// #include "paths.h"
// #include "database.h"
// #include "../network/server.h"
// 
// namespace indoorino
// {
//     namespace db
//     {
// 
//         int             DataBase::load_to_list  (std::vector<packet::ipacket>& buffer)
//         {
//             const char * filepath = paths.db_probes();
//             
//             std::ifstream file;
//             file.open(filepath, std::ios::in | std::ios::binary);
// 
//             
//             /** Checking if file is good **/
// 
//             if(!file.is_open())
//             {
//                 error_io("DATABASE:load: could not open to <%s>", filepath);
//                 return -1;
//             }
// 
//             if(!file.good())
//             {
//                 error_io("DATABASE:load: could not read from <%s>", filepath);
//                 file.close();
//                 return -2;
//             }
// 
//             uint32_t packetcount;
//             file.read((char *)&packetcount, sizeof(uint32_t));
//             
//             if (packetcount == 0)
//             {
//                 warning_io("DATABASE: %s is empty!", filepath);
//                 return 0;
//             }
//             
//             
//             for (uint32_t i=0; i<packetcount; i++)
//             {
//                 packet::ipacket s(IBACOM_PROBE_AMBIENT);
//                 iSize_t size = s.data_size();
// 
//                 file.read((char *)s.payload(), size);
//                 if (file.eof())
//                 {
//                     error_io("DATABASE:load: %s got EOF at packet %u (header)!", filepath, i);
//                     file.close();
//                     return -3;
//                 }
//                 
//                 buffer.push_back(s);
//             }
//             info_io("DATABASE:load: loading System.database from %s [%u packets]", filepath, packetcount);
//             file.close();
//             return buffer.size();
//             
//             
//         }
//         
//         int             DataBase::exist         (const char * bname)
//         {
//             int i=0;
//             for (auto &b : _data)
//             {
//                 if (strcmp(b.boardname(), bname) == 0)
//                 {
//                     return i;
//                 }
//                 else
//                 {
//                     i++;
//                 }
//             }
//             return -1;
//         }
//         
//         void            DataBase::parse         (packet::ipacket * incoming)
//         {
//             if (incoming->command() != IBACOM_PROBE_AMBIENT)
//             {
// //                     alert_os("PROBE:can not init probe with %s", incoming->label());
//                 return;
//             }
//             
//             
// //                 packet::ipacket test(
//             
//             
//             for (auto &b : _data)
//             {
//                 if (strcmp(b.boardname(), incoming->p_board()) == 0)
//                 {
// //                         warning_os("PROBE:### parsing for board %s", incoming->p_board());
//                     b.parse(incoming);
//                     return;
//                 }
//             }
//             
// //                 error_os("PROBE:### adding new board %s", incoming->p_board());
//             _data.push_back(Board(incoming->p_board()));
//             _data.back().parse(incoming);
// 
//         }
//         
//         bool            DataBase::load         (void)
//         {
// 
//             std::vector<packet::ipacket> buffer;
//             if ( load_to_list(buffer) > 0 )
//             {
//                 for (auto &p: buffer)
//                 {
//                     this->parse(&p);
//                 }
//                 return true;
//             }
//             return false;
// 
//         }
//         
//         bool            DataBase::save         (void)
//         {
//             
//             uint32_t packetcount = 0;
//             for (auto &b: _data)
//             {
//                 for (auto &d: b.devices())
//                 {
//                     packetcount +=d.probes().size();
//                 }
//             }
//             
//             if (packetcount == 0)
//             {
//                 warning_io("DATABASE: no data to save!");
//                 return false;
//             }
//             
//             
//             std::vector<packet::ipacket> buffer_D;
//             std::vector<packet::ipacket> buffer_L;
//             
//             
//             
//             
//             for (auto &b: _data)
//             {
//                 for (auto &d: b.devices())
//                 {
//                     for (auto &p: d.probes())
//                     {
//                         
//                         packet::ipacket s(IBACOM_PROBE_AMBIENT);
//                         
//                         strcpy(s.p_board(),     b.boardname());
//                         strcpy(s.p_devname(),   d.devname());
//                         strcpy(s.p_desc1(),     p.type());
//                         memcpy(s.p_epoch(),     &p.epoch(), sizeof(uint32_t));
//                         
//                         int32_t v = int32_t(p.value() * FLOAT2UINT_M);
//                         memcpy(s.p_value1(), &v, sizeof(int32_t));
//                         
//                         buffer_L.push_back(s);
// 
//                     }
//                 }
//             }
//             
//             
//             if ( load_to_list(buffer_D) <= 0 )
//             {
//                 
//             }
//             else
//             {
//                 for (auto &pd: buffer_D)
//                 {
//                     bool flag=false;
//                     for (auto &pl: buffer_L)
//                     {
//                         if (memcmp(pd.payload(), pl.payload(), pd.data_size()) == 0)
//                         {
//                             flag=true;
//                             break;
//                         }
//                     }
//                     
//                     if (!flag)
//                     {
//                         warning_os("Pulling %s:%s:%s %s from file",
//                                     pd.p_board(), pd.p_devname(), pd.p_desc1(), utils::time_string(* pd.p_epoch()));
//                         buffer_L.push_back(pd);
//                     }
//                 }
//             }
//             
//             const char * filepath = paths.db_probes();
//             
//             
//             std::ofstream file;
//             file.open(filepath, std::ios::out | std::ios::binary);
//         
//             /** Checking if file is good **/
// 
//             if(!file.is_open())
//             {
//                 error_io("DATABASE:save: could not open to <%s>", filepath);
//                 return false;
//             }
//             
//             if(!file.good())
//             {
//                 error_io("DATABASE:save: could not write to <%s>", filepath);
//                 return false;
//             }
//             
//             packetcount = buffer_L.size();
//             file.write((char *)(&packetcount), sizeof(uint32_t));
//             
//             
//             for (auto &s: buffer_L)
//             {
//                 file.write((char *)(s.payload()), s.data_size());
//                 if(!file.good())
//                 {
//                     error_io("DATABASE:save: io error while writing to <%s>", filepath);
//                     file.close();
//                     return false;
//                 }                
//             }
// 
//             file.close();
//             info_os("DATABASE:save: writing System.database to %s [%u packets]", filepath, packetcount);
//             return true;
//             
//         }
//         
//         void            DataBase::query         (const char * bname, const char * dname, const char * desc, uint32_t first, uint32_t last)
//         {
//             
//             std::vector<packet::ipacket> buffer;
//             
//             for (auto &b: _data)
//             {
//                 if ( (strlen(bname) == 0) || (strcmp(bname, b.boardname()) == 0) )
//                 {
//                     for (auto &d: b.devices())
//                     {
//                         if ( (strlen(dname) == 0) || (strcmp(dname, d.devname()) == 0) )
//                         {
//                             std::vector<std::string>    descriptions;
//                             bool flag_desc=false;
//                             /* A list with requested descriptions */
//                             for (auto &p: d.probes())
//                             {
//                                 if ( (strlen(desc) == 0) || (strcmp(desc, p.type()) == 0) )
//                                 {
//                                     flag_desc=false;
//                                     for (auto &s: descriptions)
//                                     {
//                                         if (strcmp(p.type(), s.c_str()) == 0)
//                                         {
//                                             flag_desc=true;
//                                             break;
//                                         }
//                                     }
//                                     if (!flag_desc) descriptions.push_back(std::string(p.type()));
//                                 }
//                             }
//                                                             
//                             for (auto &s: descriptions)
//                             {
//                                 iSize_t count=0;
// //                                 std::cout << "Probe-send: current description = " << s.c_str() << std::endl;
//                                 
//                                 packet::ipacket w(IBACOM_SYS_PROBE_DATA);
// 
//                                 strcpy(w.p_board(),     b.boardname());
//                                 strcpy(w.p_devname(),   d.devname());
//                                 strcpy(w.p_desc1(),     s.c_str());
//                                 
//                                 for (auto &p: d.probes())
//                                 {
//                                     if ( strcmp(s.c_str(), p.type()) == 0 )
//                                     {
//                                         if ( (first == last) || (p.epoch() >= first && p.epoch() <= last) )
//                                         {                                            
//                                             if (count == ( SRV_PROBE_PACKET_SIZE / sizeof(int32_t) ))
//                                             {
//                                                 count=0;
//                                                 Server.shell.broadcast(&w);
//                                                 memset(w.p_table1(), 0, SRV_PROBE_PACKET_SIZE);
//                                                 memset(w.p_table2(), 0, SRV_PROBE_PACKET_SIZE);
//                                             }
//                                             
//                                             int32_t v = int32_t(p.value() * FLOAT2UINT_M);
// //                                             std::cout << "Pushing " << utils::time_string(p.epoch()) << " " << s.c_str() << ": " << v << std::endl;
//                                             memcpy(w.p_table1() + count * sizeof(uint32_t), &p.epoch(), sizeof(uint32_t));
//                                             memcpy(w.p_table2() + count * sizeof(int32_t), &v, sizeof(int32_t));
// 
//                                             count++;
//                                         }
//                                     }
//                                 }
//                                 
//                                 Server.shell.broadcast(&w);
//                             }
//                         }
//                         
//                     }
//                 }
//             }
//         }
//     
//     } /* namespace:db */
//     
// } /* namespace:indoorino */
//     
// #endif /* INDOORINO_NETWORK */
