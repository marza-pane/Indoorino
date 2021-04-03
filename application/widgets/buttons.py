from common.templates import *

class ButtonCanvasPushableSign(TransparentFrameTemplate):

    def __init__(self, parent):
        TransparentFrameTemplate.__init__(self, parent)
        # self.configure(bd=0)
        self.master.bind_default('motion')
        self.master.bind("<ButtonPress-1>", self.on_press)
        self.master.bind("<ButtonRelease-1>", self.on_release)
        self.lines = [
            self.master.create_oval(
                (0,0,0,0),
                fill='white smoke',
                outline=Palette.generic.BLACK
            ),
            self.master.create_oval((0,0,0,0), fill="red"),
        ]
        self.status='offline'
        self.on_update()

    def build(self, *args, **kwargs):
        super(ButtonCanvasPushableSign, self).build()

    def on_motion(self, *event):
        super(ButtonCanvasPushableSign, self).on_motion(*event)
        if len(self.master.current_id):
            if self.master.current_id[0] in self.lines:
                self.master.itemconfig(self.lines[0], fill='blue')
                return
        self.master.itemconfig(self.lines[0], fill='white smoke')

    def on_press(self, *event):
        super(ButtonCanvasPushableSign, self).on_press(*event)
        if len(self.master.current_id):
            super(ButtonCanvasPushableSign, self).on_press(*event)
            if self.master.current_id[0] in self.lines:
                self.master.itemconfig(self.lines[1], fill='orange')
                debug_gui('BUTTON PRESSED')

    def on_release(self, *event):
        super(ButtonCanvasPushableSign, self).on_release(*event)
        if len(self.master.current_id):
            if self.master.current_id[0] in self.lines:
                debug_gui('BUTTON RELEASED')
                self.on_update()

    def on_update(self, *args, **kwargs):
        super(ButtonCanvasPushableSign, self).on_update()
        if self.status == 'offline':
            self.master.itemconfig(self.lines[1], fill=Palette.generic.OFFLINE)
        elif self.status == 'off':
            self.master.itemconfig(self.lines[1], fill=Palette.generic.OFF)
        elif self.status == 'on':
            self.master.itemconfig(self.lines[1], fill=Palette.generic.ON)
        else:
            self.master.itemconfig(self.lines[1], fill=Palette.generic.ERROR_GUI)

    def on_resize(self, *args, **kwargs):
        w0,h0,w,h=super(ButtonCanvasPushableSign, self).on_resize(*args, **kwargs)
        offset=6
        radius=min(w,h) - offset
        thickness=0.25 * radius

        self.master.coords(
            self.lines[0],
            (w0 + 0.5*(w - radius),
             h0 + 0.5*(h-radius),
             w0 + 0.5*(w - radius) + radius,
             h0 + 0.5*(h-radius) + radius))
        radius -= thickness
        self.master.coords(
            self.lines[1],
            (w0 + 0.5*(w - radius),
             h0 + 0.5*(h-radius),
             w0 + 0.5*(w - radius) + radius,
             h0 + 0.5*(h-radius) + radius))

class CanvasDeviceWidget(TransparentFrameTemplate):

    def __init__(self, parent):
        TransparentFrameTemplate.__init__(self, parent)
        self.lines= list()

    def build(self, *args, **kwargs):

        self.lines += [
            self.master.create_arc(
                (0, 0, 0, 0),
                fill='blue',
                start=0,
                style=tk.CHORD,
                extent=358
            ),
            self.master.create_oval(
                (0, 0, 0, 0),
                fill=Palette.generic.BG_DEFAULT
            ),
            self.master.create_rectangle(
                (0, 0, 0, 0),
                fill=Palette.generic.ON
            ),
            self.master.create_text(
                (0,0),
                anchor=tk.CENTER,
                justify=tk.CENTER,
                fill=Palette.generic.WHITE,
                font=Fonts.monobold(14),
                text='OFF'
            )
        ]

        self.turn_on()
        # self.master.after(2000, self.set_off)
        # self.master.after(5000, self.set_offline)

    def turn_on(self):
        self.master.itemconfig(
            self.lines[0],
            fill='gold'
        )
        self.master.itemconfig(
            self.lines[2],
            fill=Palette.generic.ONLINE
        )
        self.master.itemconfig(
            self.lines[3],
            text='ON'
        )

    def turn_pff(self):
        self.master.itemconfig(
            self.lines[0],
            dash=(),
            width=0,
            outline=Palette.generic.OFF,
            fill=Palette.generic.OFF
        )
        self.master.itemconfig(
            self.lines[2],
            fill=Palette.generic.OFF
        )
        self.master.itemconfig(
            self.lines[3],
            text='OFF'
        )

    def turn_offline(self):
        self.master.itemconfig(
            self.lines[0],
            dash=(),
            width=0,
            outline=Palette.generic.OFF,
            fill=Palette.generic.OFFLINE
        )
        self.master.itemconfig(
            self.lines[2],
            fill=Palette.generic.OFFLINE
        )
        self.master.itemconfig(
            self.lines[3],
            text='---'
        )

    def on_motion(self, *event):
        super(CanvasDeviceWidget, self).on_motion(*event)
        if len(self.master.current_id):
            if self.master.current_id[0] in self.lines:
                self.master.itemconfig(self.lines[0], fill='blue')
                return
        self.master.itemconfig(self.lines[0], fill='white smoke')

    def on_press(self, *event):
        super(CanvasDeviceWidget, self).on_press(*event)
        if len(self.master.current_id) and self.master.current_id[0] in self.lines:
            self.master.itemconfig(
                self.lines[1],
                fill=Palette.generic.WHITE
            )
            self.master.itemconfig(
                self.lines[3],
                fill=Palette.generic.BLACK
            )

    def on_release(self, *event):
        super(CanvasDeviceWidget, self).on_release(*event)
        if len(self.master.current_id) and self.master.current_id[0] in self.lines:
            self.master.itemconfig(
                self.lines[1],
                fill=Palette.generic.BG_DEFAULT
            )
            self.master.itemconfig(
                self.lines[3],
                fill=Palette.generic.WHITE
            )

    def on_resize(self, *args, **kwargs):
        w0, h0, w, h = super(CanvasDeviceWidget, self).on_resize(*args, **kwargs)
        offset=15
        radius=min(w, h) - offset
        thickness=0.25 * radius

        self.master.coords(
            self.lines[0],
            (w0 + 0.5*(w - radius),
             h0 + 0.5*(h-radius),
             w0 + 0.5*(w - radius) + radius,
             h0 + 0.5*(h-radius) + radius))

        radius -= thickness
        self.master.coords(
            self.lines[1],
            (w0 + 0.5*(w - radius),
             h0 + 0.5*(h-radius),
             w0 + 0.5*(w - radius) + radius,
             h0 + 0.5*(h-radius) + radius))

        self.master.coords(
            self.lines[2],
            (w0 + 0.5 * w - 0.15 * w,
             h0 + 0.5 * h  + 14,
             w0 + 0.5 * w + 0.15 * w,
             h0 + 0.5 * h + 20)
        )
        self.master.coords(self.lines[3], (w0 + 0.5 * w, h0 + 0.5 * h))


class CanvasWidgetLightBeam(CanvasDeviceWidget):

    def __init__(self, parent):
        CanvasDeviceWidget.__init__(self, parent)

    def turn_on(self):
        super(CanvasWidgetLightBeam, self).turn_on()
        self.master.itemconfig(
            self.lines[0],
            dash=(1,1),
            width=15,
            outline='yellow',
            dashoffset=10,
            fill='gold'
        )

class CanvasWidgetLightBulb(TransparentFrameTemplate):

    def __init__(self, parent):
        TransparentFrameTemplate.__init__(self, parent)
        self.lines= list()

    def build(self, *args, **kwargs):

        self.lines += [
            self.master.create_arc(
                (0, 0, 0, 0),
                fill='blue',
                start=0,
                style=tk.CHORD,
                extent=358
            ),
            self.master.create_oval(
                (0, 0, 0, 0),
                fill=Palette.generic.BG_DEFAULT
            ),
            self.master.create_rectangle(
                (0, 0, 0, 0),
                fill=Palette.generic.ON
            ),
            self.master.create_text(
                (0,0),
                anchor=tk.CENTER,
                justify=tk.CENTER,
                fill=Palette.generic.WHITE,
                font=Fonts.monobold(14),
                text='OFF'
            )
        ]

        self.set_on()
        # self.master.after(2000, self.set_off)
        # self.master.after(5000, self.set_offline)

    def set_on(self):
        self.master.itemconfig(
            self.lines[0],
            dash=(1,1),
            width=15,
            outline='yellow',
            fill='gold'
        )
        self.master.itemconfig(
            self.lines[2],
            fill=Palette.generic.ONLINE
        )
        self.master.itemconfig(
            self.lines[3],
            text='ON'
        )

    def set_off(self):
        self.master.itemconfig(
            self.lines[0],
            dash=(),
            width=0,
            outline=Palette.generic.OFF,
            fill=Palette.generic.OFF
        )
        self.master.itemconfig(
            self.lines[2],
            fill=Palette.generic.OFF
        )
        self.master.itemconfig(
            self.lines[3],
            text='OFF'
        )

    def set_offline(self):
        self.master.itemconfig(
            self.lines[0],
            dash=(),
            width=0,
            outline=Palette.generic.OFF,
            fill=Palette.generic.OFFLINE
        )
        self.master.itemconfig(
            self.lines[2],
            fill=Palette.generic.OFFLINE
        )
        self.master.itemconfig(
            self.lines[3],
            text='---'
        )

    def on_press(self, *event):
        super(CanvasWidgetLightBulb, self).on_press(*event)
        if len(self.master.current_id) and self.master.current_id[0] in self.lines:
            self.master.itemconfig(
                self.lines[1],
                fill=Palette.generic.WHITE
            )
            self.master.itemconfig(
                self.lines[3],
                fill=Palette.generic.BLACK
            )


    def on_release(self, *event):
        super(CanvasWidgetLightBulb, self).on_release(*event)
        if len(self.master.current_id) and self.master.current_id[0] in self.lines:
            self.master.itemconfig(
                self.lines[1],
                fill=Palette.generic.BG_DEFAULT
            )
            self.master.itemconfig(
                self.lines[3],
                fill=Palette.generic.WHITE
            )

    def on_resize(self, *args, **kwargs):
        w0, h0, w, h = super(CanvasWidgetLightBulb, self).on_resize(*args, **kwargs)
        offset=15
        radius=min(w, h) - offset
        thickness=0.25 * radius

        self.master.coords(
            self.lines[0],
            (w0 + 0.5*(w - radius),
             h0 + 0.5*(h-radius),
             w0 + 0.5*(w - radius) + radius,
             h0 + 0.5*(h-radius) + radius))

        radius -= thickness
        self.master.coords(
            self.lines[1],
            (w0 + 0.5*(w - radius),
             h0 + 0.5*(h-radius),
             w0 + 0.5*(w - radius) + radius,
             h0 + 0.5*(h-radius) + radius))

        self.master.coords(
            self.lines[2],
            (w0 + 0.5 * w - 0.15 * w,
             h0 + 0.5 * h  + 14,
             w0 + 0.5 * w + 0.15 * w,
             h0 + 0.5 * h + 20)
        )
        self.master.coords(self.lines[3], (w0 + 0.5 * w, h0 + 0.5 * h))







