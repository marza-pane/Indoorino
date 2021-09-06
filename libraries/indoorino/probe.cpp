#include "probe.h"


namespace indoorino
{
    namespace grow
    {
        
        GrowPhase grow_phase = GrowPhase::growing;
        
        namespace sim
        {
//             ThermoPoint&    thermo_solve    (Model& m, Status& s)
//             {
//                 static ThermoPoint point;
//                 
//                 double flow = m.flow_rate * air::Cp * (s.intake.temp - s.room.temp);
// 
//                 double conduction;
//                 
//                 for (uint8_t i=0; i<6; i++)
// 
//                     conduction += m.mat.wall_K100 * 
//                                     double(pow(m.volume(), 3/2)) * 
//                                     (s.boundaries[i] - s.room.temp);
// 
//                 double vapor_heat  = 0;
// 
//                 point.temp = s.room.temp + 
//                             ( (flow + conduction + m.heat_gen + vapor_heat) * 
//                             m.sim.timestep / (m.mass() * air::Cv) );
// 
//                 point.humi = s.room.humi;
//                 
//                 return point;
//             }
            
        }     /* namespace:sim       */
    }        /* namespace:grow      */
}           /* namespace:indoorino */

