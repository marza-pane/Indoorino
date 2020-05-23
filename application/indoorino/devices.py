from indoorino.parameters import *

class GenericDeviceTemplate:

    def __init__(self, board='', name='', pin=0):

        self.board = board
        self.name = name
        self.pin = pin
        self.state = False

        self.label = ''


class SensorGeneric(GenericDeviceTemplate):

    def __init__(self, board, name, pin):

        GenericDeviceTemplate.__init__(self, board, name, pin)
        self.value = AmbientValue('value')

class SensorDHT22(GenericDeviceTemplate):

    def __init__(self, board, name, pin):

        GenericDeviceTemplate.__init__(self, board, name, pin)
        self.temperature = VariableTemperature()
        self.humidity = VariableHumidiy
        self.label = 'temperature/humidity'

class SensorPH(GenericDeviceTemplate):

    def __init__(self, board, name, pin):

        GenericDeviceTemplate.__init__(self, board, name, pin)
        self.ph = VariablePH()
        self.label = 'PH reader'

class SensorLDR(GenericDeviceTemplate):

    def __init__(self, board, name, pin):

        GenericDeviceTemplate.__init__(self, board, name, pin)
        self.ph = VariablePH()
        self.label = 'light resistor'

