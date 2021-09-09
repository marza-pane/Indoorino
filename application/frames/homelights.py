from common.comtable import *
from common.templates import *
from indoorino.packet import IndoorinoPacket
import numpy as np




class UiHomeLights(CanvasTemplate):

    class DeviceTemplate(CanvasTemplate, DeviceLinkTemplate):
        def __init__(self, parent, board, device, option, **kwargs):
            CanvasTemplate.__init__(self, parent, **kwargs)
            DeviceLinkTemplate.__init__(self, board, device)
            self._option = option

    class Device(DeviceTemplate):

        def __init__(self, parent, board, device, service, option=None, **kwargs):
            UiHomeLights.DeviceTemplate.__init__(self, parent, board, device, option=option, **kwargs)

            self._service=service

            self.button=PanedTemplate(self)
            self.icon=PictureTemplate(self.button, Icons.lights.beamspot.OFFLINE())
            self.labels=dict()

            # self.label_name=LabelTemplate(self)
            # self.label_device=LabelTemplate(self)

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

            for key in ('name', 'position', 'type'):
                self.labels.update({
                    key:LabelTemplate(
                        self,
                        anchor=tk.W,
                        padx=5,
                        font=Fonts.monobold(8),
                        text=key.capitalize(),
                        bg='blue'
                    )
                })
                self.labels[key].bind_mouse_motion()

            self.bind_mouse_buttons()
            self.icon.bind_mouse_motion()

            for item in self.labels.values():
                item.bind("<ButtonPress-1>", self.on_press)
                item.bind("<ButtonRelease-1>", self.on_release)

            self.button.bind("<ButtonPress-1>", self.on_button_press)
            self.button.bind("<ButtonRelease-1>", self.on_button_release)

            self.icon.bind("<ButtonPress-1>", self.on_button_press)
            self.icon.bind("<ButtonRelease-1>", self.on_button_release)

            self.button.configure(
                relief=tk.RAISED
            )

            self.labels['name'].configure(
                font=Fonts.monobold(7),
                text='{}::{}'.format(self.board, self.device)
            )
            self.labels['type'].configure(
                font=Fonts.monobold(7),
                text='{}'.format(self._option)
            )

            p='UNKNOWN'
            for dev in System.layout.devices.values():
                if self.board == dev.boardname and self.device == dev.devname:
                    p = '{}::{}'.format(dev.area, dev.location)
                    break

            self.labels['position'].configure(
                font=Fonts.monobold(7),
                text=p
             )


            self.icon.settooltip('BUTTON 1')
            tooltip = 'BUTTON 2'
            for w in self.labels.values():
                w.settooltip(tooltip)
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

            if self.exist() and System.io.is_connected():
                device = self.get_device()
                self.waiting = True
                self.after(10000, self._rem_wait)
                if device.is_connected():
                    if device.status.dev['relay_state'].data:
                        # send turn OFF
                        d = {
                            'board':self.board,
                            'devname': self.device,
                            'desc1': self._service,
                            'command': 'SET',
                            'value1': 0
                        }
                        p = IndoorinoPacket()
                        p.build(IBACOM_LGT_DEV_SET, "SERVER", d)
                        System.io.send2server(p)
                        self._waitfor = False

                        # self.after(2000, lambda : (device.status.dev['relay_state'].set(False), self.on_update()))

                    else:
                        # send turn ON
                        d = {
                            'board':self.board,
                            'devname': self.device,
                            'desc1': self._service,
                            'command': 'SET',
                            'value1': 1
                        }
                        p = IndoorinoPacket()
                        p.build(IBACOM_LGT_DEV_SET, "SERVER", d)
                        System.io.send2server(p)
                        self._waitfor = True

                        # self.after(2000, lambda : (device.status.dev['relay_state'].set(True), self.on_update()))

                self.on_update()
                return

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
                p.build(IBACOM_LGT_DEV_SET, "SERVER", {
                    'board': self.board,
                    'devname': self.device,
                    'desc1': self._service,
                    'command': 'UPDATE',
                })
                System.io.send2server(p)

        def on_update(self, *args, **kwargs):

            self.icon.settooltip('')
            tooltip = ''
            if self.exist():

                if System.io.is_connected():

                    device = self.get_device()
                    tooltip = 'service   ::{}\ndevice    ::{}:{}\n'.format(self._service, self.board, self.device)
                    for dev in System.layout.devices.values():
                        if self.board == dev.boardname and self.device == dev.devname:
                            tooltip += 'location  ::{}:{}\n'.format(dev.area, dev.location)
                            break
                    tooltip += 'connected ::{}'.format(
                        'YES' if device.device.is_connected() else 'NO'
                    )

                    if device.is_connected():

                        tooltip += 'status    ::{}'.format(
                            'ON' if device.status.dev['relay_state'].data else 'OFF'
                        )

                        if device.status.dev['relay_state'].data == self._waitfor:
                            self.waiting = False



                        if device.status.dev['relay_state'].data:
                            self.icon.replace_image(Icons.lights.beamspot.ON())
                            self.icon.settooltip('Turn OFF')
                        else:
                            self.icon.replace_image(Icons.lights.beamspot.OFF())
                            self.icon.settooltip('Turn ON')

                    else:
                        self.icon.replace_image(Icons.lights.beamspot.DISCONNECTED())
                        self.waiting = False

                else:
                    self.icon.replace_image(Icons.lights.beamspot.OFFLINE())
                    self.waiting = False
            else:
                self.icon.replace_image(Icons.system.NOT_FOUND())
                self.waiting = False

            for w in self.labels.values():
                w.settooltip(tooltip)

        def on_resize(self, *args, **kwargs):
            w,h=super(UiHomeLights.Device, self).on_resize()

            offset = min(w,h) * 0.05
            s_icon = min(w,h) - 2 * offset
            s_butt = min(w,h) / 3

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

            # self.labels['smart'].place(
            #     # x=offset + s_icon + 5,
            #     x=offset + s_icon + 0.5 * w,
            #     y=offset,
            #     width=w - (s_icon + 2 * offset + 8),
            #     heigh=0.5 * (h - 2 * offset),
            # )
            # self.labels['timer'].place(
            #     # x=offset + s_icon + 5,
            #     x=offset + s_icon + 0.5 * w,
            #     y=0.5 * h,
            #     width=w - (s_icon + 2 * offset + 8),
            #     heigh=0.5 * (h - 2 * offset),
            # )

            self.labels['name'].place(
                x=offset + s_icon + 5,
                y=offset,
                width=w - (s_icon + 3 * offset + 2 * s_butt),
                heigh=(h - 2 * offset) / 3,
            )
            self.labels['type'].place(
                x=offset + s_icon + 5,
                y=offset + (h - 2 * offset) / 3,
                width=w - (s_icon + 3 * offset + 2 * s_butt),
                heigh=(h - 2 * offset) / 3,
            )

            self.labels['position'].place(
                x=offset + s_icon + 5,
                y=offset + 2 * (h - 2 * offset) / 3,
                width=w - (s_icon + 3 * offset + 2 * s_butt ),
                heigh=(h - 2 * offset) / 3,
            )



            self.icon.on_resize()

    class Board(PanedTemplate):

        def __init__(self, parent, name, service):
            PanedTemplate.__init__(self, parent)
            self._service=service
            self._boardname=name
            self.devices = dict()
            self.buttons = dict()

        def build(self):
            super(UiHomeLights.Board, self).build()
            self.configure(
                relief=tk.RAISED,
                bg='black'
            )
            # for item in self.devices.values():
            #     item.build()

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

                            p = IndoorinoPacket()
                            p.build(IBACOM_LGT_DEV_SET, "SERVER", {
                                'board': item.board,
                                'devname': item.device,
                                'desc1': self._service,
                                'command': 'SET',
                                'value1': 1
                            })
                            System.io.send2server(p)

                    elif command == 'turn OFF':
                        for item in self.devices.values():
                            # if not item.get_device().status.dev['relay_state'].data:
                            #     continue

                            p = IndoorinoPacket()
                            p.build(IBACOM_LGT_DEV_SET, "SERVER", {
                                'board': item.board,
                                'devname': item.device,
                                'desc1': self._service,
                                'command': 'SET',
                                'value1': 0
                            })
                            System.io.send2server(p)
                else:
                    warning_ui('Can not operate while OFFLINE')
            else:
                warning_ui('Could not ned update because board name is unknown (no devices found)')

        def on_update(self, *args, **kwargs):

            resize_flag = False

            if not self._service in System.lights.services.keys():
                warning_ui('LIGHTS:BOARD invalid service <{}>'.format(self._service))
                return

            for key in self.devices.keys():
                if not key in [dev.devname for dev in System.lights.services[self._service].devices.values() if dev.boardname == self._boardname]:
                    self.devices[key].on_closing()
                    self.devices.pop(key)
                    self.on_update()
                    return

            for light in System.lights.services.values():
                for dev in light.devices.values():
                    if dev.boardname == self._boardname \
                        and light.name == self._service \
                        and not dev.devname in self.devices.keys():

                        debug_ui('Adding {}:{}'.format(dev.boardname, dev.devname))
                        self.devices.update(
                            {
                                dev.devname: UiHomeLights.Device(self, dev.boardname, dev.devname, self._service)
                            }
                        )
                        self.devices[dev.devname].build()
                        resize_flag = True

                # if not light.devname in self.devices.keys() \
                #         and light.boardname == self._boardname \
                #         and light.group == self._group:

                if resize_flag:
                    self.on_resize()

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

        def __init__(self, parent, service, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)

            self._service=service
            self._visible=True
            self.boards = dict()
            self.label = LabelTemplate(self, text=str(service).upper())
            self.compact = PictureTemplate(self, Icons.system.COLLAPSE(), bg=Palette.generic.DISABLED)

        def build(self, *args, **kwargs):
            super(UiHomeLights.Group, self).build()
            self.configure(
                relief=tk.RAISED,
                bg=Palette.generic.DISABLED
            )
            for item in self.boards.values():
                item.build()

            self.label.configure(
                anchor=tk.W,
                font=Fonts.monobold(12),
                padx=20,
                bg=Palette.generic.DISABLED,
                fg='yellow',
                # fg=Palette.generic.BLACK,
            )

            self.compact.build()
            self.compact.configure(
                relief=tk.RAISED,
            )
            self.compact.bind("<ButtonPress-1>", self.on_press)
            self.compact.bind("<ButtonRelease-1>", self.on_release)

        def on_press(self, *event):
            super(UiHomeLights.Group, self).on_press()
            self.compact.configure( relief=tk.SUNKEN)

        def on_release(self, *event):
            super(UiHomeLights.Group, self).on_release(*event)
            self.compact.configure( relief=tk.RAISED)
            if self._visible:
                self._visible=False
                self.compact.replace_image(Icons.system.RESTORE())
            else:
                self._visible=True
                self.compact.replace_image(Icons.system.COLLAPSE())
            self.master.on_resize()

        def on_update(self, *args, **kwargs):

            resize_flag = False

            if not self._service in System.lights.services.keys():
                warning_ui('LIGHTS:GROUP invalid service <{}>'.format(self._service))
                return

            for key in self.boards.keys():
                if not key in System.lights.services[self._service].boards:
                    self.boards[key].on_closing()
                    self.boards.pop(key)
                    self.on_update()
                    return

            for light in System.lights.services.values():
                for board in light.boards:
                    if light.name == self._service and not board in self.boards.keys():
                        debug_ui('Adding {}:{}'.format(light.name, board))
                        self.boards.update(
                            {
                                board: UiHomeLights.Board(self, board, self._service)
                            }
                        )
                        self.boards[board].build()
                        resize_flag = True

            for item in self.boards.values():
                item.on_update()

            if resize_flag:
                self.on_resize()

        def loop(self, *args, **kwargs):
            for item in self.boards.values():
                item.loop()

        def on_resize(self, *args, **kwargs):
            w, h = super(UiHomeLights.Group, self).on_resize()
            h_title = 30
            w_butt = 30
            self.label.place(
                x=0,
                y=0,
                width=w,
                heigh=h_title
            )
            self.compact.place(
                x=w - w_butt,
                y=0.5 * (h_title - w_butt),
                width=w_butt,
                heigh=w_butt
            )
            self.compact.on_resize()
            if not self._visible:
                return h_title

            bnum = len(self.boards)

            if bnum == 0:
                return h_title

            weights = list()
            for item in self.boards.values():
                weights.append(len(item.devices))

            if bnum > 3:
                n = np.array(weights)
                wtable = tuple(w * n / np.sum(n))
                # wtable = tuple(w * n / np.sqrt(np.sum(n**2)))
            else:
                wtable = [int(w / bnum)] * bnum

            offset = 0
            hboards = [0,]
            for count, widget in enumerate(self.boards.values()):
                widget.place(
                    x=offset,
                    y=h_title,
                    width=wtable[count],
                    heigh=h
                )
                hboards.append(widget.on_resize())
                offset += wtable[count]

            return max(hboards) + h_title

        def resize_height(self):
            w, h = super(UiHomeLights.Group, self).on_resize()
            for widget in self.boards.values():
                widget.place(
                    heigh=h
                )
                widget.on_resize()

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        self.label = LabelTemplate(self)
        self.widgets = dict()

    def build(self, *args, **kwargs):
        super(UiHomeLights, self).build()
        self.label.configure(
            font=Fonts.monobold(16),
            text='LIGHTS',
            anchor=tk.W,
            padx=30,
            fg=Palette.frames.LIGHTS,
            bg=Palette.generic.BLACK
        )
        self.on_update()

    def on_update(self, *args, **kwargs):

        resize_flag=False

        for key in self.widgets.keys():
            if not key in [i.name for i in System.lights.services.values()]:
                self.widgets[key].on_closing()
                self.widgets.pop(key)
                self.on_update()
                return


        for key, s in System.lights.services.items():
            if not s.name in self.widgets.keys():
                self.widgets.update(
                    {
                        s.name : UiHomeLights.Group(self, s.name)
                    }
                )
                self.widgets[s.name].build()
                resize_flag=True

        for widget in self.widgets.values():
            widget.on_update()

        if resize_flag:
            self.on_resize()

    def loop(self, *args, **kwargs):

        if Config.flags.update.DEVICES or Config.flags.update.NETWORK:
            self.on_update()

        for widget in self.widgets.values():
            widget.loop()

    def on_resize(self, *args, **kwargs):
        w, h = super(UiHomeLights, self).on_resize()
        h_label = 35
        self.label.place(
            x=0,
            y=0,
            width=w,
            heigh=h_label,
        )

        n_group = len(self.widgets)

        if n_group == 0:
            return w, h

        off = 0
        hgroup = 100

        for count, widget in enumerate(self.widgets.values()):

            widget.place(
                x=0,
                y=off + h_label,
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



print('Loaded gui.frame.homelights')

