/*
 * server_module.h
 *
 *  Created on: Apr 23, 2020
 *      Author: n00b
 */

#ifndef SOURCE_SERVER_MODULE_H_
#define SOURCE_SERVER_MODULE_H_

#if defined (INDOORINO_CLIENT)
    
#include "../common/indoorino.h"
#include "../network/client.h"
#include "../indoorino/layout.h"

PyObject    *   pyi_begin               (PyObject *, PyObject *);
PyObject    *   pyi_stop                (PyObject *, PyObject *);
PyObject    *   pyi_fetch               (PyObject *, PyObject *);
PyObject    *   pyi_packet              (PyObject *, PyObject *);
PyObject    *   pyi_login               (PyObject *, PyObject *);
PyObject    *   pyi_commit              (PyObject *, PyObject *);
PyObject    *   pyi_serverequest        (PyObject *, PyObject *);
PyObject    *   pyi_config              (PyObject *, PyObject *);
PyObject    *   pyi_status              (PyObject *, PyObject *);
PyObject    *   pyi_benchmark           (PyObject *, PyObject *);
PyObject    *   pyi_packet_from_serial  (PyObject *, PyObject *);
PyObject    *   pyi_packet_to_serial    (PyObject *, PyObject *);

PyObject    *   pyi_isStringBoardName   (PyObject *, PyObject *);
PyObject    *   pyi_isStringDeviceName  (PyObject *, PyObject *);

bool        pyi_isinkeys    (PyObject * dict, const char * key);

#endif  /* INDOORINO_CLIENT */

#endif  /* SOURCE_SERVER_MODULE_H_ */
