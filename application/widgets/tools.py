from common.templates import *
import numpy as np
from tkinter import ttk

class AnalogicIndicatorWidget(CanvasTemplate):

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
        super(AnalogicIndicatorWidget, self).on_release()
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
        w,h=super(AnalogicIndicatorWidget, self).on_resize()

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

class AirFanWidget(CanvasTemplate):

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)

        self.statics=dict()
        self.blades=list()
        self.step=2
        self._running=False
        self._total=0

    def build(self, *args, **kwargs):

        for key in ('outer', 'inner'):
            self.statics.update(
                {
                    key:self.create_oval(
                        (0, 0, 0, 0,),
                        outline=Palette.generic.BLACK,
                        width=2
                    )
                }
            )

        for i in range(4):
            self.blades.append(
                self.create_polygon(
                    (0, 0, 0, 0, 0, 0, ),
                    smooth=tk.TRUE,
                    # fill=Palette.generic.WHITE,
                    fill='grey65',
                    outline=Palette.generic.BLACK,
                    width=2
                )
            )
        self.itemconfigure(
            self.statics['inner'],
            fill='grey55'
        )
        self.bind_mouse_buttons()
        self.bind_mouse_motion()
        self.loop()

    def start(self):
        self.step=3
        if self._running:
            return
        self.itemconfigure(
            self.statics['outer'],
            fill=Palette.generic.ONLINE
        )
        self._running=True

    def wait_update(self):
        self.step=1
        self.itemconfigure(
            self.statics['outer'],
            fill=Palette.generic.WARNING
        )

    def stop(self):
        if not self._running:
            return
        self.itemconfigure(
            self.statics['outer'],
            fill=Palette.generic.OFFLINE
        )
        self._running=False

    def loop(self):
        if self._running:
            self._total +=self.step
            if self._total >= 360:
                self._total=0
            self.draw_blades(self._total)

        self.after(20, self.loop)

    def on_release(self, *event):
        super(AirFanWidget, self).on_release()
        if self.current_id and \
                ( self.current_id[0] in self.statics.values() or self.current_id[0] in self.blades):
            if self._running:
                self.wait_update()
                self.after(2000,self.stop)
            else:
                self.wait_update()
                self.after(2000,self.start)

    def draw_blades(self, step=0):
        w=self.winfo_width()
        h=self.winfo_height()
        offset=10
        d_out=1.1 * min(w - offset, h - offset)
        d_in=max(10, 0.25 * d_out)
        b_angle=60

        c0=list()

        for i in [0, 90, 180, 270]:
            c0.append((
                0.5 * (w - d_out * np.cos(np.deg2rad(i + step))),
                0.5 * (h - d_out * np.sin(np.deg2rad(i + step))),
                0.5 * (w - d_in * np.cos(np.deg2rad(i + step))),
                0.5 * (h - d_in * np.sin(np.deg2rad(i + step))),
                0.5 * (w - d_in * np.cos(np.deg2rad(b_angle + i + step))),
                0.5 * (h - d_in * np.sin(np.deg2rad(b_angle + i + step))),
                0.5 * (w - d_out * np.cos(np.deg2rad(b_angle + i + step))),
                0.5 * (h - d_out * np.sin(np.deg2rad(b_angle + i + step))),
            ))
        for count, item in enumerate(self.blades):
            self.coords(
                item, c0[count]
            )

    def on_resize(self, *args, **kwargs):
        w,h=super(AirFanWidget, self).on_resize()
        offset=10
        d_out=min(w - offset, h - offset)
        d_in=max(10, 0.2 * d_out)

        self.coords(
            self.statics['outer'],(
                0.5 * (w - d_out),
                0.5 * (h - d_out),
                0.5 * (w - d_out) + d_out,
                0.5 * (h - d_out) + d_out,
            )
        )
        self.coords(
            self.statics['inner'],(
                0.5 * (w - d_in),
                0.5 * (h - d_in),
                0.5 * (w - d_in) + d_in,
                0.5 * (h - d_in) + d_in,
            )
        )

        self.draw_blades()

class TermometerWidget(CanvasTemplate):

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)

        self.statics=dict()
        self.fluid=list()
        self.options={
            'marks': 6,
            'd_bulb': 0.35,
            'ang_bulb': 65,
        }

        self._value=0.0
        self._delta=0

    def build(self, *args, **kwargs):
        super(TermometerWidget, self).build()
        for key in ('base_b', 'top_b', ):
            self.statics.update(
                {
                    key:self.create_oval(
                        (0, 0, 0, 0,),
                        outline=Palette.generic.BLACK,
                        fill=Palette.generic.BLACK,
                        # start=90 - 0.5 * self.options['ang_bulb'],
                        # extent= - (360 - self.options['ang_bulb'])
                    )
                }
            )

        for key in ('base_f', 'top_f'):
            self.statics.update(
                {
                    key:self.create_oval(
                        (0, 0, 0, 0,),
                        outline=Palette.generic.WHITE,
                        fill=Palette.generic.WHITE,
                        # start=90 - 0.5 * self.options['ang_bulb'],
                        # extent= - (360 - self.options['ang_bulb'])
                    )
                }
            )


        for key in ('body_b', 'body_f'):
            self.statics.update(
                {
                    key:self.create_rectangle(
                        (0, 0, 0, 0,),
                        outline=Palette.generic.BLACK,
                        fill=Palette.generic.BLACK,
                    )
                }
            )
        self.statics.update(
            {
                'marks':list()
            }
        )

        self.itemconfigure(
            self.statics['body_f'],
            outline=Palette.generic.WHITE,
            fill=Palette.generic.WHITE,
        )

        for i in range(self.options['marks']):
            self.statics['marks'].append(
                self.create_line(
                    (0, 0, 0, 0,),
                    fill=Palette.generic.BLACK,
                    width=5,
                )
            )
        self.fluid.append(
            self.create_rectangle(
                (0,0,0,0),
                width=0
            )
        )
        self.fluid.append(
            self.create_polygon(
                (0,0,0,0,0,0),
                outline=Palette.generic.BLACK,
                width=1
            )
        )
        self.set_online()

        self.after(1500, lambda : self.set_value(0.5))
        self.after(3000, self.set_offline)
        self.after(4500, lambda : self.set_value(0.1))

    def on_resize(self, *args, **kwargs):
        w,h=super(TermometerWidget, self).on_resize()
        offset=10
        d_out=min(w - offset, h - offset)
        d_bulb=self.options['d_bulb'] * d_out

        self.coords(
            self.statics['base_b'],(
                0.5 * (w - d_bulb),
                h - (offset + d_bulb),
                0.5 * (w + d_bulb),
                h - offset,
            )
        )
        self.coords(
            self.statics['body_b'],(
                0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.5 * self.options['ang_bulb']))),
                h - (offset + d_bulb * np.sin(np.deg2rad(90 - 0.5 * self.options['ang_bulb']))),
                0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.5 * self.options['ang_bulb']))),
                offset + 0.5 * d_bulb * np.cos(np.deg2rad(90 - 0.5 * self.options['ang_bulb'])),
            )
        )
        self.coords(
            self.statics['top_b'],(
                0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.5 * self.options['ang_bulb']))),
                offset + d_bulb * np.cos(np.deg2rad(90 - 0.5 * self.options['ang_bulb'])),
                0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.5 * self.options['ang_bulb']))),
                offset,
            )
        )

        d_bulb -= 0.05 * d_out
        offset += 0.025 * d_out

        self.coords(
            self.statics['base_f'],(
                0.5 * (w - d_bulb),
                h - (offset + d_bulb),
                0.5 * (w + d_bulb),
                h - offset,
            )
        )
        self.coords(
            self.statics['body_f'],(
                0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                h - (offset + d_bulb * np.sin(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                offset + 0.45 * d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb'])),
            )
        )
        self.coords(
            self.statics['top_f'],(
                0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                offset + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb'])),
                0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                offset,
            )
        )

        self.tag_raise(self.statics['base_f'])
        off_s = offset + 0.5 * d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))
        off_m = int((h - (off_s + d_bulb + offset)) / self.options['marks'])
        for count, mark in enumerate(self.statics['marks']):
            self.coords(
                mark,(
                    0.5 * w,
                    off_s + count * off_m,
                    0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    off_s + count * off_m,
                )
            )
        self.draw_fluid()

    def draw_fluid(self):
        w=self.winfo_width()
        h=self.winfo_height()
        offset=10
        d_out=min(w - offset, h - offset)
        d_bulb=self.options['d_bulb'] * 0.8750 * d_out

        offset += 0.025 * d_out

        off_s = offset + 0.5 * d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))
        off_m = int(h - (off_s + d_bulb + offset ))
        fluid_h = h - (offset + d_bulb + self._value * off_m)

        self.coords(
            self.fluid[0],(
                0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                h - (offset + 0.5 * d_bulb),
                0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                fluid_h
            )
        )

        self.tag_raise(self.fluid[0])
        for i in self.statics['marks']:
            self.tag_raise(i)

        if self._delta == 1:
            self.coords(
                self.fluid[1],(
                    0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 5,
                    0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 5,
                    0.5 * w,
                    fluid_h - 0.1 * d_out,
                )
            )
        elif self._delta == -1:
            print('HERE-1')
            self.coords(
                self.fluid[1],(
                    0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 0.1 * d_out,
                    0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 0.1 * d_out,
                    0.5 * w,
                    fluid_h - 5,
                )
            )
        else:
            print('HERE0')
            self.coords(
                self.fluid[1], (
                    0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 5,
                    0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 5,
                    0.5 * (w + d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 0.05* d_out,
                    0.5 * (w - d_bulb * np.cos(np.deg2rad(90 - 0.45 * self.options['ang_bulb']))),
                    fluid_h - 0.05* d_out,
                )
            )

        self.tag_raise(self.fluid[1])

    def set_offline(self):
        self.itemconfigure(
            self.statics['base_f'],
            fill=Palette.generic.OFFLINE
        )
        for item in self.fluid:
            self.itemconfigure(
                item,
                fill=Palette.generic.OFFLINE
            )

    def set_online(self):
        self.itemconfigure(
            self.statics['base_f'],
            fill=Palette.generic.ONLINE
        )
        for item in self.fluid:
            self.itemconfigure(
                item,
                fill=Palette.generic.ONLINE
            )

    def set_value(self, value):
        if value == self._value:
            if self._delta != 0:
                self._delta=0
                self.draw_fluid()
            return

        if value > self._value:
            self._delta = 1
            print('D=1')
        else:
            self._delta = -1
            print('D=-1')

        self._value=max(min(value, 1), 0)
        self.draw_fluid()

class PTZcontroller(CanvasTemplate):

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        self.buttons=dict()
        self.arrows=dict()

    def build(self, *args, **kwargs):

        for key in ('up', 'right', 'down', 'left', 'home', 'zoom+', 'zoom-', 'focus+', 'focus-'):
            self.buttons.update(
                {
                    key : self.create_polygon(
                        (0,0,0,0,0,0),
                        smooth=tk.TRUE,
                        outline=Palette.generic.BLACK,
                        width=4,
                        fill=Palette.generic.ERROR_GUI
                    )
                }
            )

            sign=key[0].upper()
            if key.endswith(('+','-')):
                sign += key[-1]

            self.arrows.update(
                {
                    key:self.create_text(
                        (0,0,),
                        anchor=tk.CENTER,
                        font=Fonts.monobold(13),
                        text=sign
                    )
                }
            )

        self.bind_mouse_motion()
        self.bind_mouse_buttons()

    def on_resize(self, *args, **kwargs):
        w,h=super(PTZcontroller, self).on_resize()

        length=min(w,h)
        offset=10
        size = int(length / 3) - 20

        """ First line """
        self.coords(
            self.buttons['zoom+'],
            self.coord_smooth_rectangle((
                0.5 * (w - size) - offset,
                offset,
                0.5 * (w - size) - (offset + size),
                offset + size,
            ), 3
            )
        )
        self.coords(
            self.buttons['up'],
            self.coord_smooth_rectangle((
                0.5 * (w - size),
                offset,
                0.5 * (w + size),
                offset + size,
            ), 3
            )
        )
        self.coords(
            self.buttons['zoom-'],
            self.coord_smooth_rectangle((
                0.5 * (w + size) + offset,
                offset,
                0.5 * (w + size) + offset + size,
                offset + size,
            ), 3
            )
        )

        self.coords(
            self.arrows['zoom+'],(
                0.5 * w  - (offset + size),
                offset + 0.5 * size,
            )

        )
        self.coords(
            self.arrows['up'],(
                0.5 * w,
                offset + 0.5 * size,
            )
        )
        self.coords(
            self.arrows['zoom-'],(
                0.5 * w  + (offset + size),
                offset + 0.5 * size,
            )
        )

        """ Second line """

        self.coords(
            self.buttons['left'],
            self.coord_smooth_rectangle((
                0.5 * (w - size) - offset,
                2 * offset + size,
                0.5 * (w - size) - (offset + size),
                2 * offset + 2 * size,
            ), 3
            )
        )
        self.coords(
            self.buttons['home'],
            self.coord_smooth_rectangle((
                0.5 * (w - size),
                2 * offset + size,
                0.5 * (w + size),
                2 * offset + 2 * size,
            ), 3
            )
        )
        self.coords(
            self.buttons['right'],
            self.coord_smooth_rectangle((
                0.5 * (w + size) + offset,
                2 * offset + size,
                0.5 * (w + size) + offset + size,
                2 * offset + 2 * size,
            ), 3
            )
        )

        self.coords(
            self.arrows['left'],(
                0.5 * w  - (offset + size),
                2 * offset + 1.5 * size,
            )
        )
        self.coords(
            self.arrows['home'],(
                0.5 * w,
                2 * offset + 1.5 * size,
            )
        )
        self.coords(
            self.arrows['right'],(
                0.5 * w  + (offset + size),
                2 * offset + 1.5 * size,
            )
        )

        """ Third line """

        self.coords(
            self.buttons['focus+'],
            self.coord_smooth_rectangle((
                0.5 * (w - size) - offset,
                3 * offset + 2 * size,
                0.5 * (w - size) - (offset + size),
                3 * offset + 3 * size,
            ), 3
            )
        )
        self.coords(
            self.buttons['down'],
            self.coord_smooth_rectangle((
                0.5 * (w - size),
                3 * offset + 2 * size,
                0.5 * (w + size),
                3 * offset + 3 * size,
            ), 3
            )
        )
        self.coords(
            self.buttons['focus-'],
            self.coord_smooth_rectangle((
                0.5 * (w + size) + offset,
                3 * offset + 2 * size,
                0.5 * (w + size) + offset + size,
                3 * offset + 3 * size,
            ), 3
            )
        )

        self.coords(
            self.arrows['focus+'],(
                0.5 * w  - (offset + size),
                3 * offset + 2.5 * size,
            )
        )
        self.coords(
            self.arrows['down'],(
                0.5 * w,
                3 * offset + 2.5 * size,
            )
        )
        self.coords(
            self.arrows['focus-'],(
                0.5 * w  + (offset + size),
                3 * offset + 2.5 * size,
            )
        )

    def on_press(self, *event):
        super(PTZcontroller, self).on_press(*event)
        if self.current_id:
            for key, item in self.buttons.items():
                if item == self.current_id[0]:
                    self.itemconfigure(
                        item,
                        fill=Palette.generic.WARNING
                    )
                    return
            for key, item in self.arrows.items():
                if item == self.current_id[0]:
                    self.itemconfigure(
                        self.buttons[key],
                        fill=Palette.generic.WARNING
                    )
                    return

    def on_release(self, *event):
        super(PTZcontroller, self).on_release(*event)
        if self.current_id:

            for key, item in self.buttons.items():
                if item == self.current_id[0]:
                    print('Cliccked on <{}>'.format(key))
                    self.itemconfigure(
                        item,
                        fill=Palette.generic.ERROR_GUI
                    )
                    return
            for key, item in self.arrows.items():
                if item == self.current_id[0]:
                    print('Cliccked on <{}>'.format(key))
                    self.itemconfigure(
                        self.buttons[key],
                        fill=Palette.generic.ERROR_GUI
                    )
                    return

class SoundMeter(CanvasTemplate):
    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)

        self.statics=dict()
        self.rays=list()
        self.options={
            'slices':80,
            'rings': 10,
            'gap' : 5,
            'd_bulb': 0.25,
            'ang_bulb': 65,
        }

        self._value=0.0
        self._delta=0

        self.after(3000, self.show_check)
        # self.after(4000, self.clear)

    def build(self):

        for key in ('inner', 'outer'):
            self.statics.update(
                {
                    key:self.create_oval(
                        (0,0,0,0),
                        # fill='0',
                        width=5,
                        outline=Palette.generic.BLACK
                    )
                }
            )

    def show_check(self):
        n=100
        for i in range(n):
            self._value += 1/n
            self.draw_rays()
        # time.sleep(0.5)
        for i in range(n):
            self._value -= 1/n
            self.draw_rays()

    def clear(self):
        for i in self.rays:
            self.delete(i)
        self.rays.clear()

    def draw_rays(self):
        # for i in self.rays:
        #     self.delete(i)
        # self.rays.clear()

        def _draw_slice(w_w, w_h, position):

            offset=10
            size = min(w_w, w_h) - offset
            d_in = size * self.options['d_bulb']
            angle = self.options['slices']
            length = int((0.5 * (size - d_in) - offset) / self.options['rings'])

            data = list()

            for j in range(angle):
                alpha = j * 360 / angle
                data.append(
                    self.create_line((
                        0.5 * w_w + (0.5 * d_in + offset + position * length) * np.cos(np.deg2rad(alpha)),
                        0.5 * w_h + (0.5 * d_in + offset + position * length) * np.sin(np.deg2rad(alpha)),
                        0.5 * w_w + (0.5 * d_in + offset + position * length + 0.85 * length) * np.cos(np.deg2rad(alpha)),
                        0.5 * w_h + (0.5 * d_in + offset + position * length + 0.85 * length) * np.sin(np.deg2rad(alpha)),
                    ),
                        width=max(2, int(0.075 * offset * position)),
                        fill='red',
                        smooth=tk.TRUE,
                    )
                )
            return data

        rings = self.options['rings']
        ringcount = len(self.rays)

        if int(rings * self._value) > ringcount:
            w=self.winfo_width()
            h=self.winfo_height()
            for i in range(ringcount, int(rings * self._value)):
                self.rays.append(
                    _draw_slice(w,h,i)
                )
        elif int(rings * self._value) < ringcount:
            print('deleting')
            for i in range(int(rings * self._value), ringcount):
                # print(i)
                self.delete(self.rays[i])
                self.rays.pop(i)
        self.update()
        # print(len(self.rays))

        # n_ray=self.options['marks_num']
        # print(n_ray * self._value)
        # print(len(self.rays))
        # if int(n_ray * self._value) > int(len(self.rays)/self.options['rays_num']):
        #     print('ADD!!!!!')
        #     """ ADD """

            # w=self.winfo_width()
            # h=self.winfo_height()
            #
            # offset=10
            # size = min(w,h) - offset
            # d_in = size * self.options['d_bulb']
            # s_ray = int((0.5 * (size - d_in) - offset) / n_ray)
            # r_count = len(self.rays)/
            # for i in range(r_count, int(n_ray * self._value)):
            #     for j in range(self.options['rays_num']):
            #         alpha = j * 360 / self.options['rays_num']
            #         self.rays.append(
            #             self.create_line( (
            #                 0.5 * w + (0.5 * d_in + offset + i * s_ray) * np.cos(np.deg2rad(alpha)),
            #                 0.5 * h + (0.5 * d_in + offset + i * s_ray) * np.sin(np.deg2rad(alpha)),
            #                 0.5 * w + (0.5 * d_in + offset + i * s_ray + 0.85 * s_ray) * np.cos(np.deg2rad(alpha)),
            #                 0.5 * h + (0.5 * d_in + offset + i * s_ray + 0.85 * s_ray) * np.sin(np.deg2rad(alpha)),
            #             ),
            #                 width=max(2, int(0.075 * offset * i)),
            #                 fill='red',
            #                 smooth=tk.TRUE,
            #         )
            #     )

    def on_resize(self, *args, **kwargs):
        w,h=super(SoundMeter, self).on_resize()

        offset=10
        size = min(w,h) - offset

        d_in = size * self.options['d_bulb']
        # self.coords(
        #     self.statics['outer'],
        #     (
        #         0.5 * (w - size),
        #         0.5 * (h - size),
        #         0.5 * (w + size),
        #         0.5 * (h + size),
        #     )
        # )
        self.coords(
            self.statics['inner'],
            (
                0.5 * (w - d_in),
                0.5 * (h - d_in),
                0.5 * (w + d_in),
                0.5 * (h + d_in),
            )
        )
        # c=self._value
        # self._value=0.0
        # self.draw_rays()
        # self._value=c
        # self.draw_rays()

class LightMeter(CanvasTemplate):
    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)

        self.statics=dict()
        self.rays=list()
        self.rays_edge=list()
        self.options={
            'slices':11,
            'rings': 5,
            'gap' : 10,
            'd_bulb': 0.25,
            'ang_bulb': 65,
        }

        self._value=0.0
        self._delta=0

        self.after(3000, self.show_check)
        # self.after(4000, self.clear)

    def build(self):
        super(LightMeter, self).build()
        self.bind_mouse_buttons()
        self.bind_mouse_motion()

        for key in ('inner', 'outer'):
            self.statics.update(
                {
                    key:self.create_oval(
                        (0,0,0,0),
                        # fill='0',
                        width=5,
                        outline=Palette.generic.BLACK
                    )
                }
            )

    def show_check(self):
        c=self._value
        self._value=0
        n=100
        for i in range(n):
            time.sleep(0.1/n)
            self._value += 1/n
            self.draw_rays()
        time.sleep(1)
        for i in range(n):
            time.sleep(0.1/n)
            self._value -= 1/n
            self.draw_rays()
        self._value=c
        self.draw_rays()

    def clear(self):

        for i in self.rays:
            for j in i:
                print('Deleting rays {}'.format(j))
                self.delete(j)

        for i in self.rays_edge:
            for j in i:
                print('Deleting edges {}'.format(j))
                self.delete(j)

        self.rays.clear()
        self.rays_edge.clear()

    def on_release(self, *event):
        super(LightMeter, self).on_release()
        if self.current_id:
            if self.current_id[0] in self.statics.values():
                self.show_check()
                return
            for entry in self.rays:
                if self.current_id[0] in entry:
                    self.show_check()
                    return
        self.draw_rays()

    def draw_rays(self):

        def _draw_ring(w, h, position):

            offset=10
            size = min(w, w) - offset
            d_in = size * self.options['d_bulb'] + self.options['gap']
            length = int((0.5 * (size - d_in) - offset) / self.options['rings'])

            rays=list()
            edges=list()

            for k in range(self.options['slices']):

                edges.append(
                    self.create_arc( (
                        0.5 * (w - d_in - position * length),
                        0.5 * (h - d_in - position * length),
                        0.5 * (w + d_in + position * length),
                        0.5 * (h + d_in + position * length),
                        ),

                        style=tk.ARC,
                        width=0.025*size,
                        outline=Palette.generic.BLACK,
                        start=(360 / self.options['slices']) * k - (1 - 0.05 * position),
                        extent=(360 / self.options['slices']) - self.options['gap'] + 2 * (1 - 0.05 * position)
                    )
                )
                print('Added arc {}'.format(edges[-1]))
                rays.append(
                    self.create_arc( (
                        0.5 * (w - d_in - position * length),
                        0.5 * (h - d_in - position * length),
                        0.5 * (w + d_in + position * length),
                        0.5 * (h + d_in + position * length),
                        ),

                        style=tk.ARC,
                        width=0.02*size,
                        outline=Palette.generic.WARNING,
                        start=(360 / self.options['slices']) * k,
                        extent=(360 / self.options['slices']) - self.options['gap']
                    )
                )
                print('Added arc {}'.format(rays[-1]))

            return rays,edges

        rings = self.options['rings']
        ringcount = len(self.rays)

        if int(rings * self._value) > ringcount:
            for i in range(ringcount, int(rings * self._value)):
                r,e =_draw_ring(self.winfo_width(), self.winfo_height(), i)
                self.rays.append(r)
                self.rays_edge.append(e)

        elif int(rings * self._value) < ringcount:
            print('deleting')
            for i in range(int(rings * self._value), ringcount):
                for j in self.rays[i]:
                    self.delete(j)
                self.rays.pop(i)
                for q in self.rays_edge[i]:
                    self.delete(q)
                self.rays_edge.pop(i)
        self.update()

            # w=self.winfo_width()
            # h=self.winfo_height()
            #
            # offset=10
            # size = min(w,h) - offset
            # d_in = size * self.options['d_bulb']
            # s_ray = int((0.5 * (size - d_in) - offset) / n_ray)
            # r_count = len(self.rays)/
            # for i in range(r_count, int(n_ray * self._value)):
            #     for j in range(self.options['rays_num']):
            #         alpha = j * 360 / self.options['rays_num']
            #         self.rays.append(
            #             self.create_line( (
            #                 0.5 * w + (0.5 * d_in + offset + i * s_ray) * np.cos(np.deg2rad(alpha)),
            #                 0.5 * h + (0.5 * d_in + offset + i * s_ray) * np.sin(np.deg2rad(alpha)),
            #                 0.5 * w + (0.5 * d_in + offset + i * s_ray + 0.85 * s_ray) * np.cos(np.deg2rad(alpha)),
            #                 0.5 * h + (0.5 * d_in + offset + i * s_ray + 0.85 * s_ray) * np.sin(np.deg2rad(alpha)),
            #             ),
            #                 width=max(2, int(0.075 * offset * i)),
            #                 fill='red',
            #                 smooth=tk.TRUE,
            #         )
            #     )

    def on_resize(self, *args, **kwargs):
        w,h=super(LightMeter, self).on_resize()

        offset=10
        size = min(w,h) - offset

        d_in = size * self.options['d_bulb']
        # self.coords(
        #     self.statics['outer'],
        #     (
        #         0.5 * (w - size),
        #         0.5 * (h - size),
        #         0.5 * (w + size),
        #         0.5 * (h + size),
        #     )
        # )
        self.coords(
            self.statics['inner'],
            (
                0.5 * (w - d_in),
                0.5 * (h - d_in),
                0.5 * (w + d_in),
                0.5 * (h + d_in),
            )
        )
        self.clear()
        self.draw_rays()

class WaterPumpWidget(CanvasTemplate):

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)

        self.statics=dict()
        self.blades=list()
        self.options = {
            'd_in': 0.4,
            'blades': 10,
            'steps': 2,
        }
        self.step=2
        self._running=False
        self._total=0

    def build(self, *args, **kwargs):

        for key in ('outer', 'inner'):
            self.statics.update(
                {
                    key:self.create_oval(
                        (0, 0, 0, 0,),
                        outline=Palette.generic.BLACK,
                        width=2
                    )
                }
            )

        for i in range(self.options['blades']):
            self.blades.append(
                self.create_arc(
                    (0, 0, 0, 0, ),
                    fill='grey65',
                    style=tk.ARC,
                    start=0,
                    extent=90,
                    outline=Palette.generic.BLACK,
                    width=2
                )
            )
        self.itemconfigure(
            self.statics['inner'],
            fill='grey55'
        )
        self.bind_mouse_buttons()
        self.bind_mouse_motion()
        self.loop()

    def start(self):
        self.step=3
        if self._running:
            return
        self.itemconfigure(
            self.statics['outer'],
            fill=Palette.generic.ONLINE
        )
        self._running=True

    def wait_update(self):
        self.step=1
        self.itemconfigure(
            self.statics['outer'],
            fill=Palette.generic.WARNING
        )

    def stop(self):
        if not self._running:
            return
        self.itemconfigure(
            self.statics['outer'],
            fill=Palette.generic.OFFLINE
        )
        self._running=False

    def loop(self):
        if self._running:
            self._total +=self.step
            if self._total >= 360:
                self._total=0
            self.draw_blades(self._total)

        self.after(20, self.loop)

    def on_release(self, *event):
        super(WaterPumpWidget, self).on_release()
        if self.current_id and \
                ( self.current_id[0] in self.statics.values() or self.current_id[0] in self.blades):
            if self._running:
                self.wait_update()
                self.after(2000,self.stop)
            else:
                self.wait_update()
                self.after(2000,self.start)

    def draw_blades(self, step=0):
        w=self.winfo_width()
        h=self.winfo_height()
        # offset=10
        # d_out=1.1 * min(w - offset, h - offset)
        # d_in=max(10, 0.25 * d_out)
        # b_angle=60
        #
        # c0=list()
        #
        # for i in [0, 90, 180, 270]:
        #     c0.append((
        #         0.5 * (w - d_out * np.cos(np.deg2rad(i + step))),
        #         0.5 * (h - d_out * np.sin(np.deg2rad(i + step))),
        #         0.5 * (w - d_in * np.cos(np.deg2rad(i + step))),
        #         0.5 * (h - d_in * np.sin(np.deg2rad(i + step))),
        #         0.5 * (w - d_in * np.cos(np.deg2rad(b_angle + i + step))),
        #         0.5 * (h - d_in * np.sin(np.deg2rad(b_angle + i + step))),
        #         0.5 * (w - d_out * np.cos(np.deg2rad(b_angle + i + step))),
        #         0.5 * (h - d_out * np.sin(np.deg2rad(b_angle + i + step))),
        #     ))
        # for count, item in enumerate(self.blades):
        #     self.coords(
        #         item, c0[count]
        #     )

    def on_resize(self, *args, **kwargs):
        w,h=super(WaterPumpWidget, self).on_resize()
        offset=10
        d_out=min(w - offset, h - offset)
        d_in=max(10, 0.4 * d_out)

        self.coords(
            self.statics['outer'],(
                0.5 * (w - d_out),
                0.5 * (h - d_out),
                0.5 * (w - d_out) + d_out,
                0.5 * (h - d_out) + d_out,
            )
        )
        self.coords(
            self.statics['inner'],(
                0.5 * (w - d_in),
                0.5 * (h - d_in),
                0.5 * (w - d_in) + d_in,
                0.5 * (h - d_in) + d_in,
            )
        )

        self.draw_blades()

class AlarmWidget(PanedTemplate, DeviceLinkTemplate):

    def __init__(self, parent, board, device, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)
        DeviceLinkTemplate.__init__(self, board, device)

        self.frame=PanedTemplate(self)
        self.button=LabelTemplate(self.frame)
        self.status=LabelTemplate(self)
        self.label_dev=LabelTemplate(self)
        self.label_count=LabelTemplate(self)
        self._enabled=False
        self._alarm=False
        self._acknowledge=False

        self._taskid=None
        self._tasktime=0
        self._taskstate=True
        self._total_events=0

    def build(self, *args, **kwargs):
        super(AlarmWidget, self).build()

        self.configure(
            bg=Palette.generic.BLACK,
        )
        self.frame.configure(
            relief=tk.SUNKEN,
            bg=Palette.generic.OFF
        )

        self.button.configure(
            font=Fonts.monobold(14),
            relief=tk.RAISED,
            text='OFFLINE',
            bg=Palette.generic.BLACK,
        )

        self.button.bind("<ButtonPress-1>", self.on_press_device)
        self.button.bind("<ButtonRelease-1>", self.on_release_device)

        self.status.configure(
            font=Fonts.monobold(14),
            relief=tk.RAISED,
            text='OFFLINE' ,
            bg=Palette.generic.BLACK,
        )

        self.status.bind("<ButtonPress-1>", self.on_press_status)
        self.status.bind("<ButtonRelease-1>", self.on_release_status)

        self.label_dev.configure(
            font=Fonts.mono(8),
            anchor=tk.W,
            text='{}:{}'.format(self.board, self.device),
            bg=Palette.generic.BLACK
        )

        self.label_count.configure(
            font=Fonts.mono(8),
            anchor=tk.E,
            bg=Palette.generic.BLACK
        )

        self.on_update()
        # self.after(1200, self.set_alarm)

    def on_update(self):

        if self.exist():

            self.label_count.configure(
                text='Events: {} '.format(self._total_events),
            )

            if System.io.is_connected():

                device = self.get_device()
                if device.is_connected():

                    if self._enabled:
                        self.button.configure(
                            bg=Palette.generic.BLACK,
                            fg=Palette.generic.ONLINE,
                            text='ARMED'
                        )
                    else:
                        self.button.configure(
                            bg=Palette.generic.BLACK,
                            fg=Palette.generic.WARNING,
                            text='DISABLED'
                        )
                    if not self._alarm:
                        self.status.configure(
                            fg=Palette.generic.WHITE,
                            text='IDLE',
                        )
                    """Armed or not ? """
                    pass

                else:
                    self.button.configure(
                        bg=Palette.generic.BG_DEFAULT,
                        fg=Palette.generic.DISABLED,
                        text='OFFLINE'
                    )
                    self.status.configure(
                        fg=Palette.generic.DISABLED,
                        text='DISCONNECTED',
                    )
                    self.frame.configure(
                        bg=Palette.generic.OFFLINE
                    )
            else:
                self.button.configure(
                    bg=Palette.generic.BG_DEFAULT,
                    fg=Palette.generic.DISABLED,
                    text='OFFLINE'
                )
                self.status.configure(
                    fg=Palette.generic.DISABLED,
                    text='DISCONNECTED',
                )
                self.frame.configure(
                    bg=Palette.generic.DISABLED
                )

        else:
            self.button.configure(
                bg=Palette.generic.BG_DEFAULT,
                fg=Palette.generic.OFFLINE,
                text='NOT FOUND',
            )
            self.status.configure(
                fg=Palette.generic.OFFLINE,
                text='ERROR',
            )
            self.frame.configure(
                bg=Palette.generic.ERROR_GUI
            )
            self.label_count.configure(
                text='Events: --- ',
            )

        # if 1:
        #     self.button.configure(
        #         bg=Palette.generic.DISABLED,
        #         fg=Palette.generic.ONLINE,
        #         text='DISABLED'
        #     )
        #     self.status.configure(
        #         fg=Palette.generic.WHITE,
        #         text='IDLE',
        #     )

    def parse(self, packet):

        if packet.command() > 10:
            self._total_events +=1


    def loop(self):

        self._taskid = self.after(750, self.loop)

        if self._taskstate:
            self.status.configure(
                fg=Palette.generic.ERROR_GUI
            )
        else:
            self.status.configure(
                fg=Palette.generic.WHITE
            )
        self._taskstate = not self._taskstate

        #
        # ima = time.perf_counter()
        #
        # if self._alarm:
        #     if ima > self._tasktime:
        #         self._tasktime = ima + 1

    def set_alarm(self):
        self._alarm=True
        self._total_events += 1
        self.status.configure(
            text='ALARM'
        )
        self.loop()

    def on_press_status(self, *event):
        self.status.configure(
            relief=tk.SUNKEN
        )

    def on_release_status(self, *event):
        self.status.configure(
            relief=tk.RAISED
        )
        if not self._taskid is None:
            self.after_cancel(self._taskid)
            self.on_update()

    def on_press_device(self, *event):
        self.button.configure(
            relief=tk.SUNKEN
        )

    def on_release_device(self, *event):
        self.button.configure(
            relief=tk.RAISED
        )
        if self.exist() and self.get_device().is_connected():
            if self._enabled:
                self._enabled=False
            else:
                self._enabled = True

        self.on_update()

    def on_resize(self, *args, **kwargs):
        w,h=super(AlarmWidget, self).on_resize()

        off=max(4, 0.045 * h)
        b_off=max(4, 0.065 * h)
        h_labels=20

        self.frame.place(
            x=off,
            y=off,
            width=w - 2 * off,
            heigh= 0.5 * h - off
        )
        self.button.place(
            x=b_off,
            y=b_off,
            width=(w - 2 * off) - 2 * b_off,
            heigh=(0.5 * h - 2 * off) - 2 * b_off
        )

        self.status.place(
            x=b_off,
            y=0.5 * h + off,
            width=w - 2 * b_off,
            heigh=0.5 * h - (off + b_off + h_labels)
        )

        self.label_dev.place(
            x=5,
            y=h - h_labels,
            width=0.5 * w - 7.5,
            heigh=h_labels
        )
        self.label_count.place(
            x=0.5 * w + 2.5,
            y=h - h_labels,
            width=0.5 * w - 7.5,
            heigh=h_labels
        )
        self.button.configure(
            font=Fonts.monobold(int(0.1*h))
        )
        self.status.configure(
            font=Fonts.monobold(int(0.1*h))
        )


class Alarms:

    class Event(DeviceLinkTemplate):
        def __init__(self, eventlevel, eventype, board, device, message, boardtime):
            DeviceLinkTemplate.__init__(self, board, device)
            self._level = eventlevel
            self._type = eventype
            self._message= message
            self._boardtime = boardtime
            self._state = 0

        @property
        def level(self):
            return self._level
        @property
        def type(self):
            return self._type
        @property
        def message(self):
            return self._message
        @property
        def boardtime(self):
            return datetime.datetime.now().fromtimestamp(self._boardtime)

        def view(self):
            self._state = 1

        def release(self):
            self._state = 2

        def delete(self):
            self._state = 3


    def __init__(self):
        self._db=list()

    @property
    def database(self):
        return self._db.copy()

    def add(self, packet):

        if packet.command():
            pass

print('Loaded widgets.tools')

