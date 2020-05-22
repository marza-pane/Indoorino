from utils.utils import debug

class AmbientValue:

    def __init__(self, name, unit='-', minval=0.0, maxval=0.0):

        self.name = name
        self.unit = unit
        self.vmin = minval
        self.vmax = maxval

        self._value = 0.0

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, x):

        if not self.vmin == self.vmax:
            if x <= self.vmin or x >= self.vmax:
                # here generate event?
                debug('WARNING:AmbientValue: {} out of range ({})'.format(self.name, x))
                pass

        self._value = x


class VariableTemperature(AmbientValue):

    def __init__(self):
        AmbientValue.__init__(self, 'temperature', unit='K', minval=0.0, maxval=1000.0)

    def getCelsious(self):
        return self._value - 273.15

    def setCelsious(self, x):
        self.value = x + 273.15


class VariableHumidiy(AmbientValue):

    def __init__(self):
        AmbientValue.__init__(self, 'humidity', unit='RH', minval=0.0, maxval=100.0)

class VariableLight(AmbientValue):

    def __init__(self):
        AmbientValue.__init__(self, 'light', unit='Lux', minval=0.0, maxval=100000.0)

class VariablePH(AmbientValue):

    def __init__(self):
        AmbientValue.__init__(self, 'PH', unit='-', minval=0.0, maxval=12.0)
