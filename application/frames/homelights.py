from common.comtable import *
from common.templates import *
from indoorino.packet import IndoorinoPacket
import numpy as np

class UiDeviceWidgetTemplate(CanvasTemplate, DeviceLinkTemplate):
    def __init__(self, parent, board, device, option, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        DeviceLinkTemplate.__init__(self, board, device)
        self._option = option

class UiHomeLights(CanvasTemplate):

    class Device(UiDeviceWidgetTemplate):

        def __init__(self, parent, board, device, option=None, **kwargs):
            UiDeviceWidgetTemplate.__init__(self, parent, board, device, option=option, **kwargs)
            self.button=PanedTemplate(self)
            self.icon=PictureTemplate(self.button, Icons.lights.beamspot.OFFLINE())
            self.label_name=LabelTemplate(self)
            self.label_device=LabelTemplate(self)

            self._waiting=False
            self._status=False
            self._waitfor=False

            if not self.exist():
                warning_boards('homelights: could not find board {}'.format(self.board))
                return

            if self.get_device().devtype != 'RELAY':
                error_ui('homelights: devices must be \'RELAY\' type')

        @property
        def waiting(self):
            return self._waitfor

        @waiting.setter
        def waiting(self, value):
            self._waiting=value
            if value:
                self.icon.configure(
                    bg=Palette.generic.WARNING
                )
                self.button.configure(
                    bg=Palette.generic.WARNING
                )
            else:
                self.icon.configure(
                    bg=Palette.generic.BG_DEFAULT
                )
                self.button.configure(
                    bg=Palette.generic.BG_DEFAULT
                )

        def _rem_wait(self):
            self.waiting=False

        def build(self, *args, **kwargs):
            super(UiHomeLights.Device, self).build()
            self.button.build()
            self.icon.build()

            self.bind_mouse_buttons()

            self.label_device.bind("<ButtonPress-1>", self.on_press)
            self.label_device.bind("<ButtonRelease-1>", self.on_release)

            self.label_name.bind("<ButtonPress-1>", self.on_press)
            self.label_name.bind("<ButtonRelease-1>", self.on_release)

            self.button.bind("<ButtonPress-1>", self.on_button_press)
            self.button.bind("<ButtonRelease-1>", self.on_button_release)
            self.icon.bind("<ButtonPress-1>", self.on_button_press)
            self.icon.bind("<ButtonRelease-1>", self.on_button_release)

            self.button.configure(
                relief=tk.RAISED
            )
            self.label_device.configure(
                anchor=tk.W,
                padx=10,
                font=Fonts.monobold(8),
                text='{}::{}'.format(self.board, self.device)
            )
            self.label_name.configure(
                anchor=tk.W,
                padx=10,
                font=Fonts.monobold(9),
                text='{}'.format(self._option)
            )

        def on_button_press(self, *event):
            self.button.configure(
                bg=Palette.generic.WHITE,
                relief=tk.SUNKEN,
            )
            self.icon.configure(
                bg=Palette.generic.WHITE,
                relief = tk.SUNKEN,
            )

        def on_button_release(self, *event):
            self.button.configure(
                bg=Palette.generic.BG_DEFAULT,
                relief=tk.RAISED,
            )
            self.icon.configure(
                bg=Palette.generic.BG_DEFAULT,
                relief=tk.RAISED,
            )

            if self._waiting:
                return

            if not self.exist():
                self.on_update()
                return

            device = self.get_device()
            self.waiting = True
            self.after(10000, self._rem_wait)
            if device.is_connected():
                if device.status.dev['relay_state'].data:
                    # send turn OFF
                    d = {
                        'devname': self.device,
                        'command': 'SET',
                        'value1': 0
                    }
                    p = IndoorinoPacket()
                    p.build(IBACOM_SET_DEVICE, self.board, d)
                    System.io.send(p)
                    self._waitfor = False

                    # self.after(2000, lambda : (device.status.dev['relay_state'].set(False), self.on_update()))

                else:
                    # send turn ON
                    d = {
                        'devname': self.device,
                        'command': 'SET',
                        'value1': 1
                    }
                    p = IndoorinoPacket()
                    p.build(IBACOM_SET_DEVICE, self.board, d)
                    System.io.send(p)
                    self._waitfor = True

                    # self.after(2000, lambda : (device.status.dev['relay_state'].set(True), self.on_update()))

                self.on_update()

        def on_press(self, *event):
            super(UiHomeLights.Device, self).on_press(*event)
            self.configure(
                bg=Palette.generic.DISABLED,
                relief=tk.SUNKEN,
            )
            self.label_device.configure(
                bg=Palette.generic.DISABLED,
            )
            self.label_name.configure(
                bg=Palette.generic.DISABLED,
            )

        def on_release(self, *event):
            super(UiHomeLights.Device, self).on_release(*event)
            self.configure(
                bg=Palette.generic.BG_DEFAULT,
                relief=tk.RAISED,
            )
            self.label_device.configure(
                bg=Palette.generic.BG_DEFAULT,
            )
            self.label_name.configure(
                bg=Palette.generic.BG_DEFAULT,
            )
            if self.exist() and self.get_device().is_connected():
                p = IndoorinoPacket()
                p.build(IBACOM_SET_DEVICE, self.board, {
                    'devname': self.device,
                    'command': 'UPDATE',
                })
                System.io.send(p)

        def on_update(self, *args, **kwargs):

            if self.exist():

                if System.io.is_connected():

                    device = self.get_device()

                    if device.is_connected():

                        if device.status.dev['relay_state'].data == self._waitfor:
                            self.waiting = False

                        if device.status.dev['relay_state'].data:
                            self.icon.replace_image(Icons.lights.beamspot.ON())
                        else:
                            self.icon.replace_image(Icons.lights.beamspot.OFF())

                    else:
                        self.icon.replace_image(Icons.lights.beamspot.ERROR())
                        self.waiting = False

                else:
                    self.icon.replace_image(Icons.lights.beamspot.OFFLINE())
                    self.waiting = False
            else:
                self.icon.replace_image(Icons.system.NOT_FOUND())
                self.waiting = False

        def on_resize(self, *args, **kwargs):
            w,h=super(UiHomeLights.Device, self).on_resize()

            offset = min(w,h) * 0.05
            s_icon = min(w,h) - 2 * offset

            self.button.place(
                x=offset,
                y=offset,
                width= s_icon,
                heigh= s_icon
            )

            self.icon.place(
                x=5,
                y=5,
                width=s_icon - 10,
                heigh=s_icon - 10,
            )

            self.label_name.place(
                x=offset + s_icon + 5,
                y=offset,
                width=w - (s_icon + 2 * offset + 8),
                heigh=0.5 * (h - 2 * offset),
            )
            self.label_device.place(
                x=offset + s_icon + 5,
                y=0.5 * h,
                width=w - (s_icon + 2 * offset + 8),
                heigh=0.5 * (h - 2 * offset),
            )

            self.icon.on_resize()

    class Board(PanedTemplate):

        def __init__(self, parent, source, name, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)

            self.devices = dict()
            self.buttons = dict()

            for key, item in source.items():
                print('Adding {}:{} with options {}'.format(name, key, item))
                self.devices.update(
                    {
                        key: UiHomeLights.Device(self, name, key, item)
                    }
                )

        def build(self):
            super(UiHomeLights.Board, self).build()
            self.configure(
                relief=tk.RAISED,
                bg='black'
            )
            for item in self.devices.values():
                item.build()

            for key in ('update', 'turn ON', 'turn OFF',):
                self.buttons.update(
                    {
                        key:ButtonTemplate(
                            self,
                            font=Fonts.monobold(9),
                            text=key,
                            command=lambda c=key : self.callback(c)
                        )
                    }
                )

        def loop(self):
            for item in self.devices.values():
                item.loop()

        def callback(self, command):

            if len(self.devices) > 0:
                dev = tuple(self.devices.values())[0]
                if dev.exist() and dev.get_device().is_connected():

                    if command == 'update':
                            p = IndoorinoPacket()
                            p.build(IBACOM_REQUEST_STATUS, tuple(self.devices.values())[0].board, {})
                            System.io.send(p)

                    elif command == 'turn ON':
                        for item in self.devices.values():
                            # if item.get_device().status.dev['relay_state'].data:
                            #     continue
                            device =item.device
                            board= item.board
                            p = IndoorinoPacket()
                            p.build(IBACOM_SET_DEVICE, board, {
                                'devname': device,
                                'command': 'SET',
                                'value1': 1
                            })
                            System.io.send(p)

                    elif command == 'turn OFF':
                        for item in self.devices.values():
                            # if not item.get_device().status.dev['relay_state'].data:
                            #     continue
                            device =item.device
                            board= item.board
                            p = IndoorinoPacket()
                            p.build(IBACOM_SET_DEVICE, board, {
                                'devname': device,
                                'command': 'SET',
                                'value1': 0
                            })
                            System.io.send(p)
                else:
                    warning_ui('Can not operate while OFFLINE')
            else:
                warning_ui('Could not ned update because board name is unknown (no devices found)')

        def on_update(self, *args, **kwargs):
            for item in self.devices.values():
                item.on_update()

        def on_resize(self, *args, **kwargs):
            w,h=super(UiHomeLights.Board, self).on_resize()

            offset = 20
            wx = 220
            wy = 60

            wb = 100
            hb = 30

            for count, item in enumerate(self.buttons.values()):
                item.place(
                    x = 2 * offset + count * wb,
                    y = 0.5 * offset,
                    width=wb - 10,
                    heigh=hb
                )

            xcount = max(np.floor((w - 2 * offset) / wx), 1)

            col = 0
            row = 0
            wx = int((w - 2 * offset) / xcount)

            for count, item in enumerate(self.devices.values()):
                item.place(
                    x=offset + col * wx,
                    y=offset + hb + row * wy,
                    width=wx - 3,
                    heigh=wy - 3
                )
                item.on_resize()

                col += 1
                if col >= xcount:
                    col = 0
                    row += 1

            return 2 * offset + hb + (row + 1) * wy

    class Group(PanedTemplate):

        def __init__(self, parent, source, group, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)

            self.boards = dict()

            print('Adding {} : {}'.format(format_type(source), format_dict(source)))

            for key, item in source.items():
                print('Adding {}:{}'.format(group, key, item))
                self.boards.update(
                    {
                        key: UiHomeLights.Board(self, item, key)
                    }
                )

        def build(self, *args, **kwargs):
            super(UiHomeLights.Group, self).build()
            self.configure(
                relief=tk.RAISED,
                bg=Palette.generic.BG_DEFAULT
            )
            for item in self.boards.values():
                item.build()

        def on_update(self, *args, **kwargs):
            for item in self.boards.values():
                item.on_update()

        def loop(self, *args, **kwargs):
            for item in self.boards.values():
                item.loop()

        def on_resize(self, *args, **kwargs):
            w, h = super(UiHomeLights.Group, self).on_resize()

            bnum = len(self.boards)

            if bnum == 0:
                return w, h

            weights = list()
            for item in self.boards.values():
                weights.append(len(item.devices))

            if bnum > 3:
                n = np.array(weights)
                wtable = tuple(w * n / np.sqrt(np.sum(n**2)))
            else:
                wtable = [int(w / bnum)] * bnum

            offset = 0
            hboards = list()
            for count, widget in enumerate(self.boards.values()):
                widget.place(
                    x=offset,
                    y=0,
                    width=wtable[count],
                    heigh=h
                )
                hboards.append(widget.on_resize())
                offset += wtable[count]

            return max(hboards)

        def resize_height(self):
            w, h = super(UiHomeLights.Group, self).on_resize()
            for widget in self.boards.values():
                widget.place(
                    heigh=h
                )
                widget.on_resize()

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        self.widgets = dict()

        for group, data in Config.layout.lights.items():
            self.widgets.update(
                {
                    group:self.Group(self, data, group),
                }
            )

    def build(self, *args, **kwargs):
        super(UiHomeLights, self).build()
        for widget in self.widgets.values():
            widget.build()

    def on_update(self, *args, **kwargs):
        for widget in self.widgets.values():
            widget.on_update()

    def loop(self, *args, **kwargs):
        if Config.flags.update.DEVICES:
            self.on_update()
        for widget in self.widgets.values():
            widget.loop()

    def on_resize(self, *args, **kwargs):
        w, h = super(UiHomeLights, self).on_resize()

        n_group = len(self.widgets)

        if n_group == 0:
            return w, h

        off = 0
        hgroup = 100

        for count, widget in enumerate(self.widgets.values()):

            widget.place(
                x=0,
                y=off,
                width=w,
                heigh=hgroup
            )

            hgroup = widget.on_resize()
            self.update()
            widget.place(
                heigh=hgroup
            )
            widget.resize_height()

            off += hgroup
        return w, h





"""   """

"""  """






from widgets.buttons import *
class CanvasBeamFrame(TransparentFrameContainer):

    def __init__(self, parent):
        TransparentFrameContainer.__init__(self, parent)
        
        """ TEST START """
        self.children = [
            ButtonCanvasPushableSign(self.master),
            CanvasDeviceWidget(self.master),
            CanvasWidgetLightBeam(self.master),
        ]

    def build(self):
        super(CanvasBeamFrame, self).build()
        self.bind()
        self.children[0].build()
        self.children[1].build()
        self.children[2].build()

    def on_resize(self):
        w0, h0, w, h = super(CanvasBeamFrame, self).on_resize()
        self.children[0].place(
            x=w0 + 0.5 * w,
            y=h0 + 0.1 * h,
            width=min(0.1 * w, 0.1 * h),
            heigh=min(0.1 * w, 0.1 * h)
        )
        self.children[0].on_resize()
        self.children[1].place(
            x=w0 + 0.2 * w,
            y=h0 + 0.5 * h,
            width=min(0.1 * w, 0.1 * h),
            heigh=min(0.1 * w, 0.1 * h)
        )
        self.children[1].on_resize()
        self.children[2].place(
            x=w0 + 0.6 * w,
            y=h0 + 0.6 * h,
            width=min(0.1 * w, 0.1 * h),
            heigh=min(0.1 * w, 0.1 * h)
        )
        self.children[2].on_resize()

        