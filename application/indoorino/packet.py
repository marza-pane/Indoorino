from codesource.ibacomSource import SRC_packets
from utils.utils import *
import datetime

"""
 TODO: REVIEW THIS PLEASE!
"""

class PacketTemplate:

    def __init__(self, name, command, label, data):

        self._name      = name
        self._command   = command
        self._label     = label
        self._payload   = data
        self._valid     = True

        self._datasize = 0
        self._epoch = int(datetime.datetime.now().timestamp())

        # for com in SRC_packets:
        #     if self._command == com[0]:
        #         self._valid = True
        #         self._label =  com[2]

    def valid(self):
        return self._valid

    @property
    def payload(self):
        return self._payload

    @property
    def command(self):
        return self._command

    @property
    def name(self):
        return self._name

    @property
    def label(self):
        return self._label

    @property
    def size(self):
        return self._datasize

    @property
    def localepoch(self):
        return self._epoch


    @property
    def packetepoch(self):
        try:
            return self._payload['epoch']
        except (TypeError, KeyError):
            print('Packet {} has no epoch field'.format(self.label))
            return 0

class IndoorinoPacket(PacketTemplate):

    def __init__(self, packet):

        if not isinstance(packet, dict):
            # error_packet('IndoorinoPacket.init: cant find field {}'.format(key))
            pass

        elif isinstance(packet, int):
            pass

        packet_trans = list()
        empty_packet = {
            'name' : 'UNKNOWN',
            'command':0,
            'label': 'unknown packet',
            'data': {},
            'data_size': 0
        }

        for key, item in empty_packet.items():
            if key not in packet.keys():
                error_packet('IndoorinoPacket: cant find field {}'.format(key))
                packet_trans.append(item)
            else:
                packet_trans.append(packet[key])


        PacketTemplate.__init__(self,)


    # def toIpacket(self):
    #     return {
    #         'name': self._name,
    #         'command': self._command,
    #         'label': self._label,
    #         'data': self._payload,
    #     }

