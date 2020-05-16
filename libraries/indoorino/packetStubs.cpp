/*
 * packetStub.cpp
 *
 *  Created on: Apr 25, 2020
 *   Edited on: May 15, 2020 - 00:29:36
 *      Author: n00b
 */

#ifndef PACKETSTUB_H_
#define PACKETSTUB_H_

#include "packeTemplate.h"

//    ________________________________________________________________________
//    |                                                                      |
//    |    payload shortcuts to variables                                    |
//    |______________________________________________________________________|
//

char       * ipacket::p_message         (void)
{
	switch(command())
	{
		case IBACOM_REPORT :
			return (char *) (payload());
		default:
			return nullptr;
	}
}

char       * ipacket::p_command         (void)
{
	switch(command())
	{
		case IBACOM_COMMAND_STRING :
			return (char *) (payload());
		case IBACOM_SET_SD :
			return (char *) (payload());
		case IBACOM_SET_DEVICE :
			return (char *) (payload());
		case IBACOM_SET_ESP_ADDR :
			return (char *) (payload());
		default:
			return nullptr;
	}
}

char       * ipacket::p_name            (void)
{
	switch(command())
	{
		case IBACOM_REPORT :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_CONF_STD :
			return (char *) (payload());
		case IBACOM_STATUS_STD :
			return (char *) (payload());
		case IBACOM_STATUS_SAMPLER :
			return (char *) (payload());
		case IBACOM_STATUS_SD :
			return (char *) (payload());
		default:
			return nullptr;
	}
}

char       * ipacket::p_type            (void)
{
	switch(command())
	{
		case IBACOM_CONF_STD :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_ALLARM :
			return (char *) (payload());
		case IBACOM_WARNINGS :
			return (char *) (payload());
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload() + LEN_DEVNAME);
		default:
			return nullptr;
	}
}

char       * ipacket::p_board           (void)
{
	switch(command())
	{
		case IBACOM_CONF_STD :
			return (char *) (payload() + LEN_NAME + LEN_NAME);
		case IBACOM_ALLARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_WARNINGS :
			return (char *) (payload() + LEN_NAME);
		default:
			return nullptr;
	}
}

char       * ipacket::p_devname         (void)
{
	switch(command())
	{
		case IBACOM_SET_DEVICE :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_CONF_ASENSOR :
			return (char *) (payload());
		case IBACOM_CONF_SWITCH :
			return (char *) (payload());
		case IBACOM_CONF_LDR :
			return (char *) (payload());
		case IBACOM_CONF_DHT22 :
			return (char *) (payload());
		case IBACOM_CONF_RELAY :
			return (char *) (payload());
		case IBACOM_CONF_DEVSTD :
			return (char *) (payload());
		case IBACOM_STATUS_ASENSOR :
			return (char *) (payload());
		case IBACOM_STATUS_SWITCH :
			return (char *) (payload());
		case IBACOM_STATUS_LDR :
			return (char *) (payload());
		case IBACOM_STATUS_DHT22 :
			return (char *) (payload());
		case IBACOM_STATUS_RELAY :
			return (char *) (payload());
		case IBACOM_STATUS_DEVSTD :
			return (char *) (payload());
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload());
		case IBACOM_DOORBELL :
			return (char *) (payload());
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_epoch           (void)
{
	switch(command())
	{
		case IBACOM_UNIX_EPOCH :
			return (uint32_t *) (payload());
		case IBACOM_REPORT :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_NAME);
		case IBACOM_STATUS_STD :
			return (uint32_t *) (payload() + LEN_NAME);
		case IBACOM_ALLARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_NAME);
		case IBACOM_WARNINGS :
			return (uint32_t *) (payload() + LEN_NAME + LEN_NAME);
		case IBACOM_PROBE_AMBIENT :
			return (uint32_t *) (payload() + LEN_DEVNAME + LEN_NAME);
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_value           (void)
{
	switch(command())
	{
		case IBACOM_ACK :
			return (uint32_t *) (payload());
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload());
		case IBACOM_STATUS_SAMPLER :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t));
		case IBACOM_STATUS_ASENSOR :
			return (uint32_t *) (payload() + LEN_DEVNAME);
		case IBACOM_STATUS_SWITCH :
			return (uint32_t *) (payload() + LEN_DEVNAME);
		case IBACOM_STATUS_LDR :
			return (uint32_t *) (payload() + LEN_DEVNAME);
		case IBACOM_PROBE_AMBIENT :
			return (uint32_t *) (payload() + LEN_DEVNAME + LEN_NAME + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_freemem         (void)
{
	switch(command())
	{
		case IBACOM_STATUS_STD :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_STATUS_SD :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint8_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_looptime        (void)
{
	switch(command())
	{
		case IBACOM_STATUS_STD :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_errors          (void)
{
	switch(command())
	{
		case IBACOM_STATUS_STD :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_totalrx         (void)
{
	switch(command())
	{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_totaltx         (void)
{
	switch(command())
	{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_serialrx        (void)
{
	switch(command())
	{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_serialtx        (void)
{
	switch(command())
	{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_param1          (void)
{
	switch(command())
	{
		case IBACOM_CONF_ASENSOR :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_CONF_LDR :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_CONF_DHT22 :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_SD :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint8_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_param2          (void)
{
	switch(command())
	{
		case IBACOM_CONF_ASENSOR :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint32_t));
		case IBACOM_CONF_LDR :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint32_t));
		case IBACOM_CONF_DHT22 :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_param3          (void)
{
	switch(command())
	{
		case IBACOM_CONF_DHT22 :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_param4          (void)
{
	switch(command())
	{
		case IBACOM_CONF_DHT22 :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_temperature     (void)
{
	switch(command())
	{
		case IBACOM_STATUS_DHT22 :
			return (uint32_t *) (payload() + LEN_DEVNAME);
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_humidity        (void)
{
	switch(command())
	{
		case IBACOM_STATUS_DHT22 :
			return (uint32_t *) (payload() + LEN_DEVNAME + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_stepday1        (void)
{
	switch(command())
	{
		case IBACOM_CONF_SAMPLER :
			return (uint32_t *) (payload());
		case IBACOM_STATUS_SAMPLER :
			return (uint32_t *) (payload() + LEN_NAME);
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_stephour1       (void)
{
	switch(command())
	{
		case IBACOM_CONF_SAMPLER :
			return (uint32_t *) (payload() + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_timeout         (void)
{
	switch(command())
	{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_IPADDRESS + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint32_t   * ipacket::p_port            (void)
{
	switch(command())
	{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_IPADDRESS);
		case IBACOM_SET_ESP_ADDR :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_IPADDRESS);
		case IBACOM_NET_ADDRESS :
			return (uint32_t *) (payload() + LEN_IPADDRESS);
		default:
			return nullptr;
	}
}

uint8_t    * ipacket::p_level           (void)
{
	switch(command())
	{
		case IBACOM_REPORT :
			return (uint8_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_NAME + sizeof(uint32_t));
		case IBACOM_SET_DEVICE :
			return (uint8_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_RELAY :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_ALLARM :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME + sizeof(uint32_t));
		case IBACOM_WARNINGS :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME + sizeof(uint32_t));
		default:
			return nullptr;
	}
}

uint8_t    * ipacket::p_status          (void)
{
	switch(command())
	{
		case IBACOM_CONF_ESP :
			return (uint8_t *) (payload() + LEN_IPADDRESS + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_CONF_RELAY :
			return (uint8_t *) (payload() + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_ASENSOR :
			return (uint8_t *) (payload() + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_STATUS_SWITCH :
			return (uint8_t *) (payload() + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_STATUS_LDR :
			return (uint8_t *) (payload() + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_STATUS_DHT22 :
			return (uint8_t *) (payload() + LEN_DEVNAME + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_STATUS_DEVSTD :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		default:
			return nullptr;
	}
}

uint8_t    * ipacket::p_devnum          (void)
{
	switch(command())
	{
		case IBACOM_CONF_STD :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME + LEN_NAME);
		default:
			return nullptr;
	}
}

uint8_t    * ipacket::p_pin             (void)
{
	switch(command())
	{
		case IBACOM_SET_DEVICE :
			return (uint8_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_DEVNAME);
		case IBACOM_CONF_ASENSOR :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_CONF_SWITCH :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_CONF_LDR :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_CONF_DHT22 :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_CONF_RELAY :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_CONF_DEVSTD :
			return (uint8_t *) (payload() + LEN_DEVNAME);
		case IBACOM_STATUS_SD :
			return (uint8_t *) (payload() + LEN_NAME);
		default:
			return nullptr;
	}
}

char       * ipacket::p_ip              (void)
{
	switch(command())
	{
		case IBACOM_CONF_ESP :
			return (char *) (payload());
		case IBACOM_SET_ESP_ADDR :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_NET_ADDRESS :
			return (char *) (payload());
		default:
			return nullptr;
	}
}


#endif  /* PACKETSTUB_H_ */

/*
Add the following headers to the <ipacket> class defintion

	char           *    p_message       (void);
	char           *    p_command       (void);
	char           *    p_name          (void);
	char           *    p_type          (void);
	char           *    p_board         (void);
	char           *    p_devname       (void);
	uint32_t       *    p_epoch         (void);
	uint32_t       *    p_value         (void);
	uint32_t       *    p_freemem       (void);
	uint32_t       *    p_looptime      (void);
	uint32_t       *    p_errors        (void);
	uint32_t       *    p_totalrx       (void);
	uint32_t       *    p_totaltx       (void);
	uint32_t       *    p_serialrx      (void);
	uint32_t       *    p_serialtx      (void);
	uint32_t       *    p_param1        (void);
	uint32_t       *    p_param2        (void);
	uint32_t       *    p_param3        (void);
	uint32_t       *    p_param4        (void);
	uint32_t       *    p_temperature   (void);
	uint32_t       *    p_humidity      (void);
	uint32_t       *    p_stepday1      (void);
	uint32_t       *    p_stephour1     (void);
	uint32_t       *    p_timeout       (void);
	uint32_t       *    p_port          (void);
	uint8_t        *    p_level         (void);
	uint8_t        *    p_status        (void);
	uint8_t        *    p_devnum        (void);
	uint8_t        *    p_pin           (void);
	char           *    p_ip            (void);

 */