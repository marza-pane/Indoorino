import datetime

TIMEOUT_BOARD = 60

class BoardTemplate:
    """
        Virtual Board template
            - father all board classes
    """
    class DefaultBoardParams:
        def __init__(self):
            pass

    class SamplerBoardParams:
        """
            Sampler is the default weather station
                - can attach up to 20 sensors
                - log data to SD
                - drop data via serial
        """
        def __init__(self):
            self.step=0
            self.cooldown=0
            self.nextsample=0
            self.totalsample=0

    class ESPserverBoardParams:
        """
            Roter data from Arduino to the net
                - can attach sensors
                - drop data via wifi
        """
        def __init__(self):
            self.attemps=0
            self.timeout=0
            self.totaltx=0
            self.totalrx=0
            self.udptx=0
            self.udprx=0

        @property
        def serialtx(self):
            return  self.totaltx - self.udptx

        @property
        def serialrx(self):
            return self.totalrx - self.udprx

    class BoardSDTemplate:
        """ SD module parametrs """
        def __init__(self):
            self.sd_flag=False
            self.status=False
            self.size=0
            self.free=0
            self.type=''

    def __init__(self, name, devtype='', boardtype=''):

        self._name = name
        self._devtype = devtype
        self._boardtime = datetime.datetime.fromtimestamp(0)
        self._boardlag = datetime.timedelta(seconds=datetime.datetime.now().timestamp())

        self.boardtype = boardtype
        self.looptime = 0
        self.freeram = 0
        self.boarderrors = 0

        self._status_time = datetime.datetime.now()

        self.parameters = self.DefaultBoardParams()
        self.sd = self.BoardSDTemplate()
        self.devices = dict()

    @property
    def name(self):
        return self._name

    @property
    def type(self):
        return self._devtype

    @type.setter
    def type(self, devtype):
        if devtype == 'SAMPLER':
            self.parameters = self.SamplerBoardParams()
            self.sd.sd_flag = True

    @property
    def status(self):
        seconds = (datetime.datetime.now() - self._status_time).seconds
        if (seconds > TIMEOUT_BOARD):
            return False
        else:
            return True


    def parse(self, packet):
        if not packet.name == self.name:
            return
        self._status_time = datetime.datetime.now()
        pass