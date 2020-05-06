/*
 * packetTemplate.cpp
 *
 *  Created on: Oct 24, 2019
 *      Author: n00b
 */


#ifndef INDOORINO_PACKET_DEFINITION_H_
#define INDOORINO_PACKET_DEFINITION_H_

#include "utils.h"
#include "packetMap.h"

#if defined (INDOORINO_PYLIB)
#include <Python.h>
#endif

class ipacket
{
private:
    ibasize_t           _ndx=0;    
protected:
    char    *           _data=nullptr;
    char    *           _fptr=nullptr;
    
public:
    ipacket     (ibacomm_t);
    ~ipacket    ()  { if (_data != nullptr) free(_data); };
    
    ibacomm_t           command         (void);
    char            *   payload         (void)      { return _data;  };
    const __FSH     *   label           (void);
    ibasize_t           data_size       (void);
    ibasize_t           full_size       (void);
    
    void                clear           (void);
    void                forge           (char *);
    void                forge           (char *, const char *);
    ibasize_t           fill            (char *, ibasize_t);
    bool                append          (uint8_t);
    uint32_t            checksum        (void);

    bool                check           (void);
    void                eval            (void) {};

    char           *    p_message       (void);
    char           *    p_command       (void);
    char           *    p_name          (void);
    char           *    p_type          (void);
    char           *    p_board         (void);
    char           *    p_devname       (void);
    uint32_t       *    p_epoch         (void);
    uint32_t       *    p_value         (void);
    uint32_t       *    p_freemem       (void);
    uint32_t       *    p_looptime      (void);
    uint32_t       *    p_errors        (void);
    uint32_t       *    p_totalrx       (void);
    uint32_t       *    p_totaltx       (void);
    uint32_t       *    p_serialrx      (void);
    uint32_t       *    p_serialtx      (void);
    uint32_t       *    p_param1        (void);
    uint32_t       *    p_param2        (void);
    uint32_t       *    p_param3        (void);
    uint32_t       *    p_param4        (void);
    uint32_t       *    p_temperature   (void);
    uint32_t       *    p_humidity      (void);
    uint32_t       *    p_stepday1      (void);
    uint32_t       *    p_stephour1     (void);
    uint32_t       *    p_timeout       (void);
    uint32_t       *    p_port          (void);
    uint8_t        *    p_level         (void);
    uint8_t        *    p_status        (void);
    uint8_t        *    p_devnum        (void);
    uint8_t        *    p_pin           (void);
    char           *    p_ip            (void);


        
#if defined (INDOORINO_PYLIB)

    PyObject        *   packet2dict     (char *);
    void                dict2packet     (PyObject *);

#endif
    
};


#endif /* INDOORINO_PACKET_DEFINITION_H_ */



