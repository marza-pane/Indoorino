
from utils.imports import *
from utils.definitions import *

#_______________________________________________________________#
#                                                               #
#           format utils                                        #
#_______________________________________________________________#
#                                                               #

def debug(string):
    if DEBUG_FLAG:  print(string)

def format_timespan(delta, short=False, seconds=True):

    if delta.total_seconds() < 0:
        string = 'EARLY '
    else:
        string = 'LATE '

    mins, secs = divmod(abs(delta.total_seconds()), 60)
    hurs, mins = divmod(mins, 60)
    days, hurs = divmod(hurs, 24)

    if days > 0:
        if short:
            string += '{}d -'.format(int(days))
            string += '{}:'.format('%02i' % int(hurs))
            string += '{}'.format('%02i' % int(mins))
        else:
            string += '{} days '.format(int(days))
            string += '{} hours '.format(int(hurs))
            string += '{} mins'.format(int(mins))

    elif hurs > 0:
        if short:
            string += '{}:'.format('%02i' % int(hurs))
            string += '{}'.format('%02i' % int(mins))
        else:
            string += '{} hours '.format(int(hurs))
            string += '{} mins'.format(int(mins))

    elif mins > 0:
        if short:
            string += '{}'.format('%02i' % int(mins))
        else:
            string += '{} mins '.format(int(mins))

    if seconds:
        if short:
            string += ':{}'.format('%02i' % int(secs))
        else:
            string += '{} sec'.format(int(secs))

    return string

def format_bytearray(array):

    e_string = list()
    d_string = list()

    e_string.append('[')
    d_string.append('[')

    d = list(array.decode(errors='ignore'))

    for n, c in enumerate(list(array)):
        e_string.append(str(c).zfill(3))
        e_string.append('|')
        if n < len(d):
            d_string.append(' ')
            if str(d[n]).isprintable():
                d_string += '\'{}\''.format(d[n])
            else:
                d_string += str(c).zfill(3)
            d_string += ' '
            d_string += '|'

        if (n % 32) == 0 and n > 0:
            d_string += '\n'
            d_string += '\n'

    d_string[-1] = ']'
    e_string[-1] = ']'
    return ''.join(e_string), ''.join(d_string)

def format_debug(array):

    r=list()
    for c in list(list(array.decode(errors='ignore'))):
        if c.isprintable() or c in ('\n', '\t'):
            r.append(c)

    return ''.join(r)

def format_packet(packet):


    code = ''
    template = ('___>>>_PACKET_RECEIVED:_{label}\n|    FROM : {name}|\n|    DATA :')

    fields = []
    for key in ['label','command','data_size','full_size', 'name']:
        try:
            fields.append(packet[key])
        except KeyError:
            if key == 'label':
                fields.append('### UNKNOWN PACKET ###')
            else:
                fields.append('---')

    label = '{0:<64}'.format('{} ibacom:{} [{}:{}]'.format(
        fields[0], fields[1], fields[2], fields[3]).upper()).replace(' ', '_')
    name = '{0:<76}'.format(fields[4])

    code += template.format(label=label, name=name)
    code += '{0:<77}|'.format('')
    # for item in packet.items():
    #     if item[0] == 'data':
    #         continue
    #     code += ('\n|    {}: {}'.format(item[0],item[1]))
    # code += ('\n|')
    for item in packet['data'].items():
        if item[0] == 'epoch':
            code += '{0:<89}|'.format('\n|          {}: {} ({})'.format(
                item[0],
                item[1],
                datetime.datetime.fromtimestamp(item[1])))
        else:
            code += '{0:<89}|'.format('\n|          {}: {}'.format(item[0], item[1]))
    code += ('\n|{0:<82}_<<<_|'.format('')).replace(' ', '_')

    return code
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
        else:
            try:
                iter(dictionary)
                r += format_list(item, tabs)
            except TypeError:
                r += '<{}>'.format(item)
    return r

def kelvin(temperature):    return temperature + 273.15

def celsius(temperature):   return temperature - 273.15

#_______________________________________________________________#
#                                                               #
#           folder and files utils                              #
#_______________________________________________________________#
#                                                               #

def generate_path(*path):

    p=pathlib.Path(DEFAULT_FOLDER_PATH)

    try:

        if len(path) == 1:
            if isinstance(path[0], pathlib.PurePath):
                p = path[0]
            elif isinstance(path[0], str):
                p = pathlib.Path(path[0])

        if p.is_dir():
            print('path:generate: folder {} found'.format(p.absolute()))
        else:
            print('path:generate: creating new folder {}'.format(p.absolute()))
            os.mkdir(p.absolute(), 0o777)

        for sub in DEFAULT_SUBFOLDER_NAME:
            if not p.joinpath(sub).is_dir():
                print('path:generate: creating new subfolder {}'.format(p.absolute()))
                os.mkdir(p.joinpath(sub).absolute(), 0o777)

    except OSError as error:

        print('path:generate:OSError: {}'.format(error))
        return generate_path()

    print('path:generate: all data files are stored in {}'.format(p))
    return p

def list_folders(path):

    def _local_folder_scan_(parent, local_path):

        # new_path = [f.path for f in os.scandir(local_path) if f.is_dir()]  # Thanks SO stranger

        new_path = [pathlib.Path(f) for f in os.scandir(local_path) if f.is_dir()]
        if len(new_path):
            parent += new_path
            for item in new_path:
                parent = _local_folder_scan_(parent, item)
        return parent

    sub_folders = list()
    if path.is_dir():
        sub_folders = _local_folder_scan_(sub_folders, path)
    return sub_folders

def list_folder_files(path):
    files = list()
    if path.is_dir():
        for entry in os.listdir(path):
            if path.joinpath(entry).is_file():
                files.append(path.joinpath(entry))

    return files

#_______________________________________________________________#
#                                                               #
#           network utils                                       #
#_______________________________________________________________#
#

def get_localip():
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.connect(("8.8.8.8", 80))
        ip = sock.getsockname()[0]
        sock.close()
        return ip
    except OSError:
        return ''

def validate_ip(ip):
    try:
        socket.inet_aton(ip)
    except socket.error:
        debug('validate_ip: invalid IP: {}'.format(ip))
        return False
    return True

def validate_port(port):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        code = s.connect_ex((socket.gethostbyname('localhost'), port))
        s.close()
        if code == 0:
            return True
    except (OverflowError, TypeError):
        print('WARNING:validate_port: INVALID PORT {}'.format(port))
    return False

if __name__ == "__main__":

    pass

# lst =  ['1234', 'hello', '6787']
# [x for x in lst if x.isalpha()]
# =>  ['hello']
#
# Or if you definitely want to use a lambda:
#
# filter(lambda x: x.isalpha(), lst)
# =>  ['hello']