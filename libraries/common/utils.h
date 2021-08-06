/*
 * common/utils.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#ifndef SOURCE_UTILS_H_
#define SOURCE_UTILS_H_

#if defined(ARDUINO)
    #include "RTClib.h"
#else
#endif
#include "debug.h"

//      _____________________________________________________________________
//      |                                                                   |
//      |       Benchmark tools                                             |
//      |___________________________________________________________________|
//

namespace benchmark
{
    void        b_debug             (void);
    void        b_board             (void);
    void        b_esp               (void);
    void        b_utils             (void);
    void        b_packets           (void);
    void        b_config            (void);
    void        b_queue             (void);
    void        b_list              (void);
    void        b_packets           (void);
}

//      _____________________________________________________________________
//      |                                                                   |
//      |       Low level tools                                             |
//      |___________________________________________________________________|
//

int memvcmp(void *data, unsigned char val, unsigned int size);

namespace utils
{
    iEpoch_t    epoch_now           (void);
    void        wait                (unsigned long msec);
    char    *   time_string         (const uint32_t epoch);
    bool        is_devname          (const char * p, uint8_t min_size=4, uint8_t max_size=LEN_NAME);
    bool        is_readable         (const char * p, uint8_t min_size=1, uint8_t max_size=SERIAL_TX_BUFFER_SIZE);
    void        debughelper         (const __FSH *, ...);
    void        dump_hex            (const char * buffer, iSize_t size, int mode=0);
    void        dump_sha            (const char * sha);
    iSize_t     chopstring          (const char *string, const char splitter, char ** buffer, iSize_t max_word_size=LEN_NAME);
    uint32_t    random_signed       (uint32_t start=0, uint32_t stop=UINT16_MAX);
    double      random_double       (double   start=0, double   stop=DBL_MAX);
    
#if defined(__linux__)
    time_t      millis              (void);
#endif /* __linux__ */    
    template <typename OBJ> class List;
    template <typename OBJ> class Queue;    
    template <typename OBJ> class ObjectList;
    template <typename OBJ> class ObjectQueue;
    
#if defined (ARDUINO)
    namespace board
    {
        size_t      available_ram       (void);
        void        reset               (void);
        void        debug_init          (void);
        bool        can_alloc           (const iSize_t size);
        bool        can_alloc_p         (const iCom_t command);
        bool        is_pin              (const iPin_t pin);       
        
        class       BoardIo;
        extern      BoardIo io;
        void        send_config                 (void);
        void        send_status                 (void);
        void        send_boot_signal            (void);
        void        send_aes_setup              (const char * key);

    } /* namespace : board */
    
#endif /* ARDUINO */

} /* namespace : utils */


#endif /* SOURCE_UTILS_H_ */

// WARNING:END:TEST:TODO

// template <class T>
// struct v2d_generic
// {
// 	T x = 0;
// 	T y = 0;
// 
// 	v2d_generic() : x(0), y(0)                        {                                                      }
// 	v2d_generic(T _x, T _y) : x(_x), y(_y)            {                                                      }
// 	v2d_generic(const v2d_generic& v) : x(v.x), y(v.y){                                                      }
// 	T mag()	                                          {return sqrt(x * x + y * y);	                         }
// 	v2d_generic  norm()                               {T r = 1 / mag(); return v2d_generic(x*r, y*r);	     }
// 	v2d_generic  perp()                               {return v2d_generic(-y, x);                            }
// 	T dot(const v2d_generic& rhs)                     {return this->x * rhs.x + this->y * rhs.y;             }
// 	T cross(const v2d_generic& rhs)                   {return this->x * rhs.y - this->y * rhs.x;             }
// 	v2d_generic  operator +  (const v2d_generic& rhs) {return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
// 	v2d_generic  operator -  (const v2d_generic& rhs) {return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
// 	v2d_generic  operator *  (const T& rhs)           {return v2d_generic(this->x * rhs, this->y * rhs);	 }
// 	v2d_generic  operator /  (const T& rhs)           {return v2d_generic(this->x / rhs, this->y / rhs);	 }
// 	v2d_generic& operator += (const v2d_generic& rhs) {this->x += rhs.x; this->y += rhs.y; return *this;     }
// 	v2d_generic& operator -= (const v2d_generic& rhs) {this->x -= rhs.x; this->y -= rhs.y; return *this;     }
// 	v2d_generic& operator *= (const T& rhs)           {this->x *= rhs; this->y *= rhs; return *this;	     }
// 	v2d_generic& operator /= (const T& rhs)           {this->x /= rhs; this->y /= rhs; return *this;	     }
// 	T& operator [] (size_t i)	                      {return *((T*)this + i);	   /* <-- D'oh :( */         }
// };


//      _________________________________________
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


/*
 * 

Question is whether we can somehow write the following expression without both then and else parts

    (integer == 5) ? (THENEXPR) : (ELSEEXPR);

If you only need the then part you can use &&:
    (integer == 5) && (THENEXPR)

If you only need the else part use ||:
    (integer == 5) || (ELSEEXPR)

If you want a return value:
    return (integer == 5) ? THENEXPR : ELSEEXPR

 *
 
 
try
{
}
catch (std::exception& e)
{
std::cerr << "Exception caught : " << e.what() << std::endl;
}

 
 
std::vector<int> vec1;
vec1.reserve(30);  // Allocate space for 30 items, but vec1 is still empty.

std::vector<int> vec2;
vec2.resize(30);  // Allocate space for 30 items, and vec2 now contains 30 items.

 
 
#include <iostream>
#include <chrono>

int main()
{
    auto start = std::chrono::steady_clock::now( );
    
    for( int i = 0; i < 2000; i++ ){ std::cout << "i: " << i << '\n'; }  // make computer to do something so time would pass.
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
    
    std::cout << "milliseconds since start: " << elapsed.count( ) << '\n';
}


*/
