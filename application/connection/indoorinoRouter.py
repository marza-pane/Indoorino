
import socket

from utils import IndoorinoModule as decoder

class IndoorinoRouter:

    def __init__(self):

        self._decoder           = decoder
        self._connection_status = False
        self._update            = False

        self.client = socket.socket()
        self.server = socket.socket()

        self._packets_sent = 0
        self._packets_received = 0

    def connect(self):

        self.localip = get_localip()

        if not validate_address((self.localip, self.config.port)):
            self.status = False

        else:
            self.client.close()
            self.server.close()
            self.client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            try:
                self.server.bind((self.localip, self.config.port))
                self.server.settimeout(SOCKET_TIMEOUT)
                self.status = True
            except OSError:
                debug('\nConnect: Unable to bind address to socket. Probably already in use')
                # if verbose: self.events.add(3, "Connect: Unable to bind address to socket. Probably already in use")
                self.status = False

    @property
    def status(self):
        return self._status

    @property
    def storage(self):
        m=self._storage
        self._storage=list()
        return m

    @property
    def debug(self):
        m=self._debug
        self._debug=str()
        return m

    @property
    def total_received(self):
        return self._packets_received

    @property
    def total_sent(self):
        return self._packets_sent

    def updateflag(self):
        if self._update:
            self._update=False
            return True
        return False

    def connect(self, *args):
        pass

    def read(self, *args):
        pass

    def write(self, *args):
        pass
