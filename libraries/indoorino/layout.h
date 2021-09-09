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

// #include "boards.h"
#include "../common/utils.h"
#include "../packets/ipacket.h"

// #include "../packets/ipacket.h"

// #define LYT_DEFAULT_NUM_DEVICES     31
// #define LYT_DEFAULT_NUM_LIGHTS      20
// #define LYT_DEFAULT_NUM_ALARMS      9
// #define LYT_DEFAULT_DEV_WEATHER     3
// 
// 
#define MAX_DEVS_PER_SERVICE          64

namespace indoorino
{

    namespace lyt
    {

        //      _________________________________________
        //      |                                       |
        //      |       Layout ambient variables        |
        //      |_______________________________________|
        //
        //      get strings with indoorino::lyt::amb::variables. type/description/unit. variables.type is for probes
            
        namespace amb
        {

            #define LYT_NUM_AMBIENT_VARIABLES 11
            const struct {
            
                const char description[LEN_LABEL];
                const char * type;
                const char * unit;

            } variables[LYT_NUM_AMBIENT_VARIABLES] = {
            
                {   "air temperature", amb::vartypes[0], "C"
                },

                {   "water temperature", amb::vartypes[0], "C"
                },
                                
                {   "air humidity", amb::vartypes[1], "RH"
                },

                {   "soil humidity", amb::vartypes[1], "g/cm3"
                },

                {   "air pressure", amb::vartypes[2], "atm"
                },

                {   "water pressure", amb::vartypes[2], "atm"
                },

                {   "wind speed", amb::vartypes[4], "m/s"
                },
                
                {   "ambient light", amb::vartypes[5], "lux"
                },
                
                {   "electric power", amb::vartypes[6], "kW"
                },
                
                {   "relay state", amb::vartypes[7], ""
                },
                
                {   "air dust", amb::vartypes[8], "ug/m3"
                },
                                
            };
            
        }; /* namespace:amb */
        
        //      _________________________________________
        //      |                                       |
        //      |       Layout Key                      |
        //      |_______________________________________|
        //
               
        struct LayoutDevKey {
        
            iCom_t command;
            char label[LEN_DEVNAME];
            
        };

        struct LayoutServiceKey {
            
            char type[LEN_NAME];
            char name[LEN_NAME];
            char desc[LEN_LABEL];
            char location[LEN_LOCATIONAME];
            char area[LEN_LOCATIONAME];
            
            LayoutServiceKey (const char * k_type,
                              const char * k_name,
                              const char * k_desc,
                              const char * k_area,
                              const char * k_loct)
            {
                strcpy(type, k_type);
                strcpy(name, k_name);
                strcpy(desc, k_desc);
                strcpy(area, k_area);
                strcpy(location, k_loct);
            }
            
            LayoutServiceKey (const LayoutServiceKey& p)
            {
                memcpy(this, &p, sizeof(LayoutServiceKey));
            }
        };
        
        class LayoutKey
        {
        private:
            
            char         _boardname[LEN_NAME]       {0};
            char         _devname[LEN_DEVNAME]      {0};
            char         _area[LEN_LOCATIONAME]     {0};
            char         _location[LEN_LOCATIONAME] {0};              
            char         _conf[LEN_LABEL]           {0};
            LayoutDevKey _devtype                   {0};
            
        public:
            
            LayoutKey(const char *, const char *, const char *, const char *, const char *, const char *);
            LayoutKey(const char *, const char *, const char *, const char *, const char *, iCom_t);
            
            const char * boardname()    { return _boardname; }
            const char * devname()      { return _devname;   }
            const char * area()         { return _area;      }
            const char * location()     { return _location;  }
            const char * conf()         { return _conf;      }     // This string tells System.services what the device is for
            const LayoutDevKey type()   { return _devtype;   }     // And this struct tells System.services what kind of device is
        };

        //      _________________________________________
        //      |                                       |
        //      |       Layout home map                 |
        //      |_______________________________________|
        //
        
        
        class HomeMap
        {
        private:

            class HomeArea
            {
            private:
                char _name[LEN_LOCATIONAME] {0};
                std::vector<std::string> _locations;
            
            public:
                 HomeArea(const char * n)   { strcpy(_name, n); }
                ~HomeArea()                 { _locations.clear(); }
                
                
                int     exist   (const char * name)
                {
                    int i=0;
                    for (auto &s : _locations)
                    {
                        if (strcmp(s.c_str(), name) == 0)
                        {
                            return i;
                        }
                        i++;
                    }
                    
                    return -1;
                }
                
                const auto name () { return _name; }
                auto    operator()  (void) { return _locations; }
                auto    operator[]  (const char * loc) { return _locations[exist(loc)]; }
                
                friend class HomeMap;
            };

        private:
            std::vector<HomeArea> _areas;            
        public:
             HomeMap() {}
            ~HomeMap() { _areas.clear(); }
            
            int     exist           (const char * name)
            {
                int i=0;
                for (auto &s : _areas)
                {
                    if (strcmp(s.name(), name) == 0)
                    {
                        return i;
                    }
                    i++;
                }
                
                return -1;
            }
            
            auto    operator()      (void) { return _areas; }
            
            auto    operator[]      (const char * name)
            {
                return _areas[exist(name)];
            }
            
        public:

            bool    add_area        (const char * n);
            bool    rem_area        (const char * n);
            bool    add_location    (const char * a, const char * l);
            bool    rem_location    (const char * a, const char * l);
            
            void    reset           (void);
            
            bool    save            (void); // TODO
            bool    load            (void); // TODO
            void    send2client     (void); // TODO
            void    parse           (packet::ipacket *);  // TODO
            
        };
        
        
        //      _________________________________________
        //      |                                       |
        //      |       Layout Service                  |
        //      |_______________________________________|
        //
        
        /*
         * Layout class is flexible (admits boards add/edit) and virtualize home locations with services.
         * Each location has its ServiceTemplate list. Each service has a LayoutKey list
         * A device (LayoutKey) can be shared across multiple services
         * 
         */
        
        
        class Service
        {
        private:
            char _type[LEN_NAME]            {0};
            char _name[LEN_NAME]            {0};
            char _desc[LEN_LABEL]           {0};
            char _area[LEN_LOCATIONAME]     {0};
            char _location[LEN_LOCATIONAME] {0};
            std::vector<LayoutKey> _devlist;
            
        public:
             Service( const char * t,
                      const char * n,
                      const char * d,
                      const char * a,
                      const char * l )
             { 
                 strcpy(_type, t);
                 strcpy(_name, n);
                 strcpy(_desc, d);
                 strcpy(_area, a);
                 strcpy(_location, l);
             }
            ~Service() { _devlist.clear(); }

        public:
            const char * type()     { return _type; }
            const char * name()     { return _name; }
            const char * desc()     { return _desc; }
            const char * area()     { return _area; }
            const char * location() { return _location; }
            auto& devices()         { return _devlist; }
            
        public:
            int exist (const char *b, const char *d)
            {
                int i=0;
                for (auto &item : _devlist)
                {
                    if ( (strcmp(item.boardname(), b) == 0) && (strcmp(item.devname(), d) == 0) )
                    {
                        return i;
                    }
                    i++;
                }
                return -1;
            }
            
            LayoutKey& operator() (const char *b, const char *d)
            {
                static LayoutKey dummy_key("ERROR","ERROR","ERROR","ERROR","ERROR", "NOTYPE");
                int i=this->exist(b,d);
                if (i != -1) return (_devlist.at(i));
                else return dummy_key;
            }
        };

        //      _________________________________________
        //      |                                       |
        //      |       Layout main class               |
        //      |_______________________________________|
        //
        
        class Layout
        {
        private:
            std::vector<Service> _services;
            indoorino::lyt::HomeMap _home_map;
        
        public:
             Layout() {}
            ~Layout() { _services.clear(); }

        public:
            
            std::vector<LayoutKey>& devices();
            
            auto&   home_map() { return _home_map; }
            
            auto&   services() { return _services; }
            
            int     is_service             (const char *);
            bool    is_board               (const char *);
            bool    is_device              (const char *, const char *);
            
            bool    clear_service          (const char *);
            
            bool    rem_board              (const char *);
            bool    rem_device             (const char *, const char *);
            bool    rem_service            (const char *);
            bool    rem_deviceFromService  (const char *, const char *, const char *);
            
        public:            
            bool    save        (void);
            bool    load        (void);
            void    reset       (void);

        public:
            void    send2client (void);
            int     check       (void);
            void    parse       (packet::ipacket *);
            void    show        (void);
            
        };

    //      _________________________________________
    //      |                                       |
    //      |       Global layout labels            |
    //      |_______________________________________|
    //

        #define LYT_NUM_AREAS           10
        const char LAYOUT_AREAS[LYT_NUM_AREAS][LEN_LOCATIONAME] {
            "home",
            "perimeter",
            "garden",
            "forest",
            "sheds",
            "gate",
            "pool",
            "barbie",
            "woodshed",
            "sauna",
        };          
                
        #define LYT_NUM_AREA_HOME       18
        const char LAYOUT_AREA_HOME[LYT_NUM_AREA_HOME][LEN_LOCATIONAME]
        {
            "entrnace",
            "studio",
            "kitchen",
            "porch",
            "pantry",
            "hallway",
            "bedroom",
            "guestroom",
            "bathroom",
            "toilet",
            "workshop",
            "cellar",
            "heating",
            "storage",
            "saferoom",
            "woodshop",
            "garage",
            "attic",
        };


        #define LYT_NUM_AREA_PERIMETER  8
        const char LAYOUT_AREA_PERIMETER[LYT_NUM_AREA_PERIMETER][LEN_LOCATIONAME]
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
        

        #define LYT_NUM_AREA_GARDEN 3
        const char LAYOUT_AREA_GARDEN[LYT_NUM_AREA_GARDEN][LEN_LOCATIONAME]
        {
            "orchard",
            "grow area",
            "greenhouse",
        };

        #define LYT_NUM_AREA_SHEDS      4
        const char LAYOUT_AREA_SHEDS[LYT_NUM_AREA_SHEDS][LEN_LOCATIONAME]
        {
            "south shed",
            "center shed",
            "north shed",
            "entrance"
        };
        
        /* Here all home locations ... */
        
        
        #define LYT_NUM_SERVICES 13
        const char LAYOUT_SERVICES_TYPE[LYT_NUM_SERVICES][LEN_NAME]
        {
            
            "ALARM:FIRE",
            "ALARM:FLOOD",
            "ALARM:HAZARD",
            "ALARM:POWERGRID",
            "ALARM:GENERIC",
            "LIGHTS:INDOOR",
            "LIGHTS:OUTDOOR",
            "CLIMATE:INDOOR",
            "CLIMATE:OUTDOOR",
            "GATES",
            "ENERGY",
            "CAMERAS",
            "GARDEN",
        };
        
        #define LYT_NUM_DEVTYPES 10
        const LayoutDevKey LAYOUT_DEVTYPES[LYT_NUM_DEVTYPES] = {

            { IBACOM_CONF_DEVSTD,       "NOTYPE"    },      /* NOTYPE must be the first entry */
            { IBACOM_CONF_RELAY,        "RELAY"     },
            { IBACOM_CONF_SWITCH,       "SWITCH"    },
            { IBACOM_CONF_FLOODSWITCH,  "FLOODSW"   },
            { IBACOM_CONF_RAINSWITCH,   "RAINSW"    },
            { IBACOM_CONF_DHT22,        "DHT22"     },
            { IBACOM_CONF_DUSTPM25,     "DUSTPM25"  },
            { IBACOM_CONF_LDR,          "LDR"       },
            { IBACOM_CONF_SERVO,        "SERVO"     },
            { IBACOM_CONF_STEPPER,      "STEPPER"   },
        };

        iCom_t devicetype_Str2Com (const char * name);
        const char * devicetype_Com2Str (iCom_t comm);

    //      _________________________________________
    //      |                                       |
    //      |       Default layput configurtation   |
    //      |_______________________________________|
    //        
        #define LYT_NUM_DEFAULT_SERVICES 20
        const struct {
            
            const char type[LEN_NAME];
            const char name[LEN_NAME];
            const char description[LEN_LABEL];
            const char area[LEN_LOCATIONAME];
            const char location[LEN_LOCATIONAME];
            const struct {
                
                const char boardname[LEN_NAME];
                const char devname[LEN_LABEL];
                const char area[LEN_LOCATIONAME];
                const char location[LEN_LOCATIONAME];
                const char devconf[LEN_LABEL];
                const char devtype[LEN_DEVNAME];
                
            }devices[MAX_DEVS_PER_SERVICE];
            
        } LAYOUT_DEFAULT_SERVICES[LYT_NUM_DEFAULT_SERVICES] = {

        //      _________________________________________
        //      |                                       |
        //      |       ALARMS                          |
        //      |_______________________________________|
        //
            
            {
                "ALARM:FIRE",
                "KITCHEN.FIRE.ALARM",
                "kitchen fire alarm",
                "home",
                "kitchen",
                {
                    { "KITCHEN.CLIMATE",   "DHT1",     "home",  "kitchen" ,        "onboard", "DHT22"  },
                    { "KITCHEN.CLIMATE",   "DHT2",     "home",  "kitchen" ,        "onboard", "DHT22"  },
                    { "KITCHEN.CLIMATE",   "DHT3",     "home",  "kitchen" ,        "onboard", "DHT22"  }
                }
            },
            
            {
                "ALARM:FIRE",
                "ATTIC.FIRE.ALARM",
                "attic fire alarm",
                "home",
                "attic",
                {
                    { "HOUSE.BEAMS",   "HEAT1",        "home",  "attic",           "onboard",  "DHT22"  }
                }
            },
            
            {
                "ALARM:FLOOD",
                "PRESCOTT.FLOOD.ALARM",
                "heating room flood alarm",
                "home",
                "heating",
                {
                    { "PRESCOTT.CLIMATE",  "FLOOD1",   "home",  "heating",         "onboard",  "FLOODSW"  },
                    { "PRESCOTT.CLIMATE",  "FLOOD2",   "home",  "heating",         "onboard",  "FLOODSW"  },
                    { "PRESCOTT.CLIMATE",  "FLOOD3",   "home",  "heating",         "onboard",  "FLOODSW"  }
                }
            },

        //      _________________________________________
        //      |                                       |
        //      |       LIGHTS                          |
        //      |_______________________________________|
        //

            {
                "LIGHTS:OUTDOOR",
                "PERIMETER.BEAMS",
                "house perimeter lights",
                "perimeter",
                "entrance",
                {
                    { "HOUSE.BEAMS",   "BEAM1",    "perimeter", "entrance",        "inner perimeter", "RELAY"  },
                    { "HOUSE.BEAMS",   "BEAM2",    "perimeter", "north facade",    "inner perimeter", "RELAY"  },
                    { "HOUSE.BEAMS",   "BEAM3",    "perimeter", "north facade",    "inner perimeter", "RELAY"  },
                    { "HOUSE.BEAMS",   "BEAM4",    "perimeter", "west facade",     "inner perimeter", "RELAY"  }, 
                    { "HOUSE.BEAMS",   "BEAM5",    "perimeter", "west facade",     "inner perimeter", "RELAY"  },
                    { "HOUSE.BEAMS",   "BEAM6",    "perimeter", "south facade",    "inner perimeter", "RELAY"  },
                    { "HOUSE.BEAMS",   "BEAM7",    "perimeter", "south facade",    "inner perimeter", "RELAY"  },
                    { "HOUSE.BEAMS",   "BEAM8",    "perimeter", "south facade",    "inner perimeter", "RELAY"  }
                }
            },
            
            {
                "LIGHTS:OUTDOOR",
                "GARDEN.LIGHTS",
                "garden lights",
                "garden",
                "grow area",
                {
                    { "GARDEN.LIGHT",  "BEAM1",    "garden",    "orchard" ,        "inner perimeter", "RELAY"  },
                    { "GARDEN.LIGHT",  "BEAM2",    "garden",    "orchard",         "inner perimeter", "RELAY"  },
                    { "GARDEN.LIGHT",  "BEAM3",    "garden",    "greenhouse",      "inner perimeter", "RELAY"  },
                    { "GARDEN.LIGHT",  "BEAM4",    "garden",    "greenhouse",      "inner perimeter", "RELAY"  }
                }
            },

        //      _________________________________________
        //      |                                       |
        //      |       CLIMATE                         |
        //      |_______________________________________|
        //
            
            {
                "CLIMATE:INDOOR",
                "KITCHEN.CLIMATE",
                "kitchen climate",
                "home",
                "kitchen",
                {
                    { "KITCHEN.CLIMATE",   "DHT1",     "home",  "kitchen" ,        "onboard", "DHT22"  },
                    { "KITCHEN.CLIMATE",   "DHT2",     "home",  "kitchen" ,        "onboard", "DHT22"  },
                    { "KITCHEN.CLIMATE",   "DHT3",     "home",  "kitchen" ,        "onboard", "DHT22"  },
                    { "KITCHEN.CLIMATE",   "LDR1",     "home",  "kitchen" ,        "onboard", "LDR"    },
                    { "KITCHEN.CLIMATE",   "AIR1",     "home",  "kitchen" ,        "onboard", "DUSTPM25" }
                }
            },
            
            {
                "CLIMATE:INDOOR",
                "PRESCOTT.CLIMATE",
                "heating room climate",
                "home",
                "heating",
                {
                    { "PRESCOTT.CLIMATE",   "DHT1",     "home",  "heating" ,        "onboard", "DHT22"  },
                    { "PRESCOTT.CLIMATE",   "DHT2",     "home",  "heating" ,        "onboard", "DHT22"  },
                    { "PRESCOTT.CLIMATE",   "DHT3",     "home",  "heating" ,        "onboard", "DHT22"  },
                    { "PRESCOTT.CLIMATE",   "AIR1",     "home",  "heating" ,        "onboard", "DUSTPM25" },
                    { "PRESCOTT.CLIMATE",   "AIR2",     "home",  "heating" ,        "onboard", "DUSTPM25" }
                }
            }
            
        };
        

    } /* namespace:lyt */

} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_LAYOUT_H_ */





















        
//         const char global_alarms_group[LYT_NUM_ALARMS_GROUP][LEN_LABEL]
//         {
//             "attic fire alarm",
//             "kitchen fire alarm",
//             "kitchen gas alarm",
//             "studio fire alarm",
//             "bedroom fire alarm",
//             "workshop fire alarm",
//             "workshop gas alarm",
//             "woodshop fire alarm",
//             "woodshop gas alarm",
//             "cellar flood alarm",
//             "pool flood alarm",
//             "sheds flood alarm",
//             "heating room flood alarm",
//             "heating room fire alarm",
//             "heating room gas alarm",
//             "garage gas alarm"
//         }; // questo non servirà più?


        

        
    //      _________________________________________
    //      |                                       |
    //      |       Layout utils                    |
    //      |_______________________________________|
    //
        
//         class linked_dev_t
//         {
//         public:
//             char        boardname[LEN_NAME]         {0};
//             char        devname[LEN_DEVNAME]        {0};
//             char        type[LEN_DEVNAME]           {0};
//             char        area[LEN_LOCATIONAME]       {0};
//             char        location[LEN_LOCATIONAME]   {0};            
//         };
//         
//         
//         template <class OBJ> class LinkedDevList
//         {
//             /* OBJ should have <linked_dev_t> as attribute named <_layout> */
//             
//         protected:        
// 
//             int     get_index   (const char * board, const char * dev)
//             {
//                 for (uint i=0; i<_classlist.size(); i++)
//                 {
//                     if ( strcmp(_classlist[i]._layout.boardname, board) == 0 &&
//                          strcmp(_classlist[i]._layout.devname, dev) == 0 )
//                     {
//                         return i;
//                     }
//                 }
//                 return -1;
//             }
// 
//         protected:        
// 
//             std::vector<OBJ>     _classlist;
// 
//         public:
//             LinkedDevList() {}
//             virtual ~LinkedDevList() { _classlist.clear(); }
//             
//             OBJ&    operator[]  (const iSize_t i) { return _classlist.at(i); }
//             
//             OBJ&    operator()  (const char * board, const char * dev)
//             {
//                 if (this->exist())
//                     return _classlist[this->get_index(board, dev)];
//                 error_os("LinkedDevList:FATAL! invalid call for <%s:%s>",board, dev);
//                 return OBJ();
//             }
//             
//             iSize_t size        (void)  { return _classlist.size(); }
// 
//             void    clear       (void)  { _classlist.clear(); }
// 
//             bool    exist       (const char * board, const char * dev)
//             {
//                 if (this->get_index(board, dev) == -1) return false;
//                     return true;
//             }
// 
//         };
// 
//         
//     //      _________________________________________
//     //      |                                       |
//     //      |       Board Layout                    |
//     //      |_______________________________________|
//     //
// 
//         inline  linked_dev_t        DefaultDeviceLayout[LYT_DEFAULT_NUM_DEVICES]
//         {
//             /***    HOUSE.BEAMS         ***/
// 
//             { "HOUSE.BEAMS", "BEAM1", "RELAY", "perimeter", "entrance"      },  // 0
//             { "HOUSE.BEAMS", "BEAM2", "RELAY", "perimeter", "north facade"  },  // 1
//             { "HOUSE.BEAMS", "BEAM3", "RELAY", "perimeter", "north facade"  },  // 2
//             { "HOUSE.BEAMS", "BEAM4", "RELAY", "perimeter", "west facade"   },  // 3
//             { "HOUSE.BEAMS", "BEAM5", "RELAY", "perimeter", "west facade"   },  // 4
//             { "HOUSE.BEAMS", "BEAM6", "RELAY", "perimeter", "south facade"  },  // 5
//             { "HOUSE.BEAMS", "BEAM7", "RELAY", "perimeter", "south facade"  },  // 6
//             { "HOUSE.BEAMS", "BEAM8", "RELAY", "perimeter", "south facade"  },  // 7
//             { "HOUSE.BEAMS", "HEAT1", "DHT22", "home", "attic",             },  // 8
//             
//             /***    GATE.BEAMS         ***/
//             
//             { "GATE.BEAMS", "BEAM1", "RELAY", "gate", "entrance"  },
//             { "GATE.BEAMS", "BEAM2", "RELAY", "gate", "entrance"  },
//             { "GATE.BEAMS", "BEAM3", "RELAY", "gate", "entrance"  },
//             { "GATE.BEAMS", "BEAM4", "RELAY", "gate", "entrance"  },
//             
//             /***    HOUSE.LIGHTS         ***/
//             
//             { "HOUSE.LIGHTS", "BEAM1", "RELAY", "perimeter", "south facade"  },
//             { "HOUSE.LIGHTS", "BEAM2", "RELAY", "perimeter", "south facade"  },
//             { "HOUSE.LIGHTS", "BEAM3", "RELAY", "perimeter", "south facade"  },
//             { "HOUSE.LIGHTS", "BEAM4", "RELAY", "perimeter", "south facade"  },
//             
//             /***    OUTER.BEAMS         ***/
//             
//             { "OUTER.BEAMS", "BEAM1", "RELAY", "perimeter", "parking lot" }, 
//             { "OUTER.BEAMS", "BEAM2", "RELAY", "sheds",     "south shed"  },
//             { "OUTER.BEAMS", "BEAM3", "RELAY", "garden",    "orchard"     },
//             { "OUTER.BEAMS", "BEAM4", "RELAY", "garden",    "grow area"   },
//              
//             /***    KITCHEN.CLIMATE     ***/
//             
//             { "KITCHEN.CLIMATE", "DHT1", "DHT22",   "home", "kitchen" },
//             { "KITCHEN.CLIMATE", "DHT2", "DHT22",   "home", "kitchen" },
//             { "KITCHEN.CLIMATE", "DHT3", "DHT22",   "home", "kitchen" },
//             { "KITCHEN.CLIMATE", "LDR1", "LDR",     "home", "kitchen" },
//             { "KITCHEN.CLIMATE", "AIR1", "DUSTPM25","home", "kitchen" },
//             
//             /***    PRESCOTT.CLIMATE ***/
// 
//             { "PRESCOTT.CLIMATE", "DHT1",   "DHT22",  "home", "heating" },
//             { "PRESCOTT.CLIMATE", "DHT2",   "DHT22",  "home", "heating" },
//             { "PRESCOTT.CLIMATE", "DHT3",   "DHT22",  "home", "heating" },
//             { "PRESCOTT.CLIMATE", "LDR1",   "LDR",    "home", "heating"   },
//             { "PRESCOTT.CLIMATE", "FLOOD1", "SWITCH", "home", "heating"   },
//         };
// 
//     //      _________________________________________
//     //      |                                       |
//     //      |       Lights Layout                   |
//     //      |_______________________________________|
//     //
// 
//         struct dev_light_t : public linked_dev_t
//         {
//             char        group[LEN_LABEL]    {0};
//             char        lightype[LEN_NAME]  {0};
//         };
//         
//         inline  dev_light_t     DefaultLightsLayout[LYT_DEFAULT_NUM_LIGHTS]
//         {       
// 
//             /***    OUTER PERIMETER.BEAMS     ***/
//             
//             {
//                 "GATE.BEAMS", "BEAM1", "RELAY", "gate", "entrance",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "GATE.BEAMS", "BEAM2", "RELAY", "gate", "entrance",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "GATE.BEAMS", "BEAM3", "RELAY", "gate", "entrance",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "GATE.BEAMS", "BEAM4", "RELAY", "gate", "entrance",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "OUTER.BEAMS", "BEAM1", "RELAY", "perimeter", "parking lot",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },         
//             {
//                 "OUTER.BEAMS", "BEAM2", "RELAY", "sheds",     "third shed",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },         
//             {
//                 "OUTER.BEAMS", "BEAM3", "RELAY", "garden",    "orchard",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },         
//             {
//                 "OUTER.BEAMS", "BEAM4", "RELAY", "garden",    "grow area",
//                 "outer perimeter",
//                 "LEDBEAM",
//             },  
//             
//             /***    HOME PERIMETER.BEAMS     ***/
//             
//             {
//                 "HOUSE.BEAMS", "BEAM1", "RELAY", "perimeter", "entrance",
//                 "home perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "HOUSE.BEAMS", "BEAM2", "RELAY", "perimeter", "north facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },            
//             {
//                 "HOUSE.BEAMS", "BEAM3", "RELAY", "perimeter", "north facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },            
//             {
//                 "HOUSE.BEAMS", "BEAM4", "RELAY", "perimeter", "west facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "HOUSE.BEAMS", "BEAM5", "RELAY", "perimeter", "west facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "HOUSE.BEAMS", "BEAM6", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "HOUSE.BEAMS", "BEAM7", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },
//             {
//                 "HOUSE.BEAMS", "BEAM8", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LEDBEAM",
//             },
//             
//             {
//                 "HOUSE.LIGHTS", "BEAM1", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LED10",
//             },
//             {
//                 "HOUSE.LIGHTS", "BEAM2", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LED10",
//             },
//             {
//                 "HOUSE.LIGHTS", "BEAM3", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LED10",
//             },
//             {
//                 "HOUSE.LIGHTS", "BEAM4", "RELAY", "perimeter", "south facade",
//                 "home perimeter",
//                 "LED10",
//             }
//             
//         };
// 
//     //      _________________________________________
//     //      |                                       |
//     //      |       Alarms Layout                   |
//     //      |_______________________________________|
//     //
//      
//         struct dev_alarm_t : public linked_dev_t
//         {
//             char        alarmtype[LEN_NAME] {0};    // global_alarms 
//             char        group[LEN_LABEL]    {0};    // 
//         };
// 
//         inline dev_alarm_t      DefaultAlarmsLayout[LYT_DEFAULT_NUM_ALARMS]
//         {
//           
//             /***    ATTIC FIRE.ALARMS           ***/
//             {
//                "HOUSE.BEAMS", "HEAT1", "DHT22", "home", "attic",
//                 "FIRE", "attic fire alarm"
//             },
// 
//             /***    KITCHEN FIRE.ALARMS         ***/
//             {
//                 "KITCHEN.CLIMATE", "DHT1", "DHT22", "home", "kitchen",
//                 "FIRE", "kitchen fire alarm"
//             },
//             {
//                 "KITCHEN.CLIMATE", "DHT2", "DHT22", "home", "kitchen",
//                 "FIRE", "kitchen fire alarm"
//             },
//             {
//                 "KITCHEN.CLIMATE", "DHT3", "DHT22", "home", "kitchen",
//                 "FIRE", "kitchen fire alarm"
//             },
//             {
//                 "KITCHEN.CLIMATE", "AIR1", "DUSTPM25", "home", "kitchen",
//                 "HAZARD", "kitchen gas alarm"
//             },
// 
//             /***    PRESCOTT FIRE.ALARMS        ***/
//             {
//                 "PRESCOTT.CLIMATE", "DHT1", "DHT22", "home", "heating",
//                 "FIRE", "heating room fire alarm"
//             },
//             {
//                 "PRESCOTT.CLIMATE", "DHT2", "DHT22", "home", "heating",
//                 "FIRE", "heating room fire alarm"
//             },
//             {
//                 "PRESCOTT.CLIMATE", "DHT3", "DHT22", "home", "heating",
//                 "FIRE", "heating room fire alarm"
//             },
//             
//             /***    PRESCOTT FLOOD.ALARMS       ***/
//             
//             {
//                 "PRESCOTT.CLIMATE", "FLOOD1", "SWITCH", "home", "heating",
//                 "FLOOD", "heating room flood alarm"
//             },
//             
//             
//         };
//         
//         struct dev_weather_t : public linked_dev_t
//         {
//             char        group[LEN_LABEL]         {0};
//         };
//         
//         inline dev_weather_t    DefaultWeatherStationLayout[LYT_DEFAULT_DEV_WEATHER]
//         {
//             
//             /***    KITCHEN.CLIMATE     ***/
//             
//             {
//                 "KITCHEN.CLIMATE", "DHT1", "DHT22", "home", "kitchen",
//                 "kitchen climate"
//             },
//             {
//                 "KITCHEN.CLIMATE", "DHT2", "DHT22", "home", "kitchen",
//                 "kitchen climate"
//             },
//             {
//                 "KITCHEN.CLIMATE", "DHT3", "DHT22", "home", "kitchen",
//                 "kitchen climate"
//             }
//         };
// //         struct alarm_group_t
// //         {
// //             char        name[LEN_LABEL]             {0};
// //             char        type[LEN_NAME]              {0};
// //             char        area[LEN_LOCATIONAME]       {0};
// //             char        location[LEN_LOCATIONAME]   {0};
// //             uint8_t     devnum                      {0};
// //             dev_alarm_t devices[MAX_DEVS_PER_ALARM] {0};
// //         };
// //         
// //         inline alarm_group_t    AlarmsDefaultGroups[ALR_DEFAULT_GROUP_NUM]
// //         {
// //             "Attic fire allarm", "FIRE",
// //             "home", "attic", 1,
// //             {
// //                 "HOUSE.BEAMS", "HEAT1",
// //                 "home", "attic",
// //                 "FIRE", 2                
// //             }
// //             
// //         };
//         
//     //      _________________________________________
//     //      |                                       |
//     //      |       Main Layout Class               |
//     //      |_______________________________________|
//     //
//              
//         class LayoutOld
//         {
//             std::vector<linked_dev_t>   _devices;
//             std::vector<dev_light_t>    _lights;
//             std::vector<dev_alarm_t>    _alarms;
//             std::vector<dev_weather_t>  _weather;
//             
//         public:
//              LayoutOld() {}
//             ~LayoutOld() { clear(); }
//             
//             void    reset       (void);
//             void    show        (void);
//             void    clear       (void);
//             int     check       (void);
//             
//             bool    save        (void);
//             bool    load        (void);
//             
//             void    parse       (packet::netpacket *);
//             
//             int     is_device   (const char *, const char *);
//             int     is_light    (const char *, const char *);
//             int     is_alarm    (const char *, const char *);
//             int     is_weather  (const char *, const char *);
// 
//             packet::netpacket   *   get_layout_packet   (const char *, const char *, const char *);
//             
//             void    send_devices    (void);
//             void    send_lights     (void);
//             void    send_alarms     (void);
//             void    send_weather    (void);
//             
//             void    send_config     (void);
//             
//             const std::vector<linked_dev_t> & devices() { return _devices; }
//             const std::vector<dev_light_t>  & lights()  { return _lights; }
//             const std::vector<dev_alarm_t>  & alarms()  { return _alarms; }
//             const std::vector<dev_weather_t>& weather() { return _weather; }
//             
//         };
        
