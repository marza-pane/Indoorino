// /*
//  * database.h
//  *
//  *  Created 3.1.0 on: May 8, 2021 (SERVER)
//  * 
//  *      Author: n00b
//  */
// 
// #ifndef _SRC_DATABASE_H_
// #define _SRC_DATABASE_H_
// 
// #if defined (INDOORINO_NETWORK)
// 
// #include "../packets/ipacket.h"
// #include "layout.h"
// #include <deque>
// 
// namespace indoorino
// {
//     namespace db
//     {
// 
//         class DataBase
//         {
// 
//             class Board
//             { 
// 
//                 class Device
//                 {
//                     
//                     class Probe
//                     {
//                         
//                     private:
//                         uint32_t _epoch=0;
//                         double   _value=0.0;
//                         char     _desc[LEN_LABEL]   {0};
//                         char     _unit[LEN_DEVNAME] {0};
//                     public:
//                         Probe(const char * t, uint32_t e, double v)
//                         {
//                             strcpy(_desc, t);
//                             _epoch=e;
//                             _value=v;
//                         }
//                         ~Probe() {};
//                         
//                         const uint32_t & epoch  () { return _epoch; }
//                         const double & value    () { return _value; }
//                         const char * type       () { return _desc;  }
//                         
//                     };
//                 
//                 private: /* DEVICE */
//                     char _boardname[LEN_NAME] {0};
//                     char _devname[LEN_DEVNAME] {0};
//                     std::vector<Probe> _data;
// 
//                     void    parse   (packet::ipacket * incoming)
//                     {
//                         uint32_t e;
//                         double   v;
//                     
//                         if (strlen(incoming->p_desc1()))
//                         {
//                             const char * t = incoming->p_desc1();
//                             v = double(* incoming->p_value1()) / FLOAT2UINT_M;
//                             memcpy(&e, incoming->p_epoch(), sizeof(uint32_t));
//                             
//                             _data.push_back( Probe(t, e, v) );
// //                             error_os("PROBE:### adding new PROBE %s:%s %s %s = %f",
// //                                      _boardname, _devname, utils::time_string(_data.back().epoch()), _data.back().type(), _data.back().value());
//                         }
//                         
//                         if (strlen(incoming->p_desc2()))
//                         {
//                             const char * t = incoming->p_desc2();
//                             v = double(* incoming->p_value2()) / FLOAT2UINT_M;
//                             memcpy(&e, incoming->p_epoch(), sizeof(uint32_t));
//                             
//                             _data.push_back( Probe(t, e, v) );
// //                             error_os("PROBE:### adding new PROBE %s:%s %s %s = %f",
// //                                      _boardname, _devname, utils::time_string(_data.back().epoch()), _data.back().type(), _data.back().value());
//                         }
// 
//                         if (strlen(incoming->p_desc3()))
//                         {
//                             const char * t = incoming->p_desc3();
//                             v = double(* incoming->p_value3()) / FLOAT2UINT_M;
//                             memcpy(&e, incoming->p_epoch(), sizeof(uint32_t));
//                             
//                             _data.push_back( Probe(t, e, v) );
// //                             error_os("PROBE:### adding new PROBE %s:%s %s %s = %f",
// //                                      _boardname, _devname, utils::time_string(_data.back().epoch()), _data.back().type(), _data.back().value());
//                         }
//                         
//                         if (strlen(incoming->p_desc4()))
//                         {
//                             const char * t = incoming->p_desc4();
//                             v = double(* incoming->p_value4()) / FLOAT2UINT_M;
//                             memcpy(&e, incoming->p_epoch(), sizeof(uint32_t));
//                             
//                             _data.push_back( Probe(t, e, v) );
// //                             error_os("PROBE:### adding new PROBE %s:%s %s %s = %f",
// //                                      _boardname, _devname, utils::time_string(_data.back().epoch()), _data.back().type(), _data.back().value());
//                         }
//                     }
// 
//                     
//                 public: /* DEVICE */
//                     Device(const char * b, const char * d) 
//                     { 
//                         strcpy(_boardname, b); 
//                         strcpy(_devname, d);
// 
//                     }
//                     ~Device() { _data.clear(); }
// 
//                     const char * devname   () { return _devname; }
//                     const char * boardname () { return _boardname; }
//                     std::vector<Probe>& probes() { return _data; }
//                                     
//                     friend class Board;
//                 };
//                         
//             private: /* BOARD */
//                 
//                 char _boardname[LEN_NAME] {0};
//                 std::vector<Device> _data;
//                 
//                 void    parse   (packet::ipacket * incoming)
//                 {
//                     for (auto &d : _data)
//                     {
//                         if (strcmp(d.devname(), incoming->p_devname()) == 0)
//                         {
// //                             warning_os("PROBE:### parsing for device %s", incoming->p_devname());
//                             d.parse(incoming);
//                             return;
//                         }
//                     }
//                     
// //                     error_os("PROBE:### adding new device %s", incoming->p_devname());
//                     _data.push_back(Device(_boardname, incoming->p_devname()));
//                     _data.back().parse(incoming);
//                 }
// 
//             public: /* BOARD */
//                 Board(const char * b) { strcpy(_boardname, b); }
//                 ~Board() { _data.clear(); }
// 
//                 const char * boardname() { return _boardname; }
//                 std::vector<Device>& devices() { return _data; }
//                 
//                 int             exist       (const char * dname)
//                 {
//                     int i=0;
//                     for (auto &d : _data)
//                     {
//                         if (strcmp(d.devname(), dname) == 0)
//                         {
//                             return i;
//                         }
//                         else
//                         {
//                             i++;
//                         }
//                     }
//                     return -1;
//                 }
//                 
//                 const Device&   operator[]  (const char * dname)
//                 {
//                     int i=this->exist(dname);
//                 
//                     if (i != -1)
//                         return _data.at(i);
//                     
//                     error_os("FATAL:PROBE: invalid call for board <%s>", dname);
//                 }
//                 
//                 friend class DataBase;
//             };
//         
//         
//         
//         private: /* DATABASE */
//         
//             std::vector<Board> _data;
//             
//             int     load_to_list    (std::vector<packet::ipacket>& buffer);
//             
//         public: /* DATABASE */
//             
//             DataBase() {};
//             ~DataBase() { _data.clear(); }
//             
//             std::vector<Board>& board() { return _data; }
//             
//             int     exist       (const char *);
//             
//             void    parse       (packet::ipacket *);
// 
//             bool    save        (void);
// 
//             bool    load        (void);
//                     
//             void    query       (const char * bname="", const char * dname="", const char * desc="", uint32_t first=0, uint32_t last=0);
// 
//             const Board&    operator[]  (const char * bname)
//             {
//                 int i=this->exist(bname);
//                 
//                 if (i != -1)
//                     return _data.at(i);
//                 
//                 error_os("FATAL:PROBE: invalid call for board <%s>", bname);
//             }
//             
//         };
//         
//     } /* namespace:db */
//     
// } /* namespace:indoorino */
//     
// #endif /* INDOORINO_NETWORK */
// 
// #endif /* _SRC_DATABASE_H_ */
