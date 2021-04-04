from frames.application import ApplicationWindow

from indoorino.core import System
print('{}'.format(System.io.tx_packets()))

from common.environment import environment

if __name__ == '__main__':

    environment.inject_at_startup('/home/n00b/.indoorino/session/packets_welcome.ndo')
    # environment.inject_in('/home/n00b/.indoorino/session/packets_welcome.ndo', 0)
    # environment.override_connection(True)
    app = ApplicationWindow()
    app.show()


