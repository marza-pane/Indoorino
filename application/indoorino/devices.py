from indoorino.parameters import *

class GenericDeviceTemplate:

    def __init__(self, board='', name='', pin=0):

        self.board = board
        self.name = name
        self.pin = pin
        self.state = False

        self.label = ''


class GenericSensor(GenericDeviceTemplate):

    def __init__(self, **kwargs):

        GenericDeviceTemplate.__init__(self, **kwargs)
        self.value = AmbientValue('value')

class DeviceDHT22(GenericDeviceTemplate):

    def __init__(self, **kwargs):

        GenericDeviceTemplate.__init__(self, **kwargs)
        self.temperature = VariableTemperature()
        self.humidity = VariableHumidiy
        self.label = 'temperature/humidity'

class DevicePH(GenericDeviceTemplate):

    def __init__(self, **kwargs):

        GenericDeviceTemplate.__init__(self, **kwargs)
        self.ph = VariablePH()
        self.label = 'PH reader'

class DeviceLDR(GenericDeviceTemplate):

    def __init__(self, **kwargs):

        GenericDeviceTemplate.__init__(self, **kwargs)
        self.ph = VariablePH()
        self.label = 'light resistor'

