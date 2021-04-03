import datetime
import inspect

_RED= "\033[1;31m"
_GRN= "\033[1;32m"
_YLW= "\033[1;33m"
_BLU= "\033[1;34m"
_PUR= "\033[1;35m"
_AZR= "\033[1;36m"
_TRS= "\033[0m"

#_______________________________________________________________#
#                                                               #
#           debug and errors utils                              #
#_______________________________________________________________#
#

def _stack_anal():
    c=list()
    try:
        s = inspect.stack()[2::][::-1]
    except IndexError:
        s = inspect.stack()
    except RecursionError as error:
        print('\n{}Recusion error while analyzing stack <{}> !{}'.format(_RED, error, _TRS))
        return '{}'.join(c).strip('\n')
    for i in s:
        if str(i[3]) == '__call__': continue
        if str(i[3]) == '<module>': continue
        try:
            if 'super' in i[4][0]:
                p = str(i[4]).split('(')[1].split(',')[0]
                c .append('{}:'.format(p))
                continue
            p = '{}:'.format(i[3])
            if len(c) > 0 and c[-1] != p:
                c.append(p)
        except TypeError:
            c.append('###:')
    return '{}'.join(c).strip('\n')

def _debug_helper(preamble, string, **kwargs):

    c = (str(preamble),
         f'{datetime.datetime.now().strftime(" %y-%m-%d %H:%M:%S ")}',
         _stack_anal(), string, _TRS)
    print(''.join(c), flush=True, **kwargs)


def debug(string, **kwargs):
    if 1:
        _debug_helper('DEBUG:', string, **kwargs)

def debug_network(string, **kwargs):
    if 1:
        _debug_helper('DEBUG:NET:', string, **kwargs)

def debug_connection(string, **kwargs):
    if 1:
        _debug_helper('DEBUG:CONN:', string, **kwargs)

def debug_packet(string, **kwargs):
    if 1:
        _debug_helper('DEBUG:PACK:', string, **kwargs)

def debug_ui(string, **kwargs):
    if 0:
        _debug_helper('DEBUG:UI:', string, **kwargs)

def debug_gui(string, **kwargs):
    if 1:
        _debug_helper('DEBUG:UI:', string, **kwargs)


def error_packet(string, **kwargs):
    _debug_helper('{}ERROR:PACKET:'.format(_RED), string, **kwargs)

def error_connection(string, **kwargs):
    _debug_helper('{}ERROR:CONNECTION:'.format(_RED), string, **kwargs)

def error_network(string, **kwargs):
    _debug_helper('{}ERROR:NETWORK:'.format(_RED), string, **kwargs)

def error_os(string, **kwargs):
    _debug_helper('{}ERROR:SYSTEM:'.format(_RED), string, **kwargs)

def error_boards(string, **kwargs):
    _debug_helper('{}ERROR:BOARDS:'.format(_RED), string, **kwargs)

def error_devices(string, **kwargs):
    _debug_helper('{}ERROR:DEVICES:'.format(_RED), string, **kwargs)

def error_database(string, **kwargs):
    _debug_helper('{}ERROR:DATABASE:'.format(_RED), string, **kwargs)

def error_ui(string, **kwargs):
    _debug_helper('{}ERROR:UI:'.format(_RED), string, **kwargs)


def warning_packet(string, **kwargs):
    _debug_helper('{}WARNING:PACKET:'.format(_YLW), string, **kwargs)

def warning_connection(string, **kwargs):
    _debug_helper('{}WARNING:CONNECTION:'.format(_YLW), string, **kwargs)

def warning_network(string, **kwargs):
    _debug_helper('{}WARNING:NETWORK:'.format(_YLW), string, **kwargs)

def warning_os(string, **kwargs):
    _debug_helper('{}WARNING:SYSTEM:'.format(_YLW), string, **kwargs)

def warning_boards(string, **kwargs):
    _debug_helper('{}WARNING:BOARDS:'.format(_YLW), string, **kwargs)

def warning_devices(string, **kwargs):
    _debug_helper('{}WARNING:DEVICES:'.format(_YLW), string, **kwargs)

def warning_database(string, **kwargs):
    _debug_helper('{}WARNING:DATABASE:'.format(_YLW), string, **kwargs)

def warning_ui(string, **kwargs):
    _debug_helper('{}WARNING:UI:'.format(_YLW), string, **kwargs)


def info_packet(string, **kwargs):
    _debug_helper('{}INFO:PACKET:'.format(_AZR), string, **kwargs)

def info_connection(string, **kwargs):
    _debug_helper('{}INFO:CONNECTION:'.format(_AZR), string, **kwargs)

def info_network(string, **kwargs):
    _debug_helper('{}INFO:NETWORK:'.format(_AZR), string, **kwargs)

def info_os(string, **kwargs):
    _debug_helper('{}INFO:SYSTEM:'.format(_AZR), string, **kwargs)

def info_boards(string, **kwargs):
    _debug_helper('{}INFO:BOARDS:'.format(_AZR), string, **kwargs)

def info_devices(string, **kwargs):
    _debug_helper('{}INFO:DEVICES:'.format(_AZR), string, **kwargs)

def info_database(string, **kwargs):
    _debug_helper('{}INFO:DATABASE:'.format(_AZR), string, **kwargs)


def alert_packet(string, **kwargs):
    _debug_helper('{}ALLERT:PACKET:'.format(_PUR), string, **kwargs)

def alert_connection(string, **kwargs):
    _debug_helper('{}ALLERT:CONNECTION:'.format(_PUR), string, **kwargs)

def alert_network(string, **kwargs):
    _debug_helper('{}ALLERT:NETWORK:'.format(_PUR), string, **kwargs)

def alert_os(string, **kwargs):
    _debug_helper('{}ALLERT:SYSTEM:'.format(_PUR), string, **kwargs)

def alert_boards(string, **kwargs):
    _debug_helper('{}ALLERT:BOARDS:'.format(_PUR), string, **kwargs)

def alert_devices(string, **kwargs):
    _debug_helper('{}ALLERT:DEVICES:'.format(_PUR), string, **kwargs)

def alert_database(string, **kwargs):
    _debug_helper('{}ALLERT:DATABASE:'.format(_PUR), string, **kwargs)

print('Loaded common.debug')

