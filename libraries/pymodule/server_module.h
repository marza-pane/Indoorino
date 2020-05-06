/*
 * server_module.h
 *
 *  Created on: Apr 23, 2020
 *      Author: n00b
 */

#ifndef SOURCE_SERVER_MODULE_H_
#define SOURCE_SERVER_MODULE_H_

#if defined (INDOORINO_PYLIB)

#include "../indoorino/indoorino.h"

static  packetParse     packetparser(false);

static  PyObject    *   parsearray      (PyObject *, PyObject *);
static  PyObject    *   parsebyte       (PyObject *, PyObject *);
static  PyObject    *   obtain          (PyObject *, PyObject *);
static  PyObject    *   appconfig       (PyObject *, PyObject *);

static PyObject     *   pyforge         (PyObject *self, PyObject *args);
static PyObject     *   pychksum        (PyObject *self, PyObject *args);
static PyObject     *   buildpacket     (PyObject *self, PyObject *args);

PyObject    *   module_packet_list  =nullptr;


bool    is_string_in_keys               (PyObject * dict, const char * key)
{     
    if (!PyDict_Contains(dict, PyUnicode_FromString(key)))
    {
        debug("\nis_string_in_keys: can not find %s", key);
        return false;
    }
    return true;
};

bool    is_valid_packet_dict            (PyObject * dict)
{
    
    if (!PyDict_Check(dict))
    {
        debug("\nis_valid_packet_dict: invalid object type");
        return false;
    }
    
    if (!is_string_in_keys(dict, "name"))
    {
        printf("\nWARNING:is_valid_packet_dict: missing name ");
        return false;
    }

    if (!is_string_in_keys(dict, "data"))
    {
        PyDict_SetItemString(dict, "data", PyDict_New());
    }
    
    if (!is_string_in_keys(dict, "command"))
    {
        debug("\nis_valid_packet_dict: missing command!");
        return false;
    }
    
    bool flag=true;
    
    char name[LEN_NAME];
    strcpy(name, PyUnicode_AsUTF8(PyDict_GetItemString(dict, "name")));
    if (!is_string_devname(name))
    {        
        debug("\nERROR: validate dict: invalid name %s",name);
        return false;
    }
    return true;
};

#endif  /* INDOORINO_PYLIB */

#endif  /* SOURCE_SERVER_MODULE_H_ */
