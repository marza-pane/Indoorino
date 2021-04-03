

# Automatically generated code by application/codesource/generator.py
#    ________________________________________________________________________
#    |                                                                      |
#    |    IBACOM - command numbers    list                                  |
#    |______________________________________________________________________|
#

TOTAL_IBACOM = 50



	# --- # [ 00000 - 00500 ] : system command  # --- #

IBACOM_NONE                         = 0
IBACOM_INIT                         = 1
IBACOM_RESET                        = 2
IBACOM_REQUEST_CONFIG               = 3
IBACOM_REQUEST_STATUS               = 4
IBACOM_COMMAND_STRING               = 5
IBACOM_FACTORY_RESET                = 6
IBACOM_UNIX_EPOCH                   = 10
IBACOM_REPORT                       = 15
IBACOM_SET_SD                       = 301
IBACOM_REQUEST_SDDROP               = 302
IBACOM_REQUEST_SDCLEAR              = 303
IBACOM_REQUEST_SDWIPE               = 304

	# --- # [ 00501 - 01000 ] : devices command # --- #

IBACOM_SET_DEVICE                   = 501
IBACOM_SET_DEVNAME                  = 502
IBACOM_SET_DEVPIN                   = 503
IBACOM_REM_DEVICE                   = 504

	# --- # [ 01001 - 01300 ] : board config    # --- #

IBACOM_CONF_STD                     = 1001
IBACOM_CONF_SAMPLER                 = 1005
IBACOM_CONF_ESP                     = 1100
IBACOM_CONF_CAMERA                  = 1300

	# --- # [ 01301 - 02000 ] : device config   # --- #

IBACOM_CONF_ASENSOR                 = 1301
IBACOM_CONF_SWITCH                  = 1302
IBACOM_CONF_LDR                     = 1310
IBACOM_CONF_DHT22                   = 1522
IBACOM_CONF_RELAY                   = 1700
IBACOM_CONF_DEVSTD                  = 2000

	# --- # [ 02001 - 02300 ] : board status    # --- #

IBACOM_STATUS_STD                   = 2001
IBACOM_STATUS_SAMPLER               = 2005
IBACOM_STATUS_ESP                   = 2100
IBACOM_STATUS_SD                    = 2270
IBACOM_STATUS_CAMERA                = 2300

	# --- # [ 02301 - 03000 ] : device status   # --- #

IBACOM_STATUS_ASENSOR               = 2301
IBACOM_STATUS_SWITCH                = 2302
IBACOM_STATUS_LDR                   = 2310
IBACOM_STATUS_DHT22                 = 2522
IBACOM_STATUS_RELAY                 = 2700
IBACOM_STATUS_DEVSTD                = 3000

	# --- # [ 03001 - 03200 ] : alarms          # --- #

IBACOM_ALLARM                       = 3001
IBACOM_WARNINGS                     = 3002
IBACOM_INFOREP                      = 3003

	# --- # [ 03201 - 03500 ] : requests        # --- #

IBACOM_REQ_PROBE                    = 3205
IBACOM_REQ_DEV_STATUS               = 3210
IBACOM_REQ_DEV_RESET                = 3212
IBACOM_REQUEST_PROBE                = 3205

	# --- # [ 03501 - 04000 ] : payloads        # --- #

IBACOM_PROBE_AMBIENT                = 3505

	# --- # [ 05001 - 07000 ] : network         # --- #

IBACOM_CONF_AES                     = 5002
IBACOM_CONF_NET                     = 5003
IBACOM_ACK                          = 5025
IBACOM_NET_ADDRESS                  = 5050

	# --- # [ 07001 - 08000 ] : client comunication # --- #

IBACOM_REQ_LOGIN                    = 7001
IBACOM_SESSION_STAT                 = 7002
IBACOM_SESSION_END                  = 7003
IBACOM_SRV_REQ                      = 7010
IBACOM_SRV_CONF                     = 7012
IBACOM_SRV_BOARD                    = 7021
IBACOM_SRV_BOARD_CONN               = 7022