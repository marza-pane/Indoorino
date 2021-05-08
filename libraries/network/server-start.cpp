
/*
 * server main.cpp
 *
 *  Created on: Oct 19, 2020
 *  Edited  on: Dec 21, 2020
 *      Author: n00b
 */

#if defined (INDOORINO_SERVER)

// #include "indoorino-server.h"
// #include "../common/utils.h"
#include "../network/server.h"
#include "../indoorino/indoorino-system.h"
// #include "../network/boardserver.h"
// #include "../network/shellserver.h"

net::IndoorinoServer        Server;
indoorino::IndoorinoSystem  System;

int main (void)
{
    signal(SIGINT, on_exit);
    srand(time(NULL));
    std::cout << "\nIndoorino Server start!" << std::endl;
    
//     iSize_t size = 40;
//     char buffer[40] {0};
//     strcpy(buffer, "Io sono Dio");
    
//     benchmark::b_debug();
//     benchmark::b_board();
//     benchmark::b_utils();
//     benchmark::b_queue();
//     benchmark::b_list();
//     benchmark::b_packets();

    System.begin();
    Server.begin();
    
	time_t last=utils::millis() + 1000;
    while (1)
	{

        System.loop();
        Server.loop();
        
        if (utils::millis() > last)
        {
            last=utils::millis() + 1000;
            std::cout << ".";
            fflush(stdout);
        }
    }
	
    std::cout << std::endl;
    std::cout << std::endl;
    return 0;
}

#endif /* INDOORINO_SERVER*/
