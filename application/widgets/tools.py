from common.templates import *
import numpy as np

# class DeviceTools:
class AnalogicQuadrant(CanvasTemplate):

    def __init__(self, parent, **kwargs):

        CanvasTemplate.__init__(self, parent, **kwargs)

        self._value=0
        self.target=0.5
        self.limits=[0.1,0.2]
        self.alerts=[0.3,0.4]
        self.statics=dict()
        self.dynamic=dict()
        self.shapes = {
            'band_thick' : 0.075,
            'band_angle' : 200,
            'band_num' : 18,
        }
        self.labels=list()
        self.after(500, self.show_check)

    def set_value(self, val):
        self._value=val
        self.resize_indicator()

    def set_name(self, name):
        self.itemconfigure(
            self.labels[0],
            text=name
        )

    def set_label(self, label):
        self.itemconfigure(
            self.labels[1],
            text=label
        )

    def build(self, *args, **kwargs):
        self.bind_mouse_buttons()

        self.statics['outer'] = self.create_oval(
            (0, 0, 0, 0,),
            width=3,
            outline=Palette.generic.WHITE,
            fill=Palette.generic.BLACK,
        )
        self.statics['center'] = self.create_oval(
            (0, 0, 0, 0,),
            width=3,
            outline=Palette.generic.BLACK,
        )

        # angle = self.shapes['band_angle']
        # self.statics['inner'] = self.create_arc(
        #     (0, 0, 0, 0,),
        #     width=3,
        #     style=tk.ARC,
        #     start=270 - 0.5 * (360 - angle),
        #     extent=-angle
        # )

        self.statics['marks'] = list()
        for i in range(self.shapes['band_num'] + 2):
            self.statics['marks'].append(
                self.create_line(
                (0,0,0,0),
                width=3,
                fill=Palette.generic.WHITE,
            ))

        self.labels.append(
            self.create_text(
                (0,0),
                fill=Palette.generic.FG_DEFAULT,
                font=Fonts.mono(10),
                anchor=tk.S,
                text='DEVICE',
            )
        )
        self.labels.append(
            self.create_text(
                (0,0),
                fill=Palette.generic.FG_DEFAULT,
                font=Fonts.mono(10),
                anchor=tk.N,
                text='Analogic reader'
            )
        )

        self.dynamic['center'] = self.create_oval(
            (0, 0, 0, 0,),
            width=0,
            fill=Palette.generic.WARNING
        )

        self.dynamic['arrow'] = self.create_polygon(
            (0, 0, 0, 0, 0, 0),
            width=0,
            fill=Palette.generic.WARNING
        )

        self.dynamic['arc'] = self.create_arc(
            (0, 0, 0, 0,),
            outline=Palette.generic.ONLINE
        )

    def on_release(self, *event):
        super(AnalogicQuadrant, self).on_release()
        self.show_check()

    def show_check(self):
        n=60
        v=self._value
        self._value=0
        self.resize_indicator()
        time.sleep(0.5)

        for i in range(n):
            self._value = i / n
            self.resize_indicator()
            self.update()
            time.sleep(0.02)
        for i in range(n):
            self._value = 1 - (i + 1) / n
            self.resize_indicator()
            self.update()
            time.sleep(0.02)

        self._value=v
        self.resize_indicator()
        time.sleep(0.5)

    def resize_indicator(self):
        w=self.winfo_width()
        h=self.winfo_height()

        r = 12
        off = 3
        size = min(w - 2 * off, h - 2 * off) - 2.5 * w * self.shapes['band_thick']

        self.coords(
            self.dynamic['center'],
            (0.5 * w - r,
             0.5 * h - r,
             0.5 * w + r,
             0.5 * h + r,
             ))

        p_ang = 0.5 * self.shapes['band_angle'] - 90
        angle = p_ang - self.shapes['band_angle'] * self._value

        self.coords(
            self.dynamic['arrow'],(
                0.5 * (w - size * np.cos(np.deg2rad(angle))),
                0.5 * (h + size * np.sin(np.deg2rad(angle))),
                0.5 * w - r * np.cos(np.deg2rad(angle + 90)),
                0.5 * h + r * np.sin(np.deg2rad(angle + 90)),
                0.5 * w + r * np.cos(np.deg2rad(angle + 90)),
                0.5 * h - r * np.sin(np.deg2rad(angle + 90)),
            )
        )

        self.tag_raise(self.statics['center'])
        self.itemconfigure(
            self.dynamic['arc'],
            width=0.5 * w * self.shapes['band_thick'],
            style=tk.ARC,
            start= - (90 + 0.5 * (360 - self.shapes['band_angle'])),
            extent=angle - p_ang
        )


        if self._value < self.target - self.limits[0]:
            if self._value < self.target - self.alerts[0]:
                self.itemconfigure(
                    self.dynamic['arc'],
                    outline=Palette.generic.ERROR_GUI
                )
            else:
                self.itemconfigure(
                    self.dynamic['arc'],
                    outline=Palette.generic.WARNING
                )

        elif self._value > self.target + self.limits[1]:
            if self._value > self.target + self.alerts[1]:
                self.itemconfigure(
                    self.dynamic['arc'],
                    outline=Palette.generic.ERROR_GUI
                )
            else:
                self.itemconfigure(
                    self.dynamic['arc'],
                    outline=Palette.generic.WARNING
                )
        else:
            self.itemconfigure(
                self.dynamic['arc'],
                outline=Palette.generic.ON
            )

    def on_resize(self, *args, **kwargs):
        w,h=super(AnalogicQuadrant, self).on_resize()

        off=3
        size = min(w - 2 * off, h - 2 * off)
        band_thick = self.shapes['band_thick'] * size
        angle = 0.5 * self.shapes['band_angle'] - 90


        self.coords(
            self.statics['outer'],
            (0.5 * (w - size) , 0.5 * (h - size),
             0.5 * (w - size) + size,
             0.5 * (h - size) + size)
        )

        self.coords(
            self.statics['center'],
            (0.5 * w - off,
             0.5 * h - off,
             0.5 * w + off,
             0.5 * h + off,
             ))

        # self.coords(
        #     self.statics['inner'],
        #     (0.5 * (w - size) + band_thick,
        #      0.5 * (h - size) + band_thick,
        #      0.5 * (w - size) + (size - band_thick),
        #      0.5 * (h - size) + (size - band_thick))
        # )

        self.coords(
            self.dynamic['arc'],
            (0.5 * (w - size) + 0.5 * band_thick,
             0.5 * (h - size) + 0.5 * band_thick,
             0.5 * (w - size) + (size - 0.5 * band_thick),
             0.5 * (h - size) + (size - 0.5 * band_thick))
        )

        self.coords(
            self.statics['marks'][0],
            (0.5 * (w - size * np.cos(np.deg2rad(angle))),
             0.5 * ( h + size * np.sin(np.deg2rad(angle))),
             0.5 * (w + (2 * band_thick -size) * np.cos(np.deg2rad(angle))),
             0.5 * (h + (size - 2 * band_thick) * np.sin(np.deg2rad(angle))) ))

        self.coords(
            self.statics['marks'][1],
            (0.5 * (w + size * np.cos(np.deg2rad(angle))),
             0.5 * ( h + size * np.sin(np.deg2rad(angle))),
             0.5 * (w - (2 * band_thick - size) * np.cos(np.deg2rad(angle))),
             0.5 * (h + (size - 2 * band_thick) * np.sin(np.deg2rad(angle))) ))

        r_mark = size - ( 3 * band_thick)
        p_ang = 0.5 * self.shapes['band_angle'] - 90
        c_ang = self.shapes['band_angle'] / (len(self.statics['marks']) - 3)
        for count, i in enumerate(range(2, len(self.statics['marks']))):
            ang = p_ang - count * c_ang
            self.coords(
                self.statics['marks'][i],
                (0.5 * (w - r_mark * np.cos(np.deg2rad(ang))),
                 0.5 * (h + r_mark * np.sin(np.deg2rad(ang))),
                 0.5 * (w + (2 * band_thick - r_mark) * np.cos(np.deg2rad(ang))),
                 0.5 * (h + (r_mark - 2 * band_thick) * np.sin(np.deg2rad(ang)))))

        self.coords(
            self.labels[0],
            (0.5 * w,
             0.75 * h,
             )
        )

        self.coords(
            self.labels[1],
            (0.5 * w,
             0.75 * h,
             )
        )

        self.resize_indicator()

if __name__ == '__main__':

    app=tk.Tk()

    c=AnalogicQuadrant(app, bg='blue')
    c.build()

    c.pack(expand=tk.TRUE, fill=tk.BOTH)
    c.on_resize()
    c.set_name('SPEED')
    c.set_label('engine:piston1')
    app.mainloop()