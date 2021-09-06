
/*
 * database.h
 *
 *  Created 3.1.0 on: May 8, 2021 (SERVER)
 * 
 *      Author: n00b
 */

// #include <stdio.h>
// #include <cmath>
// #include <time.h>
// #include <string.h>
// #include <iostream>
// #include <chrono>
// #include <algorithm>
// #include <vector>

#include "common/utils.h"

#ifndef _SRC_DATABASE_H_
#define _SRC_DATABASE_H_

#if defined(INDOORINO_NETWORK)

#include "timer.h"


namespace indoorino
{
    
    namespace grow
    {


        
        
        enum class GrowPhase
        {
            growing,
            blooming,
            harvest,
        };
        
        extern GrowPhase grow_phase;
        
        struct StatusTemplate
        {
            bool on_growing()   { return (grow_phase == GrowPhase::growing);  }
            bool on_blooming()  { return (grow_phase == GrowPhase::blooming); }
            bool on_harvest()   { return (grow_phase == GrowPhase::harvest); }
        };
        
        
        //      _________________________________________
        //      |                                       |
        //      |       L I G H T S                     |
        //      |_______________________________________|
        //
                
//         namespace light
//         {
//                         
//             class Lights : public indoorino::TimerBase
//             {
// 
//             private:
// 
//                 struct
//                 {
//                     /* time inerval for a day, should be 24h */
//                     std::chrono::hours  cycle_duration = std::chrono::hours(24);
// 
//                     /* time interval between start_point and light start */
//                     std::chrono::hours  cycle_start = std::chrono::hours(6);
// 
//                     
//                     /* Parameters for shifting smoothly between phases */
//                     struct {
//                         bool    state=false;
//                         uint    growing_days = 40;
//                         uint    bloomin_days = 15;
//                         uint    harvest_days = 15;
//                         std::chrono::hours  max_growing_hours = std::chrono::hours(22);
//                         std::chrono::hours  min_growing_hours = std::chrono::hours(18);
// 
//                         std::chrono::hours  min_blooming_hours = std::chrono::hours(12);
//                         std::chrono::hours  max_blooming_hours = std::chrono::hours(12);
//                 
//                     } smooth;
//                     
//                     struct {
//                         bool state=false;
//                         std::chrono::minutes  shifting = std::chrono::minutes(20);
//                 
//                     } dim;
//                     
//                     /* default growing cycle time */
//                     std::chrono::hours  std_growing_hours = std::chrono::hours(22);
//                     
//                     /* default blooming cycle time */
//                     std::chrono::hours  std_blooming_hours = std::chrono::hours(12);
// 
//                 } _model;
//                 
//                 struct : public StatusTemplate
//                 {
//                     /* time start of internal clock*/
//                     std::chrono::system_clock::time_point start_point;
// 
//                     /* time start after blooming command */
//                     std::chrono::system_clock::time_point start_blooming;
//                     
//                     /* current cycle clock*/
//                     std::chrono::system_clock::time_point current_point;
// 
//                 } _status;
//                 
//             private:
//                 bool    _on_duty=false;
// 
//             public:
//                 auto&   status() { return _status; }
//                 auto&   model()  { return _model; }
//                 
//             public:
//                  Lights() {}
//                 ~Lights() { this->stop(); }
// 
//                 void    start           (std::chrono::system_clock::time_point& s)
//                 {
//                     _status.start_point = s;
//                     _status.current_point = _status.start_point;
//                     _on_duty = true;
//                 }
//                 
//                 void    stop            (void)
//                 {
//                     _on_duty = false;
//                     indoorino::TimerBase::stop();
//                 }
//                 
//                 void    loop            (void)
//                 {
//                     if (!_on_duty) return;
//                                        
//                     auto now=std::chrono::system_clock::now();
//                     if (now < _status.current_point) return;
// 
//                     /* internal clock made a whole cycle so we update timers */
//                     
//                     std::vector<std::chrono::seconds> shifts;
//                     
//                     for (uint i=0; i<_timers.size(); i++)
//                     {
//                         if (_model.dim.state)
//                             shifts.push_back(_model.dim.shifting * i);
//                         else
//                             shifts.push_back(std::chrono::seconds(0));
//                     }
//                     
// //                             shifts.push_back(std::chrono::seconds(i * _model.dim.shifting));
//                     
//                     if (_status.on_blooming())
//                     {
//                         if (_model.smooth.state)
//                         {
//                         }
//                         else
//                         {
//                             for (uint i = 0; i < _timers.size(); i++)
//                             {
//                                 std::chrono::system_clock::time_point s;
//                                 s=_status.current_point + _model.cycle_start;
//                                 
//                                 std::cout << "Adding blooming light timer: " << 
//                                 _timers.at(i)->boardname() << ":" << 
//                                 _timers.at(i)->devname() << " -- start: " <<
//                                 utils::timestring_chrono(s) << " -- finish: " << 
//                                 utils::timestring_chrono(s + _model.std_blooming_hours) <<
//                                 std::endl;
// 
//                                 _timers.at(i)->add(s + shifts.at(i), _model.std_blooming_hours);
//                             }
//                         }
//                     }
//                     
//                     else // not on bloomin
//                     {
//                         if (_model.smooth.state)
//                         {
// //                             for (auto &t: _timers)
// //                             {
// //                                 // std::chrono::system_clock::time_point s;
// //                                 // std::chrono::seconds d;
// //                                 // TODO
// //                             }
//                         }
//                         else
//                         {
//                             for (uint i = 0; i < _timers.size(); i++)
//                             {
//                                 std::chrono::system_clock::time_point s;
//                                 s=_status.current_point + _model.cycle_start;
//                                 
//                                 std::cout << "Adding growing light timer: " << 
//                                 _timers.at(i)->boardname() << ":" << 
//                                 _timers.at(i)->devname() << " -- start: " <<
//                                 utils::timestring_chrono(s) << " -- finish: " << 
//                                 utils::timestring_chrono(s + _model.std_growing_hours) <<
//                                 
//                                 std::endl;
// 
//                                 _timers.at(i)->add(s + shifts.at(i), _model.std_growing_hours);
//                             }
//                         }
//                     }
//                 
//                 /* and most importantly we update the internal clock */
//                 _status.current_point = now + _model.cycle_duration;
//                     
//                 }
//                 
//             public:
// 
// 
//                 
//                 bool    is_running      (void) { return _on_duty; }
// 
//                 
//             public:
//                 
//                 bool    load            (void); //TODO
//                 bool    save            (void); //TODO
//             };
//         
//         } /* namespace:light */ ;

        
        //      _________________________________________
        //      |                                       |
        //      |       F A N S                         |
        //      |_______________________________________|
        //
        
//         namespace fan
//         {
//             
//             class Fans : public indoorino::TimerBase
//             {
//             private:
//                 
//                 struct
//                 {
//                     std::chrono::seconds cycle_duration = std::chrono::seconds(30);
//                     
//                     struct {
//                         
//                         double start_base       = 0.25;
//                         double start_day        = 0.05;
//                         double overlap_day      = 0.01;
// 
//                         double start_night      = 0.002;
//                         double overlap_night    = 0.0002;
//                         
//                         double rare_length      = 0.02;
//                         
//                     } bets;
//                     
//                     uint    d_min           = 1;
//                     uint    d_min_default   = 5;
//                     uint    d_default       = 20;
//                     uint    d_max_default   = 20;
//                     uint    d_max           = 40;
//                     
//                 } _model;
//                 
//                 struct : public StatusTemplate
//                 {
//                     /* current cycle clock*/
//                     std::chrono::system_clock::time_point current_point;
//                 } _status;
// 
//             private:
//                 indoorino::grow::light::Lights  *   _lights;
//                 bool    _on_duty=false;
//                 
//                 bool    roll_start      (double & prob)
//                 {
//                     for (auto &t: _timers)
//                     {
//                         if (t->is_on()) continue;
//                         
//                         if (( static_cast <float> (rand()) /  static_cast <float> (RAND_MAX) ) <= prob)
//                         {
//                             prob = 0.0;
//                             uint duration = 0;
//                             
//                             if (( static_cast <float> (rand()) /  static_cast <float> (RAND_MAX) ) <= _model.bets.rare_length)
//                             {
//                                 // duration è un uint tra <min> e <min_default>
//                                 
//                                 float d = 60 * _model.d_min + 
//                                     ( static_cast <float> (rand()) /  static_cast <float> (RAND_MAX) ) * 60.0 * (_model.d_min_default - _model.d_min);
//                                 duration =  uint(d);
//                                 
//                             }
//                             
//                             else if (( static_cast <float> (rand()) /  static_cast <float> (RAND_MAX) ) <= _model.bets.rare_length)
//                             {
//                                 //  duration è un uint tra <max_default> e <max>
//                                 
//                                 float d = 60 * _model.d_max_default + 
//                                     ( static_cast <float> (rand()) /  static_cast <float> (RAND_MAX) ) * 60 * (_model.d_max - _model.d_max_default);
//                                 duration =  uint(d);
// 
//                             }
//                             
//                             else
//                             {
//                                 // duration è un uint tra <min_default> e <default>
//                                 
//                                 float d = 60 * _model.d_min_default + 
//                                     ( static_cast <float> (rand()) /  static_cast <float> (RAND_MAX) ) * 60 * (_model.d_default - _model.d_min_default);
//                                 duration =  uint(d);
// 
//                             }
//                             
//                             
//                             t->add(std::chrono::system_clock::now(), std::chrono::seconds(duration));
//                             std::cout << "Roll HIT! " << duration/60 << " min." << std::endl;
//                             return true;
//                         }
//                     }
//                     return false;
//                 }
//                 
//             public:
//                 auto&   status() { return _status; }
//                 auto&   model()  { return _model; }
// 
//             public:
//                  Fans() {}
//                 ~Fans() {}
//                 
//                 
//                 void    begin       (indoorino::grow::light::Lights * p)
//                 {
//                     _on_duty = true;
//                     _lights=p;
//                 }
// 
//                 void    start           (void)
//                 {
//                     double c=1.0;
//                     roll_start(c);
//                 }
// 
//                 void    stop            (void)
//                 {
//                     _on_duty = false;
//                     indoorino::TimerBase::stop();
//                 }
//                 
//                 void    loop            (void)
//                 {
//                     static double counter=0.0;
//                     
//                     if (!_on_duty) return;
//                                        
//                     auto now=std::chrono::system_clock::now();
//                     if (now < _status.current_point) return;
//                     
//                     _status.current_point = now + _model.cycle_duration;
//                     
//                     /* every <cycle_duration> we roll dice to turn on a fan */
// 
//                     std::cout << "Rolling with prob " << counter << std::endl;
// 
//                     
//                     if (_lights->active_timers() > 0)
//                     {
//                         /* LIGHTS are ON */
//                         
//                         if (this->active_timers() > 0)
//                         {                            
//                             if ( roll_start(counter) ) return;
//                             counter = std::min(1.0 , counter + _model.bets.overlap_day);
//                             
//                         }
//                         else
//                         {
//                             // Roll start_base + start_day for each                             
//                          
//                             if (counter < _model.bets.start_base)
//                                 counter = _model.bets.start_base; 
//                             
//                             if ( roll_start(counter) ) return;
//                             counter = std::min( 1.0 , counter + (_model.bets.start_day / std::max(1, this->active_timers())) );                                
//                             
//                         }
//                     }
//                     else
//                     {
//                         /* LIGHTS are OFF */
// 
//                         if (this->active_timers() > 0)
//                         {                            
//                             if ( roll_start(counter) ) return;
//                             counter = std::min( 1.0 , counter + _model.bets.overlap_night );
//                         }
//                         else
//                         {
//                             
//                             if ( roll_start(counter) ) return;
//                             counter = std::min( 1.0 , counter + (_model.bets.start_night / std::max(1, this->active_timers())) );                                
//                             
//                         }
//                         
//                     }
//        
//                     
//                 }
// 
//                 bool    is_running      (void) { return _on_duty; }
// 
//             };
//             
//         } /* namespace:fan */

        
        //      _________________________________________
        //      |                                       |
        //      |       T H E R M O D Y N A M I C       |
        //      |_______________________________________|
        //
        
//         namespace sim
//         {
//             namespace air
//             {
//                 // namespace holding air constant properties
//                 
//                 const double    Cv  = 718.0;
//                 const double    Cp  = 1006.0;
//                 const double    R   = 287.0;
// 
//                 const double    rho_amb = 1.2;
//                 const double    prs_amb = 101325.0;
//                 const double    tmp_amb = 293.15;
//                 
//             } /* namespace::air */
// 
//             
//             class BoundaryWall
//             {
//                 // used in simulation as boundary
//                 
//             public:
//                 BoundaryWall() {}
//                 
//             public:
//                 double      temp; //? serve ?
//                 double      area;
//                 double      K;
//                 
//             };
// 
//             
//             struct Model
//             {
//                 // model holding all simultaion parameters
//                 
//                 struct
//                 {
//                     double wall_K300 = 1.0; /* [3 - 30] Cocchi A. - pag.301 */
//                     double wall_K100 = 3.0; /* [3 - 30] Cocchi A. - pag.301 */
//                 } mat;
//                 
//                 
//                 struct
//                 {
//                     double  timestep    =60.0;      // [s]
//                     double  total_time  =3600.0;    // [s]
//                     uint    total_iter  (void) { return uint( total_time / timestep ); }
//                 } sim;
//                         
//                 BoundaryWall    walls[6];   /* order is N, E, S, W, U, D */
// 
//                 double  shape[3]    = { 2,2,2 };    // [m]
//                 double  heat_gen    =0.0;           // [W]
//                 double  heat_ext    =0.0;           // [W]
//                 double  flow_rate   =0.0;           // [m3/s]
//             
//                 double  volume  (void) { return (shape[0] * shape[1] * shape[2]); }
//                 double  mass    (void) { return (volume() * air::rho_amb); }
//                 
//             };
//             
//             
//             class ThermoPoint
//             {
//                 // thermodynamic state exchange class
//                 
//             public:
//                 ThermoPoint() {};
//                 ThermoPoint(double t, double h) { temp=t; humi=h; }
// 
//             public:
//                 double temp = 0.0;
//                 double humi = 0.0;
//             };
// 
//             
//             class Status
//             {
//                 // thermodynamic room state
//             
//             public:
//                 
//                 ThermoPoint room;
//                 ThermoPoint intake;
//                 
//                 double  boundaries[6] {0};
// 
//             };
//             
//             
//             ThermoPoint&    thermo_solve    (Model& m, Status& s);            
//             
//             class HeatTransferSimulation
//             {
//             private:
//                 std::vector<double>     _temp;
//                 std::vector<double>     _humi;
//                 double                  _points;
//                 
//             public:
//                 
//                 Model   *   model;
//                 Status  *   status;
//                 double      points() { return _points; }
//                 
//             public:
//                 HeatTransferSimulation(Model * m) { model = m; }
//             
//             public:
//                 void    solve   (void);
// 
//             };
//             
//         } /* namespace:sim */
        
        
//         namespace thermo
//         {
//             
//             class ThermoManager
//             {
//             private:
//                 
//                 struct
//                 {
//                     std::chrono::seconds cycle_duration = std::chrono::seconds(30);
//                     
//                 } _model;
//                 
//                 struct
//                 {
//                     std::chrono::system_clock::time_point current_point;
// 
//                     std::vector<double> current_temperature;
//                     std::vector<double> current_humidity;
//                     
//                 } _status;
//             
//             private:
// //                 indoorino::grow::light::Lights  *   _lights;
// //                 indoorino::grow::fan::Fans      *   _fans;
//                 bool    _on_duty=false;
//                 
//             public:
//                  ThermoManager() {}
//                 ~ThermoManager() {}
//                 
// //                 void    begin       (indoorino::grow::light::Lights * pl,
// //                                      indoorino::grow::fan::Fans * pf)
// //                 {
// //                     _lights = pl;
// //                     _fans   = pf;
// //                     _on_duty= true;
// //                 }
//                 
//                 void    loop        (void)
//                 {
//                     if (!_on_duty) return;
//                     
// //                     /* We loop on all timers */
// //                     for (auto &t: _timers)  t.loop();
//                     
//                     auto now=std::chrono::system_clock::now();
// 
//                     if (now < _status.current_point) return;
//                     
//                     _status.current_point = now + _model.cycle_duration;
//                     
//                     std::cout << "Iterating thermo" << std::endl;
//                     
//                     // WE ACQUIRE DATA FROM PROBES
//                     
// //                     System.database.exist(
//                 }
//                 
//                 void    acquire_state   (void)
//                 {
//                     
//                 }
//                 
//             };
//         
//             
//         } /* namespace:thermo */
        
        
        class GrowRoom
        {
        private:
            
            /*
             * This class provides growroom overview, the following modules will be impemented:
             *  
             *  - lights:   manage day - night cycle
             *  - fans:     control fans timing and peak temperature
             *  - thermo:   regulate airflow to chase target temperature / humidity
             *  - hydro:    hydroponic feed system
             * 
             */
        
        public:
//             
//             indoorino::grow::light::Lights          lights;
//             indoorino::grow::fan::Fans              fans;
//             indoorino::grow::thermo::ThermoManager  climate;
            
        public:
             GrowRoom() {};
            ~GrowRoom() {};
            
        public:
            
            void        begin       (void)
            {
                /* T E S T : S T A R T - here should load a status*/
                    
                std::chrono::system_clock::time_point s=std::chrono::system_clock::now();
                printf("\n\n\tNOW:%s\n\n", utils::timestring_chrono(s));
                
//                 lights.add_timer("ONE","BOARD");
//                 lights.start(s);
// 
//                 fans.begin(&lights);
//                 fans.add_timer("BOARD","ICHI");
//                 fans.add_timer("BOARD","NII");
//                 fans.add_timer("BOARD","SAN");
//                 fans.add_timer("BOARD","SHI");
//                 fans.add_timer("BOARD","GO");
                
//                 climate.begin(&lights, &fans);

                /* T E S T : E N D */

            }
            void        loop        (void)
            {
//                 lights.loop();
//                 fans.loop();
//                 climate.loop();
            }
        };
        
        
    } /* namespace:growroom */
    
} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_DATABASE_H_ */








//             class Arrays
//             {
//             public:
//                 Arrays() {}
//             public:
//                 std::vector<double>     temp;
//                 std::vector<double>     humi;
//             };

