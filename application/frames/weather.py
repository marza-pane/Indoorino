from common.templates import *
from widgets.homemap import HomeMap

class UiWeather(PanedTemplate):

    # class

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        self.background=HomeMap(self)

    def build(self, *args, **kwargs):
        self.background.build()

    def on_resize(self, *args, **kwargs):
        w,h = super(UiWeather, self).on_resize()

        self.background.place(
            x=0,
            y=0,
            width=w,
            heigh=h
        )
        
        return w,h