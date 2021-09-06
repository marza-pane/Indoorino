
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


void on_exit         (int exitcode=0)
{
    printf("\n");
    
    fflush(stdout);

    System.save_state();
    System.on_exit();
    
    Server.stop();
    
    printf("\n%s\t exitcode [%d] on process [%d] - See you in space cowboy\n\n", debug::TRS, exitcode, getpid());
    
    std::exit(EXIT_SUCCESS);
}










net::IndoorinoServer        Server;
indoorino::IndoorinoSystem  System;

int main (void)
{
    signal(SIGINT, on_exit);
    srand(time(NULL));
    std::cout << "\nIndoorino Server start!" << std::endl;
    
    benchmark::b_board();
    benchmark::b_utils();
        
    System.begin();
    Server.begin();
    
	time_t last=utils::millis() + 1000;
    while (1)
	{

        Server.loop();
        
        /* Qui metti un bel wait() con condition variable su Server.incoming(). Quando arriva il pacchetto esegui System.parse */
        
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
