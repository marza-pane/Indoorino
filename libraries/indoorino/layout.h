/*
 * layout.h
 *
 *  Created 3.1.0 on: Apr 19, 2021 (SERVER)
 * 
 *      Author: n00b
 */

#ifndef _SRC_LAYOUT_H_
#define _SRC_LAYOUT_H_

#if defined (INDOORINO_NETWORK)

#include "../packets/ipacket.h"

#define LYT_DEFAULT_NUM_DEVICES     31
#define LYT_DEFAULT_NUM_LIGHTS      20
#define LYT_DEFAULT_NUM_ALARMS      9
#define LYT_DEFAULT_DEV_WEATHER     3

#define LYT_NUM_AREAS               10
#define LYT_NUM_L_HOME              18
#define LYT_NUM_L_PERIMETER         8
#define LYT_NUM_DEVTYPES            9
#define LYT_NUM_ALARMS              6
#define LYT_NUM_ALARMS_GROUP        16

#define MAX_DEVS_PER_ALARM          10
#define ALR_DEFAULT_GROUP_NUM       2
namespace indoorino
{

    namespace layout
    {

        
    //      _________________________________________
    //      |                                       |
    //      |       Global layout labels            |
    //      |_______________________________________|
    //

        const char global_area[LYT_NUM_AREAS][LEN_LOCATIONAME] {
            "home",
            "perimeter",
            "garden",
            "forset",
            "sheds",
            "gate",
            "pool",
            "barbie",
            "woodshed",
            "sauna",
        };          
        
        const char global_home[LYT_NUM_L_HOME][LEN_LOCATIONAME]
        {
            "entrnace",
            "studio",
            "kitchen",
            "porch",
            "pantry",
            "hallway",
            "bedroom_W",
            "bedroom_E",
            "bathroom_W",
            "bathroom_E",
            "workshop",
            "cellar",
            "heating",
            "storage",
            "saferoom",
            "woodshop",
            "garage",
            "attic",
        };


        const char global_perimeter[LYT_NUM_L_PERIMETER][LEN_LOCATIONAME]
        {
            "north facade",
            "south facade",
            "east facade",
            "west facade",
            "entrance",
            "garage",
            "porch",
            "parking lot"
        };
        
        const char global_devtypes[LYT_NUM_DEVTYPES][LEN_DEVNAME]
        {
            "RELAY",
            "SWITCH",
            "FLOODSW",
            "RAINSW",
            "DHT22",
            "DUSTPM25",
            "LDR",
            "SERVO",
            "STEPPER",
        };

        const char global_alarms[LYT_NUM_ALARMS][LEN_NAME]
        {
            "FIRE",
            "FLOOD",
            "HAZARD",
            "POWERGRID"
            "GENERIC",
        };
        
        const char global_alarms_group[LYT_NUM_ALARMS_GROUP][LEN_LABEL]
        {
            "attic fire alarm",
            "kitchen fire alarm",
            "kitchen gas alarm",
            "studio fire alarm",
            "bedroom fire alarm",
            "workshop fire alarm",
            "workshop gas alarm",
            "woodshop fire alarm",
            "woodshop gas alarm",
            "cellar flood alarm",
            "pool flood alarm",
            "sheds flood alarm",
            "heating room flood alarm",
            "heating room fire alarm",
            "heating room gas alarm",
            "garage gas alarm"
        };
        
    //      _________________________________________
    //      |                                       |
    //      |       Layout utils                    |
    //      |_______________________________________|
    //

//         class device_t
//         {
//         public:
//             char        boardname
//             char        name[LEN_DEVNAME] {0};
//             char        type[LEN_DEVNAME] {0};
//         };
// 
//         class board_t
//         {
//         public:
//             char        name[LEN_NAME]              {0};
//             uint8_t     devnum                      {0};
//             char        area[LEN_LOCATIONAME]       {0};
//             char        location[LEN_LOCATIONAME]   {0};
//             device_t    devices[MAX_ATTACHED_DEVICES];
// 
//         };
        
        class linked_dev_t
        {
        public:
            char        boardname[LEN_NAME]         {0};
            char        devname[LEN_DEVNAME]        {0};
            char        type[LEN_DEVNAME]           {0};
            char        area[LEN_LOCATIONAME]       {0};
            char        location[LEN_LOCATIONAME]   {0};            
        };
        
        
        template <class OBJ> class LinkedDevList
        {
            /* OBJ should have <linked_dev_t> as attribute named <_layout> */
            
        protected:        

            int     get_index   (const char * board, const char * dev)
            {
                for (uint i=0; i<_classlist.size(); i++)
                {
                    if ( strcmp(_classlist[i]._layout.boardname, board) == 0 &&
                         strcmp(_classlist[i]._layout.devname, dev) == 0 )
                    {
                        return i;
                    }
                }
                return -1;
            }

        protected:        

            std::vector<OBJ>     _classlist;

        public:
            LinkedDevList() {}
            virtual ~LinkedDevList() { _classlist.clear(); }
            
            OBJ&    operator[]  (const iSize_t i) { return _classlist.at(i); }
            
            OBJ&    operator()  (const char * board, const char * dev)
            {
                if (this->exist())
                    return _classlist[this->get_index(board, dev)];
                error_os("LinkedDevList:FATAL! invalid call for <%s:%s>",board, dev);
                return OBJ();
            }
            
            iSize_t size        (void)  { return _classlist.size(); }

            void    clear       (void)  { _classlist.clear(); }

            bool    exist       (const char * board, const char * dev)
            {
                if (this->get_index(board, dev) == -1) return false;
                    return true;
            }

        };

        
    //      _________________________________________
    //      |                                       |
    //      |       Board Layout                    |
    //      |_______________________________________|
    //

        inline  linked_dev_t        DefaultDeviceLayout[LYT_DEFAULT_NUM_DEVICES]
        {
            /***    HOUSE.BEAMS         ***/

            { "HOUSE.BEAMS", "BEAM1", "RELAY", "perimeter", "entrance"      },  // 0
            { "HOUSE.BEAMS", "BEAM2", "RELAY", "perimeter", "north facade"  },  // 1
            { "HOUSE.BEAMS", "BEAM3", "RELAY", "perimeter", "north facade"  },  // 2
            { "HOUSE.BEAMS", "BEAM4", "RELAY", "perimeter", "west facade"   },  // 3
            { "HOUSE.BEAMS", "BEAM5", "RELAY", "perimeter", "west facade"   },  // 4
            { "HOUSE.BEAMS", "BEAM6", "RELAY", "perimeter", "south facade"  },  // 5
            { "HOUSE.BEAMS", "BEAM7", "RELAY", "perimeter", "south facade"  },  // 6
            { "HOUSE.BEAMS", "BEAM8", "RELAY", "perimeter", "south facade"  },  // 7
            { "HOUSE.BEAMS", "HEAT1", "DHT22", "home", "attic",             },  // 8
            
            /***    GATE.BEAMS         ***/
            
            { "GATE.BEAMS", "BEAM1", "RELAY", "gate", "entrance"  },
            { "GATE.BEAMS", "BEAM2", "RELAY", "gate", "entrance"  },
            { "GATE.BEAMS", "BEAM3", "RELAY", "gate", "entrance"  },
            { "GATE.BEAMS", "BEAM4", "RELAY", "gate", "entrance"  },
            
            /***    HOUSE.LIGHTS         ***/
            
            { "HOUSE.LIGHTS", "BEAM1", "RELAY", "perimeter", "south facade"  },
            { "HOUSE.LIGHTS", "BEAM2", "RELAY", "perimeter", "south facade"  },
            { "HOUSE.LIGHTS", "BEAM3", "RELAY", "perimeter", "south facade"  },
            { "HOUSE.LIGHTS", "BEAM4", "RELAY", "perimeter", "south facade"  },
            
            /***    OUTER.BEAMS         ***/
            
            { "OUTER.BEAMS", "BEAM1", "RELAY", "perimeter", "parking lot" }, 
            { "OUTER.BEAMS", "BEAM2", "RELAY", "sheds",     "third shed"  },
            { "OUTER.BEAMS", "BEAM3", "RELAY", "garden",    "orchard"     },
            { "OUTER.BEAMS", "BEAM4", "RELAY", "garden",    "grow area"   },
             
            /***    KITCHEN.CLIMATE     ***/
            
            { "KITCHEN.CLIMATE", "DHT1", "DHT22",   "home", "kitchen" },
            { "KITCHEN.CLIMATE", "DHT2", "DHT22",   "home", "kitchen" },
            { "KITCHEN.CLIMATE", "DHT3", "DHT22",   "home", "kitchen" },
            { "KITCHEN.CLIMATE", "LDR1", "LDR",     "home", "kitchen" },
            { "KITCHEN.CLIMATE", "AIR1", "DUSTPM25","home", "kitchen" },
            
            /***    PRESCOTT.CLIMATE ***/

            { "PRESCOTT.CLIMATE", "DHT1",   "DHT22",  "home", "heating" },
            { "PRESCOTT.CLIMATE", "DHT2",   "DHT22",  "home", "heating" },
            { "PRESCOTT.CLIMATE", "DHT3",   "DHT22",  "home", "heating" },
            { "PRESCOTT.CLIMATE", "LDR1",   "LDR",    "home", "heating"   },
            { "PRESCOTT.CLIMATE", "FLOOD1", "SWITCH", "home", "heating"   },
        };

    //      _________________________________________
    //      |                                       |
    //      |       Lights Layout                   |
    //      |_______________________________________|
    //

        struct dev_light_t : public linked_dev_t
        {
            char        group[LEN_LABEL]    {0};
            char        lightype[LEN_NAME]  {0};
        };
        
        inline  dev_light_t     DefaultLightsLayout[LYT_DEFAULT_NUM_LIGHTS]
        {       

            /***    OUTER PERIMETER.BEAMS     ***/
            
            {
                "GATE.BEAMS", "BEAM1", "RELAY", "gate", "entrance",
                "outer perimeter",
                "LEDBEAM",
            },
            {
                "GATE.BEAMS", "BEAM2", "RELAY", "gate", "entrance",
                "outer perimeter",
                "LEDBEAM",
            },
            {
                "GATE.BEAMS", "BEAM3", "RELAY", "gate", "entrance",
                "outer perimeter",
                "LEDBEAM",
            },
            {
                "GATE.BEAMS", "BEAM4", "RELAY", "gate", "entrance",
                "outer perimeter",
                "LEDBEAM",
            },
            {
                "OUTER.BEAMS", "BEAM1", "RELAY", "perimeter", "parking lot",
                "outer perimeter",
                "LEDBEAM",
            },         
            {
                "OUTER.BEAMS", "BEAM2", "RELAY", "sheds",     "third shed",
                "outer perimeter",
                "LEDBEAM",
            },         
            {
                "OUTER.BEAMS", "BEAM3", "RELAY", "garden",    "orchard",
                "outer perimeter",
                "LEDBEAM",
            },         
            {
                "OUTER.BEAMS", "BEAM4", "RELAY", "garden",    "grow area",
                "outer perimeter",
                "LEDBEAM",
            },  
            
            /***    HOME PERIMETER.BEAMS     ***/
            
            {
                "HOUSE.BEAMS", "BEAM1", "RELAY", "perimeter", "entrance",
                "home perimeter",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM2", "RELAY", "perimeter", "north facade",
                "home perimeter",
                "LEDBEAM",
            },            
            {
                "HOUSE.BEAMS", "BEAM3", "RELAY", "perimeter", "north facade",
                "home perimeter",
                "LEDBEAM",
            },            
            {
                "HOUSE.BEAMS", "BEAM4", "RELAY", "perimeter", "west facade",
                "home perimeter",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM5", "RELAY", "perimeter", "west facade",
                "home perimeter",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM6", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM7", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM8", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LEDBEAM",
            },
            
            {
                "HOUSE.LIGHTS", "BEAM1", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LED10",
            },
            {
                "HOUSE.LIGHTS", "BEAM2", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LED10",
            },
            {
                "HOUSE.LIGHTS", "BEAM3", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LED10",
            },
            {
                "HOUSE.LIGHTS", "BEAM4", "RELAY", "perimeter", "south facade",
                "home perimeter",
                "LED10",
            }
            
        };

    //      _________________________________________
    //      |                                       |
    //      |       Alarms Layout                   |
    //      |_______________________________________|
    //
     
        struct dev_alarm_t : public linked_dev_t
        {
            char        alarmtype[LEN_NAME] {0};    // global_alarms 
            char        group[LEN_LABEL]    {0};    // 
        };

        inline dev_alarm_t      DefaultAlarmsLayout[LYT_DEFAULT_NUM_ALARMS]
        {
          
            /***    ATTIC FIRE.ALARMS           ***/
            {
               "HOUSE.BEAMS", "HEAT1", "DHT22", "home", "attic",
                "FIRE", "attic fire alarm"
            },

            /***    KITCHEN FIRE.ALARMS         ***/
            {
                "KITCHEN.CLIMATE", "DHT1", "DHT22", "home", "kitchen",
                "FIRE", "kitchen fire alarm"
            },
            {
                "KITCHEN.CLIMATE", "DHT2", "DHT22", "home", "kitchen",
                "FIRE", "kitchen fire alarm"
            },
            {
                "KITCHEN.CLIMATE", "DHT3", "DHT22", "home", "kitchen",
                "FIRE", "kitchen fire alarm"
            },
            {
                "KITCHEN.CLIMATE", "AIR1", "DUSTPM25", "home", "kitchen",
                "HAZARD", "kitchen gas alarm"
            },

            /***    PRESCOTT FIRE.ALARMS        ***/
            {
                "PRESCOTT.CLIMATE", "DHT1", "DHT22", "home", "heating",
                "FIRE", "heating room fire alarm"
            },
            {
                "PRESCOTT.CLIMATE", "DHT2", "DHT22", "home", "heating",
                "FIRE", "heating room fire alarm"
            },
            {
                "PRESCOTT.CLIMATE", "DHT3", "DHT22", "home", "heating",
                "FIRE", "heating room fire alarm"
            },
            
            /***    PRESCOTT FLOOD.ALARMS       ***/
            
            {
                "PRESCOTT.CLIMATE", "FLOOD1", "SWITCH", "home", "heating",
                "FLOOD", "heating room flood alarm"
            },
            
            
        };
        
        struct dev_weather_t : public linked_dev_t
        {
            char        group[LEN_LABEL]         {0};
        };
        
        inline dev_weather_t    DefaultWeatherStationLayout[LYT_DEFAULT_DEV_WEATHER]
        {
            
            /***    KITCHEN.CLIMATE     ***/
            
            {
                "KITCHEN.CLIMATE", "DHT1", "DHT22", "home", "kitchen",
                "kitchen climate"
            },
            {
                "KITCHEN.CLIMATE", "DHT2", "DHT22", "home", "kitchen",
                "kitchen climate"
            },
            {
                "KITCHEN.CLIMATE", "DHT3", "DHT22", "home", "kitchen",
                "kitchen climate"
            }
        };
//         struct alarm_group_t
//         {
//             char        name[LEN_LABEL]             {0};
//             char        type[LEN_NAME]              {0};
//             char        area[LEN_LOCATIONAME]       {0};
//             char        location[LEN_LOCATIONAME]   {0};
//             uint8_t     devnum                      {0};
//             dev_alarm_t devices[MAX_DEVS_PER_ALARM] {0};
//         };
//         
//         inline alarm_group_t    AlarmsDefaultGroups[ALR_DEFAULT_GROUP_NUM]
//         {
//             "Attic fire allarm", "FIRE",
//             "home", "attic", 1,
//             {
//                 "HOUSE.BEAMS", "HEAT1",
//                 "home", "attic",
//                 "FIRE", 2                
//             }
//             
//         };
        
    //      _________________________________________
    //      |                                       |
    //      |       Main Layout Class               |
    //      |_______________________________________|
    //
             
        class Layout
        {
            std::vector<linked_dev_t>   _devices;
            std::vector<dev_light_t>    _lights;
            std::vector<dev_alarm_t>    _alarms;
            std::vector<dev_weather_t>  _weather;
            
        public:
             Layout() {}
            ~Layout() { clear(); }
            
            void    reset       (void);
            void    show        (void);
            void    clear       (void);
            int     check       (void);
            
            bool    save        (void);
            bool    load        (void);
            
            void    parse       (packet::netpacket *);
            
            int     is_device   (const char *, const char *);
            int     is_light    (const char *, const char *);
            int     is_alarm    (const char *, const char *);
            int     is_weather  (const char *, const char *);

            packet::netpacket   *   get_layout_packet   (const char *, const char *, const char *);
            
            void    send_devices    (void);
            void    send_lights     (void);
            void    send_alarms     (void);
            void    send_weather    (void);
            
            void    send_config     (void);
            
            const std::vector<linked_dev_t> & devices() { return _devices; }
            const std::vector<dev_light_t>  & lights()  { return _lights; }
            const std::vector<dev_alarm_t>  & alarms()  { return _alarms; }
            const std::vector<dev_weather_t>& weather() { return _weather; }
            
        };
        
    } /* namespace:layout */

} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_LAYOUT_H_ */
