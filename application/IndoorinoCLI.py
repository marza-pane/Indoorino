import utils.IndoorinoModule
from utils.installer import build_setup
from utils.definitions import *
from utils.utils import format_packet, format_debug
from serial.tools import list_ports

import cProfile
import serial
import signal
import time
SERIAL_TRY_RECURSIVE_COUNT=0
def getSerialPorts():
    global SERIAL_TRY_RECURSIVE_COUNT
    try:
        portlist = list_ports.comports()
    except TypeError:
        if SERIAL_TRY_RECURSIVE_COUNT > 1000:
            print('ERROR:getSerialPorts: can not scan serial ports - IO error')
            SERIAL_TRY_RECURSIVE_COUNT=0
            return tuple()
        else:
            SERIAL_TRY_RECURSIVE_COUNT = SERIAL_TRY_RECURSIVE_COUNT + 1
            print(' !!! SERIAL MISS !!!')
            return getSerialPorts()

    SERIAL_TRY_RECURSIVE_COUNT=0
    returnlist=[]

    namelist = ['USB', 'ACM',]
    for port in portlist:
        for name in namelist:
           if name in port.device:
               returnlist.append(port)
               break

    return returnlist

class IndoorinoCLI_PacketListener:

    def __init__(self):
        self.serial_port_list = getSerialPorts()
        for port in self.serial_port_list:
            port.connection = serial.Serial()
            port.connection_status = False
            port.parser = utils.IndoorinoModule

    def connect(self):

        for port in self.serial_port_list:
            if not port.connection_status and not 'USB1' in port.device:
                print('Connecting to Serial port {}'.format(port.device), end='')
                try:
                    port.connection = serial.Serial(port.device, DEFALT_SERIAL_BAUDRATE)
                    print(' - ESTABLISHED')
                    port.connection_status = True
                except (serial.serialutil.SerialException, OSError, TypeError):
                    print(' - FAILED')

    def disconnect(self):
        for port in self.serial_port_list:
            if port.connection_status:
                print('Closing Serial port {}'.format(port.device), end='')
                try:
                    port.connection.close()
                    print(' - TERMINATED')
                except (serial.serialutil.SerialException, OSError, TypeError):
                    print(' - FAILED')
                    continue

    def read(self, millis=0):
        for port in self.serial_port_list:
            if port.connection_status:
                # start = time.perf_counter()
                # time.sleep(0.05)
                buffer = bytearray()
                try:
                    waiting_bytes = port.connection.in_waiting
                    if waiting_bytes > 0:
                        buffer = bytearray(port.connection.read(waiting_bytes))
                        # print('\nGot {} bytes from {}'.format(len(buffer), port.device), end='')

                except (serial.serialutil.SerialException, OSError, TypeError):
                    print('IOERROR while getting in_waiting bytes {}'.format(port.device))
                    port.connection_status = False
                    time.sleep(0.5) ## delay for reconnect try
                    continue

                if len(buffer) > 0:

                    # print(format_debug(buffer))

                    utils.IndoorinoModule.parse(buffer)
                    # time.sleep(0.1)
                    incoming = utils.IndoorinoModule.obtain()
                    for packet in incoming:
                        # print(' - receieved {} packet: '.format(len(incoming)), end='')
                        print(format_packet(packet))

    def loop(self):
        self.read()

if __name__ == '__main__':

    # build_setup()
    # time.sleep(1)
    # utils.IndoorinoModule.benchmark()
    print("\n\t *** Indoorino CLI listener START ***")

    listener = IndoorinoCLI_PacketListener()

    def keyboardInterruptHandler(sig, *args):
        print("\n\tKeyboardInterrupt (ID: {}) has been caught. Cleaning up...".format(sig))
        listener.disconnect()
        exit(0)

    listener.connect()
    signal.signal(signal.SIGINT, keyboardInterruptHandler)

    print('Now Reading...')
    while True:
        listener.read()


# def list_current_ports():
#     portlist = []
#     while True:
#         time.sleep(0.01)
#         currentlist = getSerialPorts()
#         if not len(portlist) == len(currentlist):
#             portlist = currentlist
#             print('\n\t AVAILABLE SERIAL PORTS:')
#             for port in portlist:
#                 print(port.device)


