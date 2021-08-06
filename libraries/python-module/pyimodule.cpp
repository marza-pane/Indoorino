/*
 * server_module.h
 *
 *  Created on: Apr 22, 2020
 *      Author: n00b
 */

#if defined (INDOORINO_CLIENT)

#include "pyimodule.h"

// static ShellClient * Client=nullptr;

PyMODINIT_FUNC              PyInit_IndoorinoModule  (void);

//    _______________________________________________________________________
//    |                                                                     |
//    |    MODULE UTILS                                                     |
//    |_____________________________________________________________________|
//

bool                    pyi_isinkeys        (PyObject * dict, const char * key)
{     
    if (!PyDict_Contains(dict, PyUnicode_FromString(key)))
    {
        warning_os("pyi_isinkeys: can not find %s", key);
        return false;
    }
    return true;
}
uint8_t                 pyi_validate_dict   (PyObject * dict)
{
    
    if (!PyDict_Check(dict))
    {
        error_pack("validate_dict: invalid object type");
        return 6;
    }

    if (!pyi_isinkeys(dict, "command"))
    {
        error_pack("validate_dict: missing command!");
        return 5;
    }
    
    if(!PyLong_Check(PyDict_GetItemString(dict, "command")))
    {
        error_pack("validate_dict: command is not int!");
        return 4;
    }
       
    if (packet::payloadsize(PyLong_AsLong(PyDict_GetItemString(dict, "command"))) == -1)
    {
        error_pack("validate_dict: command is invalid!");   
        return 3;        
    }
    
    bool flag=false;
    if (!pyi_isinkeys(dict, "source"))
    {
        error_pack("validate_dict: missing source ");
        PyDict_SetItemString(dict, "source", PyUnicode_FromString(""));
        flag=true;
    }
    if (!pyi_isinkeys(dict, "target"))
    {
        error_pack("validate_dict: missing target ");
        PyDict_SetItemString(dict, "target", PyUnicode_FromString(""));
        flag=true;
    }
    
    if (flag) return 2;

    char buffer[LEN_NAME];
    strcpy(buffer, PyUnicode_AsUTF8(PyDict_GetItemString(dict, "source")));
    if (!utils::is_devname(buffer))
    {        
        error_pack("validate_dict: invalid source %s", buffer);
        return 1;
    }
    strcpy(buffer, PyUnicode_AsUTF8(PyDict_GetItemString(dict, "target")));
    if (!utils::is_devname(buffer))
    {        
        error_pack("validate_dict: invalid source %s", buffer);
        return 1;
    }
//     if (!pyi_isinkeys(dict, "data"))
//     {
//         PyDict_SetItemString(dict, "data", PyDict_New());
//     }
//     else
//     {
//         PyObject * payload =  PyDict_GetItemString(dict, "data");
            /*  HERE CHECK ALL DATA FIELDS MATCHING ACTUAL IPACKET */
//     }
    return 0;
};

 PyObject     *   pyi_benchmark       (PyObject *self, PyObject *args)
{
    benchmark::b_debug();
    benchmark::b_board();
    benchmark::b_utils();
    benchmark::b_packets();
    Py_RETURN_NONE;
}

//    _______________________________________________________________________
//    |                                                                     |
//    |    MODULE PYTHON BUILD                                              |
//    |_____________________________________________________________________|
//

 PyMethodDef  IpsClassMethods[]=
{
    {"begin",           pyi_begin,          METH_VARARGS, "Connect client to server"},
    {"stop",            pyi_stop,           METH_VARARGS, "Stop client"},
    {"fetch",           pyi_fetch,          METH_VARARGS, "Fetch (any) received packet"},
    {"send",            pyi_commit,         METH_VARARGS, "Commit packet to server."},        
    
    {"login",           pyi_login,          METH_VARARGS, "Client login"},
    {"serverequest",    pyi_serverequest,   METH_VARARGS, "Commit server command"},  
    
    {"ipacket",         pyi_packet,         METH_VARARGS, "Generate packet from dictionary"},
    {"config",          pyi_config,         METH_VARARGS, "Client configuration."},
    {"status",          pyi_status,         METH_VARARGS, "Client status"},
    
    {"benchmark",       pyi_benchmark,      METH_VARARGS, "Benchmark function."},

    {"isBoardName",     pyi_isStringBoardName,  METH_VARARGS, "Check if string is a valid board name."},
    {"isDeviceName",    pyi_isStringDeviceName, METH_VARARGS, "Check if string is a valid device name."},
    {"fromSerial",      pyi_packet_from_serial, METH_VARARGS, "Generate packet from bytearray"},
    {"toSerialBytes",   pyi_packet_to_serial,   METH_VARARGS, "Generate bytearray from dictionary"},
    
    {NULL, NULL, 0, NULL}        /* Sentinel */
};
    

 struct PyModuleDef   IpsClassModule=
{
    PyModuleDef_HEAD_INIT,
    "IndoorinoModule",     /* name of module */
    NULL,                  /* module documentation, may be NULL */
    -1,                    /* size of per-interpreter state of the module,
                                or -1 if the module keeps state in global variables. */
    IpsClassMethods
};


PyMODINIT_FUNC  PyInit_IndoorinoModule   (void)
{
    /* here initialize client */
    info_os("Initializing Indoorino Module");
    srand(time(NULL));
    PyObject * object;
    object = PyModule_Create(&IpsClassModule);
    return object;
}


//       ____________________________________________________________________
//       |                                                                  |
//       |       MODULE FUNCTIONS                                           |
//       |__________________________________________________________________|

        /*       B E G I N       */
  PyObject    *   pyi_begin           (PyObject *self, PyObject *args)
{
    static PyObject * name;
    static PyObject * host;
    static PyObject * port;
    
	if (!PyArg_ParseTuple(args, "OOO", &name, &host, &port))
	{
		error_os("module:begin: invalid input");
		Py_RETURN_FALSE;
	}
	
	if (!PyUnicode_Check(name))
    {
        warning_os("module:begin: invalid name type");
        Py_RETURN_FALSE;
    }
	if (!PyUnicode_Check(host))
    {
        warning_os("module:begin: invalid host type");
        Py_RETURN_FALSE;
    }
	if (!PyLong_Check(port))
    {
        warning_os("module:begin: invalid port type");
        Py_RETURN_FALSE;
    }
    
    const char * name_c = PyUnicode_AsUTF8(name);
    
    if (!utils::is_devname(name_c))
    {
        warning_os("module:begin: invalid name <%s>", name_c);
        Py_RETURN_FALSE;        
    }
    
    std::string name_s(name_c);
    std::string host_s(PyUnicode_AsUTF8(host));
    
    if (Client != nullptr)
    {
        Client->stop();
        delete Client;
    }
    
    Client = new ShellClient();
    
    if (Client->begin(name_s, host_s, PyLong_AsLong(port))) 
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

        /*       S T O P       */
  PyObject    *   pyi_stop            (PyObject *self, PyObject *args)
{
    
    alert_client("Stopping client!");
    if (Client != nullptr)
    {
        Client->stop();
        delete Client;
        Client = nullptr;
    }
    Py_RETURN_NONE;
}

                    /*   G E N   I P A C K E T   */
PyObject    *       pyi_packet                  (PyObject *self, PyObject *args)
{
    static PyObject     *   obj = nullptr;
    
    if (!PyArg_ParseTuple(args, "O", &obj))
	{
		error_os("module:ipacket: invalid input");
		Py_RETURN_FALSE;
	}
    
    if (pyi_validate_dict(obj) > 2)
    {
        warning_os("module:buildpacket: can not build packet");
        Py_RETURN_FALSE;
    }
    debug_module("module:buildpacket:Parsing dictionary");
    
    
    packet::netpacket p;
    
    p.fromDictionary(obj);
    
    PyDict_Clear(obj);
    
    p.toDictionary(obj);

/*
     packet::netpacket * p = new packet::netpacket();
    
    p->fromDictionary(obj);
    
    PyDict_Clear(obj);
    
    p->toDictionary(obj);
    
    delete p;
*/    
    
    Py_RETURN_TRUE;
}

                    /* F R O M  S E R I A L */
PyObject    *       pyi_packet_from_serial      (PyObject *self, PyObject *args)
{
    static PyObject     *   obj = nullptr;
    static PyObject     *   head = nullptr;
    
    if (!PyArg_ParseTuple(args, "OO", &obj, &head))
	{
		error_os("module:ipacket: invalid input");
		Py_RETURN_FALSE;
	}
    
    if (!PyDict_Check(obj))
    {
        warning_os("module:fromSerial: input must be <dict>, <bytes>");
        Py_RETURN_FALSE;
    }
    
    if (!PyBytes_Check(head))
    {
        error_io("invalid header type");
        Py_RETURN_FALSE;
    }
    
    char * head_buffer = (char *)malloc(PyBytes_Size(head));

    debug_module("module:buildpacket:Parsing head");
    memcpy(head_buffer, PyBytes_AsString(head), PyBytes_Size(head));    
    packet::netpacket p(head_buffer, true);
    free(head_buffer);
    
    if (p.command() > 0)
    {
        p.toDictionary(obj);
        Py_RETURN_TRUE;
    }
    
    Py_RETURN_FALSE;

}

        /*   T O    S E R I A L   */
PyObject    *       pyi_packet_to_serial    (PyObject *self, PyObject *args)
{
    static PyObject     *   obj = nullptr;
//     static PyObject     *   arr = nullptr;

    
    if (!PyArg_ParseTuple(args, "O", &obj))
	{
		error_os("module:ipacket: invalid input");
		Py_RETURN_FALSE;
	}

//     if (!PyBytes_Check(array))
//     {
//         error_io("invalid header type");
//         Py_RETURN_FALSE;
//     }

    if (pyi_validate_dict(obj) > 2)
    {
        warning_os("module:buildpacket: can not build packet");
        Py_RETURN_FALSE;
    }
    debug_module("module:buildpacket:Parsing dictionary");

    
    packet::netpacket p;
    p.fromDictionary(obj);
    
    if (p.command() == 0)
    {
        error_io("module:packet2serial: invalid input dictionary");
        Py_RETURN_FALSE;
    }
    
    iSize_t size = p.full_size();
    char * buffer=(char *)calloc(size, sizeof(char));
    
    p.forge(buffer);
    PyObject * arr = PyBytes_FromStringAndSize(buffer, size);
    free(buffer);
    
    return arr;
}

        /*       F E T C H       */
PyObject    *   pyi_fetch                 (PyObject *self, PyObject *args)
{    
    static PyObject * obj = nullptr;
    
    if (Client == nullptr) Py_RETURN_FALSE;
    
    if (Client->incoming().is_empty()) Py_RETURN_FALSE;

    if (!PyArg_ParseTuple(args, "O", &obj))
	{
		error_os("module:fetch: invalid input");
		Py_RETURN_FALSE;
	}
	if(!PyDict_Check(obj))
    { 
        warning_os("module:fetch: invalid type");
        Py_RETURN_FALSE;
    }

    packet::netpacket * p = Client->incoming().pop_front();
    
    if (p == nullptr || p->command() == 0)
    { 
        warning_os("module:fetch: got invalid packet");
        Py_RETURN_FALSE;
    }
    
    p->dump();
    
    p->toDictionary(obj);
    
    delete p;
    
    Py_RETURN_TRUE;
}


        /*      C O M M I T      */
PyObject    *   pyi_commit          (PyObject *self, PyObject *args)
{
    static PyObject * obj=nullptr;

	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		error_os("module:commit: invalid input");
	}
	else if (pyi_validate_dict(obj) > 2)
    {
        warning_pack("module:checksum: invalid packet dictionaty");
    }
    else
    {
        if (Client == nullptr) Py_RETURN_FALSE;
        packet::netpacket p;
        p.fromDictionary(obj);
        Client->send(&p);        
    }
    Py_RETURN_NONE;
}

        /*  C O M M I T R E Q U E S T  */
  PyObject    *   pyi_serverequest    (PyObject *self, PyObject *args)
{
    static PyObject     *   string = nullptr;

    
    if (!PyArg_ParseTuple(args, "O", &string))
	{
		error_os("module:server:request: invalid input");
		Py_RETURN_NONE;
	}
    
    if(!PyUnicode_Check(string))
    {
        error_os("module:fetch: invalid name type");
        Py_RETURN_NONE;
    }
    
    if (Client == nullptr) Py_RETURN_FALSE;

    packet::netpacket p(IBACOM_SRV_REQ);
    strcpy(p.p_command(), PyUnicode_AsUTF8(string));
    strcpy(p.source, Client->name().c_str());
    strcpy(p.target, DEFAULT_TARGET);
    
    debug_module("server:request: <%s>", p.p_command());


    Client->send(&p);
    Py_RETURN_NONE;
}


        /*       L O G I N       */
  PyObject    *   pyi_login           (PyObject *self, PyObject *args)
{
    static PyObject * name;
    static PyObject * password;
    
	if (!PyArg_ParseTuple(args, "OO", &name, &password))
	{
		error_os("module:login: ivalid input");
		Py_RETURN_FALSE;
	}
	
	if(!PyUnicode_Check(name))     { warning_os("module:login: invalid name type"); Py_RETURN_FALSE; }
	if(!PyUnicode_Check(password)) { warning_os("module:login: invalid password type"); Py_RETURN_FALSE; }

	if (Client == nullptr) Py_RETURN_FALSE;

	Client->login(PyUnicode_AsUTF8(name), PyUnicode_AsUTF8(password));
    Py_RETURN_TRUE;
}

        /*       S T A T U S      */
  PyObject    *   pyi_status          (PyObject *self, PyObject *args)
{
    static PyObject     *   data = nullptr;
    
    if (!PyArg_ParseTuple(args, "O", &data))
	{
		error_io("module:status: invalid input");
		Py_RETURN_NONE;
	}
	if(!PyDict_Check(data))
    {
        warning_os("module:fetch: invalid type");
        Py_RETURN_NONE;
    }
	
    PyDict_SetItemString(data, "netstat", PyBool_FromLong(int(net::netstatus())));

    if (Client == nullptr)
    {
        PyDict_SetItemString(data, "clientstat", Py_False);
    }
    else
    {
        PyDict_SetItemString(data, "clientstat", PyBool_FromLong(int(Client->is_connected())));
    }
//     PyDict_SetItemString(data, "sessiontime", PyLong_FromUnsignedLong(client.session_status()));
    
    Py_RETURN_NONE;
}

        /*      C O N F I G      */
  PyObject    *   pyi_config           (PyObject *self, PyObject *args)
{
    static PyObject     *   data = nullptr;
    
    if (!PyArg_ParseTuple(args, "O", &data))
	{
		error_os("module:config: invalid input");
		Py_RETURN_NONE;
	}
	if(!PyDict_Check(data))
    {
        warning_os("module:config: invalid type");
        Py_RETURN_NONE;
    }
    
    PyObject * macros =  PyDict_New();
    PyDict_SetItemString(macros, "SERIAL_DEFAULT_BAUDRATE", PyLong_FromUnsignedLong(SERIAL_DEFAULT_BAUDRATE));
    PyDict_SetItemString(macros, "DEFAULT_SERVER_IP", PyUnicode_FromString(DEFAULT_SERVER_IP));
    PyDict_SetItemString(macros, "DEFAULT_SHELL_PORT", PyLong_FromUnsignedLong(DEFAULT_SHELL_PORT));
    PyDict_SetItemString(macros, "DEFAULT_BOARD_PORT", PyLong_FromUnsignedLong(DEFAULT_BOARD_PORT));
    PyDict_SetItemString(macros, "SHA_DIGEST_LENGTH", PyLong_FromUnsignedLong(SHA_DIGEST_LENGTH));
    PyDict_SetItemString(macros, "LEN_AES_MASTERKEY", PyLong_FromUnsignedLong(LEN_AES_MASTERKEY));
    PyDict_SetItemString(macros, "SIZEOF_PACKET_HEADER", PyLong_FromUnsignedLong(SIZEOF_PACKET_HEADER));
    PyDict_SetItemString(macros, "SIZEOF_NET_HEADER", PyLong_FromUnsignedLong(SIZEOF_NET_HEADER));
    PyDict_SetItemString(macros, "INET_ADDRSTRLEN", PyLong_FromUnsignedLong(INET_ADDRSTRLEN));
    PyDict_SetItemString(macros, "TIMEOUT_BOARD", PyLong_FromUnsignedLong(TIMEOUT_BOARD));
    PyDict_SetItemString(macros, "TIMEOUT_CLIENT", PyLong_FromUnsignedLong(TIMEOUT_CLIENT));
    PyDict_SetItemString(macros, "TIMEOUT_CLIENT_SHELL", PyLong_FromUnsignedLong(TIMEOUT_CLIENT_SHELL));
    PyDict_SetItemString(macros, "TIMEOUT_CLIENT_BOARD", PyLong_FromUnsignedLong(TIMEOUT_CLIENT_BOARD));
    PyDict_SetItemString(macros, "FLOAT2INT", PyLong_FromUnsignedLong(FLOAT2UINT_M));
    PyDict_SetItemString(macros, "PREAMBLE", PyBytes_FromStringAndSize((char * )PREAMBLE, SIZEOF_PREAMBLE));
    
    PyObject * constants = PyDict_New();
    
    PyObject * list_area        = PyList_New(0);
    PyObject * list_sub_home    = PyList_New(0);
    PyObject * list_sub_perim   = PyList_New(0);
    PyObject * list_devtype     = PyList_New(0);
    
    PyObject * list_alarmtype   = PyList_New(0);
    PyObject * list_alarmgroup  = PyList_New(0);
    
    PyObject * list_physic_vals = PyList_New(0);
    
    for (int i=0; i<LYT_NUM_AREAS; i++)
    {
        PyList_Append(list_area, PyUnicode_FromString(indoorino::layout::global_area[i]));
    }
    
    for (int i=0; i<LYT_NUM_L_HOME; i++)
    {
        PyList_Append(list_sub_home, PyUnicode_FromString(indoorino::layout::global_home[i]));
    }

    for (int i=0; i<LYT_NUM_L_PERIMETER; i++)
    {
        PyList_Append(list_sub_perim, PyUnicode_FromString(indoorino::layout::global_perimeter[i]));
    }

    for (int i=0; i<LYT_NUM_DEVTYPES; i++)
    {
        PyList_Append(list_devtype, PyUnicode_FromString(indoorino::layout::global_devtypes[i]));
    }

    for (int i=0; i<LYT_NUM_ALARMS; i++)
    {
        PyList_Append(list_alarmtype, PyUnicode_FromString(indoorino::layout::global_alarms[i]));
    }
    
    for (int i=0; i<LYT_NUM_ALARMS_GROUP; i++)
    {
        PyList_Append(list_alarmgroup, PyUnicode_FromString(indoorino::layout::global_alarms_group[i]));
    }
    
    
    
    for (int i=0; i<LYT_NUM_AMBIENTVAR; i++)
    {
        PyObject * b = PyList_New(0);
                                                                                /* v */
        PyList_Append(b, PyUnicode_FromString(indoorino::layout::global_ambient_types[i]));
        PyList_Append(b, PyUnicode_FromString(indoorino::layout::global_ambient_units[i]));
        
        PyList_Append(list_physic_vals, b);
    }
    
/*    
    
    
            const char global_ambient_types[LYT_NUM_AMBIENTVAR][LEN_NAME]
        {
            "TEMPERATURE",
            "HUMIDITY",
            "LIGHT",
            "POWER",
            "STATE",
            "AIR POLLUTION",
        };
        const char global_ambient_units[LYT_NUM_AMBIENTVAR][LEN_FIELD]
        {
            "C",
            "RH",
            "Lux",
            "kW",
            "",
            "ug/m3",
        };*/

    
    
    PyDict_SetItemString(constants, "area",             list_area);
    PyDict_SetItemString(constants, "area_home",        list_sub_home);
    PyDict_SetItemString(constants, "area_perimeter",   list_sub_perim);
    PyDict_SetItemString(constants, "devtypes",         list_devtype);
    PyDict_SetItemString(constants, "alarmtypes",       list_alarmtype);
    PyDict_SetItemString(constants, "alarmgroup",       list_alarmgroup);
    PyDict_SetItemString(constants, "ambientypes",      list_physic_vals);

//     PyDict_SetItemString(constants, "num_area",             PyLong_FromUnsignedLong(LYT_NUM_AREAS));
//     PyDict_SetItemString(constants, "num_area_home",        PyLong_FromUnsignedLong(LYT_NUM_L_HOME));
//     PyDict_SetItemString(constants, "num_area_perimeter",   PyLong_FromUnsignedLong(LYT_NUM_L_PERIMETER));
//     PyDict_SetItemString(constants, "num_devtypes",         PyLong_FromUnsignedLong(LYT_NUM_DEVTYPES));
//     PyDict_SetItemString(constants, "num_alarmtypes",       PyLong_FromUnsignedLong(LYT_NUM_ALARMS));
//     PyDict_SetItemString(constants, "num_alarmgroup",       PyLong_FromUnsignedLong(LYT_NUM_ALARMS_GROUP));

    
    PyDict_SetItemString(data, "version", PyUnicode_FromString(INDOORINO_VERSION));    
    PyDict_SetItemString(data, "macros", macros);
    PyDict_SetItemString(data, "const", constants);
    Py_RETURN_NONE;
};

        /*   I S S T R I N G   B O A R D   */
  PyObject    *   pyi_isStringBoardName       (PyObject * self, PyObject * args)
{
    static PyObject * obj;
    
	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		error_os("module:isBoardName: ivalid string object");
		Py_RETURN_FALSE;
	}
	
    const char * buf = PyUnicode_AsUTF8(obj);
    
    if (utils::is_devname(buf, 1, LEN_DEVNAME)) Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

        /*  I S S T R I N G   D E V I C E   */
  PyObject    *   pyi_isStringDeviceName      (PyObject * self, PyObject * args)
{
    static PyObject * obj;
    
	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		error_os("module:isBoardName: ivalid string object");
		Py_RETURN_FALSE;
	}
	
    const char * buf = PyUnicode_AsUTF8(obj);
    
    if (utils::is_devname(buf, 1, LEN_DEVNAME)) Py_RETURN_TRUE;
    Py_RETURN_FALSE;

}


#endif /* INDOORINO_CLIENT */




// 
// /*    pyi_packet_head(dict, packet);
//     
//     switch(packet->command())
//     
//     {
//         case IBACOM_SESSION_STAT:
//         {
//             PyDict_SetItemString(dict, "status", PyLong_FromUnsignedLong(*packet->p_epoch()));
//             PyDict_SetItemString(dict, "epoch", PyLong_FromUnsignedLong(*packet->p_status()));            
//             break;
//         }
//         case IBACOM_SRV_CONF:
//         {
//             PyDict_SetItemString(dict, "ip",            PyUnicode_FromString(packet->p_ip1()));
//             PyDict_SetItemString(dict, "boardport",     PyLong_FromUnsignedLong(*packet->p_port1()));
//             PyDict_SetItemString(dict, "shellport",     PyLong_FromUnsignedLong(*packet->p_port2()));
//             break;
//         }
//         case IBACOM_SRV_BOARD_LIST:
//         {
//             break;
//         }
//         
//         case IBACOM_SRV_BOARD:
//         {
//             PyDict_SetItemString(dict, "type",     PyUnicode_FromString(packet->p_type()));
//             PyDict_SetItemString(dict, "board",    PyUnicode_FromString(packet->p_board()));
//             PyDict_SetItemString(dict, "ip",       PyUnicode_FromString(packet->p_ip1()));
// 
//             PyDict_SetItemString(dict, "devnum",    PyLong_FromUnsignedLong(*packet->p_devnum()));
//             PyDict_SetItemString(dict, "epoch",     PyLong_FromUnsignedLong(*packet->p_epoch()));
//             PyDict_SetItemString(dict, "looptime",  PyLong_FromUnsignedLong(*packet->p_looptime()));
//             PyDict_SetItemString(dict, "freemem",   PyLong_FromUnsignedLong(*packet->p_freemem()));
//             PyDict_SetItemString(dict, "port",         PyLong_FromUnsignedLong(*packet->p_port1()));
//             break;
//         }
//         case IBACOM_SRV_BOARD_ESP:
//         {
//             PyDict_SetItemString(dict, "type",     PyUnicode_FromString(packet->p_type()));
//             PyDict_SetItemString(dict, "board",    PyUnicode_FromString(packet->p_board()));
//             PyDict_SetItemString(dict, "ip",       PyUnicode_FromString(packet->p_ip1()));
//             PyDict_SetItemString(dict, "boardip",  PyUnicode_FromString(packet->p_ip2()));
//             PyDict_SetItemString(dict, "serverip", PyUnicode_FromString(packet->p_ip3()));
//             PyDict_SetItemString(dict, "aeskey",   PyByteArray_FromStringAndSize(packet->p_aeskey1(), LEN_AES_MASTERKEY));
// 
//             PyDict_SetItemString(dict, "devnum",    PyLong_FromUnsignedLong(*packet->p_devnum()));
//             PyDict_SetItemString(dict, "epoch",     PyLong_FromUnsignedLong(*packet->p_epoch()));
//             PyDict_SetItemString(dict, "looptime",  PyLong_FromUnsignedLong(*packet->p_looptime()));
//             PyDict_SetItemString(dict, "freemem",   PyLong_FromUnsignedLong(*packet->p_freemem()));
//             PyDict_SetItemString(dict, "port",         PyLong_FromUnsignedLong(*packet->p_port1()));
//             PyDict_SetItemString(dict, "boardport",    PyLong_FromUnsignedLong(*packet->p_port2()));
//             PyDict_SetItemString(dict, "serverport",   PyLong_FromUnsignedLong(*packet->p_port3()));
// 
//             PyDict_SetItemString(dict, "clientimeout", PyLong_FromUnsignedLong(*packet->p_timeout1()));
//             PyDict_SetItemString(dict, "packetimeout", PyLong_FromUnsignedLong(*packet->p_timeout2()));
//             PyDict_SetItemString(dict, "packetattemps",PyLong_FromUnsignedLong(*packet->p_count1()));
//             
//             PyDict_SetItemString(dict, "serialtx",     PyLong_FromUnsignedLong(*packet->p_serialtx()));
//             PyDict_SetItemString(dict, "serialrx",     PyLong_FromUnsignedLong(*packet->p_serialrx()));
//             PyDict_SetItemString(dict, "tcptx",        PyLong_FromUnsignedLong(*packet->p_tcptx()));
//             PyDict_SetItemString(dict, "tcprx",        PyLong_FromUnsignedLong(*packet->p_tcprx()));
//             PyDict_SetItemString(dict, "totaltx",      PyLong_FromUnsignedLong(*packet->p_totaltx()));
//             PyDict_SetItemString(dict, "totalrx",      PyLong_FromUnsignedLong(*packet->p_totalrx()));
//             
//             PyDict_SetItemString(dict, "missing",      PyLong_FromUnsignedLong(*packet->p_errors1()));
//             PyDict_SetItemString(dict, "unsent",       PyLong_FromUnsignedLong(*packet->p_errors2()));
//             PyDict_SetItemString(dict, "discounter",   PyLong_FromUnsignedLong(*packet->p_errors3()));
//             PyDict_SetItemString(dict, "overflows",    PyLong_FromUnsignedLong(*packet->p_errors4()));
//             
//             break;
//         }
//         default:
//         {}
//     }
//     */
