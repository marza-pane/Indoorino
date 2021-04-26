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

#define LYT_DEFAULT_NUM_BOARDS      4
#define LYT_DEFAULT_NUM_LIGHTS      8
#define LYT_DEFAULT_NUM_ALARMS      1

#define LYT_NUM_AREAS               10
#define LYT_NUM_L_HOME              17
#define LYT_NUM_L_PERIMETER         7
#define LYT_NUM_DEVTYPES            5

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
        };
        
        const char global_devtypes[LYT_NUM_DEVTYPES][LEN_DEVNAME]
        {
            "RELAY",
            "DHT22",
            "SWITCH",
            "LDR",
            "SERVO",
        };
        
    //      _________________________________________
    //      |                                       |
    //      |       Layout utils                    |
    //      |_______________________________________|
    //

        class device_t
        {
        public:
            char        name[LEN_DEVNAME] {0};
            char        type[LEN_DEVNAME] {0};
        };

        class board_t
        {
        public:
            char        name[LEN_NAME]              {0};
            uint8_t     devnum                      {0};
            char        area[LEN_LOCATIONAME]       {0};
            char        location[LEN_LOCATIONAME]   {0};
            device_t    devices[MAX_ATTACHED_DEVICES];

        };
        
        class linked_dev_t
        {
        public:
            char        boardname[LEN_NAME]         {0};
            char        devname[LEN_DEVNAME]        {0};
            char        area[LEN_LOCATIONAME]       {0};
            char        location[LEN_LOCATIONAME]   {0};            
        };
        
        
        template <class OBJ> class LinkedDevList
        {
            /* OBJ should have linked_dev_t as _layout */
            
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

            bool    exist       (const char * board, const char * dev) { if (this->get_index(board, dev) == -1) { return false; } return true; }

        };

        
    //      _________________________________________
    //      |                                       |
    //      |       Board Layout                    |
    //      |_______________________________________|
    //

        inline  board_t         BoardDefaultLayout[LYT_DEFAULT_NUM_BOARDS]
        {
            
            /***    HOUSE.BEAMS         ***/
            {
                "SRV.HOUSE.BEAMS", 0,
                "home", "attic",
                {
                }
            },

            {
                "HOUSE.BEAMS", 9,
                "home", "attic",
                {
                    { "BEAM1", "RELAY" },
                    { "BEAM2", "RELAY" },
                    { "BEAM3", "RELAY" },
                    { "BEAM4", "RELAY" },
                    { "BEAM5", "RELAY" },
                    { "BEAM6", "RELAY" },
                    { "BEAM7", "RELAY" },
                    { "BEAM8", "RELAY" },
                    { "HEAT1", "DHT22" },
                }
            },

            /***    KITCHEN.WEATHER     ***/
            {
                "SRV.KITCHEN.WEATHER", 0,
                "home", "kitchen",
                {
                }
            },

            {
                "KITCHEN.WEATHER", 4,
                "home", "kitchen",
                {
                    { "DHT1", "DHT22" },
                    { "DHT2", "DHT22" },
                    { "DHT3", "DHT22" },
                    { "LDR1", "LDR" },
                }
            }
            
        };

    //      _________________________________________
    //      |                                       |
    //      |       Lights Layout                   |
    //      |_______________________________________|
    //

        struct dev_light_t : public linked_dev_t
        {
            char        group[LEN_NAME] {0};
            char        type[LEN_NAME]  {0};
        };
        
        inline  dev_light_t     LightsDefaultLayout[LYT_DEFAULT_NUM_LIGHTS]
        {
          
            /***    PERIMETER.BEAMS     ***/
            {
                "HOUSE.BEAMS", "BEAM1",
                "perimeter", "entrance",
                "PERIMETER",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM2",
                "perimeter", "north facade"
                "PERIMETER",
                "LEDBEAM",
            },            
            {
                "HOUSE.BEAMS", "BEAM3",
                "perimeter", "north facade"
                "PERIMETER",
                "LEDBEAM",
            },            
            {
                "HOUSE.BEAMS", "BEAM4",
                "perimeter", "west facade"
                "PERIMETER",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM5",
                "perimeter", "west facade"
                "PERIMETER",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM6",
                "perimeter", "south facade"
                "PERIMETER",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM7",
                "perimeter", "south facade"
                "PERIMETER",
                "LEDBEAM",
            },
            {
                "HOUSE.BEAMS", "BEAM8",
                "perimeter", "south facade"
                "PERIMETER",
                "LEDBEAM",
            }
            
        };

    //      _________________________________________
    //      |                                       |
    //      |       Alarms Layout                   |
    //      |_______________________________________|
    //
     
        struct dev_alarm_t : public linked_dev_t
        {
            char        type[LEN_NAME]  {0};
            uint8_t     level           {0};
        };

        inline dev_alarm_t      AlarmsDefaultLayout[LYT_DEFAULT_NUM_ALARMS]
        {
          
            /***    HEAT.ALARMS         ***/
            {
                "HOUSE.BEAMS", "HEAT1",
                "home", "attic",
                "HEAT", 2
            },
            
        };

        
        class Layout
        {
            std::vector<board_t>        _boards;
            std::vector<dev_light_t>    _lights;
            std::vector<dev_alarm_t>    _alarms;
            
        public:
             Layout() {}
            ~Layout() {}
            
            void    reset       (void);
            void    show        (void);
            void    save        (const char *);
            void    load        (const char *);
            
            int     check       (void);
            int     is_board    (const char * entry);

            packet::netpacket   *   get_boards_layout   (void);
            packet::netpacket   *   get_lights_layout   (void);
            packet::netpacket   *   get_alarm_layout    (void);
            packet::netpacket   *   get_weather_layout  (void);
            packet::netpacket   *   get_camera_layout   (void);
            packet::netpacket   *   get_hydro_layout    (void);
            packet::netpacket   *   get_pool_layout     (void);
            
            const std::vector<board_t>& boards() { return _boards; }
            const std::vector<dev_light_t>& lights() { return _lights; }
            const std::vector<dev_alarm_t>& alarms() { return _alarms; }
            
        };
        
    } /* namespace:layout */

} /* namespace:indoorino */
    
#endif /* INDOORINO_NETWORK */

#endif /* _SRC_LAYOUT_H_ */
