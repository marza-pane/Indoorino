from common.templates import *
from widgets.homemap import HomeMap


class DataViewer(PanedTemplate):
    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)


class UiWeather(PanedTemplate):

    class Station(PanedTemplate, BoardLinkTemplate):

        def __init__(self, parent, boardname, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            BoardLinkTemplate.__init__(self, boardname)
            self.labels=dict()
            self.status_widget=None

        def build(self, *args, **kwargs):

            self.labels.update(
                {
                    'title':LabelTemplate(
                        self,
                        font=Fonts.monobold(13),
                        anchor=tk.W,
                        padx=20,
                        relief=tk.FLAT,
                        text='cucco'
                    )
                }
            )

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