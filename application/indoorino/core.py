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
                try:
                    if not name in self._boards.keys():
                        info_boards('Adding board {}'.format(name))
                        self._boards[name] = IndoorinoBoard(name)
                    self._boards[name].parse(packet)
                except TypeError as error:
                    error_os('board:parse: invalid name <{}>'.format(error))

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

            Config.layout.parse(packet)
            self.boards.parse(packet)

    def on_exit(self):
        if Config.options.SAVE_ON_EXIT:
            self.save_session()
        Client.stop()

    def save_session(self):

        filepath=os.path.join(PATH_WS, 'session/boards.ndo')

        if len( self.io.rx_packets() ) > 0:
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

System = IndoorinoCore()

print('Loaded indoorino.core')

# /home/n00b/.indoorino/session/borads.ndo found!