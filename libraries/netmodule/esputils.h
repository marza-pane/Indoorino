/*
 * espServerUtils.h
 *
 *  Created on: May 11, 2020
 *      Author: n00b
 */

#ifndef ESPUTILS_SOURCE_H_
#define ESPUTILS_SOURCE_H_

#include <stdint.h>
#include <stdlib.h>
//    ________________________________________________________________________
//    |                                                                      |
//    |    Keep Alive Connection loop                                        |
//    |______________________________________________________________________|
//

class                               connectionLoop
{
    uint32_t    _last=0;
    uint32_t    _reqtime=0;
    bool        _status=false;
    
public:
     connectionLoop();
    ~connectionLoop()   {};

    bool    status      (void)  { return _status; };
    void    begin       (void);
    void    loop        (void);
    
};

#endif /* ESPUTILS_SOURCE_H_ */
