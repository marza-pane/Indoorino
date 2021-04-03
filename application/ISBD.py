
from common.utils import *
from common.comtable import *
from indoorino.packet import IndoorinoPacket
import serial
from serial.tools import list_ports_common

DEFAULT_SERIAL_DEVICE_PATH      = '/dev/ttyUSB2'
DEFALT_SERIAL_BAUDRATE          = 115200

N_BLOCK=16
LEN_NAME = 24
SIZEOF_PREAMBLE = 4
SIZEOF_PACKET_HEADER = SIZEOF_PREAMBLE + 6 + 2 * LEN_NAME
SIZEOF_NET_HEADER = SIZEOF_PREAMBLE + 2 + N_BLOCK

class IndoorinoSerialBoardDebugger:

    def __init__(self):
        self._status=False
        self._connection=serial.Serial()
        self._socket=DEFAULT_SERIAL_DEVICE_PATH
        self._baudrate=DEFALT_SERIAL_BAUDRATE

    def start(self):

        if not self._status:
            try:
                self._connection = serial.Serial(self._socket, self._baudrate)
                self._status = True
                info_os('Connected to {} [BAUD {}]!'.format(self._socket, self._baudrate))
                return True
            except (serial.serialutil.SerialException, OSError, TypeError) as error:
                self._status = False
                error_os('Could not connect to {}\n\t{}'.format(self._socket, error))
                return False

    def stop(self):
        self._status = False
        try:
            info_os('Serial connection closed')
            self._connection.close()
        except (serial.serialutil.SerialException, OSError, TypeError) as error:
            error_os('Could not close serial port!\n\t{}'.format(error))

    def read(self):

        if self._connection.in_waiting > 0:
            first = self._connection.read(1)
            if int.from_bytes(first, 'big') == 255:
                preamble = self._connection.read(SIZEOF_PREAMBLE - 1)
                if [x for x in preamble] == [ 1, 1, 255]:
                    # print('preamble pass! {} {}'.format(first, preamble))
                    buffer = self._connection.read(SIZEOF_PACKET_HEADER - SIZEOF_PREAMBLE)

                    command = int.from_bytes(buffer[0:2], 'little')
                    source = str(buffer[2:LEN_NAME + 2].decode())
                    target = str(buffer[LEN_NAME + 2:2 * LEN_NAME + 2].decode())
                    pid = int.from_bytes(buffer[2 * LEN_NAME + 2:SIZEOF_PACKET_HEADER], 'little')

                    # print('command = {}\nsource = {}\ntarget = {}\npid = {}'.format(command, source, target, pid))

                    p=dict()
                    packet = IndoorinoPacket()
                    packet.build(command, target, p)

                    payload = self._connection.read(packet.data_size)
                    # print(payload)
                    data = first + preamble + buffer + payload
                    Client.fromSerial(p, data)
                    # print(format_dict(p))
                    packet.from_client(p)

                    print(packet)

    def send(self, packet):

        data = b'\xff\x01\x01\xff'
        data += int(packet.command).to_bytes(2, 'little')
        data += bytes(str(packet.source).encode())
        data += max(LEN_NAME - len(packet.source), 0) * b'\x00'
        data += bytes(str(packet.target).encode())
        data += max(LEN_NAME - len(packet.target), 0) * b'\x00'
        data += int(packet.pid).to_bytes(4, 'little')

        self._connection.write(data)

        # while self._connection.in_waiting > SIZEOF_PREAMBLE:
        #     buffer = self._connection.read(self._connection.in_waiting)
        #     print('Serial.serial.read(N) returns type {}'.format(format_type(buffer)))
        #     try:
        #         print(buffer.decode())
        #     except UnicodeDecodeError:
        #         print('undecodeable char!')

        # if self._connection.in_waiting > SIZEOF_PACKET_HEADER:
        #     buffer = self._connection.read(SIZEOF_PACKET_HEADER)



if __name__ == '__main__':

    app = IndoorinoSerialBoardDebugger()

    app.start()

    timeout = time.perf_counter() + 20
    flag = False
    while time.perf_counter() < timeout:
        app.read()
        if time.perf_counter() > (timeout - 10) and not flag:
            flag=True
            p = IndoorinoPacket()
            p.build(IBACOM_CONF_ESP, 'HOUSE.BEAMS', dict())
            app.send(p)
            time.sleep(0.1)
            app.send(p)



    app.stop()