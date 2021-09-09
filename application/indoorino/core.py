import pickle

from common.utils import *
from common.comtable import *
from indoorino.packet import IndoorinoPacket
from indoorino.boards import IndoorinoBoard


class PacketIo:

    def __init__(self):
        self.status_connection = False
        self._buffer = list()
        self._rx = list()
        self._tx = list()
        self._is_connected = False
        self._is_online = False
        self._timeout_reconnect = 0.0
        self._run = False

    def clear_tx(self):
        self._tx.clear()

    def clear_rx(self):
        self._rx.clear()

    def rx_packets(self):
        return tuple(self._rx)

    def tx_packets(self):
        return tuple(self._tx)

    def is_connected(self):
        # return True
        return self._is_connected

    def is_online(self):
        # return True
        return self._is_online

    def begin(self):
        if self._start_client():
            self._run = True

    def _start_client(self):
        if not Client.begin(Config.username, Config.server_ip, Config.server_port):
            error_network('app:begin Could not connect')
            self.status_connection = False
            return False
        self.status_connection = True
        return True

    def stop(self):
        self._run = False
        self.status_connection = False
        Client.stop()

    def loop(self):
        stat = dict()
        buffer = dict()
        Client.status(stat)

        if not self._is_online == stat['netstat']:
            self._is_online = stat['netstat']
            Config.flags.update.NETWORK = True

        if not self._is_connected == stat['clientstat']:
            self._is_connected = stat['clientstat']
            Config.flags.update.NETWORK = True

        if self._run:
            if not self._is_connected:
                if time.perf_counter() > self._timeout_reconnect:
                    self._timeout_reconnect = time.perf_counter() + 5.0
                    self._start_client()
                    return

        else:
            return

        while Client.fetch(buffer):
            Config.flags.update.PACKET = True
            packet = IndoorinoPacket()
            packet.from_client(buffer)
            # print('appending packet {}'.format(packet.description))
            self._buffer.append(packet)
            self._rx.append(packet)

    def send(self, packet):
        self._tx.append(packet)
        Client.send(packet.dictionary())

    def send2board(self, boardname, packet):
        packet.target = boardname
        self._tx.append(packet)
        Client.send(packet.dictionary())

    def send2server(self, packet):
        packet.target = 'SERVER'
        self._tx.append(packet)
        Client.send(packet.dictionary())

    @staticmethod
    def send_server_request(command):
        Client.serverequest(command)

    @staticmethod
    def send_system_request(command, param1=0, param2=0, param3=0, param4=0):
        p = IndoorinoPacket()
        p.build(IBACOM_SYS_REQ, 'SERVER', {
            'command': command,
            'value1': param1,
            'value2': param2,
            'value3': param3,
            'value4': param4,
        })
        System.io.send(p)

    def ready(self):
        return len(self._buffer) > 0

    def fetch(self):
        try:
            return self._buffer.pop(0)
        except IndexError:
            error_os('Fetching on empty buffer!')
            return None

class Boards:

        def __init__(self):

            self._boards = dict()

        def __getitem__(self, item):
            if item in self._boards.keys():
                return self._boards[item]
            error_boards('CORE:invalid call for board {}'.format(item))
            return None

        def __call__(self, *args, **kwargs):
            return dict(self._boards)

        def __len__(self):
            return len(self._boards)

        def values(self):
            return  tuple(self._boards.values())

        def loop(self):

            for board in self._boards.values():
                board.loop()

        def parse(self, packet):

            if IBACOM_CONF_STD <= packet.command <= IBACOM_STATUS_DEVSTD:

                Config.flags.update.BOARD=True

                debug('parsing for boards {}'.format(packet.description))
                name = packet.payload['name']
                if not isinstance(name, str) or len(name) == 0:
                    error_os('board:parse: invalid name <{}> from packet {}'.format(name, packet))
                    return
                if not name in self._boards.keys():
                    info_boards('Adding board {}'.format(name))
                    self._boards[name] = IndoorinoBoard(name)
                self._boards[name].parse(packet)


            elif packet.source in self._boards.keys():
                self._boards[packet.source].parse(packet)

            elif packet.command in (IBACOM_SYS_PROBE_DATA,):
                if packet.payload['board'] in self._boards.keys():
                    self._boards[packet.payload['board']].parse(packet)

        def clear(self):
            self._boards.clear()

        def remove(self, name):
            if name in self._boards.keys():
                self._boards.pop(name)
                return
            warning_boards('remove board: {} is not in list'.format(name))

        def set_offline(self):
            for board in self._boards.values():
                board.set_offline()

class Layout:

    class HomeMap(dict):
        def __init__(self):
            dict.__init__(self)

    class Device:
        def __init__(self, board, device, d_type, **kwargs):
            self._board = board
            self._device = device
            self._devtype = d_type
            self.location = kwargs.pop('location', 'none')
            self.area = kwargs.pop('area', 'none')
            self.comfiguration = kwargs.pop('conf', '')

        @property
        def boardname(self):
            return self._board

        @property
        def devname(self):
            return self._device

        @property
        def devtype(self):
            return self._devtype

    class Service:

        def __init__(self, name, s_type, **kwargs):
            self._name = name
            self._type = s_type

            self.description = kwargs.pop('description', 'unknown')
            self.location = kwargs.pop('location', 'none')
            self.area = kwargs.pop('area', 'none')

            self.devices = dict()

        @property
        def name(self):
            return self._name

        @property
        def service_type(self):
            return self._type

        @property
        def boards(self):
            data = list()
            for entry in self.devices.values():
                if not entry.boardname in data:
                    data.append(entry.boardname)
            return data

    def __init__(self):

        self.homemap = self.HomeMap()
        self.services = dict()

    @property
    def devices(self):

        data = dict()
        for entry in self.services.values():
            for key, dev in entry.devices.items():
                if not key in data.keys():
                    data.update({
                        key: dev
                    })
        return data

    def parse(self, packet):
        # info_database('Layout:parse: packet {}'.format(packet))

        if packet.command == IBACOM_LYT_MAP:
            """ Parsing H O M E M A P layout """
            if not packet.payload['label1'] in self.homemap.keys():
                self.homemap.update({packet.payload['label1']: list()})
            self.homemap[packet.payload['label1']].append(packet.payload['label2'])

        elif packet.command == IBACOM_LYT_SERVICE:
            """ Parsing S E R V I C E layout """
            if not packet.payload['desc1'] in self.services.keys():
                self.services.update({
                    packet.payload['desc1']: self.Service(
                        packet.payload['desc1'],
                        packet.payload['desc2'],
                        description=packet.payload['label1'],
                        area=packet.payload['label2'],
                        location=packet.payload['label3'],
                    )
                })

        elif packet.command == IBACOM_LYT_DEVICE:
            """ Parsing D E V I C E layout """
            if packet.payload['desc1'] in self.services.keys():

                key = (packet.payload['board'], packet.payload['devname'])

                if not key in self.services[packet.payload['desc1']].devices.keys():

                    self.services[packet.payload['desc1']].devices.update({
                        key: self.Device(
                            packet.payload['board'],
                            packet.payload['devname'],
                            packet.payload['type'],
                            area=packet.payload['label1'],
                            location=packet.payload['label2'],
                            conf=packet.payload['label3'],
                        )
                    })

    def clear(self):
        warning_os('Clearing System.services')
        self.services.clear()

class Alarms:

    class AlarmService:

        class AlarmDev:

            def __init__(self, board_name, dev_name, dev_group, dev_type):
                self._boardname = board_name
                self._devname = dev_name
                self._group = dev_group
                self._alarmtype = dev_type
                self._enabled = False
                self._on_alarm = False
                self._events = dict()

            @property
            def group(self):
                return self._group

            @property
            def alarmtype(self):
                return self._alarmtype

            @property
            def is_enabled(self):
                return self._enabled

            @property
            def is_onalarm(self):
                return self._on_alarm

            @property
            def boardname(self):
                return self._boardname

            @property
            def devname(self):
                return self._devname

            @property
            def events(self):
                return self._events.copy()

            def clear_events(self):
                self._events.clear()

            def parse(self, packet):

                if packet.command == IBACOM_ALARM_DEVSTAT:
                    if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                        if not self._enabled == bool(packet.payload['status']):
                            self._enabled = bool(packet.payload['status'])
                        if not self._on_alarm == bool(packet.payload['value1']):
                            self._on_alarm = bool(packet.payload['value1'])
                        self._events.update({
                            datetime.datetime.now(): packet
                        })
                        Config.flags.update.SYSTEM = True

                elif packet.command == IBACOM_SET_ENV_ALARM:
                    if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                        self._events.update({
                            datetime.datetime.now(): packet
                        })
                        Config.flags.update.SYSTEM = True
                        # should not recv this

                elif packet.command == IBACOM_ACK_ENV_ALARM:
                    if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                        self._events.update({
                            datetime.datetime.now(): packet
                        })
                        Config.flags.update.SYSTEM = True
                        # should not recv this

                elif IBACOM_HEAT_ALARM <= packet.command <= IBACOM_GENERIC_ALARM:
                    if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                        self._events.update({
                            datetime.datetime.now(): packet
                        })
                        Config.flags.update.SYSTEM = True

        def __init__(self, name, alarm_type):
            self._name = name
            self._alarm_type = alarm_type
            self._devices = dict()
            self._state = 0
            self._wait4ack = 0
            self._events = dict()

        @property
        def name(self):
            return self._name

        @property
        def alarmtype(self):
            return self._alarm_type

        @property
        def is_onalarm(self):
            return self._state

        @property
        def devices(self):
            return self._devices.copy()

        @property
        def events(self):
            return self._events.copy()

        def parse(self, packet):

            if packet.command == IBACOM_LYT_DEVICE:

                if not self._name in _core_layout.services.keys():
                    error_os('core::alarms: could not find service in layout!')
                    return

                for dev in self._devices.keys():

                    if not dev in _core_layout.services[self._name].devices.keys():
                        alert_os('Removing core::alarm::device {}:{}'.format(dev[0], dev[1]))
                        self._devices.pop(dev)
                        Config.flags.update.SYSTEM = True
                        self.parse(packet)
                        return

                for key, dev in _core_layout.services[self._name].devices.items():
                    if not key in self._devices.keys():
                        alert_os('Adding core::alarm {}:{}'.format(key[0], key[1]))
                        self._devices.update({
                            key: self.AlarmDev(
                                dev.boardname,
                                dev.devname,
                                self._alarm_type,
                                dev.devtype)
                        })
                        Config.flags.update.SYSTEM = True

            # if packet.command == IBACOM_LYT_ALARMS:
            #
            #     if packet.payload['label1'] == self._name:
            #         name = (packet.payload['board'], packet.payload['devname'])
            #         if not name in self._devices.keys():
            #             self._devices.update(
            #                 {
            #                     name : self.AlarmDev(
            #                         packet.payload['board'],
            #                         packet.payload['devname'],
            #                         self._name,
            #                         self._alarm_type
            #                     )
            #                 }
            #             )
            #     Config.flags.update.SYSTEM=True
            #     return

            if packet.command == IBACOM_ENV_ALARM:

                print('\n*** ALARM DEBUG:\n\t_name={} _alarm_type={}\n\tdesc1={} desc2={}'.format(
                    self._name, self._alarm_type, packet.payload['desc1'], packet.payload['desc2']
                ))
                if packet.payload['desc1'] == self._name:
                    self._events.update({
                        datetime.datetime.now(): packet
                    })

                if packet.payload['desc1'] == self._name:
                    if packet.payload['value1'] > 0:
                        self._state = packet.payload['value1']
                        Config.flags.update.SYSTEM = True
                        # Here we have an alarm!

            for dev in self._devices.values():
                dev.parse(packet)

            if not any([dev.is_onalarm for dev in self._devices.values()]):
                self._state = 0

    def __init__(self):
        self.services = dict()

    def devices(self):
        data = dict()
        for g in self.services.values():
            for key, dev in g.devices.items():
                data.update({key: dev})
        return data

    def parse(self, packet):

        if packet.command  == IBACOM_LYT_SERVICE:

            for service in self.services.keys():
                if not service in _core_layout.services.keys():
                    alert_os('Removing core::alarm {}'.format(self.services.pop(service).name))
                    Config.flags.update.SYSTEM = True

            for service in _core_layout.services.values():
                if not service.name in self.services.keys():
                    if 'ALARM' in service.service_type:
                        alert_os('Adding core::alarm {}'.format(service.name))
                        self.services.update({
                            service.name : self.AlarmService(service.name, service.service_type)
                        })
                        Config.flags.update.SYSTEM = True

        for s in self.services.values():
            s.parse(packet)

    def clear(self):
        warning_os('Clearing core.services.alarms ({} items)'.format(len(self.services)))
        self.services.clear()


class Lights:

    class LightService:

        class LightDev:

            class LighTimer:
                def __init__(self):
                    self.enabled=False
                    self.is_on=False
                    self.timers=list()

            def __init__(self, board_name, dev_name, dev_service, dev_type):
                self._boardname = board_name
                self._devname = dev_name
                self._service = dev_service
                self._lightype = dev_type
                self._turnoff = 0
                self._timer = self.LighTimer()

            @property
            def boardname(self):
                return self._boardname

            @property
            def devname(self):
                return self._devname

            @property
            def timer(self):
                return self._timer

            @property
            def turnoff(self):
                return self._turnoff

            @property
            def service(self):
                return self._service

            @property
            def lightype(self):
                return self._lightype

            def parse(self, packet):

                # ( 7150,     'lgt_dev_set',       'light device set',                    ('board', 'devname', 'desc1', 'command','value1')),                                 # SET/ON/OFF and value
                # ( 7151,     'lgt_auto_off',      'light auto turn off',                 ('board', 'devname', 'desc1', 'command','value1',)),                                # SET/ABORT and seconds
                # ( 7152,     'lgt_timer_set',     'light timer set',                     ('board', 'devname', 'desc1', 'command','epoch', 'value1',)),                       # ADD/CLEAR, start epoch and seconds
                # ( 7153,     'lgt_timer_stat',    'light timer status',                  ('board', 'devname', 'desc1', 'level', 'status', 'value1', 'value2', 'value3')),    # is_on, is_enabled and num of timers, starts in
                # ( 7154,     'lgt_timer_entry',   'light timer interval',                ('board', 'devname', 'desc1', 'level', 'epoch',  'value1')),                        # id (1,2,3...), start, seconds

                if packet.command == IBACOM_LGT_DEV_SET:
                    if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                        warning_os('I have no idea how to parse IBACOM_LGT_DEV_SET in client')
                if packet.command == IBACOM_LGT_AUTO_OFF:
                    if packet.payload['board'] == self._boardname \
                            and packet.payload['devname'] == self._devname\
                            and packet.payload['desc1'] == self._service:
                        self._turnoff = packet.payload['value1']
                        Config.flags.update.SYSTEM = True

                elif packet.command == IBACOM_LGT_TIMER_STAT:
                    if packet.payload['board'] == self._boardname \
                            and packet.payload['devname'] == self._devname\
                            and packet.payload['desc1'] == self._service:
                        self._timer.enabled = packet.payload['status']
                        self._timer.is_on = packet.payload['level']
                        if not len(self._timer.timers) == packet.payload['value1']:
                            self._timer.timers.clear()
                        Config.flags.update.SYSTEM = True

                elif packet.command == IBACOM_LGT_TIMER_ENTRY:
                    if packet.payload['board'] == self._boardname \
                            and packet.payload['devname'] == self._devname \
                            and packet.payload['desc1'] == self._service:

                        if packet.payload['level'] >= len(self._timer.timers):
                            self._timer.timers.append(
                                [ packet.payload['epoch'], packet.payload['value1'], ]
                            )
                        else:
                            self._timer.timers[ packet.payload['level'] ] = \
                                [ packet.payload['epoch'], packet.payload['value1'], ]

                        Config.flags.update.SYSTEM = True

        def __init__(self, name, light_type):
            self._name = name
            self._light_type = light_type
            self._devices = dict()

        @property
        def name(self):
            return self._name

        @property
        def lightype(self):
            return self._light_type

        @property
        def devices(self):
            return self._devices.copy()

        @property
        def boards(self):
            return list(set([d.boardname for d in self._devices.values()]))


        def parse(self, packet):

            if packet.command == IBACOM_LYT_DEVICE:

                if not self._name in _core_layout.services.keys():
                    error_os('core::lights: could not find service in layout!')
                    return

                for dev in self._devices.keys():

                    if not dev in _core_layout.services[self._name].devices.keys():
                        alert_os('Removing core::lights::device {}:{}'.format(dev[0], dev[1]))
                        self._devices.pop(dev)
                        Config.flags.update.SYSTEM = True
                        self.parse(packet)
                        return

                for key, dev in _core_layout.services[self._name].devices.items():
                    if not key in self._devices.keys():
                        alert_os('Adding core::lights {}:{}'.format(key[0], key[1]))
                        self._devices.update({
                            key: self.LightDev(
                                dev.boardname,
                                dev.devname,
                                self._light_type,
                                dev.devtype)
                        })
                        Config.flags.update.SYSTEM = True

            for dev in self._devices.values():
                dev.parse(packet)

    def __init__(self):
        self.services = dict()

    def parse(self, packet):

        if packet.command == IBACOM_LYT_SERVICE:

            for service in self.services.keys():  # this should be run somewhere else.
                if not service in _core_layout.services.keys():
                    alert_os('Removing core::lights {}'.format(self.services.pop(service).name))
                    Config.flags.update.SYSTEM = True

            for service in _core_layout.services.values():
                if 'LIGHTS' in service.service_type:
                    if not service.name in self.services.keys():
                        alert_os('Adding core::lights {}'.format(service.name))
                        self.services.update({
                            service.name: self.LightService(service.name, service.service_type)
                        })
                        Config.flags.update.SYSTEM = True

        for s in self.services.values():
            s.parse(packet)

    def clear(self):
        warning_os('Clearing core.services.lights ({} items)'.format(len(self.services)))
        self.services.clear()


class Session:

    def __init__(self):
        self._time = 0
        self._status = False

    def __call__(self, *args, **kwargs):  # call with 0 to set offline
        if args and isinstance(args[0], bool) and not args[0]:
            self._time = 0
            self._status = False
        return self._status

    def time(self):
        return datetime.timedelta(seconds=self._time)

    def parse(self, packet):

        if packet.command == IBACOM_SESSION_STAT:
            self._time = packet.payload['epoch']
            self._status = bool(packet.payload['status'])
        elif packet.command == IBACOM_SESSION_END:
            self._time = 0
            self._status = False



_core_io = PacketIo()
_core_boards = Boards()
_core_layout = Layout()
_core_alarms = Alarms()
_core_lights = Lights()
_core_session = Session()



class IndoorinoCore:

    def __init__(self):

        self.io = _core_io
        self.boards = _core_boards
        self.layout = _core_layout
        self.alarms = _core_alarms
        self.lights = _core_lights
        self.session = _core_session

        if Config.options.LOAD_ON_START:
            self.load_session()

    def begin(self):

        return self.io.begin()

    def loop(self):

        self.io.loop()
        self.boards.loop()
        if not self.io.is_connected():
            self.session(0)
            for board in self.boards().values():
                if board.is_connected():
                    board.set_offline()
                    Config.flags.update.BOARD = True

            # return

        while self.io.ready():
            packet = self.io.fetch()

            if packet.command in (IBACOM_SESSION_STAT, IBACOM_SESSION_END):
                self.session.parse(packet)
                return

            self.boards.parse(packet)
            self.layout.parse(packet)
            self.alarms.parse(packet)
            self.lights.parse(packet)

    def on_exit(self):
        if Config.options.SAVE_ON_EXIT:
            self.save_session()
        Client.stop()

    def save_session(self):

        filepath=os.path.join(PATH_WS, 'session/boards.ndo')
        r = list()

        if len( self.boards ) > 0:
            alert_os('Saving boards to {}'.format(filepath))
            try:
                handler = open(filepath, 'wb')
                pickle.dump(self.boards, handler, pickle.HIGHEST_PROTOCOL)
                handler.close()
            except (FileNotFoundError, FileExistsError, IOError) as error:
                error_os('Can not save! error:{}'.format(error))
                r.append(filepath)

        filepath = os.path.join(PATH_WS, 'session/packets.ndo')
        if len( self.io.rx_packets() ) > 0:
            alert_os('Saving packets to {}'.format(filepath))
            data = (
                self.io.rx_packets(),
                self.io.tx_packets()
            )
            try:
                handler = open(filepath, 'wb')
                pickle.dump(data, handler, pickle.HIGHEST_PROTOCOL)
                handler.close()
            except (FileNotFoundError, FileExistsError, IOError) as error:
                error_os('Can not save! error:{}'.format(error))
                r.append(filepath)

        filepath = os.path.join(PATH_WS, 'session/layout.ndo')
        alert_os('Saving layout to {}'.format(filepath))
        data = (
            self.layout.homemap,
            self.layout.services,
        )
        try:
            handler = open(filepath, 'wb')
            pickle.dump(data, handler, pickle.HIGHEST_PROTOCOL)
            handler.close()
        except (FileNotFoundError, FileExistsError, IOError) as error:
            error_os('Can not save! error:{}'.format(error))
            r.append(filepath)

        filepath = os.path.join(PATH_WS, 'session/alarms.ndo')
        alert_os('Saving alarms to {}'.format(filepath))
        try:
            handler = open(filepath, 'wb')
            pickle.dump(self.alarms.services, handler, pickle.HIGHEST_PROTOCOL)
            handler.close()
        except (FileNotFoundError, FileExistsError, IOError) as error:
            error_os('Can not save! error:{}'.format(error))
            r.append(filepath)

        filepath = os.path.join(PATH_WS, 'session/lights.ndo')
        alert_os('Saving lights to {}'.format(filepath))
        try:
            handler = open(filepath, 'wb')
            pickle.dump(self.lights.services, handler, pickle.HIGHEST_PROTOCOL)
            handler.close()
        except (FileNotFoundError, FileExistsError, IOError) as error:
            error_os('Can not save! error:{}'.format(error))
            r.append(filepath)

        return r

    def load_session(self):
        # per lo stimato ed illustre collega FRENZ detto biggus dickus
        # nel repo di github trovi la cartella <saved> con dentro <boards.ndo>
        # che contiene dati-esempio e viene letto qui sotto. Correggi la variabile filepath:
        # filepath = '/home/n00b/Code/boards.ndo'
        filepath = os.path.join(PATH_WS, 'session/boards.ndo')
        if os.path.isfile(filepath):
            alert_os('Loading boards')
            try:
                handler = open(filepath, 'rb')
                self.boards = pickle.load(handler)
                Config.flags.update.BOARD = True
                handler.close()
            except ModuleNotFoundError:
                error_os('invalid file: {} could not read!'.format(filepath))
        else:
            warning_os('invalid file: {} does not exist!'.format(filepath))

        filepath = os.path.join(PATH_WS, 'session/layout.ndo')
        if os.path.isfile(filepath):
            alert_os('Loading layout')
            try:
                handler = open(filepath, 'rb')
                data = pickle.load(handler)
                Config.flags.update.BOARD = True
                handler.close()

                self.layout.homemap = data[0]
                self.layout.services = data[1]

            except ModuleNotFoundError:
                error_os('invalid file: {} could not read!'.format(filepath))
        else:
            warning_os('invalid file: {} does not exist!'.format(filepath))

        filepath = os.path.join(PATH_WS, 'session/alarms.ndo')
        if os.path.isfile(filepath):
            alert_os('Loading alarms')
            try:
                handler = open(filepath, 'rb')
                self.alarms.services = pickle.load(handler)
                Config.flags.update.BOARD = True
                handler.close()
            except ModuleNotFoundError:
                error_os('invalid file: {} could not read!'.format(filepath))
        else:
            warning_os('invalid file: {} does not exist!'.format(filepath))

        filepath = os.path.join(PATH_WS, 'session/lights.ndo')
        if os.path.isfile(filepath):
            alert_os('Loading lights')
            try:
                handler = open(filepath, 'rb')
                self.lights.services = pickle.load(handler)
                Config.flags.update.BOARD = True
                handler.close()
            except ModuleNotFoundError:
                error_os('invalid file: {} could not read!'.format(filepath))
        else:
            warning_os('invalid file: {} does not exist!'.format(filepath))

System = IndoorinoCore()


class HomeSensors:

    def __init__(self):
        pass

    def query(self):
        pass



print('Loaded indoorino.core')

# /home/n00b/.indoorino/session/borads.ndo found!