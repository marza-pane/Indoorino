/*
 * paths.h
 *
 *  Created on: Oct 24, 2020
 *      Author: n00b
 */
#pragma once

#ifndef SOURCE_PATH_CLASS_H_
#define SOURCE_PATH_CLASS_H_

#if defined (__linux__)

#include "../common/definitions.h"
#include <pwd.h>
#include <stdlib.h>

namespace indoorino
{
    
    class Paths
    {
    private:
        char            _buffer[LEN_SERVER_PATH] {0};
        
        const char      _d_db[LEN_LABEL]        = "/database";
        const char      _d_conf[LEN_LABEL]      = "/conf";

        const char      _f_passwd[LEN_LABEL]    = "/users.db";
        const char      _f_boards[LEN_LABEL]    = "/boards.db";
        const char      _f_devs[LEN_LABEL]      = "/devices.db";
        const char      _f_layout[LEN_LABEL]    = "/layout.db";

    public:    
                 Paths();
        virtual ~Paths();

        const char  *   root        (void);     /* return </home/user/.indoorino> */
        const char  *   passwd      (void);     /* return shell users password database </home/user/.indoorino/database/users.db */
        const char  *   db_boards   (void);     /* return board database </home/user/.indoorino/database/boards.db */
        const char  *   db_devices  (void);     /* return devices database </home/user/.indoorino/database/devices.db */
        const char  *   db_layout   (void);     /* return layout database </home/user/.indoorino/database/layout.db */
    };
    
    extern Paths paths;

} /* namespace : indoorino */

#endif /* __linux__ */
#endif /* SOURCE_PATH_CLASS_H_ */
