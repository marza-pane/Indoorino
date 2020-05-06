
from utils.comtable import *
from connection.connectUtils import *


class BoardModel:

    def __init__(self, name, address):

        self._name = name
        self._address = address

        self._boardtype = 'UNKNOWN TYPE'
        self._boardname = 'UNKNOWN BOARD'
        self._epoch     = datetime.datetime.fromtimestamp(0)
        self._lag       = datetime.timedelta(seconds=0)
        self._freeram   = 0
        self._looptime  = 0
        self._devnum    = 0
        self._boarderrors = 0

    # @property
    # def isSerial(self):
    #     return self.address.isSerial()
    #

    @property
    def address(self):
        return self._address

    @property
    def name(self):
        return self._name

    @property
    def boardtype(self):
        return self._boardtype

    @property
    def boardname(self):
        return self._boardname

    @property
    def epoch(self):
        return self._boardname

    @property
    def freeram(self):
        return self._freeram

    @property
    def looptime(self):
        return self._looptime

    @property
    def boarderrors(self):
        return self._boarderrors

    @property
    def devnum(self):
        return self._devnum

    def parse(self, packet):

        if isinstance(packet, IndoorinoPacket):
            print(format_dict(packet.payload))
            if packet.command == IBACOM_CONF_STD:
                self._boardtype = packet.payload['type']
                self._boardname = packet.payload['board']
                self._devnum = packet.payload['devnum']
            if packet.command == IBACOM_STATUS_STD:
                self._epoch = datetime.datetime.fromtimestamp(packet.packetepoch)
                self._lag = datetime.datetime.fromtimestamp(packet.localepoch) - self._epoch
                self._looptime = packet.payload['looptime']
                self._freeram = packet.payload['freeram']