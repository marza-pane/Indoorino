from source import *

packet_comments = (
    (   0,  500, 'system command'),
    ( 501, 1000, 'devices command'),
    (1001, 1300, 'board config'),
    (1301, 2000, 'device config'),
    (2001, 2300, 'board status'),
    (2301, 3000, 'device status'),
    (3001, 3200, 'alarms'),
    (3201, 3500, 'requests'),
    (3501, 4000, 'payloads'),
    (5001, 7000, 'network'),
    (7001, 8000, 'client comunication'),
)

class CodeSourceVariables:

    class Variable:
        def __init__(self, name, size, checkstring, value_min, value_max):
            self.name = name
            self.size = size
            self.vmin = value_min
            self.vmax = value_max
            self.check = checkstring

    def __init__(self):

        self.generic = dict()
        self.network = dict()
        self.server  = dict()

        for packet in variable_source_standard:
            self.generic[packet[0]] = self.Variable( packet[0],
                                                     packet[1],
                                                     packet[2],
                                                     packet[3],
                                                     packet[4] )

        for packet in variable_source_network:
            self.network[packet[0]]  = self.Variable( packet[0],
                                                     packet[1],
                                                     packet[2],
                                                     packet[3],
                                                     packet[4] )

        for packet in variable_source_server:
            self.server[packet[0]]  = self.Variable( packet[0],
                                                     packet[1],
                                                     packet[2],
                                                     packet[3],
                                                     packet[4] )

class CodeSourcePacket:

    class Packet:
        def __init__(self, varmap, command, name, label, fields=()):
            self.command = command
            self.name = name
            self.vars = dict()

            for count, entry in enumerate(fields):
                if not entry in (tuple(varmap.generic.keys()) + tuple(varmap.network.keys()) + tuple(varmap.server.keys())):
                    error = 'IBACOM:CODE:ERROR:{} no variable <{}> found for {} ({})'.format(count, entry, name, command)
                    raise KeyError(error)
                else:
                    if entry in varmap.generic.keys():
                        self.vars[entry] = varmap.generic[entry]
                    elif entry in varmap.network.keys():
                        self.vars[entry] = varmap.network[entry]
                    else:
                        self.vars[entry] = varmap.server[entry]

            if len(label) == 0:
                self.label = ' '.join(self.name.split()[::-1])
            else:
                self.label = label

        def ibacomstring(self):
            return 'IBACOM_{}'.format(self.name.upper().replace(' ', '_'))

    def __init__(self, ):

        self.generic = dict()
        self.network = dict()
        self.server  = dict()

        varmap = CodeSourceVariables()
        for packet in packet_source_standard:
            try:
                self.generic['IBACOM_{}'.format(packet[1].upper().replace(' ', '_'))] = self.Packet (varmap,
                    packet[0], name=packet[1], label=packet[2], fields=packet[3]
                )
            except IndexError:
                print('\n\t===> invalid source fields number: probably you missed a comma in packet definitions !')

        for packet in packet_source_network:
            try:
                self.network['IBACOM_{}'.format(packet[1].upper().replace(' ', '_'))] = self.Packet (varmap,
                    packet[0], name=packet[1], label=packet[2], fields=packet[3]
                )
            except IndexError:
                print('\n\t===> invalid source fields number: you missed a comma @ {} !'.format(packet[1]))

        for packet in packet_source_server:
            try:
                self.server['IBACOM_{}'.format(packet[1].upper().replace(' ', '_'))] = self.Packet (varmap,
                    packet[0], name=packet[1], label=packet[2], fields=packet[3]
                )
            except IndexError:
                print('\n\t===> invalid source fields number: probably you missed a comma in packet definitions !')


def print_ibacomtable():

    def print_list_line(com, name, label, vars):
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

    packmap = CodeSourcePacket()

    print('\n\tIBACOM PACKET LIST:\n')
    print_list_line('COMMAND', 'NAME', 'LABEL', 'VARIABLES')
    print_list_line('','','','')
    for entry in packmap.generic.values():
        print_list_line(entry.command, entry.name, entry.label, entry.vars)

    print_list_line('','','','')
    for entry in packmap.network.values():
        print_list_line(entry.command, entry.name, entry.label, entry.vars)

    print_list_line('','','','')
    for entry in packmap.server.values():
        print_list_line(entry.command, entry.name, entry.label, entry.vars)

def print_vartable():

    def print_list_line(name='', size='', check='', vmin='', vmax=''):
        s = '| {0:<20} |'.format(name) + ' {0:<24} |'.format(size) + ' {0:<20} |'.format(check) + \
            ' {0:<30} |'.format(vmin) + ' {0:<30}'.format(vmax)
        print(s)

    print('\n\tIBACOM VARIABLE LIST:\n')
    print_list_line('NAME', 'SIZE', 'CHECK', 'MIN VALUE', 'MAX VALUE')

    print_list_line()
    for key, entry in CodeSourceVariables().generic.items():
        print_list_line(name=key, size=entry.size, check=entry.check, vmin=entry.vmin, vmax=entry.vmax)

    print_list_line()
    for key, entry in CodeSourceVariables().network.items():
        print_list_line(name=key, size=entry.size, check=entry.check, vmin=entry.vmin, vmax=entry.vmax)
    print_list_line()

    for key, entry in CodeSourceVariables().server.items():
        print_list_line(name=key, size=entry.size, check=entry.check, vmin=entry.vmin, vmax=entry.vmax)

if __name__ == '__main__':

    print_vartable()
    print_ibacomtable()
