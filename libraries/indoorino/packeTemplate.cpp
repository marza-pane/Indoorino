/*
 * packetTemplate.cpp
 *
 *  Created on: Oct 24, 2019
 *      Author: n00b
 */

#include "packeTemplate.h"

ipacket::ipacket            (ibacomm_t com)
{
    packetmap_template p;
    for (ibacomm_t i=0; i<TOTAL_IBACOM; i++)
    {
        memcpy_P(&p, &PackeTable[i], sizeof(p)); 
        if (p.comm == com)
        {
            _fptr = (char *)&PackeTable[i];
            _data = (char *)calloc(data_size(), sizeof(char));
            debug_mem("\nMEM: new packet %s (malloc %u bytes)", F2C(label()), data_size());
            return;
        }
    }
    error_mem("\nFATAL: con not init packet %u\nCrashing...\n",com);
    _fptr = (char *)&PackeTable[0];
}

ibacomm_t       ipacket::command        (void)
{
    packetmap_template p;
    memcpy_P(&p, _fptr, sizeof(p));
    return p.comm;    
}

const  __FSH *  ipacket::label          (void)
{
    return FPSTR(_fptr + 3);   
}

ibasize_t       ipacket::data_size      (void)
{
    ibasize_t r=0;
    varmap_template s;
    packetmap_template p;

    memcpy_P(&p, _fptr, sizeof(p));
    for (ibavar_t  i=0; i<p.fields; i++)
    {
        for (ibavar_t j=0; j<VARMAP_NUMBER; j++)
        {
            if (p.var[i] == j)
            { 
                memcpy_P(&s, &VarTable[j], sizeof(s));
                r+=s.size;
            }
        }
    }
    return r;
}

ibasize_t       ipacket::full_size      (void)
{
    return(2 + SIZEOF_PREAMBLE + LEN_NAME + SIZEOF_COMMAND + data_size());
}

void            ipacket::forge          (char * buffer)
{
    forge(buffer, P2C(BOARD_NAME));
}

void            ipacket::forge          (char * buffer, const char * name)
{
    ibasize_t   ndx=SIZEOF_PREAMBLE;

    memcpy_P (buffer, PREAMBLE, SIZEOF_PREAMBLE);
    memcpy (buffer + ndx, name, LEN_NAME);
    ndx += LEN_NAME;

    debug_mem("\nMEM: forging packet %s:%s %s:[%u]",
              name, F2C(label()), buffer + SIZEOF_PREAMBLE, command());
    buffer[ndx] = 1;
    ndx++;
    
    ibacomm_t comm = command();
    
    memcpy(buffer + ndx, &comm, SIZEOF_COMMAND);
    ndx += SIZEOF_COMMAND;
    
    memcpy(buffer + ndx, _data, data_size());
    ndx+=data_size();

    buffer[ndx]=0;
    debug_mem("...done");
}

void            ipacket::clear          (void)
{
    memset(_data, 0, data_size());
    _ndx=0;
}

bool            ipacket::append         (uint8_t app)
{
    if (_ndx < data_size())
    {
        _data[_ndx]=app;
        _ndx++;
        return true;
    }
    return false;
}

ibasize_t       ipacket::fill           (char * buffer, ibasize_t size)
{
    if (size <= data_size())
    {
        memcpy(_data, buffer, size);
        return true;
    }
    return false;
}

uint32_t        ipacket::checksum       (void)
{
    debug_net("\nNET: ipacket %u:%s: checksum", command(), F2C(label()));
    return packet_chksum(_data, data_size());
}

bool            ipacket::check          (void)
{
    /* Implement packetCheck.cpp : for each type check data based on var types */
    return true;
}

#if defined (INDOORINO_PYLIB)


PyObject  *     ipacket::packet2dict    (char * name)
{
    static  PyObject *   obj_buffer      =nullptr;
//     Py_XDECREF(obj_buffer);
    
    obj_buffer = PyDict_New();
    
    PyDict_SetItemString(obj_buffer, "label",        PyUnicode_FromString(F2C(label())));
    PyDict_SetItemString(obj_buffer, "name",         PyUnicode_FromString(name));
    PyDict_SetItemString(obj_buffer, "command",      PyLong_FromUnsignedLong(command()));
    PyDict_SetItemString(obj_buffer, "data_size",    PyLong_FromUnsignedLong(data_size()));
    PyDict_SetItemString(obj_buffer, "full_size",    PyLong_FromUnsignedLong(full_size()));
    
    PyObject * data=PyDict_New();
    
    packetmap_template pck;
    varmap_template    pvr;

    memcpy_P(&pck, _fptr, sizeof(pck));

    
    ibasize_t   increment=0;
    char        type_name[LEN_FIELD];
    
    for (ibavar_t i=0; i<pck.fields; i++)
    {       
        for(ibavar_t j=0; j<VARMAP_NUMBER; j++)
        {
            if (pck.var[i] == j)
            {
                memcpy(&pvr, &VarTable[j], sizeof(pvr));
                strcpy(type_name, pvr.name);

                switch (pvr.type)
                {
                    case 'c':
                    {
                        char buffer[pvr.size] {0};
                        strcpy(buffer, _data + increment);

                        if (is_string_human(buffer))
                        {
                            PyDict_SetItemString(data, type_name ,PyUnicode_FromString(buffer));
//                             printf("ipacket2dict: copying string %s for %s", buffer, type_name);               
                        }
                        else
                        {
                            PyDict_SetItemString(data, type_name ,PyUnicode_FromString(""));
                            printf("ipacket2dict: Invalid string for %s", type_name);               
                        }
                        break;
                    }
                    case 's':
                    {
                        uint8_t u8=0;
                        memcpy(&u8, _data + increment, sizeof(uint8_t));
                        PyDict_SetItemString(data, type_name ,PyLong_FromUnsignedLong(u8));
                        break;
                    }
                    case 'l':
                    {
                        uint32_t u32=0;
                        memcpy(&u32, _data + increment, sizeof(uint32_t));
                        PyDict_SetItemString(data, type_name ,PyLong_FromUnsignedLong(u32));
                        break;
                    }
                    default:
                    {
                        printf("ipacket2dict: Invalid type %c for %s", pvr.type, type_name);               
                    }
                }
                increment += pvr.size;
                break;
            }
        }
    }
    
    PyDict_SetItemString(obj_buffer, "data", data);
    return obj_buffer;
}


void            ipacket::dict2packet    (PyObject  * ptr)
{
    if (!PyDict_Check(ptr))
    {
        debug_mem("\nWARING:packet:dict2packet: object is not a dict()!");
        return;
    }
    
    packetmap_template pck;
    varmap_template    pvr;

    memcpy_P(&pck, _fptr, sizeof(pck));

    
    ibasize_t   increment=0;
    char        var_name[LEN_FIELD];
    
    for (ibavar_t i=0; i<pck.fields; i++)
    {       
        for(ibavar_t j=0; j<VARMAP_NUMBER; j++)
        {
            if (pck.var[i] == j)
            {
                memcpy(&pvr, &VarTable[j], sizeof(pvr));
                strcpy(var_name, pvr.name);
                
                debug_mem("\n\tpacket:dict2packet:found variable %s [%c : %d] ", var_name, pvr.type, pvr.size);

                if (PyDict_Contains(ptr, PyUnicode_FromString(var_name)))
                {
                    switch (pvr.type)
                    {
                        case 'c':
                        {
                            const char * p = PyUnicode_AsUTF8(PyDict_GetItemString(ptr, var_name));
                            if (is_string_human(p))
                            {
                                debug(" --> string : %s",p);
                                strcpy(_data + increment, p);               
//                                 strncpy(_data + increment, p, min((uint16_t)strlen(p), (uint16_t)SERIAL_TX_BUFFER_SIZE));               
                            }
                            else
                            {
                                error_io("\nWARNING:packet:dict2packet: invalid string for %s <%s>", var_name, p);
                            }
                            break;
                        }
                        case 's':
                        {
                            uint8_t u8 = PyLong_AsLong(PyDict_GetItemString(ptr, var_name));;
                            memcpy(_data + increment, &u8, sizeof(uint8_t));               
                            debug(" --> uint8_t : %u",u8);
                            break;
                        }
                        case 'l':
                        {
                            uint32_t u32 = PyLong_AsLong(PyDict_GetItemString(ptr, var_name));;
                            memcpy(_data + increment, &u32, sizeof(uint32_t));               
                            debug(" --> uint8_t : %d",u32);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    
                    increment += pvr.size;
                    break;
                }
            }
        }
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
//     map_packet s;
//     memcpy_P(&s, _fptr, sizeof(s));
// 
//     char        w=0;
//     ibasize_t   t=0;
//     ibasize_t   size=0;
//     char        typebuf[LEN_NAME];
//     
    
//     for (uint8_t i=0; i<s.fields; i++)
//     {
//         w = s.var[i].type;
//         size = s.var[i].size;
//         strcpy(typebuf, P2C(s.var[i].name));
// 
//         if (!PyDict_Contains(ptr, PyUnicode_FromString(typebuf)))
//         {
//             mem_error("\nWARNING:pydict: invalid data dictionary missing %s", typebuf);
//             continue;
//         }
//         
//         debug("\nipacket:fromPyobject: [%u] - %s(%c)",i, typebuf,s.var[i].type);
//                     
//         switch (w)
//         {
// 
//             case 'c':
//             {
//                 const char * p = PyUnicode_AsUTF8(PyDict_GetItemString(ptr, typebuf));
//                 debug("\n\tstring : %s",p);
//                 strncpy(_data + t, p, min((uint16_t)strlen(p), (uint16_t)SERIAL_TX_BUFFER_SIZE));               
//                 break;
//             }
//             case 's':
//             {
//                  uint8_t u8 = PyLong_AsLong(PyDict_GetItemString(ptr, typebuf));;
//                 memcpy(_data + t, &u8, sizeof(uint8_t));               
//                 break;
//             }
//             case 'l':
//             {
//                  uint32_t u32 = PyLong_AsLong(PyDict_GetItemString(ptr, typebuf));;
//                 memcpy(_data + t, &u32, sizeof(uint32_t));               
//                 break;
//             }
//             default:
//             {
//                 mem_error("ERROR: Invalid size for type %c [%u bytes]", typebuf, i);
//             }
//             
//         }
//         
//         t += size;
//         
//     }

//     if (is_dictkey(ptr, "name"))
//     {
//         const char * name=PyUnicode_AsUTF8(PyDict_GetItemString(ptr, "name"));
//         strcpy(_data->name(), name);
//     }
// 
//     if (is_dictkey(ptr, "nextprobe"))
//     {
//         *_data->nextprobe() = PyLong_AsLong(PyDict_GetItemString(ptr, "nextprobe"));
//     }
// 
//     if (is_dictkey(ptr, "probes"))
//     {
//         *_data->probes() = PyLong_AsLong(PyDict_GetItemString(ptr, "probes"));
//     }

}

#endif  
