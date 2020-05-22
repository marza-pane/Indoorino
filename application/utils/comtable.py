

# This code has been written by Python/Generator.py script
#    ________________________________________________________________________
#    |                                                                      |
#    |    IBACOM - command numbers    list                                  |
#    |______________________________________________________________________|
#

TOTAL_IBACOM = 38



	# --- #    [00000  - 00300 ] : system command # --- #

IBACOM_NONE                         = 0
IBACOM_INIT                         = 1
IBACOM_RESET                        = 2
IBACOM_REQUEST_CONFIG               = 3
IBACOM_REQUEST_STATUS               = 4
IBACOM_COMMAND_STRING               = 5
IBACOM_UNIX_EPOCH                   = 10
IBACOM_REPORT                       = 15
IBACOM_ACK                          = 25
IBACOM_RESET_CONFIG                 = 30

	# --- #    [00301  - 00500 ] : general command # --- #

IBACOM_SET_SD                       = 301

	# --- #    [00501  - 01000 ] : devices command # --- #

IBACOM_SET_DEVICE                   = 501

	# --- #    [01001  - 01300 ] : board config # --- #

IBACOM_CONF_STD                     = 1001
IBACOM_CONF_ESP                     = 1003
IBACOM_CONF_SAMPLER                 = 1005

	# --- #    [01301  - 02000 ] : device config # --- #

IBACOM_CONF_ASENSOR                 = 1301
IBACOM_CONF_SWITCH                  = 1302
IBACOM_CONF_LDR                     = 1310
IBACOM_CONF_DHT22                   = 1522
IBACOM_CONF_RELAY                   = 1700
IBACOM_CONF_DEVSTD                  = 2000

	# --- #    [02001  - 02300 ] : board status # --- #

IBACOM_STATUS_STD                   = 2001
IBACOM_STATUS_ESP                   = 2003
IBACOM_STATUS_SAMPLER               = 2005

	# --- #    [02301  - 03000 ] : device status # --- #

IBACOM_STATUS_ASENSOR               = 2301
IBACOM_STATUS_SWITCH                = 2302
IBACOM_STATUS_LDR                   = 2310
IBACOM_STATUS_DHT22                 = 2522
IBACOM_STATUS_RELAY                 = 2700
IBACOM_STATUS_SD                    = 2880
IBACOM_STATUS_DEVSTD                = 3000

	# --- #    [03001  - 03200 ] : alarms # --- #

IBACOM_ALLARM                       = 3001
IBACOM_WARNINGS                     = 3002

	# --- #    [03201  - 03500 ] : requests # --- #

IBACOM_REQUEST_PROBE                = 3205

	# --- #    [03501  - 04000 ] : payloads # --- #

IBACOM_PROBE_AMBIENT                = 3505

	# --- #    [05001  - 06000 ] : network # --- #

IBACOM_SET_ADDR                     = 5001
IBACOM_ESP_ADDRESS                  = 5002
IBACOM_DOORBELL                     = 65530