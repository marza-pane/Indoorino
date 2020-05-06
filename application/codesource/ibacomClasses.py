from codesource.ibacomSource import *
from utils.utils import debug

import pathlib

class ibacomVariable:

    def __init__(self, name, size, check='', vmin='0', vmax='0'):

        self.name = name
        self.size = size
        self.check = check
        self.vmin = vmin
        self.vmax = vmax

class ibacomVariableMap(dict):

    def __init__(self, **kwargs):
        dict.__init__(self, **kwargs)

        for p in SRC_variables:
            self[p[0]] = ibacomVariable( p[0], p[1], p[2], p[3], p[4] )

class ibacomPacket:

    def __init__(self, command, name='', label='', fields=()):

        self.command = command
        self.name = name
        self.vars = dict()

        ibacomvarmap = ibacomVariableMap()

        for field in fields:
            if not field in ibacomvarmap.keys():
                raise KeyError('IBACOM:CODE:ERROR: no variable <{}> found for {} ({})'.format(field, name, command))
            else:
                self.vars[field] = ibacomvarmap[field]

        if len(label) == 0:
            self.label = ' '.join(self.name.split()[::-1])
        else:
            self.label = label

    def ibacomstring(self):
        return 'IBACOM_{}'.format(self.name.upper().replace(' ', '_'))

class ibacomPacketMap(dict):

    def __init__(self, **kwargs):

        dict.__init__(self, **kwargs)

        for p in SRC_packets:
            self['IBACOM_{}'.format(p[1].upper().replace(' ', '_'))] = ibacomPacket (
                p[0], name=p[1], label=p[2], fields=p[3]
            )

    def __getitem__(self, item):

        if isinstance(item, int):
            for key, obj in self.items():
                if obj.command == item:
                    return self.__getitem__(key)
            raise KeyError('IBACOM:CODE:ERROR: no IBACOM found for {}'.format(item))
        else:
            return super(ibacomPacketMap, self).__getitem__(item)


def print_ibacomtable():

    def print_list_line(com='', name='', label='', vars=''):
        s = '| {0:<20} |'.format(com) + ' {0:<24} |'.format(name) + ' {0:<20} |'.format(label)
        if isinstance(vars, str):
            s += ' {0:<20}'.format(vars)
        elif isinstance(vars, dict):
            if len(vars.keys()) == 0:
                s += ' --- '
            else:
                s += ' ['
                for k in vars.keys():
                    s+='{}, '.format(k)
                s = list(s)
                s[-2] = ']'
                s = ''.join(s)
        else:
            raise TypeError('print_ibacomtable: invalid variable type {} ({})'.format(vars, type(vars)))

        print(s)

    print('\n\tIBACOM PACKET LIST:\n')
    print_list_line('COMMAND', 'NAME', 'LABEL', 'VARIABLES')
    print_list_line()
    for k, i in ibacomPacketMap().items():
        print_list_line(i.command, i.name, i.label, i.vars)

def print_vartable():

    def print_list_line(name='', size='', check='', vmin='', vmax=''):
        s = '| {0:<20} |'.format(name) + ' {0:<24} |'.format(size) + ' {0:<20} |'.format(check) + \
            ' {0:<30} |'.format(vmin) + ' {0:<30}'.format(vmax)
        print(s)

    print('\n\tIBACOM VARIABLE LIST:\n')
    print_list_line('NAME', 'SIZE', 'CHECK', 'MIN VALUE', 'MAX VALUE')
    print_list_line()

    for k, i in ibacomVariableMap().items():
        print_list_line(name=k, size=i.size, check=i.check, vmin=i.vmin, vmax=i.vmax)

if __name__ == '__main__':

    print_ibacomtable()
    print_vartable()