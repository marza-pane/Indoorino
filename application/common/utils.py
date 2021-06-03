from common.debug import *
from common.comtable import *

import common.IndoorinoModule as Client

import datetime
import pathlib
import time
import os

""" Macros and global variables """

FIRST_EPOCH = 1262300400  # 1 Jan 2010
LAST_EPOCH = 4102444800  # 1 Jan 2100

FIRST_DATE = datetime.datetime.fromtimestamp(FIRST_EPOCH)
LAST_DATE = datetime.datetime.fromtimestamp(LAST_EPOCH)

PATH_WS = str(pathlib.Path().home().joinpath('.indoorino').absolute())  # <-- dipende da sys.platform?
PATH_APP = '{}'.format(os.path.dirname(os.path.abspath(__file__ + '/..')))

DEFAULT_PATH_ICON=os.path.join(PATH_APP, '/gui/pictures/icons/not-found.png')

class ClockBase:

    def __init__(self, startime=0.0):

        self._current_datetime = datetime.datetime.fromtimestamp(0)
        self.set(startime)

        self._iterate_flag=False
        self._last_iterate=time.perf_counter()

    def set(self, startime):
        if isinstance(startime, (float,int)):
            self._current_datetime=datetime.datetime.fromtimestamp(startime)
        elif isinstance(startime, datetime.datetime):
            self._current_datetime=startime
        else:
            error_os('ClockBase set with <{}>'.format(format_type(startime)))

    def get(self):
        return self._current_datetime

    def start(self):
        self._iterate_flag = True

    def stop(self):
        self._iterate_flag = False

    def update(self):
        if self._iterate_flag:
            ima = time.perf_counter()
            delta = datetime.timedelta(seconds=(ima - self._last_iterate))
            self._last_iterate = ima
            self._current_datetime += delta
            return True
        return False

def format_type(obj):
    return str(type(obj)).strip('<class >').strip('\'')

def format_timespan(delta, short=False, seconds=True, micros=False):

    if delta.total_seconds() < 0:
        string = 'EARLY'
    else:
        string = 'LATE'

    mins, secs = divmod(abs(delta.total_seconds()), 60)
    hurs, mins = divmod(mins, 60)
    days, hurs = divmod(hurs, 24)

    if short and hurs == 0 and mins == 0 and micros:
        string += ' {}.{} sec'.format('%i' % int(secs), int(1e-2 * delta.microseconds))
        return string

    if short:
        if days > 0:    string += ' {} days - '.format(int(days))
        else:           string += ' - '.format(int(days))
    else:
        string += ' {} days'.format(int(days))

    if short:
        if hurs > 0:    string += '{}:'.format('%02i' % int(hurs))
        else:           string += '00:'
    else:
        string += ' {} hours'.format(int(hurs))

    if short:
        if mins > 0:    string += '{}:'.format('%02i' % int(mins))
        else:           string += '00:'
    else:
        string += ' {} mins'.format(int(mins))

    if short:
        if seconds:     string += '{}'.format('%02i' % int(secs))
        else:           string += '00'
    elif seconds:
        string += ' {} sec'.format(int(secs))

    if micros:
        if short:
            string += '.{}'.format('%i' % int(1e-2 * delta.microseconds))
        else:
            string += ' {} micros'.format(int(1e-2 * delta.microseconds))

    return string

def format_event(event, verbose=False):
    if not event:
        if verbose:
            print('No event')
        return
    else:
        print(event[0])
        if verbose:
            for entry in event[0].__dir__():
                if not str(entry).startswith('__'):
                    print('\t {0:<16}:'.format(entry), end='')
                    print('{}'.format(event[0].__getattribute__(entry)))

def format_list(iterable, tabs=0):

    r='[ iterable ]'
    tabs += 1

    for item in iterable:
        r += '\n'
        for t in range(tabs): r += '\t'

        if isinstance(item, str):
            r += item
        elif isinstance(item, dict):
            r += format_dict(item, tabs)
        elif isinstance(item, (bytearray, bytes)):
            r += str(item.decode(errors='ignore'))
            # r += format_readable(item)
        else:
            try:
                iter(item)
                r += format_list(item, tabs)
            except TypeError:
                r += '{}'.format(item)
    return r

def format_dict(dictionary, tabs=0):

    r='[ dict ]:'
    tabs += 1

    for key, item in dictionary.items():
        r += '\n'
        for t in range(tabs): r += '\t'
        r += '{0:<25}:'.format('<' + str(key) + '>')

        if isinstance(item, str):
            r += item
        elif isinstance(item, dict):
            r += format_dict(item, tabs)
        elif isinstance(item, (bytearray, bytes)):
            r += str(item.decode(errors='ignore'))
            # r += format_readable(item)[1]
        else:
            try:
                iter(dictionary)
                r += format_list(item, tabs)
            except TypeError:
                r += '<{}>'.format(item)
    return r

def format_address(address):

    if isinstance(address, (tuple, list)):
        string=''
        if len(address) > 0:
            for entry in address:
                string += '{}:'.format(entry)
            string = string[:-1]
        return string
    elif isinstance(address, (str, int, float)):
        return '{}'.format(address)

    return '<addressTypeError:{}>'.format(format_type(address))

#_______________________________________________________________#
#                                                               #
#           physic utils                                        #
#_______________________________________________________________#
#                                                               #

def kelvin(temperature):    return temperature + 273.15

def celsius(temperature):   return temperature - 273.15

#_______________________________________________________________#
#                                                               #
#           folder and files utils                              #
#_______________________________________________________________#
#                                                               #

def list_folders(path):

    def _local_folder_scan_(parent, local_path):

        # new_path = [f.path for f in os.scandir(local_path) if f.is_dir()]  # Thanks SO stranger

        new_path = [pathlib.Path(f) for f in os.scandir(local_path) if f.is_dir()]
        if len(new_path):
            parent += new_path
            for item in new_path:
                parent = _local_folder_scan_(parent, item)
        return parent

    if isinstance(path, str):
        path= pathlib.Path(path)
    sub_folders = list()
    if path.is_dir():
        sub_folders = _local_folder_scan_(sub_folders, path)
    return sub_folders

def list_folder_files(path):
    if isinstance(path, str):
        path= pathlib.Path(path)
    files = list()
    if path.is_dir():
        for entry in os.listdir(path):
            if path.joinpath(entry).is_file():
                files.append(path.joinpath(entry))

    return files

#_______________________________________________________________#
#                                                               #
#           Configuration tools                                 #
#_______________________________________________________________#
#

def _version():
    path = os.path.join(PATH_APP, '../build/.version')
    info_os('version path {}'.format(path))
    file = open(path, "r")
    data = str(file.read()).strip('\n')
    file.close()
    return data

class AppConfig:

    """ This class is stuffed at start up and holds C some macros defined in Indoorino C part"""

    class Macros:
        def __init__(self, conf):
            self.SERIAL_DEFAULT_BAUDRATE    = conf['macros']['SERIAL_DEFAULT_BAUDRATE']
            self.DEFAULT_SERVER_IP          = conf['macros']['DEFAULT_SERVER_IP']
            self.DEFAULT_SHELL_PORT         = conf['macros']['DEFAULT_SHELL_PORT']
            self.DEFAULT_BOARD_PORT         = conf['macros']['DEFAULT_BOARD_PORT']
            self.SHA_DIGEST_LENGTH          = conf['macros']['SHA_DIGEST_LENGTH']
            self.SIZEOF_PACKET_HEADER       = conf['macros']['SIZEOF_PACKET_HEADER']
            self.SIZEOF_NET_HEADER          = conf['macros']['SIZEOF_NET_HEADER']
            self.LEN_AES_MASTERKEY          = conf['macros']['LEN_AES_MASTERKEY']
            self.INET_ADDRSTRLEN            = conf['macros']['INET_ADDRSTRLEN']
            self.TIMEOUT_BOARD              = int( 1e-3 * conf['macros']['TIMEOUT_BOARD'])
            self.TIMEOUT_CLIENT             = int( 1e-3 * conf['macros']['TIMEOUT_CLIENT'])
            self.TIMEOUT_CLIENT_SHELL       = int( 1e-3 * conf['macros']['TIMEOUT_CLIENT_SHELL'])
            self.TIMEOUT_CLIENT_BOARD       = int( 1e-3 * conf['macros']['TIMEOUT_CLIENT_BOARD'])
            self.PREAMBLE                   = conf['macros']['PREAMBLE']
            self.FLOAT2INT                  = conf['macros']['FLOAT2INT']

    class Options:
        def __init__(self):
            self.SAVE_ON_EXIT=False
            self.LOAD_ON_START=False

    class Layout:

        class Lights(dict):
            """
             [group-name]   [board-name1]    [device1]      [ 'icon_type ]'
            """
            def __init__(self):
                dict.__init__(self, {})


            def parse(self, packet):
                bname=packet.payload['board']
                dname=packet.payload['devname']

                data = []
                if packet.payload['type'] == 'LEDBEAM':
                    data.append('beam-spot')

                if not packet.payload['label1'] in self.keys():
                    self.update({ packet.payload['label1'] : dict() })

                # self.__getitem__(packet.payload['label1'])
                if not bname in self.__getitem__(packet.payload['label1']).keys():
                    self.__getitem__(packet.payload['label1']).update({ bname : dict() })

                self.__getitem__(packet.payload['label1'])[bname].update({ dname : data })

        class Sensors(dict):
            """
             [group-name]   [board-name1]    [device1]      [ 'icon_type ]'
            """

            def __init__(self):
                dict.__init__(self, {})
                self.update(
                    {
                        'weather': {
                            # """ Outer perimeter boards """
                            'WEATHER.GARDEN': {
                                'DHT1': ['termometer', ],
                                'DHT2': ['termometer', ],
                                'DHT3': ['termometer', ],
                                'RAIN1': ['beam-spot', ],
                                'WIND1': ['beam-spot', ],
                                'LDR1': ['beam-spot', ],
                                'LDR2': ['beam-spot', ],
                            }
                        },
                        'home climate': {
                            # """ Inner perimeter boards """
                            'KITCHEN.WEATHER': {
                                'DHT1': ['termometer', ],
                                'DHT2': ['termometer', ],
                                'DHT3': ['termometer', ],
                            },
                            'BEDROOM.WEATHER': {
                                'DHT1': ['termometer', ],
                                'DHT2': ['termometer', ],
                                'DHT3': ['termometer', ],
                            }
                        },
                        'heat sensors': {
                            # """ Garden boards """
                            'HOUSE.BEAMS': {
                                'HEAT1': ['termometer', 'HEAT', ],
                            },
                            'BEDROOM.WEATHER': {
                                'HEAT1': ['termometer', 'HEAT', ],
                                'HEAT2': ['termometer', 'HEAT', ],
                                'HEAT3': ['termometer', 'HEAT', ],
                            },
                            'KITCHEN.WEATHER': {
                                'HEAT1': ['termometer', 'HEAT', ],
                                'HEAT2': ['termometer', 'HEAT', ],
                                'HEAT3': ['termometer', 'HEAT', ],
                            },
                            'WEATHER.GARDEN': {
                                'HEAT1': ['termometer', 'HEAT', ],
                                'HEAT2': ['termometer', 'HEAT', ],
                                'HEAT3': ['termometer', 'HEAT', ],
                            },
                            'HOME.LIGHTS': {
                                'HEAT1': ['termometer', 'HEAT', ],
                            },
                            'GARDEN.LIGHTS': {
                                'HEAT1': ['termometer', 'HEAT', ],
                            },
                        },

                    }
                )

        class Alarms(dict):
            """
             [alarm-type] [group-name] [board-name1] [device1]      [ 'icon_type ]'
            """

            def __init__(self):
                dict.__init__(self, {})
                # self.update(
                #     {
                #         'FLOOD': {
                #             'attic fire alarm': {
                #                 'HOUSE.BEAMS': {
                #                     'HEAT1': ['dht22',],
                #             },},},
                #     }
                # )

            def parse(self, packet):
                bname=packet.payload['board']
                dname=packet.payload['devname']

                group = packet.payload['label1']
                almtype = packet.payload['type']

                data = []

                if not packet.payload['type'] in self.keys():
                    self.update( { almtype: dict() } )

                if not group in self.__getitem__(almtype).keys():
                    self.__getitem__(almtype).update( { group: dict() } )

                if not bname in self.__getitem__(almtype)[group].keys():
                    self.__getitem__(almtype)[group].update( { bname : dict() } )

                self.__getitem__(almtype)[group][bname].update( { dname : data } )

        def __init__(self):
            self.devtypes = (
                'ANALOG',
                'RELAY',
                'DUSTPM25'
                'DHT22',
                'TIMER',
                'STEPPER',
                'SERVO'
            )
            # self.devices = dict()
            # self.lights=self.Lights()
            # self.sensors=self.Sensors()
            # self.alarms=self.Alarms()

        # def parse(self, packet):
        #
        #     try:
        #         key = '{}:{}'.format(packet.payload['board'], packet.payload['devname'])
        #     except KeyError:
        #         return
        #
        #     info_database('Layout:parse: packet {}'.format(packet))
        #     if packet.command == IBACOM_LYT_DEVICE:
        #         self.devices.update(
        #             {   key : {
        #                     'devtype'   : packet.payload['type'],
        #                     'area'      : packet.payload['label1'],
        #                     'location'  : packet.payload['label2'],
        #                 }
        #             }
        #         )
        #
        #     # key is something like <KITCHEN.WEATHER::DHT2>
        #
        #     elif packet.command == IBACOM_LYT_LIGHTS:
        #
        #         if not key in self.devices.keys():
        #             warning_os('Layout:lights: could not find {} in device dict'.format(key))
        #
        #         self.lights.parse(packet)
        #
        #     elif packet.command == IBACOM_LYT_ALARMS:
        #
        #         if not key in self.devices.keys():
        #             warning_os('Layout:lights: could not find {} in device dict'.format(key))

                # self.alarms.parse(packet)



    class Flags:

        class Template:

            def any(self):
                for key in self.__dir__():
                    if key[0].isupper(): ## and isinstance(self.__getattribute__(key), bool):
                        try:
                            if self.__getattribute__(key):
                                debug('Flags: returned TRUE on {}'.format(key))
                                return True
                        except AttributeError:
                            pass
                return False
            def all(self):
                for key in self.__dir__():
                    if key[0].isupper(): ## and isinstance(self.__getattribute__(key), bool):
                        try:
                            if not self.__getattribute__(key):
                                debug('Flags: returned TRUE on {}'.format(key))
                                return False
                        except AttributeError:
                            pass
                return True
            def clear(self):
                for key in self.__dir__():
                    if key[0].isupper(): ## and isinstance(self.__getattribute__(key), bool):
                        try:
                            self.__setattr__(key, False)
                        except AttributeError:
                            pass

        class Updates(Template):
            def __init__(self):
                self.NETWORK = False
                self.PACKET = False
                self.BOARD = False
                self.DEVICES = False
                self.SYSTEM = False


        def __init__(self):
            self.update = self.Updates()

        def clear(self):
            self.update.clear()

    class Version:

        def __init__(self, client_version):
            local_version = _version()
            if local_version != client_version:
                warning_os('Mismatching versions:\n\tclient version <{}>\n\tlocal version <{}>'.format(
                    client_version, local_version
                ))
            self.___version = client_version

        def __call__(self, *args, **kwargs):
            return self.___version

        def major(self):
            return int(self.___version.split('.')[0])

        def minor(self):
            return int(self.___version.split('.')[1])

    def __init__(self):

        appconf = dict()
        Client.config(appconf)

        ### If you want to load some option/conf do it here

        self.macros=self.Macros(appconf)
        self.options=self.Options()
        self.flags=self.Flags()
        self.layout=self.Layout()

        self.username='ANNSATSU'
        self.password='ONAKAGASUITA'

        self.version=self.Version(appconf.pop('version', ''))

    @property
    def server_ip(self):
        return self.macros.DEFAULT_SERVER_IP

    @property
    def server_port(self):
        return self.macros.DEFAULT_SHELL_PORT


def IndorinoTitle():

    return 'i n d o r i n o . ' + Config.version().replace('.',' . ')

Config = AppConfig()

print('Loaded common.utils')

if __name__ == '__main__':
    print('\nPATH is {}'.format(PATH_APP))
    print('WORKSPACE is {}'.format(PATH_WS))

    print('WORKSPACE is {}\n'.format(PATH_WS))
    print('PATH is {}'.format(PATH_APP))










# self.update(
                #     {
                #         'outer perimeter': {
                #             # """ Outer perimeter boards """
                #             'OUTER.BEAMS': {
                #                 'BEAM1': ['beam-spot',],
                #                 'BEAM2': ['beam-spot',],
                #                 'BEAM3': ['beam-spot',],
                #                 'BEAM4': ['beam-spot',],
                #             }
                #         },
                #         'inner perimeter': {
                #             # """ Inner perimeter boards """
                #             'HOUSE.BEAMS' : {
                #                 'BEAM1': ['beam-spot',],
                #                 'BEAM2': ['beam-spot',],
                #                 'BEAM3': ['beam-spot',],
                #                 'BEAM4': ['beam-spot',],
                #                 'BEAM5': ['beam-spot',],
                #                 'BEAM6': ['beam-spot',],
                #                 'BEAM7': ['beam-spot',],
                #                 'BEAM8': ['beam-spot',],
                #             },
                #             'HOME.LIGHTS': {
                #                 'BULB1': ['lightbulb', ],
                #                 'BULB2': ['lightbulb', ],
                #                 'BULB3': ['lightbulb', ],
                #                 'BULB4': ['lightbulb', ],
                #             }
                #         },
                #         'garden': {
                #             # """ Garden boards """
                #             'GARDEN.LIGHTS': {
                #                 'LAMP1': ['fluorescent'],
                #                 'LAMP2': ['fluorescent'],
                #                 'LAMP3': ['fluorescent'],
                #                 'LAMP4': ['fluorescent'],
                #             }
                #         },
                #
                #     }
                # )



