/*
 * paths.cpp
 *
 *  Created on: Oct 24, 2020
 *      Author: n00b
 */

#if defined (__linux__)
#include "paths.h"

char    _syspath_buffer_ [LEN_SERVER_PATH] {0};

void            init_path_ws        (void)
{
    memset(_syspath_buffer_, 0, LEN_SERVER_PATH);
    struct passwd *pw = getpwuid(getuid());
    strcpy(_syspath_buffer_, pw->pw_dir);
    strcat(_syspath_buffer_, "/.indoorino");
}

//      _____________________________________________________________________
//      |                                                                   |
//      |       Indoorino paths tool                                        |
//      |___________________________________________________________________|
//

namespace indoorino
{
    
    Paths::Paths()
    {
        init_path_ws();
    }

    Paths::~Paths()
    {}

    const char  *   Paths::root         (void)
    {
        return (const char *)_syspath_buffer_;
    }
    const char  *   Paths::passwd       (void)
    {
        memset(_buffer, 0, LEN_SERVER_PATH);
        strcat(_buffer, _syspath_buffer_);
        strcat(_buffer, _d_db);
        strcat(_buffer, _f_passwd);
        return (const char *)_buffer;
    }

    const char  *   Paths::db_boards    (void)
    {
        memset(_buffer, 0, LEN_SERVER_PATH);
        strcat(_buffer, _syspath_buffer_);
        strcat(_buffer, _d_db);
        strcat(_buffer, _f_boards);
        return (const char *)_buffer;    
    }

    const char  *   Paths::db_devices   (void)
    {
        memset(_buffer, 0, LEN_SERVER_PATH);
        strcat(_buffer, _syspath_buffer_);
        strcat(_buffer, _d_db);
        strcat(_buffer, _f_devs);
        return (const char *)_buffer;    
    }

    Paths paths;
    
} /* namespace : indoorino */

#endif /* __linux__ */
