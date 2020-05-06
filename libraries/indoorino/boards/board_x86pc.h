/*
 * board_x86pc.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_PC_X86GENERIC_H_
#define SOURCE_PC_X86GENERIC_H_

    #include <stdint.h>
    
    const char BOARD_TYPE[] = "APPLICATION";
    #pragma message "Compiling for PC DESKTOP APPLICATION"

    #define RAMEND 0

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

//     #ifndef strcmp_P
//     #define strcmp_P(...) strcmp(__VA_ARGS__)
//     #endif

    #ifndef vsnprintf_P
    #define vsnprintf_P(...) vsnprintf(__VA_ARGS__)
    #endif
    
//      ________________________________
//      |                               |
//      |       DEBUG TOOLS             |
//      |_______________________________|
//
    
    #define SerialDebugPrint(S) printf("%s", (const char*)S)

#endif /* SOURCE_PC_X86GENERIC_H_ */
