/*
 * server_module.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#if defined (INDOORINO_PYLIB)

#include "server_module.h"

//    _______________________________________________________________________
//    |                                                                     |
//    |    MODULE UTILS                                                     |
//    |_____________________________________________________________________|
//

static PyObject     *   pybenchmark     (PyObject *self, PyObject *args)
{
    benchmark_debug();
    benchmark_board();
    benchmark_utils();
    benchmark_packets();
    Py_RETURN_NONE;
}

//    _______________________________________________________________________
//    |                                                                     |
//    |    MODULE PYTHON BUILD                                              |
//    |_____________________________________________________________________|
//

static PyMethodDef			IpsClassMethods[]=
{
    {"parse",       parsearray,     METH_VARARGS, "Parse byte array."},
    {"parsebyte",   parsebyte,      METH_VARARGS, "Parse single byte."},
    {"obtain",      obtain,         METH_VARARGS, "Retrive incoming packet(s)."},        
    {"appconfig",   appconfig,      METH_VARARGS, "Default configuration."},
    {"checksum",    pychksum,       METH_VARARGS, "Indoorino packet checksum."},
    {"build",       buildpacket,    METH_VARARGS, "Build valid dictionary from scratches"},
    {"forge",       pyforge,        METH_VARARGS, "From dictionatry to bytearray."},
    {"benchmark",   pybenchmark,    METH_VARARGS, "Benchmark function."},

    
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


static struct PyModuleDef	IpsClassModule=
{
	PyModuleDef_HEAD_INIT,
	"IndoorinoModule",     /* name of module */
	NULL,                  /* module documentation, may be NULL */
	-1,                    /* size of per-interpreter state of the module,
								or -1 if the module keeps state in global variables. */
	IpsClassMethods
};


PyMODINIT_FUNC				PyInit_IndoorinoModule(void)
{	
    packetparser.begin();
    module_packet_list=PyList_New(0);
    
	PyObject * object;
	object = PyModule_Create(&IpsClassModule);	
	return object;
};


//       ____________________________________________________________________
//       |                                                                  |
//       |       MODULE FUNCTIONS                                           |
//       |__________________________________________________________________|
// 


static PyObject     *   parsearray      (PyObject *self, PyObject *args)
{
		
    static PyObject      *   obj;
    
	if (!PyArg_ParseTuple(args, "Y", &obj))
	{
		printf("\niba:module:parse: ivalid bytearray object\n");
		Py_RETURN_FALSE;
	}
	
    char * array=(char*)calloc(MAX_PAYLOAD_SIZE, sizeof(char));
	uint32_t size = PyByteArray_Size(obj);
    ipacket * incoming=nullptr;
    memcpy(array, PyByteArray_AsString(obj), size);
			
	debug_io("\npyimodule: parsersing array");
//     dump_array(array, size);
	
	for (uint32_t m=0; m<size; m++)
	{
		packetparser.append(array[m]);
        
        incoming = packetparser.packet();
        if (incoming != nullptr)
        {
            debug_io(" :: added <%s> packet\nParsed name : <%s>", incoming->label(), packetparser.buffername());
            PyList_Append(module_packet_list, incoming->packet2dict(packetparser.buffername()));            
        }
	}    

    free(array);
    Py_RETURN_NONE;
};

static PyObject     *   parsebyte       (PyObject *self, PyObject *args)
{
    int8_t bytearray;
    ipacket * incoming=nullptr;
    
	if (!PyArg_ParseTuple(args, "b", &bytearray))
	{
		debug("iba:module:parse: ivalid byte input");
		Py_RETURN_FALSE;
    }

    packetparser.append(bytearray);
    incoming = packetparser.packet();
    if (incoming != nullptr)
    {
        debug_io(" :: added %s packet", incoming->label());
        PyList_Append(module_packet_list, incoming->packet2dict(packetparser.buffername()));
    }
    
    Py_RETURN_NONE;
}

static PyObject     *   obtain          (PyObject *self, PyObject *args)
{
    static PyObject * ptr;
    ptr=PyList_AsTuple(module_packet_list);    
    module_packet_list=PyList_New(0);
    return ptr;
};

static PyObject     *   appconfig       (PyObject *self, PyObject *args)
{
    static PyObject * ptr=nullptr;
    
    ptr=PyDict_New();
    
    PyDict_SetItemString(ptr, "name"        ,PyUnicode_FromString(BOARD_NAME));
    PyDict_SetItemString(ptr, "platform"    ,PyUnicode_FromString(BOARD_TYPE));
    PyDict_SetItemString(ptr, "type"        ,PyUnicode_FromString(INDOORINO_TYPE));
    PyDict_SetItemString(ptr, "ip"          ,PyUnicode_FromString(DEFAULT_TARGET_IP));
    PyDict_SetItemString(ptr, "port"        ,PyLong_FromUnsignedLong(DEFAULT_TARGET_PORT));
    PyDict_SetItemString(ptr, "baud"        ,PyLong_FromUnsignedLong(SERIAL_DEFAULT_BAUDRATE));

    return ptr;
};

static PyObject     *   pyforge         (PyObject *self, PyObject *args)
{
    static PyObject * pdict=nullptr;
    static PyObject * packet=nullptr;	
    static ipacket  * ipk = new ipacket(0);
	
	if (!PyArg_ParseTuple(args, "O", &pdict))
	{
		printf("\nmodule:forge invalid input");
		Py_RETURN_NONE;
	}

    if (!is_valid_packet_dict(pdict))
    {
        char z=0;
        printf("\nmodule:forge: can not build packet\n");
        return PyByteArray_FromStringAndSize(&z, 1);
    }
    
    debug(F("\nmodule:forge:Parsing dictionary"));
    
    ipk = reallocPacket(ipk, PyLong_AsLong(PyDict_GetItemString(pdict, "command")));
    ipk->dict2packet(PyDict_GetItemString(pdict, "data"));
    
    char * buff=(char *)calloc(MAX_PAYLOAD_SIZE, sizeof(char));
    ipk->forge(buff, PyUnicode_AsUTF8(PyDict_GetItemString(pdict, "name")));
    packet = PyByteArray_FromStringAndSize(buff, ipk->full_size());
    
    free(buff);
    
    return packet;
};

static PyObject     *  pychksum         (PyObject *self, PyObject *args)
{
    
    static PyObject * pdict=nullptr;
    uint32_t chksum=0;
	
	if (!PyArg_ParseTuple(args, "O", &pdict))
	{
		printf("\nmodule:checksum: input is not a dictionary");
	}
	else if (!is_valid_packet_dict(pdict))
    {
        printf("\nmodule:checksum: invalid packet dictionaty\n");
    }
    else
    {
        debug(F("\nmodule:checksum: generating checksum from dictionary"));
        ipacket * ipk = new ipacket(PyLong_AsLong(PyDict_GetItemString(pdict, "command")));
        ipk->dict2packet(PyDict_GetItemString(pdict, "data"));
        chksum = ipk->checksum();
    }
    
    
    return PyLong_FromUnsignedLong(chksum);
};

static PyObject 	*	buildpacket	(PyObject *self, PyObject *args)
{
    static PyObject   * pdict=nullptr;
    static PyObject   * packet=PyDict_New();	
        
    printf("\nipstools:build");
	
	if (!PyArg_ParseTuple(args, "O", &pdict))
	{
		debug("\nmodule:buildpacket invalid input");
		Py_RETURN_NONE;
	}

    if (!is_valid_packet_dict(pdict))
    {
        debug("\nmodule:buildpacket: can not build packet\n");
        return packet;
    }
    debug(F("\nmodule:buildpacket:Parsing dictionary\n"));
  
    char name[LEN_NAME];
    ipacket * ipk = new ipacket(PyLong_AsLong(PyDict_GetItemString(pdict, "command")));

    ipk->dict2packet(PyDict_GetItemString(pdict, "data"));    
    strcpy(name, PyUnicode_AsUTF8(PyDict_GetItemString(pdict, "name")));
    packet = ipk->packet2dict(name);
    
    return packet;
};

#endif
