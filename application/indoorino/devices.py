import datetime

from indoorino.parameters import *
# from indoorino.packet import IndoorinoPacket
from common.comtable import *

class DeviceProbes:

    def __init__(self):
        self._db = dict()

    def __call__(self, *args, **kwargs):

        if args:
            if args[0] in self._db.keys():

                return tuple([datetime.datetime.fromtimestamp(epoch) for epoch in self._db[args[0]].keys()]), \
                       tuple([float(value)/Config.macros.FLOAT2INT for value in self._db[args[0]].values()])

        else:
            return self._db


    def parse(self, packet):

        if packet.command == IBACOM_SYS_PROBE_DATA:
            if len(packet.payload['desc1']):
                if not packet.payload['desc1'] in self._db.keys():
                    self._db.update({
                        packet.payload['desc1']: dict()
                    })

                e = packet.payload['table1']
                v = packet.payload['table2']

                if not (isinstance(e, bytearray) and isinstance(v, bytearray)):
                    print(format_type(e))
                    print(format_type(v))
                    return

                for i in range (0, int(1024/4), 4): # <<< da mettere 1024 in Config.macros.SRV_PROBE_PACKET_SIZE
                                                    # <<< e volendoo anche 4 come SIZEOF_INT(32)

                    current_epoch = int.from_bytes(e[i: i + 4], byteorder='little', signed=True)

                    if current_epoch == 0:
                        break

                    current_value = int.from_bytes(v[i: i + 4], byteorder='little', signed=True)

                    self._db[packet.payload['desc1']].update({
                        current_epoch: current_value
                    })


        elif packet.command == IBACOM_PROBE_AMBIENT:

            for i in range(4):
                desc = packet.payload['desc{}'.format(i + 1)]
                value = packet.payload['value{}'.format(i + 1)]
                if len(desc):
                    if not desc in self._db.keys():
                        self._db.update({
                            desc:dict()
                        })

                    self._db[desc].update({
                        packet.payload['epoch']:value
                    })


class DeviceParameters:

    class _DevParam:

        class _Entry(dict):
            def __init__(self, *args, **kwargs):
                dict.__init__(self, *args, **kwargs)
                self.packet=IndoorinoPacket()

        def __init__(self):
            self.std = self._Entry()
            self.dev = self._Entry()

    def __init__(self, boardname, name, pin):

        self.config = self._DevParam()
        self.status = self._DevParam()

        self.config.std.update(
            {
                'devtype' : ParameterString(
                    name='type',
                    label='device type',
                    desc='device type',
                    value='NONE'
                ),
                'name' : ParameterString(
                    name='devname',
                    label='device name',
                    desc='device name',
                    value=name
                ),
                'boardname' : ParameterString(
                    name='name',
                    label='board name',
                    desc='board attached to',
                    value=boardname
                ),
                'pin' : ParameterInt(
                    name='pin1',
                    label='device pin',
                    desc='main pin',
                    value=pin
                ),

            }
        )
        self.status.std.update(
            {
                'status': ParameterString(
                    name='status',
                    label='device status',
                    desc='device connection status',
                    value='OFFLINE'
                ),
            }
        )
        self.status.dev.update(
            {
                'lastupdate': ParameterDatetime(
                    name='last',
                    label='last update ',
                    desc='system time @ last packet',
                    value=0
                ),
            }
        )

    def update(self):

        for key, item in self.config.std.items():
            if not key == 'devtype' and not key == 'lastupdate':
                item.set(self.config.dev.packet.payload[item.name])

        for key, item in self.config.dev.items():
            item.set(self.config.dev.packet.payload[item.name])
            # try:
            #     pass
            # except KeyError:
            #     if self.config.std.packet.payload['type'] and len(self.config.dev.keys()) == 0:
            #         self.set_board_type(self.config.std.packet.payload['type'])

        for key, item in self.status.dev.items():
            try:
                item.set(self.status.dev.packet.payload[item.name])
            except KeyError as e:
                # print('MISS! : {}'.format(e))
                continue


class IndoorinoDevice(DeviceParameters):

    # STATUS:
    # 0 = no errors
    # 1 = invalid pin
    # 2 = offline
    # 3 = io error
    # 4 = type error

    def __init__(self, boardname, name, pin):
        DeviceParameters.__init__(self, boardname, name, pin)
        self._timeout=time.perf_counter()
        self._probes=DeviceProbes()

    @property
    def probes(self):
        return self._probes

    @property
    def name(self):
        return self.config.std['name'].data

    @property
    def boardname(self):
        return self.config.std['boardname'].data

    @property
    def pin(self):
        return self.config.std['pin'].data

    @property
    def devtype(self):
        return self.config.std['devtype'].data

    def is_connected(self):
        if self.status.std['status'].data == 'OFFLINE':
            return False
        return True

    def set_offline(self):
        self.status.std['status'].set('OFFLINE')
        Config.flags.update.DEVICES = True

    def set_type(self, packet):     ## mod this when adding sensors

        """
            types:
                "RELAY"
                "SWITCH"
                "FLOODSW"
                "RAINSW"
                "DHT22"
                "DUSTPM25"
                "LDR"
                "SERVO"
                "STEPPER"
        """

        self.config.dev.update(
            {
                'name': ParameterString(
                    name='devname',
                    label='device name',
                    desc='device name',
                    value=packet.payload['devname']
                ),
                'boardname': ParameterString(
                    name='name',
                    label='board name',
                    desc='board attached to',
                    value=packet.payload['name']
                ),
                'pin': ParameterInt(
                    name='pin1',
                    label='device pin',
                    desc='main pin',
                    value=packet.payload['pin1']
                ),
            }
        )

        if packet.command == IBACOM_CONF_ASENSOR:
            self.config.std['devtype'].set('ASENSOR')


        elif packet.command == IBACOM_CONF_RELAY:
            self.config.std['devtype'].set('RELAY')
            self.status.dev.update(
                {
                    'relay_state': ParameterInt(
                        name='level',
                        label='relay state',
                        desc='relay coil state',
                        # value=packet.payload['level']
                    ),
                }
            )

        elif packet.command == IBACOM_CONF_FLOODSWITCH:
            self.config.std['devtype'].set('FLOODSW')
            self.status.dev.update(
                {
                    'switch_state': ParameterInt(
                        name='level',
                        label='switch state',
                        desc='switch state',
                        # value=packet.payload['level']
                    ),
                }
            )

        elif packet.command == IBACOM_CONF_DUSTPM25:
            self.config.std['devtype'].set('DUSTPM25')
            self.config.std.update(
                {
                    'analpin': ParameterInt(
                        name='pin2',
                        label='analogical pin',
                        desc='analogical board pin',
                    ),
                    'coeffA': ParameterInt(
                        name='param1',
                        label='linear K',
                        desc='voltage to dust density linear constant',
                    ),
                    'coeffB': ParameterInt(
                        name='param2',
                        label='linear C',
                        desc='voltage to dust density correction',
                    ),
                    'alarm': ParameterInt(
                        name='param3',
                        label='dust limit',
                        desc='dust density alarm limit',
                    ),
                    'sampletime': ParameterInt(
                        name='timeout1',
                        label='sampling time',
                        desc='delay between led ON and measure',
                        unit='us',
                    ),
                    'deltatime': ParameterInt(
                        name='timeout2',
                        label='rest time',
                        desc='delay between measure and led OFF',
                        unit='us',
                    ),
                }
            )

            self.status.dev.update(

                {
                    'value': ParameterFloat(
                        name='value1',
                        label='dust density',
                        desc='dust density',
                        unit='ug/m3',
                    ),
                }
            )

        elif packet.command == IBACOM_CONF_DHT22:
            self.config.std['devtype'].set('DHT22')
            self.config.std.update(
                {
                    'coeffAT': ParameterInt(
                        name='param1',
                        label='alpha temp.',
                        desc='temperature linear correction',
                    ),
                    'coeffAH': ParameterInt(
                        name='param3',
                        label='alpha humi.',
                        desc='humidity linear correction',
                    ),
                    'coeffBT': ParameterInt(
                        name='param2',
                        label='beta temp.',
                        desc='temperature correction',
                    ),
                    'coeffBH': ParameterInt(
                        name='param2',
                        label='beta humi.',
                        desc='humidity correction',
                    ),
                }
            )

            self.status.dev.update(

                {
                    'temperature': ParameterTemperature(
                        name='value1',
                        label='temperature',
                        desc='air temperature',
                        limits=(10, 30)
                    ),
                    'humidity': ParameterRHumidity(
                        name='value2',
                        label='humidity',
                        desc='air humidity',
                        limits=(10, 90)
                    ),
                }
            )

        elif packet.command == IBACOM_CONF_DEVSTD:
            self.config.std['devtype'].set('DEFAULT')

        else:
            warning_os('Could not set device {}:{} with packet {}'.format(
                self.config.std['boardname'].value, self.config.std['name'].value, packet.description))

    def loop(self):
        if self.is_connected() and time.perf_counter() > self._timeout:
            self.set_offline()

    def parse(self, packet):

        # if not isinstance(packet, IndoorinoPacket): return
        if packet.command in (IBACOM_SYS_PROBE_DATA, IBACOM_PROBE_AMBIENT,):
            if packet.payload['board'] == self.boardname and packet.payload['devname'] == self.name:
                self._probes.parse(packet)

        if 'name' in packet.payload.keys() and 'devname' in packet.payload.keys():

            if packet.payload['devname'] == self.name and packet.payload['name'] == self.boardname:

                if packet.source == self.boardname:
                    self._timeout = time.perf_counter() + Config.macros.TIMEOUT_BOARD
                    self.status.std['status'].set('ONLINE')

                if IBACOM_CONF_ASENSOR <= packet.command <= IBACOM_CONF_DEVSTD:

                    if self.config.dev.packet.command == 0:
                        info_devices('{}:{} setting config.board to {}'.format(self.boardname, self.name, packet.label))
                        self.config.dev.packet.build(packet.command, packet.source, dict())
                    elif self.config.dev.packet.command != packet.command:
                        warning_devices('illegal replace {} CONFIG type'.format(self.name))
                        return

                    self.config.dev.packet.payload = packet.payload

                    if self.config.std['devtype'].data == 'NONE':
                        self.set_type(packet)

                    self.update()

                elif IBACOM_STATUS_ASENSOR <= packet.command <= IBACOM_STATUS_DEVSTD:
                    if self.status.dev.packet.command == 0:
                        info_devices('{}:{} setting config.device to {}'.format(self.boardname, self.name, packet.label))
                        self.status.dev.packet.build(packet.command, packet.source, dict())
                    elif self.status.dev.packet.command != packet.command:
                        warning_devices('illegal replace {} CONFIG type'.format(self.name))
                        return
                    self.status.dev.packet.payload = packet.payload
                    self.status.dev['lastupdate'].set(datetime.datetime.now())
                    self.update()

                    if 'status' in packet.payload.keys():
                        s = packet.payload['status']
                        if s == 0:
                            self.status.std['status'].set('ONLINE')
                        elif s == 1:
                            self.status.std['status'].set('INVALID PIN')
                        elif s == 2:
                            self.status.std['status'].set('IO ERROR')
                        elif s == 3:
                            self.status.std['status'].set('CORRUPTED DATA')
                        elif s == 4:
                            self.status.std['status'].set('TYPE ERROR')
                        else:
                            self.status.std['status'].set('UNDEFINED')

                return True

        return False


# class DeviceRelay(IndoorinoDevice):
#
#     def __init__(self, boardname, name, pin):
#         IndoorinoDevice.__init__(self,boardname,name,pin)
#         self.devtype.value = 'RELAY'
#         self.status.desc = 'Relay status'
#         self.status.value = 2
#         self.pin.desc = 'Relay pin'
#         self.name.desc = 'Relay name'
#
#         self.relay_state = ParameterInt(
#             name='level',
#             label='Relay state',
#             desc='Relay ON/OFF parameter',
#             value=0
#         )
#
#     def parse(self, packet):
#         if super(DeviceRelay, self).parse(packet):
#             if packet.command == IBACOM_CONF_RELAY:
#                 return True
#             elif packet.command == IBACOM_STATUS_RELAY:
#                 self.status.value = packet.payload['status']
#                 self.relay_state.value = packet.payload['level']
#                 return True
#         return False

print('Loaded indoorino.devices')
