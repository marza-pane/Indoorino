/*
 * utils.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_UTILS_H_
#define SOURCE_UTILS_H_

#include "definitions.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       Benchmark tools                                             |
//      |___________________________________________________________________|
//

void        benchmark_debug     (void);
void        benchmark_board     (void);
void        benchmark_esp       (void);
void        benchmark_utils     (void);
void        benchmark_packets   (void);

//      _____________________________________________________________________
//      |                                                                   |
//      |       Low level board tools                                       |
//      |___________________________________________________________________|
//

size_t      freeRam             (void);
void        resetBoard          (void);

#if defined (ARDUINO)
void        serialwrite         (char * payload, uint16_t size);

#if defined (ESP8266)
void        udpwrite            (const char * ip, uint16_t port, char * data, ibasize_t size);
#endif  

#endif

void        dump_array          (char * array, ibasize_t size);
void        time_string         (char * buf, uint32_t epoch);

bool        is_string_devname   (const char * p, uint8_t min_size=1, uint8_t max_size=LEN_NAME);
bool        is_string_human     (const char * p, uint8_t min_size=1, uint8_t max_size=SERIAL_TX_BUFFER_SIZE);

bool        is_string_ip        (const char * ip);
uint32_t    packet_chksum       (char * buf, ibasize_t size);

#endif /* SOURCE_UTILS_H_ */


//      ________________________________________
//      |                                       |
//      |       PROGMEM functions map           |
//      |_______________________________________|
    
/*
    int memcmp_P(const void* buf1, PGM_VOID_P buf2P, size_t size);
    void* memccpy_P(void* dest, PGM_VOID_P src, int c, size_t count);
    void* memmem_P(const void* buf, size_t bufSize, PGM_VOID_P findP, size_t findPSize);
    void* memcpy_P(void* dest, PGM_VOID_P src, size_t count);
    char* strncpy_P(char* dest, PGM_P src, size_t size);
    char* strcpy_P(dest, src)
    char* strncat_P(char* dest, PGM_P src, size_t size);
    char* strcat_P(dest, src)
    int strncmp_P(const char* str1, PGM_P str2P, size_t size);
    int strcmp_P(str1, str2P)
    int strncasecmp_P(const char* str1, PGM_P str2P, size_t size);
    int strcasecmp_P(str1, str2P)
    size_t strnlen_P(PGM_P s, size_t size);
    size_t strlen_P(strP)
    char* strstr_P(const char* haystack, PGM_P needle);
    int printf_P(PGM_P formatP, ...);
    int sprintf_P(char *str, PGM_P formatP, ...);
    int snprintf_P(char *str, size_t strSize, PGM_P formatP, ...);
    int vsnprintf_P(char *str, size_t strSize, PGM_P formatP, va_list ap);
    pgm_read_byte
    pgm_read_word
    pgm_read_byte(*data)
*/
