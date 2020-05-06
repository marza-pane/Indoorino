from libraries.indoorino_serial import *
from libraries.indoorino_udp import *
from libraries.boardModel import *

class PacketRouter:

    class Flags:
        def __init__(self):
            self._update_status = False
            self._update_packet = False
            self._update_device = False
            self._update_current = False


        @property
        def update_status(self):
            value = self._update_status
            self._update_status = False
            return value
        @property
        def update_packet(self):
            value = self._update_packet
            self._update_packet = False
            return value
        @property
        def update_device(self):
            value = self._update_device
            self._update_device = False
            return value
        @property
        def update_current(self):
            value = self._update_current
            self._update_current = False
            return value

    class Connections:
        def __init__(self):
            self.status = False        ## True = any connection is on
            self.serial = False        ## True = any serial connection

    def __init__(self):

        self._connected_devices=dict()
        self._rx_database =list()
        self._tx_database =list()
        self._pending     =list()

        self.connections = self.Connections()
        self.flags = self.Flags()
        self.flags._update_packet = True
        self.flags._update_device = True

        self._current_device='' # hidden var is the devices dictionary key as string


        self.serial    =IndoorinoSerialReader()
        self.udpserver =IndoorinoUdpReader()

    @property
    def current_device(self):
        if self._current_device in self._connected_devices.keys():
            return self._connected_devices[self._current_device]
        else:
            return BoardModel('','')

    @current_device.setter
    def current_device(self, value):

        self.flags._update_current = True
        if isinstance(value, str):
            self._current_device = value
        elif (isinstance(value, int)):
            self._current_device = tuple(self._connected_devices.keys())[value]
        debug('\tCURRENT DEVICE SET : {}'.format(self._current_device))

        # if isinstance(value, int):
        #     if len(self._connected_devices.keys()) > value:
        #         self._current_device = list(self._connected_devices.keys())[value]
        #         debug('\tCURRENT DEVICE SET : {}'.format(self._current_device))
        # elif isinstance(value, float):
        #     self.current_device = int(value)
        # elif isinstance(value, str):
        #     if value in self._connected_devices.keys():
        #         self._current_device = value
        #         debug('\tCURRENT DEVICE SET : {}'.format(self._current_device))

    # @property
    # def update(self):
        # u =self._to_update
        # self._to_update =False
        # return u

    @property
    def connected(self):
        return self.connections.status
    @property
    def devices(self):
        return self._connected_devices
    @property
    def rx_database(self):
        return self._rx_database
    @property
    def tx_database(self):
        return self._tx_database

    def clear_database(self):
        self._rx_database = list()
        self._tx_database = list()

    def connect(self):
        print('Connecting...')
        self.serial.connect()
        self.udpserver.connect()

    def disconnect(self):
        print('Disconnection..')
        self.serial.disconnect()
        self.udpserver.disconnect()


    def loop_200(self):

        if self.serial.status != self.connections.serial:
            self.connections.serial = self.serial.status
            self.flags._update_status = True

        if not self.connections.serial:
            if self.udpserver.status != self.connections.status:
                self.connections.status = self.udpserver.status
                self.flags._update_status = True

        self.connections.status = self.serial.status or self.udpserver.status

    def loop_100(self):

        if len(self._pending) > 0:
            print('\n\n\t\t||||| SENDING ON LOOP |||||\n')
            packet, address = self._pending[0]
            self.flags._update_packet = True
            self._tx_database.append(packet)
            if address.isSerial:
                print('Sending Serial {} --> {}'.format(packet.label, address.path))
                self.serial.send(packet)
            else:
                print('Sending UDP {} --> [ip:{} port:{}]'.format(packet.label, address.ip, address.port))
                self.udpserver.send(packet, address())
            self._pending.pop(0)

    def loop(self):

        ### Check for new packets ###
        self.serial.read(0)
        for item in self.serial.storage:
            self.parse(item)

        self.udpserver.read()
        for item in self.udpserver.storage:
            self.parse(item)




    def parse(self, packet):

        ### Parse received packets ###
        print('Parsing {} from {}'.format(packet.payload, packet.address))
        self.flags._update_packet = True
        self._rx_database.append(packet)
        if len(self._rx_database) > 5000:
            self._rx_database.pop(0)


        ### Check for new boards ###

        key = packet.name

        if packet.name in self._connected_devices.keys() and packet.address() == self._connected_devices[key].address():
            pass
        else:
            self.flags._update_device = True
            if not packet.name in self._connected_devices.keys():
                self._connected_devices[key] = BoardModel(packet.name, packet.address)
            elif packet.address.isSerial and not self._connected_devices[key].address.isSerial:
                key += '_SERIAL'
                self._connected_devices[key] = BoardModel(packet.name, packet.address)
            elif not packet.address.isSerial and self._connected_devices[key].address.isSerial:
                key += '_WIFI'
                self._connected_devices[key] = BoardModel(packet.name, packet.address)
            else:
                print('\n\n!!! ERROR - DIFFERENT ADDRESSES old[{} | {}]new FOR NAME {} !!!\n'.format(
                    packet.address,
                    self._connected_devices[key].address,
                    key))
                return

        self._connected_devices[key].parse(packet)

        ### Digest the packet ###
        self.process(packet)


    def process(self, packet):
        # send packet to the proper module
        pass


    def send(self, packet, broadcast=False, current=False, *args):

        # possible combination if not broadcasted:

        # 1) send(<IdoorinoPacket>)
        # IdoorinoPacket.address  will be used.

        # 2) send(<IdoorinoPacket>, <BoardAddress>) or send(<IdoorinoPacket>, (<BoardAddress>,<BoardAddress>) )
        # IdoorinoPacket will be ignored.

        # 3) send(<dict>) or send(<dict>, <BoardAddress> ) or send(<dict>, (<BoardAddress>,<BoardAddress>) )
        # dict['name'] will be compared against connected devices name list

        targets = list()

        if current:
            if len(self._current_device) > 0:
                return self.send(packet, self.current_device.address)
            else:
                print('\tERROR!!! - no CURRENT DEVICE to send.')

        if broadcast:
            for key, item in self._connected_devices.items():
                targets.append(item.address)

        # First append all targets as a list of BoardAddress
        elif args:
            p = args[0]
            if isinstance(p, (list, tuple)):
                for s in p:
                    if isinstance(s, BoardAddress): targets.append(s)
                    else:
                        raise TypeError('\tERROR!!! - IN LIST Invalid ADDRESS type {}'.format(type(p)))
            elif isinstance(p, BoardAddress):
                targets.append(p)
            else:
                raise TypeError('\tERROR!!! - Invalid ADDRESS type {}'.format(type(p)))
        else:

            if isinstance(packet, IndoorinoPacket):
                targets.append(packet.address)
            elif isinstance(packet, dict) and validate_packet(packet):

                packet = IndoorinoPacket(packet, '/dev/null')
                if len(targets) == 0:
                    # here I got a dict with no address so I lookup connected_devices
                    for key, item in self._connected_devices.items():
                        if item.name == packet['name']:
                            print('Found target device in lookup !!')
                            targets.append(item.address)
                            packet = IndoorinoPacket(packet, item.address)
                            break

            else:
                raise TypeError('\tERROR!!! - Invalid PACKET type {}'.format(type(packet)))


        for t in targets:
            self._pending.append((packet,t,))



router = PacketRouter()

if __name__ == "__main__":

    # m = {'command': 0}
    # p = {
    #     'name': 'TESTER',
    #     'command': 15,
    #     'plsize': 84,
    #     'data': {
    #         'epoch': 1234,
    #         'message':'This is the message'
    #     }
    # }
    #
    # router.send(m)
    # router.send(p)
    # router.send(p)

    pass












































# if current:
#     if len(self._current_device) > 0:
#         return self.send(packet, self.current_device.address)
#     else:
#         print('\tERROR!!! - no CURRENT DEVICE to send')
#         return
# if broadcast:
#     for key, item in self._connected_devices.items():
#         if os.path.exists(item.address):
#             address.append(item.address)
#         else:
#             a = item.address.split(':')
#             address.append((a[0], int(a[1])))
# elif args:
#     par = args[0]
#     if validate_address(par):
#         address.append((par[0], par[1]))
#     elif isinstance(par, (tuple, list)):
#         for i in par:
#             return self.send(packet, i)
#     elif isinstance(par, UdpConnectionConfig):
#         address.append((par.ip, par.port))
#     elif isinstance(par, SerialConnectionConfig):
#         address.append(par.device)
#     elif len(args) == 2 and validate_address(par, args[1]):
#         address.append((par, args[1]))
#     elif isinstance(par, str):
#         ip = par.split(':')
#         if os.path.exists(par):
#             address.append(par)
#         elif validate_address(ip[0], int(ip[1])):
#             address.append((ip[0], ip[1]))
#     else:
#         print('WARNING !!! Cant send {} to {}'.format(format_dict(packet), par))
#         return
# else:
#     if isinstance(packet, (tuple, list)):
#         for p in packet:
#             self.send(p, broadcast, *args)
#     if isinstance(packet, dict):
#         self.send(IndoorinoPacket(packet, ''), *args)
#     elif isinstance(packet, IndoorinoPacket):
#         name = packet.name
#         print('Sending to {}'.format(packet.address))
#         if len(packet.address) == 0:
#             for key, item in self._connected_devices.items():
#                 if item.name == name:
#                     print('Found target device :):) !!')
#                     if os.path.exists(item.address):
#                         address.append(item.address)
#                     else:
#                         a = item.address.split(':')
#                         address.append((a[0], a[1]))
#
#         else:
#             if isinstance(packet.address, str):
#                 if os.path.exists(packet.address):
#                     address.append(packet.address)
#                 else:
#                     a = packet.address.split(':')
#                     address.append((a[0], int(a[1])))
#             elif isinstance(packet.address, (list, tuple)):
#                 address.append(packet.address)
#             else:
#                 print('WARNING !!! Cant send {} to NO ONE'.format(format_dict(packet)))
#     else:
#         print('WARNING !!! Cant send {} to NO ONE'.format(format_dict(packet)))
#         return
#
# for target in address:
#     if isinstance(target, (str, bytes)) and os.path.exists(target):
#         print('Sending Serial {} --> {}'.format(packet.label, target))
#         self.serial.send(packet)
#     elif validate_ip(target[0]) and validate_udport(target[1]):
#         print('Sending UDP {} --> [ip:{} port:{}]'.format(packet.label, target[0], target[1]))
#         self.udpserver.send(packet, target)
#     else:
#         print('ERROR: invalid target {}'.format(target))
