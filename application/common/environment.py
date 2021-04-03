from common.utils import *
from indoorino.core import System
import pickle
import threading

class Environment:

    def __init__(self):

        self._connection = {
            'target' : False,
            'override' : False,
            'thread': threading.Thread() }

    @staticmethod
    def _get_data(filepath):
        try:
            handler = open(filepath, 'rb')
            data = pickle.load(handler)
            handler.close()
            return data
        except Exception as error:
            error_os('environment:Could not read {}\n{}'.format(filepath, error))
            exit(0)

    def _override(self):
        while self._connection['override']:
            System.io._is_online=self._connection['target']

    def stop_overriding(self):
        self._connection['override'] = False
        self._connection['thread'].join(10)

    def override_connection(self, value):
        self._connection['target'] = value
        if not self._connection['override']:
            self._connection['override'] = True
            self._connection['thread'] = threading.Thread(target=self._override)
            # self._connection['thread'].start()
            self._connection['thread'].run()
            if not value:
                System.boards.set_offline()

    def inject_at_startup(self, filepath):
        self.inject_in(filepath, 0)

        while len(System.io._buffer) > 0:
            print('.',end='')
            System.loop()

    def inject_in(self, filepath, secs):

        data = self._get_data(filepath)

        def _inject(packets, delay):
            for rxp in packets:
                time.sleep(delay)
                System.io._buffer.append(rxp)
                System.io._rx.append(rxp)

        t = threading.Thread(target=_inject, args=(data[0], secs))
        t.start()

environment = Environment()

if __name__ == '__main__':

    environment.inject_at_startup('/home/n00b/.indoorino/session/packets_welcome.ndo')