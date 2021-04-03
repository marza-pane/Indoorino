import random
from common.utils import *

class IndoorinoPacket:

    class Payload(dict):

        def __init__(self, *kwargs):
            dict.__init__(self, *kwargs)

        def __getitem__(self, key):
            if not key in self.keys():
                error_packet('Can not find {} in payload'.format(key))
                return 0
            return dict.__getitem__(self, key)

    def __init__(self, **kwargs):
        self.___tx_repr = True

        self._command   =kwargs.pop('command', 0)
        self._source    =kwargs.pop('source', Config.username)
        self._target    =kwargs.pop('target', 'SERVER')
        self._label     =kwargs.pop('label', '???')
        self._datasize  =kwargs.pop('datasize', 0)
        self._payload   =self.Payload(kwargs.pop('data', dict()))
        self._id        =random.randint(0, 350000)

        self._epoch=datetime.datetime.now()
        self._valid=False
        self._checksum=bytearray()

    def __eq__(self, other):
        if isinstance(other, type(self)):
            if self._id == other._id and \
                    self._command == other._command and \
                    self._payload == other._payload and \
                    self._epoch == other._epoch:
                return True
        return False

    def __str__(self):

        code = '\n'
        template = '___{versus}____packet____:_{label}___\n|  SOURCE : {source}|\n|  TARGET : {target}|\n|    DATA :'

        label = '{0:<64}'.format('{} id:{} ({} Bytes)'.format(
            self._label,
            self._id,
            self._datasize)).replace(' ', '_')

        source = '{0:<76}'.format(self._source)
        target = '{0:<76}'.format(self._target)
        if self.___tx_repr:
            direction='<<<'
        else:
            direction='>>>'
        code += template.format(versus=direction, label=label, source=source, target=target)

        code += '{0:<77}|'.format('')
        for item in self._payload.items():
            if item[0] == 'epoch':
                code += '{0:<89}|'.format('\n|          {}: {} ({})'.format(
                    item[0],
                    item[1],
                    datetime.datetime.fromtimestamp(item[1])))
            else:
                code += '{0:<89}|'.format('\n|          {}: {}'.format(item[0], item[1]))
        code += ('\n|{0:<80}_<<<___|'.format('')).replace(' ', '_')
        return code

    def __repr__(self):
        return self.description + '{}'.format(self.payload)

    def __call__(self, *args, **kwargs):
        return self._payload

    def dictionary(self):
        packet={
            'source': self._source,
            'target': self._target,
            'command': self._command,
            'data': dict(self._payload),
        }
        Client.ipacket(packet)
        packet['epoch']= self._epoch
        return packet

    def build(self, command, target, data):

        d = {
            'command':command,
            'source':Config.username,
            'target':target,
            'data':data
        }
        Client.ipacket(d)
        self.from_client(d)
        self.___tx_repr = False

    def from_client(self, packet):

        if not isinstance(packet, dict):
            error_packet('Can not init packet with {}'.format(type(packet)))
            return False

        if not len(set(packet.keys()).intersection(['source','target','command'])) == 3:
            error_packet('Invalid initializer dict: {}'.format(format_dict(packet)))

        self._payload = self.Payload( packet.pop('data', dict()) )
        self._target = packet.pop('target', '???')
        self._source = packet.pop('source', Config.username)
        self._command =packet.pop('command',0)

        self._label     =packet['label']
        self._datasize  =packet['data_size']
        self._checksum  =packet.pop('checksum', bytearray())

        if 'pid' in packet.keys():
            self._id = packet['pid']

        return True

    @property
    def pid(self):
        return self._id

    @property
    def source(self):
        return str(self._source)

    @property
    def target(self):
        return str(self._target)

    @property
    def label(self):
        return str(self._label)

    @target.setter
    def target(self, value):
        if Client.isBoardName(value):
            self._target=str(value)
            return
        warning_packet('invalid name <{}> for packet {}'.format(value, self._label))

    @property
    def command(self):
        return int(self._command)

    @property
    def data_size(self):
        return self._datasize

    @property
    def payload(self):
        return dict(self._payload)

    @payload.setter
    def payload(self, value):
        if isinstance(value, dict):
            if self._payload.keys() == value.keys():
                self._payload = self.Payload(value)
                return
            error_packet('Illegal payload overwrite of packet {}'.format(self.description))
            return
        error_packet('Invalid payload assignment type {}'.format(format_type(value)))
        return

    @property
    def valid(self):
        # if client.isBoardName(self._name) and self._valid:
        #     return True
        return False

    @property
    def localtime(self):
        return self._epoch

    @property
    def boardtime(self):
        return datetime.datetime.fromtimestamp(self._payload['epoch'])

    @property
    def description(self):
        return '{}:{}:{}:{}'.format(self._source, self._target, self._id, self._label)



print('Loaded indoorino.packets')
