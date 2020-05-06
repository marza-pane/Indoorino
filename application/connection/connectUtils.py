
from utils.utils import *
from codesource.ibacomSource import SRC_packets


class BoardAddress:
    def __init__(self, address):

        self._path = None
        self._ip = None
        self._port = None

        if isinstance(address, BoardAddress):
            self._ip=address.ip
            self._port=address.port
            self._path=address.path
            self._serial=address.isSerial

        elif isinstance(address, (list, tuple)) and validate_address(address):
            self._ip=address[0]
            self._port=address[1]
            self._serial=False

        elif isinstance(address, str) and os.path.exists(address):
            self._serial=True
            self._path=address
        else:
            raise TypeError('Board address needs list, tuple or valid dev path. not {}:{}'.format(address, type(address)))

    @property
    def isSerial(self):
        return self._serial

    @property
    def path(self):
        if self._serial:
            return self._path
        return ''

    @property
    def ip(self):
        if not self._serial:
            return self._ip
        return ''

    @property
    def port(self):
        if not self._serial:
            return self._port
        return ''

    def __call__(self, *args, **kwargs):
        if self._serial:
            return self._path
        else:
            return (self._ip, self._port)

    def __str__(self):
        if self._serial:
            return str(self._path)
        else:
            return '{}:{}'.format(self._ip, self._port)

    def __getitem__(self, item):
        if self._serial:
            return self._path
        else:
            if item == 0:
                return self._ip
            elif item == 1:
                return self._port
            else:
                return ''

    def __format__(self, spec):
        return self.__str__()


class IndoorinoPacket:

    def __init__(self, payload, source):

        self._source   = BoardAddress(source)

        if validate_packet(payload):
            self._valid    = True
            self._payload  = payload['data']
            self._command  = payload['command']
            self._label    = payload['label']
            self._name     = payload['name']
            self._datasize = payload['data_size']
            self._epoch    = int(datetime.datetime.now().timestamp())
        else:
            self._valid    = False
            self._payload  = dict()
            self._epoch    = 0
            self._name     = 'invalid packet'
            self._label    = 'invalid packet'
            self._command  = 0
            self._datasize = 0
            for com in SRC_packets:
                if self._command == com[0]:
                    self._datasize =  com[2]

    def valid(self):
        return self._valid

    @property
    def         payload(self):
        return self._payload

    @property
    def         command(self):
        return self._command

    @property
    def         address(self):
        return self._source

    @property
    def         localepoch(self):
        return self._epoch

    @property
    def         name(self):
        return self._name

    @property
    def         label(self):
        return self._label

    @property
    def         size(self):
        return self._datasize

    @property
    def         packetepoch(self):
        try:
            return self._payload['epoch']
        except (TypeError, KeyError):
            print('Packet {} has no epoch field'.format(self.label))
            return 0

    def toIpacket(self):
        return {
            'name': self._name,
            'command': self._command,
            'label': self._label,
            'data': self._payload,
        }

