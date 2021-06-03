from common.utils import *
from indoorino.packet import IndoorinoPacket
# from common.templates import BoardLinkTemplate, DeviceLinkTemplate

class ParameterTemplate:

    def __init__(self, **kwargs):

        self.name = kwargs.pop('name','UNDEFINED')
        self.label = kwargs.pop('label','UNDEFINED')
        self.desc = kwargs.pop('desc','UNDEFINED')
        self.unit = kwargs.pop('unit','')
        self.value = 'NONE'

        self.data = None
        self.tags = kwargs.pop('tags', list())
        self._last_update = datetime.datetime.fromtimestamp(0)

    @property
    def last_update(self):
        return self._last_update

    def set(self, value):
        warning_os('ParameterTemplate None value to [{}]: {}'.format(format_type(value), value))
        self.data = value
        self.value = (self.__str__())

    def __str__(self):
        return str(self.data)

    def __repr__(self):
        s = '{}:{}'.format(self.label, self.__str__())
        if self.unit:
            s+=':{}'.format(self.unit)

    # def __call__(self, *args, **kwargs):
    #     return self.data
    def __eq__(self, other):
        return self.data.__eq__(other)

class ParameterInt(ParameterTemplate):

    def __init__(self, **kwargs):
        ParameterTemplate.__init__(self, **kwargs)
        self.data=int(kwargs.pop('value', 0))
        self.limits=kwargs.pop('limits', list())
        self.value = self.__str__()

    def __str__(self):
        return '{0:<8}'.format('%u' % self.data )

    def set(self, value):
        self._last_update = datetime.datetime.now()
        try:
            self.data = int(value)
            self.value = self.__str__()
        except TypeError:
            warning_os('Can not set INT value with [{}]:{}'.format(format_type(value), value))

class ParameterFloat(ParameterTemplate):

    def __init__(self, **kwargs):
        ParameterTemplate.__init__(self, **kwargs)
        self.data=float(kwargs.pop('value', 0))
        self.limits=kwargs.pop('limits', list())
        self.value = self.__str__()

    def __str__(self):
        return '{0:<8}'.format('%3.3f' % (self.data / Config.macros.FLOAT2INT) )

    def set(self, value):
        self._last_update = datetime.datetime.now()
        try:
            self.data = float(value)
            self.value = self.__str__()
        except TypeError:
            warning_os('Can not set FLOAT value with [{}]:{}'.format(format_type(value), value))

class ParameterString(ParameterTemplate):

    def __init__(self, **kwargs):
        ParameterTemplate.__init__(self, **kwargs)
        self.data=str(kwargs.pop('value', '---'))
        self.value = self.__str__()

    def set(self, value):
        if isinstance( value, str ):
            self.data = value
        elif isinstance( value, (int, float) ):
            self.data = str(value)
        elif isinstance(value, (bytearray, bytes)):
            self.data = str(value.decode(errors='ignore'))
        else:
            try:
                self.data = str(value)
            except TypeError:
                warning_os('Can not set STR value with [{}]:{}'.format(format_type(value), value))
                return

        self.value = self.data

class ParameterDatetime(ParameterTemplate):

    def __init__(self, **kwargs):
        ParameterTemplate.__init__(self, **kwargs)
        self.data = datetime.datetime.fromtimestamp(int(kwargs.pop('value', 0)))
        self.limits = kwargs.pop('limits', (FIRST_DATE, LAST_DATE))
        self.value = self.__str__()

    def __str__(self):
        return self.data.strftime('%H:%M:%S')

    def timestring(self):
        return self.data.strftime('%b %d %Y - %H:%M:%S')

    @property
    def time(self):
        return self.data.time()
    @property
    def date(self):
        return self.data.date()

    def __eq__(self, other):
        if isinstance(other, (int, float)):
            return self.data.__eq__(datetime.datetime.fromtimestamp(int(other)))
        else:
            return self.data.__eq__(other)

    def set(self, value):
        self._last_update = datetime.datetime.now()
        if isinstance(value, (int, float)):
            try:
                self.data = datetime.datetime.fromtimestamp(int(value))
            except ValueError:
                error_os('Can not set DATETIME value with [{}]:{}'.format(format_type(value), value))

        elif isinstance(value, datetime.datetime):
            self.data = value
        else:
            warning_os('Can not set DATETIME value with [{}]:{}'.format(format_type(value), value))
        self.value = self.__str__()

class ParameterAddress(ParameterTemplate):

    def __init__(self, **kwargs):
        ParameterTemplate.__init__(self, **kwargs)
        self.data = list(kwargs.pop('value', ('0.0.0.0',  0)))
        self.value = self.__str__()

    def __str__(self):
        return format_address(self.data)

    @property
    def ip(self):
        return self.data[0]
    @ip.setter
    def ip(self, value):
        self.data[0] = value
        self.value = self.__str__()

    @property
    def port(self):
        return self.data[1]
    @port.setter
    def port(self, value):
        self.data[1] = value
        self.value = self.__str__()

    def set(self, value):
        if isinstance(value, (list, tuple)):
            self.data = list(value)
            self.value = self.__str__()
        elif isinstance(value, str):
            self.data = [value, 0]
            self.value = self.__str__()
        else:
            warning_os('Can not set ADDRESS value with [{}]:{}'.format(format_type(value), value))

class ParameterBytes(ParameterTemplate):

    def __init__(self, **kwargs):
        ParameterTemplate.__init__(self, **kwargs)
        self.data = tuple(kwargs.pop('value', bytearray()))
        self.value = self.__str__()

    def __str__(self):
        return str(bytearray(self.data).decode(errors='ignore'))


    def set(self, value):
        self._last_update = datetime.datetime.now()
        try:
            if isinstance(value, str):
                self.data = bytearray(value.encode())
                self.value = self.__str__()
            elif isinstance(value, (bytearray, bytes)):
                self.data = bytearray(value)
                self.value = self.__str__()
            else:
                raise Exception
        except Exception as error:
            warning_os('Error setting BYTES value with [{}]:{}'.format(format_type(value), value))
            print(error)

class ParameterTemperature(ParameterFloat):

    def __init__(self, **kwargs):
        limits=kwargs.pop('limits', (373.15, 273.15))
        unit=kwargs.pop('unit', '°C')
        ParameterFloat.__init__(self, limits=limits, unit=unit, **kwargs)

    def __str__(self):
        return '{0:<8}'.format('%3.3f' % (self.data / Config.macros.FLOAT2INT) )

class ParameterRHumidity(ParameterFloat):

    def __init__(self, **kwargs):
        limits=kwargs.pop('limits', (100, 0))
        unit=kwargs.pop('unit', 'RH')
        ParameterFloat.__init__(self, limits=limits, unit=unit, **kwargs)

    def __str__(self):
        return '{0:<8}'.format('%3.3f' % (self.data / Config.macros.FLOAT2INT) )



print('Loaded server.parameters [available keywords are : name, label, desc, unit, value, tags, limits]')


