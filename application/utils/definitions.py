import datetime
import pathlib
import serial

#--------------------------------------------------------------#
#           Macros and global variables                        #
#--------------------------------------------------------------#

DEBUG_FLAG              = True
DEBUG_FLAG_GUI          = True

DEFAULT_FOLDER_NAME     = '.Indoorino'
DEFAULT_SUBFOLDER_NAME  = ('session', 'conf', 'database')
DEFAULT_FOLDER_PATH = pathlib.Path().home().joinpath(DEFAULT_FOLDER_NAME) # <-- dipende da sys.platform?

FIRST_EPOCH = 1262300400        # 1 Jan 2010
LAST_EPOCH = 4102444800         # 1 Jan 2100

FIRST_DATE = datetime.datetime.fromtimestamp(FIRST_EPOCH)
LAST_DATE = datetime.datetime.fromtimestamp(LAST_EPOCH)

DEFAULT_SERIAL_DEVICE_PATH      = '/dev/ttyUSB0'
DEFALT_SERIAL_BAUDRATE          = 115200
SERIAL_TIMEOUT                  = 0.001
BAUDRATES                       = tuple(sorted(list(serial.Serial.BAUDRATES) + [76800]))

SOCKET_TIMEOUT                  = 0.001
DEFAULT_UDP_PORT                = 7890

SIZE_MAX_PACKET_TX              =5000
SIZE_MAX_PACKET_RX              =5000