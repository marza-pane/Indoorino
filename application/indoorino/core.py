import pickle

from common.utils import *
from common.comtable import *
from indoorino.packet import IndoorinoPacket
from indoorino.boards import IndoorinoBoard

class IndoorinoCore:

    class PacketIo:

        def __init__(self):
            self.status_connection = False
            self._buffer = list()
            self._rx = list()
            self._tx = list()
            self._is_connected=False
            self._is_online=False
            self._timeout_reconnect=0.0
            self._run=False

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
                self._run=True

        def _start_client(self):
            if not Client.begin(Config.username, Config.server_ip, Config.server_port):
                error_network('app:begin Could not connect')
                self.status_connection = False
                return False
            self.status_connection = True
            return True


        def stop(self):
            self._run=False
            self.status_connection = False
            Client.stop()

        def loop(self):
            stat = dict()
            buffer = dict()
            Client.status(stat)

            if not self._is_online == stat['netstat']:
                self._is_online=stat['netstat']
                Config.flags.update.NETWORK=True

            if not self._is_connected == stat['clientstat']:
                self._is_connected=stat['clientstat']
                Config.flags.update.NETWORK=True

            if self._run:
                if not self._is_connected:
                    if time.perf_counter() > self._timeout_reconnect:
                        self._timeout_reconnect = time.perf_counter() + 5.0
                        self._start_client()
                        return

            else:
                return

            while Client.fetch(buffer):
                Config.flags.update.PACKET=True
                packet = IndoorinoPacket()
                packet.from_client(buffer)
                print('appending packet {}'.format(packet.description))
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

        def send_server_request(self, command):
            Client.serverequest(command)
            pass

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

            if IBACOM_CONF_STD <= packet.command <= IBACOM_STATUS_DEVSTD or \
                packet.command in (IBACOM_CONF_ESP, IBACOM_STATUS_ESP):

                Config.flags.update.BOARD=True

                debug('parsing for boards {}'.format(packet.description))
                name = packet.payload['name']
                if not isinstance(name, str):
                    error_os('board:parse: invalid name <{}> from packet {}'.format(name, packet))
                try:
                    if not name in self._boards.keys():
                        info_boards('Adding board {}'.format(name))
                        self._boards[name] = IndoorinoBoard(name)
                    self._boards[name].parse(packet)
                except TypeError as error:
                    error_os('board:parse: invalid <name:{}> parameter. Packet {}'.format(name, packet))
                    error_os(error.__repr__())

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

        class DeviceEntry:
            def __init__(self, board, device, **kwargs):
                self._board = board
                self._device = device
                self.location = kwargs.pop('location','none')
                self.area = kwargs.pop('area','none')
                self.devtype = kwargs.pop('devtype','unknown')

            @property
            def boardname(self):
                return self._board

            @property
            def devname(self):
                return self._device

        class LightEntry(DeviceEntry):
            def __init__(self, board, device, **kwargs):
                IndoorinoCore.Layout.DeviceEntry.__init__(self, board, device, **kwargs)
                self.lightype=kwargs.pop('lightype','none')
                self.group=kwargs.pop('group','none')

        class WeatherEntry(DeviceEntry):
            def __init__(self, board, device, **kwargs):
                IndoorinoCore.Layout.DeviceEntry.__init__(self, board, device, **kwargs)
                self.weathertype=kwargs.pop('weathertype','none')
                self.group=kwargs.pop('group','none')

        def __init__(self):

            self.devices = dict()
            self.lights = dict()
            self.weather = dict()

        def parse(self, packet):
            info_database('Layout:parse: packet {}'.format(packet))
            if packet.command == IBACOM_LYT_DEVICE:
                key = (packet.payload['board'], packet.payload['devname'])
                self.devices.update( {
                    key : self.DeviceEntry(
                        packet.payload['board'],
                        packet.payload['devname'],
                        devtype=packet.payload['type'],
                        area=packet.payload['label1'],
                        location=packet.payload['label2'],
                        )
                    }
                )

            elif packet.command == IBACOM_LYT_LIGHTS:
                key = (packet.payload['board'], packet.payload['devname'])

                if not key in self.devices.keys():
                    warning_os('Layout:lights: could not find {} in device dict'.format(key))
                else:
                    self.lights.update( {
                        key: self.LightEntry(
                            packet.payload['board'],
                            packet.payload['devname'],
                            devtype=self.devices[key].devtype,
                            location=self.devices[key].location,
                            area=self.devices[key].area,
                            lightype=packet.payload['type'],
                            group=packet.payload['label1'],
                            )
                        } )

            elif packet.command == IBACOM_LYT_WEATHER:
                key = (packet.payload['board'], packet.payload['devname'])

                if not key in self.devices.keys():
                    warning_os('Layout:weather: could not find {} in device dict'.format(key))
                else:
                    self.weather.update( {
                        key: self.WeatherEntry(
                            packet.payload['board'],
                            packet.payload['devname'],
                            devtype=self.devices[key].devtype,
                            location=self.devices[key].location,
                            area=self.devices[key].area,
                            group=packet.payload['label1'],
                            weathertype=packet.payload['type'],
                            )
                        } )

        def clear(self):
            self.devices.clear()
            self.lights.clear()
            self.weather.clear()

    class Alarms:

        class AlarmGroup:

            class AlarmDev:

                def __init__(self, board_name, dev_name, dev_group, dev_type):
                    self._boardname = board_name
                    self._devname = dev_name
                    self._group=dev_group
                    self._alarmtype=dev_type
                    self._enabled=False
                    self._on_alarm=False
                    self._events=list()

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
                            self._events.append(packet)

                    elif packet.command == IBACOM_SET_ENV_ALARM:
                        if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                            self._events.append(packet)
                            # should not recv this

                    elif packet.command == IBACOM_ACK_ENV_ALARM:
                        if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                            self._events.append(packet)
                            # should not recv this

                    elif IBACOM_HEAT_ALARM <= packet.command <= IBACOM_GENERIC_ALARM:
                        if packet.payload['board'] == self._boardname and packet.payload['devname'] == self._devname:
                           self._events.append(packet)

            def __init__(self, name, alarm_type):
                self._name = name
                self._alarm_type = alarm_type
                self._devices = dict()
                self._state = 0
                self._waitforack = 0
                self._events = list()

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

                if packet.command == IBACOM_LYT_ALARMS:

                    if packet.payload['label1'] == self._name:
                        name = (packet.payload['board'], packet.payload['devname'])
                        if not name in self._devices.keys():
                            self._devices.update(
                                {
                                    name : self.AlarmDev(
                                        packet.payload['board'],
                                        packet.payload['devname'],
                                        self._name,
                                        self._alarm_type
                                    )
                                }
                            )
                    Config.flags.update.SYSTEM=True
                    return

                if packet.command == IBACOM_ENV_ALARM:

                    self._events.append(packet)
                    Config.flags.update.SYSTEM=True
                    if packet.payload['desc1'] == self._alarm_type:
                        if packet.payload['value1'] > 0:
                            self._state = packet.payload['value1']

                    # (7703, 'env_alarm', 'environment alarm signal',
                    #  ('desc1', 'desc2', 'label1', 'label2', 'epoch', 'value1', 'status')),

                for dev in self._devices.values():
                    dev.parse(packet)

                if self._waitforack:
                    if not all( [dev.is_onalarm for dev in self._devices.values()] ):
                        self._state=0
                        self._waitforack=0

        def __init__(self):
            self.groups = dict()

        def devices(self):
            data = dict()
            for g in self.groups.values():
                for key, dev in g.devices.items():
                    data.update( { key:dev } )
            return data

        def parse(self, packet):

            if packet.command == IBACOM_LYT_ALARMS:

                if not packet.payload['label1'] in self.groups.keys():
                    self.groups.update(
                        {
                            packet.payload['label1'] :
                                self.AlarmGroup(packet.payload['label1'], packet.payload['type'])
                        }
                    )
                Config.flags.update.SYSTEM = True

            for group in self.groups.values():
                group.parse(packet)

        def clear(self):
            self.groups.clear()

    class Session:

        def __init__(self):
            self._time=0
            self._status=False

        def __call__(self, *args, **kwargs): # call with 0 to set offline
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

    def __init__(self):

        self.io = self.PacketIo()
        self.boards = self.Boards()
        self.alarms = self.Alarms()
        self.layout = self.Layout()
        self.session = self.Session()

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

            # Config.layout.parse(packet)
            self.boards.parse(packet)
            self.alarms.parse(packet)
            self.layout.parse(packet)

    def on_exit(self):
        if Config.options.SAVE_ON_EXIT:
            self.save_session()
        Client.stop()

    def save_session(self):

        filepath=os.path.join(PATH_WS, 'session/boards.ndo')

        if len( self.boards ) > 0:
            alert_os('Saving boards to {}'.format(filepath))
            try:
                handler = open(filepath, 'wb')
                pickle.dump(self.boards, handler, pickle.HIGHEST_PROTOCOL)
                handler.close()
            except (FileNotFoundError, FileExistsError, IOError) as error:
                error_os('Can not save! error:{}'.format(error))

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

        filepath = os.path.join(PATH_WS, 'session/layout.ndo')
        alert_os('Saving layout to {}'.format(filepath))
        data = (
            self.layout.devices,
            self.layout.lights,
            self.layout.weather,
        )
        try:
            handler = open(filepath, 'wb')
            pickle.dump(data, handler, pickle.HIGHEST_PROTOCOL)
            handler.close()
        except (FileNotFoundError, FileExistsError, IOError) as error:
            error_os('Can not save! error:{}'.format(error))

        filepath = os.path.join(PATH_WS, 'session/alarms.ndo')
        alert_os('Saving alarms to {}'.format(filepath))
        try:
            handler = open(filepath, 'wb')
            pickle.dump(self.alarms.groups, handler, pickle.HIGHEST_PROTOCOL)
            handler.close()
        except (FileNotFoundError, FileExistsError, IOError) as error:
            error_os('Can not save! error:{}'.format(error))

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

                self.layout.devices = data[0]
                self.layout.lights = data[1]
                self.layout.weather = data[2]

            except ModuleNotFoundError:
                error_os('invalid file: {} could not read!'.format(filepath))
        else:
            warning_os('invalid file: {} does not exist!'.format(filepath))

        filepath = os.path.join(PATH_WS, 'session/alarms.ndo')
        if os.path.isfile(filepath):
            alert_os('Loading alarms')
            try:
                handler = open(filepath, 'rb')
                self.alarms.groups = pickle.load(handler)
                Config.flags.update.BOARD = True
                handler.close()
            except ModuleNotFoundError:
                error_os('invalid file: {} could not read!'.format(filepath))
        else:
            warning_os('invalid file: {} does not exist!'.format(filepath))

System = IndoorinoCore()

print('Loaded indoorino.core')

# /home/n00b/.indoorino/session/borads.ndo found!