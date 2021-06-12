/*
 * packetStub.cpp
 *
 *  Created on: Apr 25, 2020
 *   Edited on: Jun 11, 2021 - 02:14:00
 *      Author: n00b
 *  This code is code-generated
 */

#ifndef PACKETSTUB_H_
#define PACKETSTUB_H_

#include "ipacket.h"

void error_packet_call (char * label)
{
	error_mem("FATAL: invalid call for packet %s!", label);
}

//    ________________________________________________________________________
//    |                                                                      |
//    |    payload shortcuts to variables                                    |
//    |______________________________________________________________________|
//

namespace packet
{
	char       * ipacket::p_message         (void)
	{
		switch(command())
		{
		case IBACOM_REPORT :
			return (char *) (payload());
		default:
			error_packet_call(label());
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
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_AES :
			return (char *) (payload());
		case IBACOM_CONF_NET :
			return (char *) (payload());
		case IBACOM_NET_ADDRESS :
			return (char *) (payload());
		case IBACOM_REQ_LOGIN :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SRV_REQ :
			return (char *) (payload());
		case IBACOM_SYS_REQ :
			return (char *) (payload());
		case IBACOM_LYT_CONF :
			return (char *) (payload());
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_name            (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_BOOT :
			return (char *) (payload());
		case IBACOM_REPORT :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_SET_DEVICE :
			return (char *) (payload());
		case IBACOM_SET_DEVNAME :
			return (char *) (payload());
		case IBACOM_SET_DEVPIN :
			return (char *) (payload());
		case IBACOM_REM_DEVICE :
			return (char *) (payload());
		case IBACOM_REQ_DEV_STATUS :
			return (char *) (payload());
		case IBACOM_REQ_DEV_RESET :
			return (char *) (payload());
		case IBACOM_CONF_STD :
			return (char *) (payload());
		case IBACOM_CONF_SAMPLER :
			return (char *) (payload());
		case IBACOM_CONF_ESP :
			return (char *) (payload());
		case IBACOM_CONF_CAMERA :
			return (char *) (payload());
		case IBACOM_CONF_ASENSOR :
			return (char *) (payload());
		case IBACOM_CONF_SWITCH :
			return (char *) (payload());
		case IBACOM_CONF_FLOODSWITCH :
			return (char *) (payload());
		case IBACOM_CONF_RAINSWITCH :
			return (char *) (payload());
		case IBACOM_CONF_LDR :
			return (char *) (payload());
		case IBACOM_CONF_DUSTPM25 :
			return (char *) (payload());
		case IBACOM_CONF_DHT22 :
			return (char *) (payload());
		case IBACOM_CONF_RELAY :
			return (char *) (payload());
		case IBACOM_CONF_DEVSTD :
			return (char *) (payload());
		case IBACOM_STATUS_STD :
			return (char *) (payload());
		case IBACOM_STATUS_SAMPLER :
			return (char *) (payload());
		case IBACOM_STATUS_ESP :
			return (char *) (payload());
		case IBACOM_STATUS_SD :
			return (char *) (payload());
		case IBACOM_STATUS_CAMERA :
			return (char *) (payload());
		case IBACOM_STATUS_ASENSOR :
			return (char *) (payload());
		case IBACOM_STATUS_SWITCH :
			return (char *) (payload());
		case IBACOM_STAT_FLOODSWITCH :
			return (char *) (payload());
		case IBACOM_STAT_RAINSWITCH :
			return (char *) (payload());
		case IBACOM_STATUS_LDR :
			return (char *) (payload());
		case IBACOM_STATUS_DUSTPM25 :
			return (char *) (payload());
		case IBACOM_STATUS_DHT22 :
			return (char *) (payload());
		case IBACOM_STATUS_RELAY :
			return (char *) (payload());
		case IBACOM_STATUS_DEVSTD :
			return (char *) (payload());
		case IBACOM_REQ_LOGIN :
			return (char *) (payload());
		case IBACOM_SESSION_STAT :
			return (char *) (payload());
		case IBACOM_SYS_BRD_CNF :
			return (char *) (payload());
		case IBACOM_SYS_BRD_STS :
			return (char *) (payload());
		case IBACOM_SYS_DEV_CNF :
			return (char *) (payload());
		case IBACOM_SYS_DEV_STS :
			return (char *) (payload());
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_type            (void)
	{
		switch(command())
		{
		case IBACOM_CONF_STD :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_GENERIC_ALARM :
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + sizeof(int32_t));
		case IBACOM_SYS_BRD_CNF :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SYS_DEV_CNF :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_LYT_DEVICE :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_LABEL + LEN_LABEL);
		case IBACOM_LYT_LIGHTS :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_LABEL);
		case IBACOM_LYT_WEATHER :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_LABEL);
		case IBACOM_LYT_ALARMS :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_LABEL);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_board           (void)
	{
		switch(command())
		{
		case IBACOM_CONF_STD :
			return (char *) (payload() + LEN_NAME + LEN_NAME);
		case IBACOM_HEAT_ALARM :
			return (char *) (payload());
		case IBACOM_OVERH_ALARM :
			return (char *) (payload());
		case IBACOM_FIRE_ALARM :
			return (char *) (payload());
		case IBACOM_FLOOD_ALARM :
			return (char *) (payload());
		case IBACOM_MOIST_ALARM :
			return (char *) (payload());
		case IBACOM_SMOG_ALARM :
			return (char *) (payload());
		case IBACOM_HAZARD_ALARM :
			return (char *) (payload());
		case IBACOM_SMOKE_ALARM :
			return (char *) (payload());
		case IBACOM_GRID_ALARM :
			return (char *) (payload());
		case IBACOM_GENERIC_ALARM :
			return (char *) (payload());
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload());
		case IBACOM_SYS_BRD_CNF :
			return (char *) (payload() + LEN_NAME + LEN_NAME);
		case IBACOM_SET_ENV_ALARM :
			return (char *) (payload() + LEN_DEVNAME);
		case IBACOM_ACK_ENV_ALARM :
			return (char *) (payload() + LEN_DEVNAME);
		case IBACOM_ALARM_DEVSTAT :
			return (char *) (payload() + LEN_DEVNAME);
		case IBACOM_LYT_CONF :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_LYT_DEVICE :
			return (char *) (payload() + LEN_DEVNAME);
		case IBACOM_LYT_LIGHTS :
			return (char *) (payload() + LEN_DEVNAME);
		case IBACOM_LYT_WEATHER :
			return (char *) (payload() + LEN_DEVNAME);
		case IBACOM_LYT_ALARMS :
			return (char *) (payload() + LEN_DEVNAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_label1          (void)
	{
		switch(command())
		{
		case IBACOM_ENV_ALARM :
			return (char *) (payload() + LEN_NAME + LEN_NAME);
		case IBACOM_LYT_DEVICE :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME);
		case IBACOM_LYT_LIGHTS :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME);
		case IBACOM_LYT_WEATHER :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME);
		case IBACOM_LYT_ALARMS :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_label2          (void)
	{
		switch(command())
		{
		case IBACOM_ENV_ALARM :
			return (char *) (payload() + LEN_NAME + LEN_NAME + LEN_LABEL);
		case IBACOM_LYT_DEVICE :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_LABEL);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_label3          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_label4          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_devnum          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_STD :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME + LEN_NAME);
		case IBACOM_SYS_BRD_CNF :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_devname         (void)
	{
		switch(command())
		{
		case IBACOM_SET_DEVICE :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SET_DEVNAME :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SET_DEVPIN :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_REM_DEVICE :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_REQ_DEV_STATUS :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_REQ_DEV_RESET :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_ASENSOR :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_SWITCH :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_FLOODSWITCH :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_RAINSWITCH :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_LDR :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_DUSTPM25 :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_DHT22 :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_RELAY :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_DEVSTD :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_SD :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_ASENSOR :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_SWITCH :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STAT_FLOODSWITCH :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STAT_RAINSWITCH :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_LDR :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_DUSTPM25 :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_DHT22 :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_RELAY :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_STATUS_DEVSTD :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_HEAT_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_OVERH_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_FIRE_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_FLOOD_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_MOIST_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SMOG_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_HAZARD_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SMOKE_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_GRID_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_GENERIC_ALARM :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_SET_ENV_ALARM :
			return (char *) (payload());
		case IBACOM_ACK_ENV_ALARM :
			return (char *) (payload());
		case IBACOM_ALARM_DEVSTAT :
			return (char *) (payload());
		case IBACOM_LYT_CONF :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_NAME);
		case IBACOM_LYT_DEVICE :
			return (char *) (payload());
		case IBACOM_LYT_LIGHTS :
			return (char *) (payload());
		case IBACOM_LYT_WEATHER :
			return (char *) (payload());
		case IBACOM_LYT_ALARMS :
			return (char *) (payload());
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_newname         (void)
	{
		switch(command())
		{
		case IBACOM_SET_DEVNAME :
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_desc1           (void)
	{
		switch(command())
		{
		case IBACOM_CONF_STD :
			return (char *) (payload() + LEN_NAME + LEN_NAME + LEN_NAME + sizeof(uint8_t));
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_ENV_ALARM :
			return (char *) (payload());
		case IBACOM_ALARM_DEVSTAT :
			return (char *) (payload() + LEN_DEVNAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_desc2           (void)
	{
		switch(command())
		{
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME);
		case IBACOM_ENV_ALARM :
			return (char *) (payload() + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_desc3           (void)
	{
		switch(command())
		{
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_desc4           (void)
	{
		switch(command())
		{
		case IBACOM_PROBE_AMBIENT :
			return (char *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME + LEN_NAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_epoch           (void)
	{
		switch(command())
		{
		case IBACOM_UNIX_EPOCH :
			return (uint32_t *) (payload());
		case IBACOM_PING :
			return (uint32_t *) (payload());
		case IBACOM_REPORT :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_NAME);
		case IBACOM_STATUS_STD :
			return (uint32_t *) (payload() + LEN_NAME);
		case IBACOM_HEAT_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_OVERH_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_FIRE_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_FLOOD_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_MOIST_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_SMOG_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_HAZARD_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_SMOKE_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_GRID_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_GENERIC_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_PROBE_AMBIENT :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_SESSION_STAT :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint8_t));
		case IBACOM_SET_ENV_ALARM :
			return (uint32_t *) (payload() + LEN_DEVNAME + LEN_NAME + sizeof(int32_t));
		case IBACOM_ACK_ENV_ALARM :
			return (uint32_t *) (payload() + LEN_DEVNAME + LEN_NAME);
		case IBACOM_ENV_ALARM :
			return (uint32_t *) (payload() + LEN_NAME + LEN_NAME + LEN_LABEL + LEN_LABEL);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_freemem         (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_VERSION :
			return (uint32_t *) (payload() + sizeof(int32_t) + sizeof(int32_t));
		case IBACOM_STATUS_STD :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
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
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_errors1         (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_STD :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_STATUS_ESP :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_errors2         (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_STD :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t));
		case IBACOM_STATUS_ESP :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_errors3         (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_STD :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t));
		case IBACOM_STATUS_ESP :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_errors4         (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_STD :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t));
		case IBACOM_STATUS_ESP :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_totalrx         (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_totaltx         (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value1          (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_BOOT :
			return (int32_t *) (payload() + LEN_NAME);
		case IBACOM_SET_DEVICE :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_STATUS_SAMPLER :
			return (int32_t *) (payload() + LEN_NAME + sizeof(uint32_t));
		case IBACOM_STATUS_ASENSOR :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_SWITCH :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STAT_FLOODSWITCH :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STAT_RAINSWITCH :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_LDR :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_DUSTPM25 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_HEAT_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_OVERH_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_FIRE_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_FLOOD_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_MOIST_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_SMOG_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_HAZARD_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_SMOKE_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_GRID_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_GENERIC_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t));
		case IBACOM_PROBE_AMBIENT :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME + LEN_NAME + LEN_NAME + LEN_NAME);
		case IBACOM_SYS_REQ :
			return (int32_t *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_SET_ENV_ALARM :
			return (int32_t *) (payload() + LEN_DEVNAME + LEN_NAME);
		case IBACOM_ENV_ALARM :
			return (int32_t *) (payload() + LEN_NAME + LEN_NAME + LEN_LABEL + LEN_LABEL + sizeof(uint32_t));
		case IBACOM_ALARM_DEVSTAT :
			return (int32_t *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value2          (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t));
		case IBACOM_PROBE_AMBIENT :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME + LEN_NAME + LEN_NAME + LEN_NAME + sizeof(int32_t));
		case IBACOM_SYS_REQ :
			return (int32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value3          (void)
	{
		switch(command())
		{
		case IBACOM_PROBE_AMBIENT :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME + LEN_NAME + LEN_NAME + LEN_NAME + sizeof(int32_t) + sizeof(int32_t));
		case IBACOM_SYS_REQ :
			return (int32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value4          (void)
	{
		switch(command())
		{
		case IBACOM_PROBE_AMBIENT :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint32_t) + LEN_NAME + LEN_NAME + LEN_NAME + LEN_NAME + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t));
		case IBACOM_SYS_REQ :
			return (int32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value5          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value6          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value7          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_value8          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param1          (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_VERSION :
			return (int32_t *) (payload());
		case IBACOM_CONF_ASENSOR :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_CONF_LDR :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_CONF_DUSTPM25 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_CONF_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		case IBACOM_STATUS_SD :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param2          (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_VERSION :
			return (int32_t *) (payload() + sizeof(int32_t));
		case IBACOM_CONF_ASENSOR :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t));
		case IBACOM_CONF_LDR :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t));
		case IBACOM_CONF_DUSTPM25 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t));
		case IBACOM_CONF_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t));
		case IBACOM_STATUS_SD :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param3          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_DUSTPM25 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t));
		case IBACOM_CONF_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param4          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param5          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_DHT22 :
			return (int32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param6          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param7          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	int32_t    * ipacket::p_param8          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stepday1        (void)
	{
		switch(command())
		{
		case IBACOM_CONF_SAMPLER :
			return (uint32_t *) (payload() + LEN_NAME);
		case IBACOM_STATUS_SAMPLER :
			return (uint32_t *) (payload() + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stepday2        (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stepday3        (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stepday4        (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stephour1       (void)
	{
		switch(command())
		{
		case IBACOM_CONF_SAMPLER :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stephour2       (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stephour3       (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_stephour4       (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_timeout1        (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_CONF_DUSTPM25 :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint8_t));
		case IBACOM_CONF_NET :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + INET_ADDRSTRLEN + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_timeout2        (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_CONF_DUSTPM25 :
			return (uint32_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t));
		case IBACOM_CONF_NET :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_timeout3        (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_timeout4        (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_level           (void)
	{
		switch(command())
		{
		case IBACOM_REPORT :
			return (uint8_t *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_NAME + sizeof(uint32_t));
		case IBACOM_STATUS_RELAY :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_status          (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ASENSOR :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_SWITCH :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STAT_FLOODSWITCH :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STAT_RAINSWITCH :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_LDR :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_DUSTPM25 :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_DHT22 :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_RELAY :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_DEVSTD :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_SESSION_STAT :
			return (uint8_t *) (payload() + LEN_NAME);
		case IBACOM_SRV_CONF :
			return (uint8_t *) (payload() + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t));
		case IBACOM_SYS_BRD_STS :
			return (uint8_t *) (payload() + LEN_NAME);
		case IBACOM_SYS_DEV_STS :
			return (uint8_t *) (payload() + LEN_NAME);
		case IBACOM_ENV_ALARM :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME + LEN_LABEL + LEN_LABEL + sizeof(uint32_t) + sizeof(int32_t));
		case IBACOM_ALARM_DEVSTAT :
			return (uint8_t *) (payload() + LEN_DEVNAME + LEN_NAME + LEN_NAME + sizeof(int32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count1          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint8_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count2          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint8_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count3          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint8_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count4          (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint8_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count5          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count6          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count7          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_count8          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_pin1            (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_VERSION :
			return (uint8_t *) (payload() + sizeof(int32_t) + sizeof(int32_t) + sizeof(uint32_t));
		case IBACOM_SET_DEVPIN :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_ASENSOR :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_SWITCH :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_FLOODSWITCH :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_RAINSWITCH :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_LDR :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_DUSTPM25 :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_DHT22 :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_RELAY :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_CONF_DEVSTD :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_STATUS_SD :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME);
		case IBACOM_SYS_DEV_CNF :
			return (uint8_t *) (payload() + LEN_NAME + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_pin2            (void)
	{
		switch(command())
		{
		case IBACOM_BOARD_VERSION :
			return (uint8_t *) (payload() + sizeof(int32_t) + sizeof(int32_t) + sizeof(uint32_t) + sizeof(uint8_t));
		case IBACOM_CONF_DUSTPM25 :
			return (uint8_t *) (payload() + LEN_NAME + LEN_DEVNAME + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_pin3            (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint8_t    * ipacket::p_pin4            (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_tcprx           (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_tcptx           (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t) + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_serialrx        (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + LEN_NAME + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_serialtx        (void)
	{
		switch(command())
		{
		case IBACOM_STATUS_ESP :
			return (uint32_t *) (payload() + LEN_NAME);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_ip1             (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (char *) (payload() + LEN_NAME);
		case IBACOM_CONF_NET :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_NET_ADDRESS :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		case IBACOM_SRV_CONF :
			return (char *) (payload());
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_ip2             (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (char *) (payload() + LEN_NAME + INET_ADDRSTRLEN);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_ip3             (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_ip4             (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_port1           (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN);
		case IBACOM_CONF_NET :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + INET_ADDRSTRLEN);
		case IBACOM_NET_ADDRESS :
			return (uint32_t *) (payload() + SERIAL_TX_BUFFER_SIZE + INET_ADDRSTRLEN);
		case IBACOM_SRV_CONF :
			return (uint32_t *) (payload() + INET_ADDRSTRLEN);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_port2           (void)
	{
		switch(command())
		{
		case IBACOM_CONF_ESP :
			return (uint32_t *) (payload() + LEN_NAME + INET_ADDRSTRLEN + INET_ADDRSTRLEN + sizeof(uint32_t));
		case IBACOM_SRV_CONF :
			return (uint32_t *) (payload() + INET_ADDRSTRLEN + sizeof(uint32_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_port3           (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	uint32_t   * ipacket::p_port4           (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

#if defined(INDOORINO_NETWORK)

	char       * ipacket::p_hash1           (void)
	{
		switch(command())
		{
		case IBACOM_ACK :
			return (char *) (payload());
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_hash2           (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_hash3           (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_hash4           (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_aeskey1         (void)
	{
		switch(command())
		{
		case IBACOM_CONF_AES :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_aeskey2         (void)
	{
		switch(command())
		{
		case IBACOM_CONF_AES :
			return (char *) (payload() + SERIAL_TX_BUFFER_SIZE + LEN_AES_MASTERKEY);
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

#endif /* INDOORINO_NETWORK */

#if defined (__linux__)

	char       * ipacket::p_table1          (void)
	{
		switch(command())
		{
		case IBACOM_SRV_CONF :
			return (char *) (payload() + INET_ADDRSTRLEN + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t));
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_table2          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_table3          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

	char       * ipacket::p_table4          (void)
	{
		switch(command())
		{
		default:
			error_packet_call(label());
			return nullptr;
		}
	}

#endif /* __linux__ */

} /* packet */

#endif  /* PACKETSTUB_H_ */

/*
Add the following headers to the <ipacket> class defintion

		char       *    p_message       (void);
		char       *    p_command       (void);
		char       *    p_name          (void);
		char       *    p_type          (void);
		char       *    p_board         (void);
		char       *    p_label1        (void);
		char       *    p_label2        (void);
		char       *    p_label3        (void);
		char       *    p_label4        (void);
		uint8_t    *    p_devnum        (void);
		char       *    p_devname       (void);
		char       *    p_newname       (void);
		char       *    p_desc1         (void);
		char       *    p_desc2         (void);
		char       *    p_desc3         (void);
		char       *    p_desc4         (void);
		uint32_t   *    p_epoch         (void);
		uint32_t   *    p_freemem       (void);
		uint32_t   *    p_looptime      (void);
		int32_t    *    p_errors1       (void);
		int32_t    *    p_errors2       (void);
		int32_t    *    p_errors3       (void);
		int32_t    *    p_errors4       (void);
		uint32_t   *    p_totalrx       (void);
		uint32_t   *    p_totaltx       (void);
		int32_t    *    p_value1        (void);
		int32_t    *    p_value2        (void);
		int32_t    *    p_value3        (void);
		int32_t    *    p_value4        (void);
		int32_t    *    p_value5        (void);
		int32_t    *    p_value6        (void);
		int32_t    *    p_value7        (void);
		int32_t    *    p_value8        (void);
		int32_t    *    p_param1        (void);
		int32_t    *    p_param2        (void);
		int32_t    *    p_param3        (void);
		int32_t    *    p_param4        (void);
		int32_t    *    p_param5        (void);
		int32_t    *    p_param6        (void);
		int32_t    *    p_param7        (void);
		int32_t    *    p_param8        (void);
		uint32_t   *    p_stepday1      (void);
		uint32_t   *    p_stepday2      (void);
		uint32_t   *    p_stepday3      (void);
		uint32_t   *    p_stepday4      (void);
		uint32_t   *    p_stephour1     (void);
		uint32_t   *    p_stephour2     (void);
		uint32_t   *    p_stephour3     (void);
		uint32_t   *    p_stephour4     (void);
		uint32_t   *    p_timeout1      (void);
		uint32_t   *    p_timeout2      (void);
		uint32_t   *    p_timeout3      (void);
		uint32_t   *    p_timeout4      (void);
		uint8_t    *    p_level         (void);
		uint8_t    *    p_status        (void);
		uint8_t    *    p_count1        (void);
		uint8_t    *    p_count2        (void);
		uint8_t    *    p_count3        (void);
		uint8_t    *    p_count4        (void);
		uint8_t    *    p_count5        (void);
		uint8_t    *    p_count6        (void);
		uint8_t    *    p_count7        (void);
		uint8_t    *    p_count8        (void);
		uint8_t    *    p_pin1          (void);
		uint8_t    *    p_pin2          (void);
		uint8_t    *    p_pin3          (void);
		uint8_t    *    p_pin4          (void);
		uint32_t   *    p_tcprx         (void);
		uint32_t   *    p_tcptx         (void);
		uint32_t   *    p_serialrx      (void);
		uint32_t   *    p_serialtx      (void);
		char       *    p_ip1           (void);
		char       *    p_ip2           (void);
		char       *    p_ip3           (void);
		char       *    p_ip4           (void);
		uint32_t   *    p_port1         (void);
		uint32_t   *    p_port2         (void);
		uint32_t   *    p_port3         (void);
		uint32_t   *    p_port4         (void);

#if defined(INDOORINO_NETWORK)

		char       *    p_hash1         (void);
		char       *    p_hash2         (void);
		char       *    p_hash3         (void);
		char       *    p_hash4         (void);
		char       *    p_aeskey1       (void);
		char       *    p_aeskey2       (void);

#endif // INDOORINO_NETWORK 

#if defined (__linux__)

		char       *    p_table1        (void);
		char       *    p_table2        (void);
		char       *    p_table3        (void);
		char       *    p_table4        (void);

#endif // __linux__

 */