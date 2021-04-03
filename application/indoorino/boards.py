from common.comtable import *
from indoorino.parameters import *
from indoorino.devices import IndoorinoDevice


class BoardParameters:

    class _BoardParam:

        class _Entry(dict):
            def __init__(self, *args, **kwargs):
                dict.__init__(self, *args, **kwargs)
                self.packet=IndoorinoPacket()

        def __init__(self):
            self.std = self._Entry()
            self.board = self._Entry()

    def __init__(self):

        self.config = self._BoardParam()
        self.status = self._BoardParam()

        self.config.std.update(
            {
                'name': ParameterString(
                    name='name',
                    label='board name',
                    desc='board name',
                ),
                'type': ParameterString(
                    name='type',
                    label='board type',
                    desc='board type',
                ),
                'board': ParameterString(
                    name='board',
                    label='board model',
                    desc='board model',
                ),
                'group': ParameterString(
                    name='desc1',
                    label='resource group',
                    desc='resource group',
                ),
                'devnum': ParameterInt(
                    name='devnum',
                    label='board devices',
                    desc='attached devices',
                )
            }
        )
        self.status.std.update(
            {

                'boardtime': ParameterDatetime(
                    name='epoch',
                    label='board time',
                    desc='on board rtc datetime',
                ),
                'freeram': ParameterInt(
                    name='freemem',
                    label='available SRAM',
                    desc='heap size',
                    unit='KB'
                ),
                'looptime': ParameterInt(
                    name='looptime',
                    label='loop time',
                    desc='main loop time',
                    unit='us'
                ),
                'missing': ParameterInt(
                    name='errors1',
                    label='invalid packets',
                    desc='invalid parsed packets',
                ),
                'errors': ParameterInt(
                    name='errors2',
                    label='errors',
                    desc='generic errors',
                ),

            }
        )

    def set_board_type(self, packet):

        if packet.payload['type'] == 'ESPROUTER':

            self.config.board.packet.build(IBACOM_CONF_ESP, packet.payload['name'], dict())
            self.status.board.packet.build(IBACOM_STATUS_ESP, packet.payload['name'], dict())

            self.config.board.update(
                {
                    'local_address': ParameterAddress(
                        name='ip1',
                        label='local address',
                        desc='local board address',
                    ),
                    'remote_address': ParameterAddress(
                        name='ip2',
                        label='remote address',
                        desc='remote server address',
                    ),
                    'timeout_client': ParameterInt(
                        name='timeout1',
                        label='timeout client',
                        desc='timeout client',
                        unit='ms'
                    ),
                    'timeout_resend': ParameterInt(
                        name='timeout2',
                        label='resend packet time',
                        desc='resend packet time delta',
                        unit='ms'
                    ),
                    'attemps_resend': ParameterInt(
                        name='count1',
                        label='resend attemps',
                        desc='resend attemps count',
                    ),
                }
            )

            self.status.board.update(
                {

                    'serialtx': ParameterInt(
                        name='serialtx',
                        label='serial tx',
                        desc='transmitted (serial)',
                    ),
                    'serialrx': ParameterInt(
                        name='serialrx',
                        label='serial rx',
                        desc='received (serial)',
                    ),

                    'tcptx': ParameterInt(
                        name='tcptx',
                        label='tcp tx',
                        desc='transmitted (TCP)',
                    ),
                    'tcprx': ParameterInt(
                        name='tcprx',
                        label='tcp rx',
                        desc='received (TCP)',
                    ),

                    'totaltx': ParameterInt(
                        name='totaltx',
                        label='total tx',
                        desc='transmitted (total)',
                    ),
                    'totalrx': ParameterInt(
                        name='totalrx',
                        label='total rx',
                        desc='received (total)',
                    ),

                    'unmatched': ParameterInt(
                        name='errors1',
                        label='unmatched packets',
                        desc='unmatched packets (no ACK)',
                    ),
                    'unsent': ParameterInt(
                        name='errors2',
                        label='unsent packets',
                        desc='unsent packets',
                    ),
                    'discnns': ParameterInt(
                        name='errors3',
                        label='disconnections',
                        desc='disconnection counter',
                    ),
                    'overflow': ParameterInt(
                        name='errors4',
                        label='net overflow',
                        desc='network overflow',
                    )
                }
            )

        elif packet.payload['type'] == 'SAMPLER':
            self.config.board.packet.build(IBACOM_CONF_SAMPLER, packet.payload['name'], dict())
            self.status.board.packet.build(IBACOM_STATUS_SAMPLER, packet.payload['name'], dict())
            #TODO
        elif packet.payload['type'] == 'CONTROLLER':
            self.config.board.packet.build(IBACOM_CONF_STD, packet.payload['name'], dict())
            self.status.board.packet.build(IBACOM_STATUS_STD, packet.payload['name'], dict())
            #TODO

    def update(self):

        for key, item in self.config.std.items():
            item.set(self.config.std.packet.payload[item.name])

        for key, item in self.status.std.items():
            item.set(self.status.std.packet.payload[item.name])

        for key, item in self.config.board.items():
            try:
                item.set(self.config.board.packet.payload[item.name])
            except KeyError:
                if self.config.std.packet.payload['type'] and len(self.config.board.keys()) == 0:
                    self.set_board_type(self.config.std.packet.payload['type'])

        for key, item in self.status.board.items():
            item.set(self.status.board.packet.payload[item.name])

class IndoorinoBoard (BoardParameters):

    def __init__(self, name):

        BoardParameters.__init__(self)
        self._devs = dict()

        self._online = False
        self._timeout = time.perf_counter()
        self._error = 0
        self._boardtime = ClockBase(0)
        self._boardtime.start()

        self.config.std.packet.build(IBACOM_CONF_STD, name, dict())
        self.status.std.packet.build(IBACOM_STATUS_STD, name, dict())

        self.config.std.packet.payload['name'] = name

    def loop(self):
        if self._online and time.perf_counter() > self._timeout:
            self._online=False
            Config.flags.update.BOARD=True

        for dev in self._devs.values():
            dev.loop()

    def parse(self, packet):
        # if not isinstance(packet, IndoorinoPacket):
        #     return

        if isinstance(self.name, str) and packet.source != self.name:
            return

        self._timeout = time.perf_counter() + Config.macros.TIMEOUT_BOARD
        self._online=True

        if packet.command == IBACOM_CONF_STD:
            if not packet.payload['type'] == self.config.std.packet.payload['type']:
                self.set_board_type(packet)
            self.config.std.packet.payload = packet.payload
            self.update()
            return

        if packet.command == IBACOM_STATUS_STD:
            self.status.std.packet.payload = packet.payload
            self.update()
            return

        if IBACOM_CONF_STD < packet.command <= IBACOM_CONF_CAMERA:

            if self.config.board.packet.command == 0:
                info_boards('%s setting config.board to %s'.format(self.name, packet.label))
                self.config.board.packet.build(packet.command, packet.payload['name'], packet.payload)
            elif self.config.board.packet.command != packet.command:
                error_boards('illegal replace {} CONFIG type'.format(self.name))
                return
            self.config.board.packet.payload = packet.payload
            self.update()

            if packet.command == IBACOM_CONF_ESP:
                self.config.board['local_address'].ip = packet.payload['ip1']
                self.config.board['local_address'].port = packet.payload['port1']
                self.config.board['remote_address'].ip = packet.payload['ip2']
                self.config.board['remote_address'].port = packet.payload['port2']

            return

        if IBACOM_STATUS_STD < packet.command <= IBACOM_STATUS_CAMERA:

            if self.status.board.packet.command == 0:
                self.status.board.packet.build(packet.command, packet.payload['name'], packet.payload)
            elif self.status.board.packet.command != packet.command:
                error_boards('illegal replace {} STATUS type'.format(self.name))
                return
            self.status.board.packet.payload = packet.payload
            self.update()
            return

        if IBACOM_CONF_ASENSOR <= packet.command <= IBACOM_CONF_DEVSTD or \
            IBACOM_STATUS_ASENSOR <= packet.command <= IBACOM_STATUS_DEVSTD:

            name = packet.payload['devname']
            if not name in self._devs.keys() and IBACOM_CONF_ASENSOR <= packet.command <= IBACOM_CONF_DEVSTD:
                self._devs[name] = IndoorinoDevice(self.name, name, packet.payload['pin1'])
            try:
                self._devs[name].parse(packet)
                Config.flags.update.DEVICES = True
            except KeyError:
                pass

                #qui eventualmente fare else: send pack request conf

        ### if comand == probe
        ### if comand == ???

    def set_offline(self):
        self._online = False
        for dev in self._devs.values():
            dev.set_offline()

    def is_connected(self):
        return self._online

    @property
    def device(self):
        return dict(self._devs)

    @property
    def name(self):
        return self.config.std.packet.payload['name']

    @property
    def type(self):
        return self.config.std.packet.payload['type']

    @property
    def board(self):
        return self.config.std.packet.payload['board']

    @property
    def boardtime(self):
        self._boardtime.update()
        return self._boardtime.get()

    def remove_dev(self, name):
        if not name in self._devs.keys():
            warning_boards('{}:remove-dev:could not find device {}'.format(self.name, name))

        alert_boards('{}:Removed device {}'.format(self.name, self._devs.pop(name)))

print('Loaded indoorino.boards')
