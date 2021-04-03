/*
 * pyipacket.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: n00b
 */

#if defined (INDOORINO_CLIENT)

// #include "../packets/ipacket.h"
#include "pyimodule.h"

namespace packet
{

    void        netpacket::toDictionary         (PyObject  * obj)
    {

//         if (!PyDict_Check(obj))
//         {
//             error_io("toDictionary: buffer must be a dictionary");
//             return;
//         }
        
        char sha_buffer[SHA_DIGEST_LENGTH] {0};
        this->checksum((uint8_t *)sha_buffer);
        
        PyDict_SetItemString(obj, "label",        PyUnicode_FromString(this->label()));
        PyDict_SetItemString(obj, "source",       PyUnicode_FromString(this->source));
        PyDict_SetItemString(obj, "target",       PyUnicode_FromString(this->target));
        PyDict_SetItemString(obj, "pid",          PyLong_FromUnsignedLong(this->_pid));
        
        PyDict_SetItemString(obj, "command",      PyLong_FromUnsignedLong(this->command()));
        PyDict_SetItemString(obj, "full_size",    PyLong_FromUnsignedLong(this->full_size()));
        PyDict_SetItemString(obj, "data_size",    PyLong_FromUnsignedLong(this->data_size()));    
        PyDict_SetItemString(obj, "checksum",     PyByteArray_FromStringAndSize((char *)sha_buffer,SHA_DIGEST_LENGTH));
        
        PyObject * data=PyDict_New();
        
        packet_fst_     pck;
        variable_fst_   pvr;

        memcpy_P(&pck, &PacketMap[this->_flash_index], sizeof(pck));
        
//         debug_module("Flash points to %s[%u] - %u elements", pck.label, pck.comm, pck.count);
        
        iSize_t     increment=0;
        char        type_name[LEN_FIELD];
        
        for (uint8_t i=0; i<pck.count; i++)
        {
            memcpy(&pvr, &VariableMap[pck.params[i]], sizeof(pvr));
            strcpy(type_name, pvr.name);

            switch (pvr.type)
            {
                case 'c':
                {
                    debug_module("Adding string %s", pvr.name);
                    char buffer[pvr.size] {0};
                    memcpy(buffer,this->payload() + increment, pvr.size);
//                     strcpy(buffer, this->payload() + increment);

                    if (utils::is_readable(buffer))
                    {
                        PyDict_SetItemString(data, type_name ,PyUnicode_FromString(buffer));
                    }
                    else
                    {
                        PyDict_SetItemString(data, type_name , PyByteArray_FromStringAndSize(buffer, pvr.size));
                        debug_io("toDictionary: Invalid string for %s", type_name);               
                    }
                    break;
                }
                case 's':
                {
                    debug_module("Adding uint8_t %s", pvr.name);
                    uint8_t u8=0;
                    memcpy(&u8, this->payload() + increment, sizeof(uint8_t));
                    PyDict_SetItemString(data, type_name ,PyLong_FromUnsignedLong(u8));
                    break;
                }
                case 'u':
                {
                    debug_module("Adding uint32_t %s", pvr.name);
                    uint32_t u32=0;
                    memcpy(&u32, this->payload() + increment, sizeof(uint32_t));
                    PyDict_SetItemString(data, type_name ,PyLong_FromUnsignedLong(u32));
                    break;
                }
                case 'n':
                {
                    debug_module("Adding int32_t %s", pvr.name);
                    int32_t i32=0;
                    memcpy(&i32, this->payload() + increment, sizeof(int32_t));
                    PyDict_SetItemString(data, type_name ,PyLong_FromLong(i32));
                    break;
                }
                case 'h':
                {
                    debug_module("Adding hash %s", pvr.name);
                    char buffer[pvr.size] {0};
                    memcpy(buffer, this->payload() + increment, pvr.size);
                    PyDict_SetItemString(data, type_name , PyByteArray_FromStringAndSize(buffer, pvr.size));
                    break;
                }
                default:
                {
                    error_io("toDictionary: Invalid type %c for %s", pvr.type, type_name);               
                }
            }
            increment += pvr.size;

        }
        
        PyDict_SetItemString(obj, "data", PyDict_Copy(data));
        Py_XDECREF(data);
    }

    
    
    
    
    void        netpacket::fromDictionary       (PyObject  * obj)
    {
        
        if (pyi_isinkeys(obj, "source"))
            strcpy(this->source, PyUnicode_AsUTF8( PyDict_GetItemString(obj, "source") ));
        else
            warning_io("module:netpacket:init: can not find source");

        if (pyi_isinkeys(obj, "target"))
            strcpy(this->target, PyUnicode_AsUTF8( PyDict_GetItemString(obj, "target") ));
        else
            warning_io("module:netpacket:init: can not find target");            

        if (pyi_isinkeys(obj, "command"))
        {
            init(PyLong_AsLong( PyDict_GetItemString(obj, "command") ));
        }
        else
        {
            error_io("module:netpacket:init: can not find COMMAND");
            return;
        }
        
//         thisPyLong_AsLong(PyDict_GetItemString(obj, "command"))
        
        obj = PyDict_GetItemString(obj, "data");
        
        packet_fst_   pck;
        variable_fst_ pvr;

        memcpy_P(&pck, &PacketMap[this->_flash_index], sizeof(pck));
        
        iSize_t   increment=0;
        
        for (uint8_t i=0; i<pck.count; i++)
        {       

            memcpy(&pvr, &VariableMap[pck.params[i]], sizeof(pvr));
            
            debug_module("fromDictionary:mapping variable %s [%c : %d] ", pvr.name, pvr.type, pvr.size);

            if (PyDict_Contains(obj, PyUnicode_FromString(pvr.name)))
            {
                PyObject * item = PyDict_GetItemString(obj, pvr.name);
                switch (pvr.type)
                {
                    case 'c':
                    {
                        
                        if (PyUnicode_Check(item))
                        {
                            const char * string = PyUnicode_AsUTF8(item);
                            if (utils::is_readable(string))
                            {
                                debug_module(" --> string : %s",string);
                                strcpy(this->payload() + increment, string);
                            }
                            else
                            {
                                debug_module("fromDictionary: invalid string for %s.p_%s", label(), pvr.name);
                            }
                        }
                        else
                        {
                            debug_module("fromDictionary: invalid object for %s ", pvr.name);
                        }
                        break;
                    }
                    case 's':
                    {
                        uint8_t u8;
                        if (PyLong_Check(item))
                        {
                            u8 = PyLong_AsLong(item);
                            memcpy(this->payload() + increment, &u8, sizeof(uint8_t));               
                            debug_module(" --> uint8_t : %u",u8);
                        }
                        else if (PyFloat_Check(item))
                        {
                            double value = PyFloat_AsDouble(item);
                            uint8_t u8 = uint8_t(value);
                            memcpy(this->payload() + increment, &u8, sizeof(uint8_t));               
                            debug_module(" --> uint8_t (float) : %u",u8);
                        }
                        else
                        {                                
                            debug_io("fromDictionary: invalid type for %s ", pvr.name);
                        }
                        break;
                    }
                    case 'u':
                    {
                        uint32_t u32;
                        if (PyLong_Check(item))
                        {
                            u32 = PyLong_AsLong(item);
                            memcpy(this->payload() + increment, &u32, sizeof(uint32_t));
                            debug_module(" --> uint32_t : %d",u32);
                            break;
                        }
                        else if (PyFloat_Check(item))
                        {
                            double value = PyFloat_AsDouble(item);
                            u32 = uint32_t(value);
                            memcpy(this->payload() + increment, &u32, sizeof(uint32_t));               
                            debug_module(" --> uint32_t (float) : %u",u32);
                        }
                        else
                        {
                            debug_io("fromDictionary: invalid object for %s ", pvr.name);                                
                        }
                    }
                    case 'n':
                    {
                        int32_t i32;
                        if (PyLong_Check(item))
                        {
                            i32 = PyLong_AsLong(item);
                            memcpy(this->payload() + increment, &i32, sizeof(int32_t));
                            debug_module(" --> int32_t : %d", i32);
                            break;
                        }
                        else if (PyFloat_Check(item))
                        {
                            double value = PyFloat_AsDouble(item);
                            i32 = int32_t(value);
                            memcpy(this->payload() + increment, &i32, sizeof(int32_t));               
                            debug_module(" --> int32_t (float) : %u",i32);
                        }
                        else
                        {
                            debug_io("fromDictionary: invalid object for %s ", pvr.name);                                
                        }
                    }
                    case 'h':
                    {
                        if (PyByteArray_Check(item))
                        {
                            char * hash = PyByteArray_AsString(item);
                            memcpy(this->payload() + increment, hash, pvr.size);               

                        }
                        else
                        {
                            debug_io("fromDictionary: invalid object for %s ", pvr.name);                                
                        }
                    }
                    default:
                    {
                        break;
                    }
                }
                
            }
            else
            {
                debug_module("fromDictionary: can not find %s", pvr.name);
            }
            increment += pvr.size;
        }
    }
    
} /* namespace : packet */

#endif /* INDOORINO_CLIENT */
