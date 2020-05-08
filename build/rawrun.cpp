
#define DEBUG_GENERIC  
#define DEBUG_DEVICES  
#define DEBUG_MEMORY
#define DEBUG_IO
#define DEBUG_NET
    

#include <stdio.h>
#include <string.h>

#include "../libraries/indoorino/packetParse.h"

void    logo    (const char * l)
{
    const int s=64;
    
    printf("\n\t");
    for (int i=0; i<s; i++)    { printf("_"); }
    printf("\n\t|");
    for (int i=0; i<s-1; i++)    { printf(" "); }
    printf("|\n\t|");
    for (int i=0; i<5; i++)    { printf(" "); }
    printf("RAWRUN - %s", l);
    for (long unsigned int i=0; i<s - strlen(l) - 15; i++)    { printf(" "); }
    printf("|\n\t|");
    for (int i=0; i<s-1; i++)    { printf("_"); }
    printf("|\n");
}

int     main    (void)
{
    logo("DEBUG");
    
    printf(F("\n\n\tDEBUG START - all debug will be sent in the debug port\n"));

    benchmark_debug();
    benchmark_board();
    benchmark_utils();
    benchmark_packets();
    debug("\n\n");
    
    resetBoard();
    
    packetParse * parser = new packetParse;
    parser->begin();
    
    ipacket * ptr = new ipacket(IBACOM_REPORT);
    ipacket * obt = nullptr;
    
    strcpy(ptr->p_message(), "This is a rawrun test string ");
    strcpy(ptr->p_name(), "RAWRUNAPP");
    *ptr->p_epoch()=123456;
    *ptr->p_level()=2;
    
    char * packetbuffer=(char*)calloc(ptr->full_size(), sizeof(char));
    ptr->forge(packetbuffer);
    for (ibasize_t i=0; i< ptr->full_size(); i++)
    {
        parser->append(packetbuffer[i]);
        obt = parser->packet();
        if (obt != nullptr)
        {
            printf("\n\t *** PACKET OBTAINED! ***");
            if (obt->command() == IBACOM_REPORT)
            {
                printf("\n\tMessage :%s\n\tName    :%s", obt->p_message(), obt->p_name());
                printf("\n\tEpoch   :%d\n\tLevel   :%d", *obt->p_epoch(), *obt->p_level());
                
            }
        }
    }
    
    delete(ptr);
    if (obt != nullptr) delete(obt);
    
    printf("\n\n");
    return 0;
}
