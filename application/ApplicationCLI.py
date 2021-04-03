from common.utils import *
from common.comtable import *
from indoorino.packet import IndoorinoPacket
from indoorino.core import IndoorinoCore

from signal import signal, SIGINT

def on_system_exit(sigcode, *args):
    info_os('exiting {} :'.format(sigcode))
    app.on_exit()
    print('\033[0m\n\n\tand see you in space cowboy\n\n')
    exit(sigcode)


if __name__ == '__main__':

    print('\n\t\t \033[1;36m** Indoorino {} client CLI start! **\033[0m\n'.format(Config.version()))

    app = IndoorinoCore()
    signal(SIGINT, on_system_exit)
    if not app.begin():
        error_network('Could not connect to server')
        on_system_exit(0)

    i = IndoorinoPacket()

    info_os('paket at init:')
    print(str(i))

    d= {
        'message': 'A python message',
        'name': 'PNAME',
        'level': 3
    }

    i.build(IBACOM_REPORT, 'SERVER', d)
    info_os('paket at build:')
    print(str(i))
    app.io.send(i)
    timeout=time.perf_counter() + 30
    stm=time.perf_counter() + 3
    while time.perf_counter() < timeout:
        app.loop()
        if time.perf_counter() > stm:
            stm=time.perf_counter() + 3
            app.io.send(i)
        # packet = dict()
        # if Client.fetch(packet):
        #     info_packet('GOT {}'.format(packet['label']))


    on_system_exit(0)






    # app = IndoorinoCore()
    #
    # if not app.begin():
    #     exit(0)
    #
    # i = IndoorinoPacket()
    #
    # info_os('packet at init:')
    # print(str(i))
    #
    # d= {
    #     'message': 'A python message',
    #     'name': 'PNAME',
    #     'level': 3
    # }
    #
    # i.build(IBACOM_REPORT, 'SERVER', d)
    # info_os('packet at build:')
    # print(str(i))
    # app.io.send(i)
    #
    # timeout=time.perf_counter() + 50
    # while time.perf_counter() < timeout:
    #     app.loop()
    #     packet = dict()
    #     if Client.fetch(packet):
    #         info_packet('GOT {}'.format(packet['label']))
    #
    #
    # time.sleep(5)
    #
    # Client.stop()






    # p={
    #     'command':IBACOM_REPORT,
    #     'source':'ANNSATSU',
    #     'target':'SERVER',
    #     'data':
    #         {
    #             'message':'A python message',
    #             'name':'PNAME',
    #             'level':3
    #         }
    # }
    # print(format_dict(p))
    # Client.ipacket(p)
    # print(format_dict(p))
