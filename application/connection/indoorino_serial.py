from libraries.connectUtils import *
from serial.tools import list_ports_common


class SerialConnectionConfig(list_ports_common.ListPortInfo):

    def __init__(self, *arg):

        list_ports_common.ListPortInfo.__init__(self, *arg)
        self._baudrate   = 0
        self._options    = dict()

    @property
    def baudrate(self):
        return self._baudrate

    @baudrate.setter
    def baudrate(self, baudrate):
        if isinstance(baudrate, int):
            if not baudrate in BAUDRATES:
                debug('SerialConnectionConf: WARNING: baudrate {} not in list'.format(baudrate))
            self._baudrate = baudrate
        else:
            debug('SerialConnectionConf: WARNING: baudrate must be int() - {} given'.format(
                str(type(baudrate)).strip('class ').strip('\'')))

    @property
    def options(self):
        return self._options

    def option_rem(self, option):
        if option in self._options.keys():
            self._options.pop(option)

    def option_add(self, key, value):

        opts = ('bytesize',
                'parity',
                'stopbits',
                'timeout',
                'xonxoff',
                'rtscts',
                'write_timeout',
                'dsrdtr',
                'inter_byte_timeout',
                'exclusive',)

        if not key in opts:
            debug('SerialConnectionConf: ERROR: {} is not a valid Serial option'.format(value))
            return

        self._options[key]=value


class IndoorinoSerialReader(IndoorinoReceiverTemplate):

    def __init__(self):

        IndoorinoReceiverTemplate.__init__(self)

        self.config     =SerialConnectionConfig(DEFAULT_SERIAL_DEVICE_PATH)
        self.board      =serial.Serial()
        self.config.baudrate = DEFALT_SERIAL_BAUDRATE

    def connect(self):

        self._update = True
        if self._status:
            debug('Already connected to {}'.format(self.config.device))
            return True
        if not self.config.device:
            debug('No configuration for serial connection')
            return False
        print('Connecting to {}...'.format(self.config.device), end='')
        # self.board = serial.Serial(self.address, self.baud, xonxoff=False, write_timeout=1)
        # self._status_ = True
        # return True
        try:
            # TODO aggiungere le config.options
            self.board=serial.Serial(self.config.device, self.config.baudrate)
            self._status = True
            print('CONNECTED!')
            return True
        except (serial.serialutil.SerialException, OSError, TypeError):
            self._status = False
            print('FAILED CONNECTION!')
            return False

    def disconnect(self):

        self._update = True
        self._status = False
        self.board.close()

    def read(self, millis=0):

        if not self._status:
            return

        start = time.perf_counter()
        while True:
            try:
                while self.board.in_waiting > 0:

                    # Write in_waiting bytes from Serial to buffer
                    buffer = bytearray(self.board.read(self.board.in_waiting))

                    self._debug += format_debug(buffer)

                    # This string part of AVR flash command.
                    # So when found means the board is being written
                    if "AVRISP_2t" in buffer.decode(errors='ignore'):
                        self.disconnect()
                        self.board.flush()
                        return

                    # Pass incoming bytes to decoder in order to obtain packets

                    self._decoder.parse(buffer)
                    incoming=self._decoder.obtain()
                    # print('DEBUG PARSE: {}'.format(incoming))
                    if len(incoming) > 0:
                        debug('\nserial:read: receieved {} packet:\n{}\n'.format(len(incoming), format_dict(incoming[0])))
                        for i in incoming:
                            self._packets_received += 1
                            self._storage.append(IndoorinoPacket(i, BoardAddress(self.config.device)))

                        self._update = True

            except (OSError, TypeError):
                if self._status:
                    print('\nserial:read: IO ERROR\tdisconnecting')
                    self.disconnect()

               ## vv this prevents overflow vv      vv this check time for real ##
            if time.perf_counter() - start < 0 or time.perf_counter() > start + millis/1000:
                break

    def write(self, array):

        if self._status:
            debug('serial:write: sending {} bytes:\n {}'.format(len(array), format_bytearray(array)[1]))
            try:
                self.board.write(bytes(array))
                self.board.flushOutput()
                self.board.flush()
                self._packets_sent += 1
            except OSError:
                if self._status:
                    print('\nserial:write: IO ERROR\tdisconnecting')
                    self.disconnect()

    def send(self, packet):

        if isinstance(packet, (list, tuple)):
            for item in packet: self.send(item)

        elif isinstance(packet, IndoorinoPacket):
            self.send(packet.toIpacket())

        elif isinstance(packet, dict):
            try:
                print('Following dict will be sent:')
                print(format_dict(packet))
                iba = ipstools.forge(packet)
                self.write(iba)
            except:
                print('ERROR:serial:send: ipstools returned an error')
        else:
            print('WARNING:serial:send: invalid packet type ({})'.format(type(packet)))

    def listen(self, seconds):

        if self.status:
            start = time.perf_counter()
            while time.perf_counter() - start < seconds:

                self.read(5)
                s = self.storage
                print(self.debug, end='', flush=True)
                format_packet(s)

if __name__ == "__main__":

    c=ipsreader
    z = ipstools.forge(PACKET_REPORT_DATA)
    print(format_bytearray(z))
    c.parse(bytearray(z))
    print(c.obtain())

    R = IndoorinoSerialReader()

    if not R.connect():
        print('Can not connect to ' + DEFAULT_SERIAL_DEVICE_PATH)
    else:
        R.listen(5)
        R.write(ipstools.forge(PACKET_EPOCH_DATA))
        R.listen(5)
        R.write(ipstools.forge(PACKET_REPORT_DATA))
        R.listen(5)


