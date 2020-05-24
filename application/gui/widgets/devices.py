import tkinter as tk

from utils.utils import debug
from indoorino.devices import SensorDHT22
from gui.templates import PanedTemplate
from gui.utils import Fonts

class SensorWidgetTemplate(PanedTemplate):

    def __init__(self, parent, sensor, **kwargs):

        self.sensor = sensor
        PanedTemplate.__init__(self, parent, **kwargs)
        self.title = tk.Label(self)
        self.label = tk.Label(self)

        self.build()

    def build(self):
        super(SensorWidgetTemplate, self).build()
        self.title.configure(
            bg='green',
            padx=10,
            anchor=tk.W,
            font=Fonts.monobold(12),
            relief=tk.FLAT,
        )
        self.label.configure(
            bg='green',
            padx=8,
            anchor=tk.W,
            font=Fonts.monobold(12),
            relief=tk.FLAT,
        )


class SensorDHT22_Widget(SensorWidgetTemplate):

    def __init__(self, parent, sensor, **kwargs):

        SensorWidgetTemplate.__init__(self, parent, sensor, **kwargs)

    def build(self):
        super(SensorDHT22_Widget, self).build()
        self.title.configure(
            text='{} - DHT22 sensor'.format(self.sensor.name),
        )
        self.label.configure(
            text='{} on pin {}'.format(self.sensor.label, self.sensor.pin),
        )

    def on_resize(self):
        super(SensorDHT22_Widget, self).on_resize()
        w = self.winfo_width()
        h = self.winfo_height()
        self.title.place(
            x=min(10, 0.1 * w),
            y=min(5, 0.025 * h),
            width=w - 2 * min(10, 0.1 * w),
            height=min(30, 0.1*h),
        )
        self.label.place(
            x=min(10, 0.1 * w),
            y=min(h - min(30, 0.1*h), 0.025 * h),
            width=w - 2 * min(10, 0.1 * w),
            height=min(30, 0.1*h),
        )
