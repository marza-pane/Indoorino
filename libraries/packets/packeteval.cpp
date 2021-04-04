/*
 * packetEval.cpp
 *
 *  Created on: Apr 25, 2020
 *   Edited on: Jan 13, 2021 - 23:31:10
 *      Author: n00b
 *  This code is code-generated
 */

#include "../common/common.h"

namespace packet
{
    void		ipacket::eval		(void)
    {
        info_os("eval %s", description());

        switch (command())
        {

    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 000 - IBACOM_NONE - empty packet                            |
    //    |______________________________________________________________________|
    //

            case IBACOM_NONE:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 001 - IBACOM_INIT - init byte                               |
    //    |______________________________________________________________________|
    //

            case IBACOM_INIT:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 002 - IBACOM_RESET - reset command                          |
    //    |______________________________________________________________________|
    //

            case IBACOM_RESET:
            {
            #if defined (ARDUINO)
                utils::board::reset();
                break; // This is pointless
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 003 - IBACOM_REQUEST_CONFIG - config request                |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQUEST_CONFIG:
            {
            #if defined (ARDUINO)
//                 #if defined (ESP8266)
// 
//                 #endif
                utils::board::send_config();
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 004 - IBACOM_REQUEST_STATUS - status request                |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQUEST_STATUS:
            {
            #if defined (ARDUINO)
                utils::board::send_status();
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 005 - IBACOM_COMMAND_STRING - command string                |
    //    |______________________________________________________________________|
    //

            case IBACOM_COMMAND_STRING:
            {
            #if defined (ARDUINO)
                /* TODO sampler.parseString(this->data()->message()); */

                #if defined (INDOORINO_ROUTER)
                #elif defined (INDOORINO_CONTROLLER)
                #elif defined (INDOORINO_SAMPLER)
                #endif
                
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 006 - IBACOM_FACTORY_RESET - factory reset                  |
    //    |______________________________________________________________________|
    //

            case IBACOM_FACTORY_RESET:
            {
            #if defined (ARDUINO)
                conf.factory();
                utils::board::reset();
                break; // This is pointless
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 010 - IBACOM_UNIX_EPOCH - timestamp                         |
    //    |______________________________________________________________________|
    //

            case IBACOM_UNIX_EPOCH:
            {
            #if defined (ARDUINO)
                iEpoch_t t=0;
                memcpy(&t, this->p_epoch(), sizeof(iEpoch_t));
                debug_os("Updating RTC time: <%s>", utils::time_string(t));
                rtc.set(t);
                utils::board::send_status();
                break;
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 015 - IBACOM_REPORT - generic report                        |
    //    |______________________________________________________________________|
    //

            case IBACOM_REPORT:
            {
            #if defined (ARDUINO)
                info_pack("\tMessage: %s",   p_message());
                info_pack("\tName:    %s",   p_name()); 
                info_pack("\tEpoch:   %lu", *p_epoch());
                info_pack("\tDate:    %s",   utils::time_string(*p_epoch())); 
                info_pack("\tLevel:   %u",  *p_level());            
                return;
            #else
                char intro[16] {0};
                switch (*p_level())
                {
                    case 0: { strcpy(intro, debug::DEBUG);  break; }
                    case 1: { strcpy(intro, debug::INFO);   break; }
                    case 2: { strcpy(intro, debug::ALERT);  break; }
                    case 3: { strcpy(intro, debug::WARNING);break; }
                    case 4: { strcpy(intro, debug::ERROR);  break; }
                }
                info_pack(":REPORT:%s: %s", intro, this->description());
                std::cout << "\tMessage: " << p_message() << std::endl;
                std::cout << "\tName:    " << p_name()    << std::endl; 
                std::cout << "\tEpoch:   " << *p_epoch()  << std::endl;
                std::cout << "\tDate:    " << utils::time_string(*p_epoch()) << std::endl; 
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 301 - IBACOM_SET_SD - sd card setting                       |
    //    |______________________________________________________________________|
    //

            case IBACOM_SET_SD:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 302 - IBACOM_REQUEST_SDDROP - drop request                  |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQUEST_SDDROP:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 303 - IBACOM_REQUEST_SDCLEAR - clear request sd             |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQUEST_SDCLEAR:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 304 - IBACOM_REQUEST_SDWIPE - wipe request sd               |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQUEST_SDWIPE:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 501 - IBACOM_SET_DEVICE - device setting                    |
    //    |______________________________________________________________________|
    //

            case IBACOM_SET_DEVICE:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                /*
                * DATA item:
                * 
                * devname: device name
                * command: string command. Cann be SET, REM, PIN
                * pin:     device pin, required for PIN command
                * level:   actual value for setting. SET:1 for device on, SET:0 for off
                */

                if (conf.indexFromName(this->p_devname()) >= 0)
                {
                    if (strcmp(this->p_command(), "SET") == 0)
                    {
                        devices[this->p_devname()]->set(*this->p_value1());
                        devices[this->p_devname()]->send_dev_stat();
                    }
                    else if (strcmp(this->p_command(), "UPDATE") == 0)
                    {
                        devices[this->p_devname()]->send_dev_conf();
                        devices[this->p_devname()]->send_dev_stat();
                    }
                    else if (strcmp(this->p_command(), "RESET") == 0)
                    {
                        devices[this->p_devname()]->reset();
                        devices[this->p_devname()]->send_dev_stat();
                    }
                    else
                    {
                        warning_os("parse: invalid SET device command <%s>", this->p_command());
    //                     sendReport(2, this->label(), F("Invalid SET device command <%s>"), this->p_command());
                    }
                }
                else
                {
                    warning_os("parse: no device with name <%s>", this->p_devname());
                }
                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 502 - IBACOM_SET_DEVNAME - set devname                      |
    //    |______________________________________________________________________|
    //

            case IBACOM_SET_DEVNAME:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (conf.indexFromName(this->p_devname()) >= 0)
                    {
                        devices.setName(this->p_devname(), this->p_newname());
                        devices[this->p_newname()]->send_dev_conf();
                    }
                    else
                    {
                        warning_os("parse: no device with name <%s>", this->p_devname());
                        devices[this->p_devname()]->send_dev_conf();
                    }

                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 503 - IBACOM_SET_DEVPIN - set devpin                        |
    //    |______________________________________________________________________|
    //

            case IBACOM_SET_DEVPIN:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (conf.indexFromName(this->p_devname()) >= 0)
                    {
                        devices.setPin(this->p_devname(), *this->p_pin1());
                    }
                    else
                    {
                        warning_os("parse: no device with name <%s>", this->p_devname());
                    }
                    devices[this->p_devname()]->send_dev_conf();
                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 504 - IBACOM_REM_DEVICE - rem device                        |
    //    |______________________________________________________________________|
    //

            case IBACOM_REM_DEVICE:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (conf.indexFromName(this->p_devname()) >= 0)
                    {
                        devices.rem(this->p_devname());
                        conf.devRem(this->p_devname());
                        utils::board::send_config();
                    }
                    else
                    {
                        warning_os("parse: no device with name <%s>", this->p_devname());
                    }
                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1001 - IBACOM_CONF_STD - standard conf                      |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_STD:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1005 - IBACOM_CONF_SAMPLER - sampler conf                   |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_SAMPLER:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1301 - IBACOM_CONF_ASENSOR - analog sensor conf             |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_ASENSOR:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (!devices.mod(this))
                    {
                        devices.add(this);
                    }
                    utils::board::send_config();

                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1302 - IBACOM_CONF_SWITCH - switch conf                     |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_SWITCH:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (!devices.mod(this))
                    {
                        devices.add(this);
                    }
                    utils::board::send_config();

                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1310 - IBACOM_CONF_LDR - ldr sensor conf                    |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_LDR:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (!devices.mod(this))
                    {
                        devices.add(this);
                    }
                    utils::board::send_config();

                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1522 - IBACOM_CONF_DHT22 - DHT22 sensor conf                |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_DHT22:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (!devices.mod(this))
                    {
                        devices.add(this);
                    }
                    utils::board::send_config();

                #endif
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 1700 - IBACOM_CONF_RELAY - relay conf                       |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_RELAY:
            {
            #if defined (ARDUINO)
                #if defined (INDOORINO_DEVS)
                    if (!devices.mod(this))
                    {
                        devices.add(this);
                    }
                    utils::board::send_config();

                #endif
            #else
            #endif

                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2000 - IBACOM_CONF_DEVSTD - device generic conf             |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_DEVSTD:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2001 - IBACOM_STATUS_STD - standard status                  |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_STD:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2005 - IBACOM_STATUS_SAMPLER - sampler status               |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_SAMPLER:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2270 - IBACOM_STATUS_SD - sd statistics                     |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_SD:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2301 - IBACOM_STATUS_ASENSOR - analog sensor stat           |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_ASENSOR:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2302 - IBACOM_STATUS_SWITCH - switch sensor stat            |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_SWITCH:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2310 - IBACOM_STATUS_LDR - ldr sensor stat                  |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_LDR:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2522 - IBACOM_STATUS_DHT22 - DHT22 sensor stat              |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_DHT22:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 2700 - IBACOM_STATUS_RELAY - relay stat                     |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_RELAY:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 3000 - IBACOM_STATUS_DEVSTD - device generic stat           |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_DEVSTD:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 3001 - IBACOM_ALLARM - allarm                               |
    //    |______________________________________________________________________|
    //

            case IBACOM_ALLARM:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 3002 - IBACOM_WARNINGS - general warning                    |
    //    |______________________________________________________________________|
    //

            case IBACOM_WARNINGS:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 3205 - IBACOM_REQUEST_PROBE - probe request                 |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQUEST_PROBE:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 3505 - IBACOM_PROBE_AMBIENT - ambient probe                 |
    //    |______________________________________________________________________|
    //

            case IBACOM_PROBE_AMBIENT:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }

    #if defined (INDOORINO_NETWORK)

    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 5001 - IBACOM_CONF_ESP - esp conf                           |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_ESP:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 5002 - IBACOM_CONF_AES - aes conf                           |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_AES:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 5003 - IBACOM_CONF_NET - network conf                       |
    //    |______________________________________________________________________|
    //

            case IBACOM_CONF_NET:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 5025 - IBACOM_ACK - acknowledge                             |
    //    |______________________________________________________________________|
    //

            case IBACOM_ACK:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 5050 - IBACOM_NET_ADDRESS - network address                 |
    //    |______________________________________________________________________|
    //

            case IBACOM_NET_ADDRESS:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 6001 - IBACOM_STATUS_ESP - esp status                       |
    //    |______________________________________________________________________|
    //

            case IBACOM_STATUS_ESP:
            {
            #if defined (ARDUINO)
            #else
            #endif
                break;
            }

    #endif /* INDOORINO_NETWORK */
    
    #if defined (__linux__)

    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7001 - IBACOM_REQ_LOGIN - login request                     |
    //    |______________________________________________________________________|
    //

            case IBACOM_REQ_LOGIN:
            {
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7002 - IBACOM_SESSION_STAT - session status                 |
    //    |______________________________________________________________________|
    //

            case IBACOM_SESSION_STAT:
            {
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7003 - IBACOM_SESSION_END - session end                     |
    //    |______________________________________________________________________|
    //

            case IBACOM_SESSION_END:
            {
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7010 - IBACOM_SRV_REQ - server request                      |
    //    |______________________________________________________________________|
    //

            case IBACOM_SRV_REQ:
            {
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7012 - IBACOM_SRV_CONF - server config                      |
    //    |______________________________________________________________________|
    //

            case IBACOM_SRV_CONF:
            {
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7021 - IBACOM_SRV_BOARD - board data                        |
    //    |______________________________________________________________________|
    //

            case IBACOM_SRV_BOARD:
            {
                break;
            }


    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: 7022 - IBACOM_SRV_BOARD_CONN - board connection             |
    //    |______________________________________________________________________|
    //

            case IBACOM_SRV_BOARD_CONN:
            {
                break;
            }

    #endif /* __linux__ */

    //    ________________________________________________________________________
    //    |                                                                      |
    //    |    EVAL: DEFAULT                                                     |
    //    |______________________________________________________________________|
    //

            default:
            {
                return;
            }
        }
    }

}   /* namespace : packet */
