/*
 * board_x86pc.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_PC_X86GENERIC_H_
#define SOURCE_PC_X86GENERIC_H_

    #include <stdint.h>
    
    #if defined(INDOORINO_SERVER)
        const char BOARD_TYPE[] = "SERVER";
//         #pragma message "Compiling INDOORINO SERVER"
    #elif defined(INDOORINO_CLIENT)
        const char BOARD_TYPE[] = "MODULE";
//         #pragma message "Compiling PYTHON MODULE"
    #else
        const char BOARD_TYPE[] = "APPLICATION";
        #pragma message "Compiling GENERIC APPLICATION"
    #endif

    #define RAMEND 0
    #define SRAM_LIMIT 0

//      ____________________________________
//      |                                   |
//      |       Arduino PROGMEM 4 PC        |
//      |___________________________________|
//

    #ifndef PROGMEM
    #define PROGMEM
    #endif
    	
    #ifndef __FlashStringHelper
    #define __FlashStringHelper char
    #endif

    #ifndef F
    #define F(...) __VA_ARGS__
    #endif

    #ifndef FPSTR
	#define FPSTR(...) __VA_ARGS__
	#endif

    #ifndef memcmp_P
    #define memcmp_P(...) memcmp(__VA_ARGS__)
    #endif

    #ifndef memcpy_P
    #define memcpy_P(...) memcpy(__VA_ARGS__)
    #endif

    #ifndef strcpy_P
    #define strcpy_P(...) strcpy(__VA_ARGS__)
    #endif
    
    #ifndef strncpy_P
    #define strncpy_P(...) strncpy(__VA_ARGS__)
    #endif
    
    #ifndef sprintf_P
    #define sprintf_P(...) sprintf(__VA_ARGS__)
    #endif
    
    #ifndef strcmp_P
    #define strcmp_P(...) strcmp(__VA_ARGS__)
    #endif

    #ifndef strlen_P
    #define strlen_P(...) strlen(__VA_ARGS__)
    #endif

    #ifndef vsnprintf_P
    #define vsnprintf_P(...) vsnprintf(__VA_ARGS__)
    #endif

    #define P2C(...) __VA_ARGS__
    #define F2C(...) __VA_ARGS__

//      ________________________________
//      |                               |
//      |       DEBUG TOOLS             |
//      |_______________________________|
//
    
    #define SerialDebugPrint(S) std::cout << S

#endif /* SOURCE_PC_X86GENERIC_H_ */
