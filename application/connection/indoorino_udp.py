from utils.comtable import *
from libraries.connectUtils import *
import socket

class UdpConnectionConfig:

    def __init__(self, ip='0.0.0.0', port=0):
        self._ip         = ip
        self._port       = port
        self._options    = dict()

    @property
    def device(self):
        return '{}:{}'.format(self._ip, self._port)

    @property
    def ip(self):
        return self._ip

    @ip.setter
    def ip(self, ip):
        if validate_ip(ip):
            self._ip = ip
        else:
            debug('UdpConnectionConfig: WARNING: invalid IP')

    @property
    def port(self):
        return self._port

    @port.setter
    def port(self, port):
        if validate_udport(port):
            self._port = port
        else:
            debug('UdpConnectionConfig: WARNING: invalid UDP port')

    @property
    def options(self):
        return self._options

    def option_rem(self, option):
        if option in self._options.keys():
            self._options.pop(option)

    def option_add(self, key, value):

        opts = ()

        if not key in opts:
            debug('UdpConnectionConfig: ERROR: {} is not a valid UDP socket option'.format(value))
            return

        self._options[key]=value

class IndoorinoUdpReader(IndoorinoReceiverTemplate):

    def __init__(self):
        IndoorinoReceiverTemplate.__init__(self)

        self.config = UdpConnectionConfig()
        self.config.port = DEFAULT_UDP_PORT

        self.client = socket.socket()
        self.server = socket.socket()

        self.localip = get_localip()

        self._connect_timeout = 5000
        self._connect_last = time.perf_counter()

    @property
    def status(self):
        return self._status

    @status.setter
    def status(self, val):

        if val and not self._status:
            print('{} - CONNECTED'.format(datetime.datetime.now()))
            self._update = True

        elif not val and self._status:
            print('{} - DISCONNECTED'.format(datetime.datetime.now()))
            self._update = True

        # qui mettere cosa fare all disconnessione/riconnessione

        self._status = val

        """
            ARDUINO SIDE:
                      +85 + 78 + 76 + 79 + 67 + 75 + 69 + 68 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0
                      +80 + 72 + 79 + 78 + 69 + 68 + 79 + 79 + 82 + 0 + 67 + 79 + 78 + 70 + 0 + 73
                      +33 + 88 + 141 + 94
                      +1 = 2007

            PC SIDE:  +85 + 78 + 76 + 79 + 67 + 75 + 69 + 68 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0
                      +80 + 72 + 79 + 78 + 69 + 68 + 79 + 79 + 82 + 0 + 0 + 0 + 0 + 0 + 0 + 0
                      +33 + 88 + 141 + 94
                      + 1 = 1640
        """
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

    def disconnect(self):
        self.client.close()
        self.server.close()
        self._status = False
        self._update = True

    def net_status(self):

        if time.perf_counter() > self._connect_last + self._connect_timeout:

            self._connect_last = time.perf_counter()
            if self.status and not get_localip():
                self.status = False
            elif not self.status:
                self.connect()

        return self.status

    def read(self, millis=0):

        if not self._status:
            return

        start = time.perf_counter()
        while True:

            # Write in_waiting bytes from Serial to buffer
            try:
                buffer, address = self.server.recvfrom(2048)
                print("---> ADDRESS:(type {} - {})".format(type(address[0]), type(address[1])))
                print(address)
                print("---> PACKET:")
                print(format_bytearray(buffer))

                self._decoder.parse(bytearray(buffer))
                incoming = self._decoder.obtain()
                self._debug += format_debug(buffer)

                if len(incoming) > 0:
                    debug('\nReceieved {} packet: {} {}\n'.format(len(incoming),type(incoming),incoming))
                    for packet in incoming:
                        self._packets_received += 1
                        self._storage.append(
                            IndoorinoPacket(
                                packet, ( str(address[0]), str(address[1]) )
                            )
                        )
                        self.sendChecksum(packet, address)

                    self._update = True

            except (socket.timeout, OSError):
                ## vv this prevents overflow vv      vv this check time for real ##
                if time.perf_counter() - start < 0 or time.perf_counter() > start + millis / 1000:
                    break
                continue


            # Pass incoming bytes to decoder in order to obtain packets

    def sendChecksum(self, pack, address):
        p = {'name':pack['name'],
             'command':IBACOM_ACK,
             'data': { 'value': ipstools.checksum(pack) }
             }
        self.send(p, address)


    def write(self, array, address):

        if self._status:
            debug('Sending UDP {} bytes'.format(len(array)))

            try:
                self.client.sendto(bytes(array), tuple(address))
            except OSError as error:
                if self._status:
                    print("ERROR: send: unable to send packet via socket. Error: {}".format(error.strerror))
                    self.disconnect()

    def send(self, packet, address):

        if isinstance(packet, (list, tuple)):
            for item in packet:
                self.send(item, address)

        elif isinstance(packet, IndoorinoPacket):
            self.send(packet.toIpacket(), address)

        elif isinstance(packet, dict):
            iba = ipstools.forge(packet)
            try:
                self.write(iba, address)
            except:
                print('ERROR:UDP:SEND:IPSTOOL:FAILED!')
        else:
            print('Warning - UDP:Send received a invalid packet')

