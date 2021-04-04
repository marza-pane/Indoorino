from common.comtable import IBACOM_SET_DEVICE
from common.templates import *

import tkinter.messagebox

from indoorino.packet import IndoorinoPacket
from frames.devices import TopBoardEditor, TopDeviceEditor

""" Resources/Boards on GUI treeview"""

class _LocalStatus:
    def __init__(self):
        self.board=''
        self.device=''

_local_status = _LocalStatus()

class UiBoards(PanedTemplate):

    class BoardList(ScrollableFrameTemplate):

        class Header(CanvasTemplate, BoardLinkTemplate):

            def __init__(self, parent, board):

                CanvasTemplate.__init__(self, parent)
                BoardLinkTemplate.__init__(self, board)

                self.frame = CanvasTemplate(self)
                self.icon = PictureTemplate(self, Icons.BOARD_RED())

                self.strings = dict()
                self.labels = dict()

            def build(self, *args, **kwargs):
                super(UiBoards.BoardList.Header, self).build()

                self.configure(bg=Palette.generic.BLACK)
                self.icon.build()
                self.icon.configure(
                    relief=tk.RAISED,
                )
                self.frame.configure(
                    relief=tk.FLAT,
                    bg=Palette.generic.BLACK,
                )
                self.frame.bind("<ButtonPress-1>", lambda event: self.icon.configure(bg=Palette.generic.DISABLED))
                self.frame.bind("<ButtonRelease-1>", self.on_icon_release)

                for key in ('name', 'board', 'status', 'epoch'):
                    self.strings.update(
                        {key: tk.StringVar()}
                    )
                    self.labels.update(
                        {
                            key: LabelTemplate(
                                self.frame,
                                textvariable=self.strings[key],
                                anchor=tk.W,
                                padx=20,
                                font=Fonts.mono(8),
                                relief=tk.FLAT,
                                bg=Palette.generic.BLACK,
                                fg=Palette.generic.DISABLED
                            )
                        }
                    )
                    self.labels[key].bind("<ButtonPress-1>",
                                          lambda event: self.icon.configure(bg=Palette.generic.DISABLED))
                    self.labels[key].bind("<ButtonRelease-1>", self.on_icon_release)

                self.labels['name'].configure(
                    font=Fonts.monobold(12),
                )

                self.labels['board'].configure(
                    font=Fonts.monobold(10),
                )

                self.icon.bind("<ButtonPress-1>", lambda event: self.icon.configure(bg=Palette.generic.DISABLED))
                self.icon.bind("<ButtonRelease-1>", self.on_icon_release)

                self.on_update()

            # def loop(self):
            #     if Config.flags.update.BOARD:
            #         self.on_update()

            def on_icon_release(self, *event):
                self.icon.configure(bg=Palette.generic.BG_DEFAULT)
                self.frame.configure(bg=Palette.generic.BLACK)
                alert_os('Setting local board to {}'.format(self.board))
                self.master.master.master.master.master.show_frame(self.board)
                # _local_status.board = self.board

                # info_os('Requesting update for {}'.format(self.board))
                # client.serverequest("UPDATE:{}".format(self.board))
                # client.serverequest("SYNC:{}".format(self.board))

            def on_update(self, *args, **kwargs):
                super(UiBoards.BoardList.Header, self).on_update()

                if self.exist():

                    board = System.boards()[self.board]
                    self.strings['name'].set(board.name)
                    self.strings['board'].set('{}:{}'.format(board.type, board.board))

                    if System.io.is_connected():

                        if board.is_connected():

                            self.icon.replace_image(Icons.BOARD_GREEN())
                            self.strings['status'].set(
                                '{0:7} KB : '.format(board.status.std['freeram'].data) + \
                                '{0:5} ms'.format(board.status.std['looptime'].data))

                            if any([item for key, item in board.status.std.packet.payload.items() if
                                    str(key).startswith('error')]):
                                string = ''.join([
                                    ':{}'.format(item) for key, item in board.status.std.packet.payload.items() if
                                    str(key).startswith('error')
                                ])
                            else:
                                string = board.boardtime.strftime('%b %d %Y - %H:%M:%S')

                            self.strings['epoch'].set(string)
                            for widget in self.labels.values():
                                widget.configure(fg=Palette.generic.WHITE)

                        else:
                            self.icon.replace_image(Icons.BOARD_RED())
                            self.strings['status'].set('OFFLINE')
                            self.strings['epoch'].set('OFFLINE')
                            for widget in self.labels.values():
                                widget.configure(fg=Palette.generic.WARNING)
                    else:
                        self.icon.replace_image(Icons.BOARD_RED())
                        self.strings['status'].set('OFFLINE')
                        self.strings['epoch'].set('OFFLINE')
                        for widget in self.labels.values():
                            widget.configure(fg=Palette.generic.DISABLED)
                else:
                    if self.strings['name'].get == '':
                        self.strings['name'].set('ERROR')
                    if self.strings['board'].get == '':
                        self.strings['board'].set('ERROR')

                    self.strings['epoch'].set('NOT-FOUND')
                    self.strings['status'].set('NOT-FOUND')
                    self.icon.replace_image(Icons.system.NOT_FOUND())

                    for widget in self.labels.values():
                        widget.configure(fg=Palette.generic.WARNING)

            def on_resize(self):
                w, h = super(UiBoards.BoardList.Header, self).on_resize()

                w_frame = w - h
                h_frame = h - 4

                self.icon.place(
                    x=2,
                    y=2,
                    width=h - 4,
                    heigh=h - 4
                )

                self.frame.place(
                    x=h - 2,
                    y=2,
                    width=w_frame,
                    heigh=h_frame
                )

                h_name = 0.3 * h_frame
                h_type = 0.25 * h_frame
                self.labels['name'].place(
                    x=2,
                    y=2,
                    width=w_frame - 4,
                    heigh=h_name
                )
                self.labels['board'].place(
                    x=2,
                    y=h_name + 4,
                    width=w_frame - 4,
                    heigh=h_type
                )
                hr = h_frame - (h_type + h_name + 8)
                self.labels['epoch'].place(
                    x=2,
                    y=h_type + h_name + 6,
                    width=w_frame - 4,
                    heigh=0.5 * hr
                )
                self.labels['status'].place(
                    x=2,
                    y=0.5 * hr + h_type + h_name + 6,
                    width=w_frame - 4,
                    heigh=0.5 * hr
                )
                self.icon.on_resize()
                # pass

        def __init__(self, parent, **kwargs):
            ScrollableFrameTemplate.__init__(self, parent, **kwargs)

        def build(self):
            super(UiBoards.BoardList, self).build()
            self.on_update()

        def on_update(self, *args, **kwargs):
            super(UiBoards.BoardList, self).on_update()
            flag=False
            for board in System.boards().keys():
                if not board in [ widget.board for widget in self.widgetlist ]:
                    self.add(self.Header, str(board))
                    flag=True

            for widget in self.widgetlist:
                widget.on_update()

            if flag:
                self.on_resize()

        def loop(self):
            if Config.flags.update.BOARD:
                self.on_update()

        def on_resize(self, *args, **kwargs):
            widget_width = 70
            return super(UiBoards.BoardList, self).on_resize(widget_width)

    class Devices(PanedTemplate):

        class Header:

            class Template(CanvasTemplate, DeviceLinkTemplate):

                def __init__(self, parent, board, sensor, **kwargs):
                    CanvasTemplate.__init__(self, parent, **kwargs)
                    DeviceLinkTemplate.__init__(self, board, sensor)
                    self.icon = PictureTemplate(self, Icons.BOARD_RED())
                    self.labels=dict()

                def build(self):
                    super(UiBoards.Devices.Header.Template, self).build()
                    self.configure(bg=Palette.generic.BLACK)
                    self.icon.build()
                    self.icon.configure(
                        relief=tk.RAISED,
                    )
                    self.icon.bind("<ButtonPress-1>", self.on_press)
                    self.icon.bind("<ButtonRelease-1>", self.on_release)

                    for key in ('name', 'desc'):
                        self.labels.update(
                            {
                                key:LabelTemplate(
                                    self,
                                    relief=tk.FLAT,
                                    anchor=tk.W,
                                    font=Fonts.monobold(11),
                                    padx=10,
                                    bg=Palette.generic.BLACK
                                )
                            }
                        )
                        self.labels[key].bind("<ButtonPress-1>", self.on_press)
                        self.labels[key].bind("<ButtonRelease-1>", self.on_release)

                    self.bind_default('selection')

                    device = System.boards()[self.board].device[self.device]
                    self.labels['name'].configure(text=device.name)
                    self.labels['desc'].configure(text=device.devtype, font=Fonts.monobold(8))

                def on_press(self, *event):
                    self.icon.configure(bg=Palette.generic.DISABLED)

                def on_release(self, *event):
                    self.icon.configure(bg=Palette.generic.BG_DEFAULT)
                    if self.exist():
                        error_ui('setting status to {}'.format(self.device))
                        self.master.master.master.master.master.show_device(self.device)
                        # _local_status.device=self.device

                def on_update(self, *args, **kwargs):
                    super(UiBoards.Devices.Header.Template, self).on_update(*args, **kwargs)
                    if not self.exist():
                        self.labels['desc'].configure(text='---', fg=Palette.generic.ERROR_GUI)
                        self.labels['name'].configure(text='---', fg=Palette.generic.ERROR_GUI)
                        return

                    device = System.boards()[self.board].device[self.device]

                    self.labels['desc'].configure(text='{} on PIN {}'.format(device.devtype, device.pin))

                    if not System.io.is_connected():
                        self.labels['desc'].configure( fg=Palette.generic.DISABLED )
                        self.labels['name'].configure(
                            text=device.name,
                            fg=Palette.generic.DISABLED
                        )

                    elif not device.is_connected():
                        self.labels['desc'].configure( fg=Palette.generic.OFFLINE )
                        self.labels['name'].configure(
                            text=device.name,
                            fg=Palette.generic.OFFLINE
                        )

                    else:
                        self.labels['desc'].configure( fg=Palette.generic.FG_DEFAULT )
                        self.labels['name'].configure(
                            text=device.name,
                            fg=Palette.generic.ONLINE
                        )

                def on_resize(self, *args, **kwargs):
                    w,h = super(UiBoards.Devices.Header.Template, self).on_resize(*args, **kwargs)
                    ratio = 0.6

                    self.icon.place(
                        x=0,
                        y=0,
                        width=h,
                        heigh=h
                    )
                    self.labels['name'].place(
                        x=h,
                        y=2,
                        width=w - (h + 2),
                        heigh=ratio * h
                    )

                    self.labels['desc'].place(
                        x=h,
                        y=ratio * h,
                        width=w - (h + 2),
                        heigh=h - (ratio * h)
                    )
                    self.icon.on_resize()
                    return w, h

            class Relay(Template):

                def __init__(self, parent, board, sensor, **kwargs):
                    UiBoards.Devices.Header.Template.__init__(self, parent, board, sensor, **kwargs)
                    self.icon = PictureTemplate(self, Icons.devices.diodes.OFFLINE())


                def build(self):
                    super(UiBoards.Devices.Header.Relay, self).build()

                #     if not self.exist():
                #         error_os('<{}:{}> does not exist'.format(self.board, self.device))
                #         return
                #
                #     device = System.boards()[self.board].device[self.device]
                #     alert_os('Requesting SET {} for {}:{}'.format(int(device.relay_state()), self.board, self.device))
                #     payload = {
                #         'name': self.board,
                #         'devname': self.device,
                #         'command': 'SET',
                #         'value1': 0
                #     }
                #     if device.relay_state() == 0:
                #         payload['value1'] = 1
                #
                #     p = IndoorinoPacket(
                #         name=self.board,
                #         command=IBACOM_SET_DEVICE,
                #         data=payload)
                #
                #     client.send_board(self.board, p.dictionary())

                def on_update(self, *args, **kwargs):
                    super(UiBoards.Devices.Header.Relay, self).on_update()
                    if not self.exist():
                        self.icon.replace_image(Icons.devices.diodes.ERROR())
                        return

                    device = System.boards()[self.board].device[self.device]

                    if not System.io.is_connected() or not device.is_connected():
                        self.icon.replace_image(Icons.devices.diodes.OFFLINE())
                        return

                    if device.status.dev['relay_state'].data:
                        self.icon.replace_image(Icons.devices.diodes.ON())
                    else:
                        self.icon.replace_image(Icons.devices.diodes.OFF())

        class DeviceList(ScrollableFrameTemplate, BoardLinkTemplate):

            def __init__(self, parent, board, **kwargs):
                ScrollableFrameTemplate.__init__(self, parent, **kwargs)
                BoardLinkTemplate.__init__(self, board)

            def build(self):
                super(UiBoards.Devices.DeviceList, self).build()
                self.on_update()

            def on_update(self, *args, **kwargs):

                flag=False
                if self.exist():
                    board = self.get_board()
                    for device in board.device.values():
                        if not device.name in [widget.device for widget in self.widgetlist]:
                            flag=True
                            if device.devtype == 'RELAY':
                                self.add(UiBoards.Devices.Header.Relay, board.name, device.name)

                super(UiBoards.Devices.DeviceList, self).on_update()
                if flag:
                    self.on_resize()

            def loop(self):
                if Config.flags.update.DEVICES:
                    self.on_update()

            def on_resize(self, *args, **kwargs):
                widget_width = 42
                return super(UiBoards.Devices.DeviceList, self).on_resize(widget_width)

        class DeviceDisplay(CanvasTemplate, DeviceLinkTemplate):

            class Widgets:

                class Relay(CanvasTemplate, DeviceLinkTemplate):

                    def __init__(self, parent, board, sensor, **kwargs):
                        CanvasTemplate.__init__(self, parent, **kwargs)
                        DeviceLinkTemplate.__init__(self, board, sensor)

                        self.lines=dict()
                        self.buttons=dict()
                        self.label = LabelTemplate(self)

                        self._waitfor=None  # status change needed to exit <waiting>
                        self._status=0      # inner flag for graphic layout and wait

                        """
                        0 = OFF
                        1 = ON
                        2 = WAITING
                        3 = OFFLINE
                        4 = ERROR
                        """

                    def build(self, *args, **kwargs):
                        super(UiBoards.Devices.DeviceDisplay.Widgets.Relay, self).build()

                        self.configure(relief=tk.SUNKEN)
                        self.bind_default('all')
                        for key in ('led', 'switch', 'cursor'):
                            self.lines.update(
                                {
                                    key:self.create_polygon(
                                        (0,0,0,0),
                                        fill=Palette.generic.BG_DEFAULT,
                                        outline=Palette.generic.BLACK,
                                        width=2,
                                        smooth=tk.TRUE,
                                    )
                                }
                            )

                        for key in ('Reset', 'Update', 'Configure', 'Switch ON', 'Switch OFF'):
                            self.buttons.update(
                                {
                                    key:ButtonTemplate(
                                        self,
                                        font=Fonts.mono(8),
                                        text=key,
                                        command=lambda c=key: self.callback(c)
                                    )
                                }
                            )

                        self.itemconfigure(self.lines['switch'], smooth=tk.FALSE)
                        self.label.configure(
                            font=Fonts.monobold(10),
                            relief=tk.FLAT,
                        )
                        self.on_update()

                    def loop(self):
                        if Config.flags.update.DEVICES:
                            self.on_update()

                    def on_update(self, *args, **kwargs):

                        if not self.exist():
                            self._status = 4
                            for widget in self.buttons.values():
                                widget.configure(state=tk.DISABLED)
                            return

                        for widget in self.buttons.values():
                            widget.configure(state=tk.NORMAL)

                        device = self.get_device()
                        if not device.is_connected(): # or not System.io.is_connected(): # reactivate this after finish
                            self._status = 3
                            return

                        if self._status != 2 or device.status.dev['relay_state'].data == self._waitfor:
                            if device.status.dev['relay_state'].data:
                                self._status = 1
                            else:
                                self._status = 0

                        self.resize_cursor()

                    def on_motion(self, *event):
                        super(UiBoards.Devices.DeviceDisplay.Widgets.Relay, self).on_motion()
                        if not self.exist():
                            self.settooltip('')
                            self.on_update()
                            return

                        if self.lines['switch'] in self.current_id:
                            if self._status == 0:
                                self.settooltip('Click to turn ON')
                                return
                            elif self._status == 1:
                                self.settooltip('Click to turn OFF')
                                return
                            elif self._status == 2:
                                self.settooltip('Waiting update')
                                return
                            elif self._status == 3:
                                self.settooltip('{} OFFLINE'.format(self.device))
                                return

                        self.settooltip('')

                    def on_press(self, *event):
                        super(UiBoards.Devices.DeviceDisplay.Widgets.Relay, self).on_press()
                        if any(item in (self.lines['cursor'], self.lines['switch']) for item in self.current_id):
                            self.itemconfigure(self.lines['cursor'], fill=Palette.generic.DISABLED)

                    def on_release(self, *event):
                        super(UiBoards.Devices.DeviceDisplay.Widgets.Relay, self).on_release()
                        if any(item in (self.lines['cursor'], self.lines['switch']) for item in self.current_id):
                            self.itemconfigure(self.lines['cursor'], fill=Palette.generic.BG_DEFAULT)
                            if self.exist():
                                device = self.get_device()
                                if device.is_connected():
                                    if device.status.dev['relay_state'].data:
                                        #send turn OFF
                                        d = {
                                                'devname':self.device,
                                                'command':'SET',
                                                'value1':0
                                            }
                                        p = IndoorinoPacket()
                                        p.build(IBACOM_SET_DEVICE, self.board, d)
                                        System.io.send(p)
                                        self._waitfor=False
                                        self._status=2

                                        # self.after(2000, lambda : (device.status.dev['relay_state'].set(False), self.on_update()))

                                    else:
                                        #send turn ON
                                        d = {
                                                'devname':self.device,
                                                'command':'SET',
                                                'value1':1
                                            }
                                        p = IndoorinoPacket()
                                        p.build(IBACOM_SET_DEVICE, self.board, d)
                                        System.io.send(p)
                                        self._waitfor=True
                                        self._status=2

                                        # self.after(2000, lambda : (device.status.dev['relay_state'].set(True), self.on_update()))

                            self.on_update()
                            self.on_motion(0)

                    def callback(self, command):
                        print('Callback : {}'.format(command))
                        if command == 'Update':
                            self.on_update()
                            self.on_resize()
                            p = IndoorinoPacket()
                            p.build(IBACOM_SET_DEVICE, self.board, {
                                'devname': self.device,
                                'command': 'UPDATE',
                            })
                            System.io.send(p)
                        elif command == 'Reset':
                            p = IndoorinoPacket()
                            p.build(IBACOM_SET_DEVICE, self.board, {
                                'devname': self.device,
                                'command': 'RESET',
                            })
                            System.io.send(p)

                        elif command == 'Configure':

                            TopDeviceEditor(self, self.board, self.device).show()
                            self.master.master.master.master.on_update()
                            self.on_resize()

                        elif command == 'Switch ON':
                            p = IndoorinoPacket()
                            p.build(IBACOM_SET_DEVICE, self.board, {
                                'devname': self.device,
                                'command': 'SET',
                                'value1': 1
                            })
                            System.io.send(p)

                        elif command == 'Switch OFF':
                            p = IndoorinoPacket()
                            p.build(IBACOM_SET_DEVICE, self.board, {
                                'devname': self.device,
                                'command': 'SET',
                                'value1': 0
                            })
                            System.io.send(p)

                    def resize_cursor(self):
                        w=self.winfo_width()
                        h=self.winfo_height()

                        w_swt = max(100, 0.3 * w)
                        w_but = max(20, 0.1 * w) - 4
                        h_but = w_but

                        off_g=10
                        off_x = 20
                        off_y = 0.5 * (h - h_but)


                        shift = 2 * off_g
                        if self._status == 0:
                            shift += (w_swt - (w_but + off_g))
                            self.itemconfigure(self.lines['led'], fill=Palette.generic.OFFLINE)
                            self.itemconfigure(self.lines['switch'], fill=Palette.generic.OFF)
                            self.label.configure(
                                text='{}:OFF'.format(self.device),
                                fg=Palette.generic.WHITE)
                        elif self._status == 1:
                            shift += 0
                            self.itemconfigure(self.lines['led'], fill=Palette.generic.ON)
                            self.itemconfigure(self.lines['switch'], fill=Palette.generic.ONLINE)
                            self.label.configure(
                                text='{}:ON'.format(self.device),
                                fg=Palette.generic.WHITE)
                        elif self._status == 2:
                            shift += 0.5 * (w_swt - (w_but + off_g))
                            self.itemconfigure(self.lines['led'], fill=Palette.generic.WARNING)
                            self.label.configure(
                                text='WAITING'.format(self.device),
                                fg=Palette.generic.WHITE)
                        elif self._status == 3:
                            shift += 0.5 * (w_swt - (w_but + off_g))
                            self.itemconfigure(self.lines['led'], fill=Palette.generic.OFF)
                            self.itemconfigure(self.lines['switch'], fill=Palette.generic.DISABLED)
                            self.label.configure(
                                text='{}:OFFLINE'.format(self.device),
                                fg=Palette.generic.WARNING)
                        else:
                            shift += 0.5 * (w_swt - (w_but + off_g))
                            self.itemconfigure(self.lines['led'], fill=Palette.generic.DISABLED)
                            self.itemconfigure(self.lines['switch'], fill=Palette.generic.DISABLED)
                            self.label.configure(
                                text='{} NOT FOUND'.format(self.device),
                                fg=Palette.generic.DISABLED)

                        sharpness = 10
                        self.coords(self.lines['cursor'], self.coord_smooth_rectangle(
                            (off_x + w_but + shift,
                             off_y,
                             off_x + w_but + shift + w_but,
                             off_y + h_but), sharpness
                        ))

                    def on_resize(self, *args, **kwargs):
                        w,h = super(UiBoards.Devices.DeviceDisplay.Widgets.Relay, self).on_resize()

                        sharpness=10
                        w_led = max(20, 0.1 * w)
                        h_led = w_led
                        h_lab = 30
                        h_butt = 24

                        off_g = 10
                        off_x = 20
                        off_b = 0.5 * max(0, h - len(self.buttons) * h_butt)
                        off_y = 0.5 * (h - h_led)
                        w_swt = max(100, 0.3 * w)
                        h_swt = max(40, 0.1 * w)

                        self.coords(self.lines['led'], self.coord_smooth_rectangle(
                            (off_x,
                             off_y,
                             off_x + w_led,
                             off_y + h_led), sharpness
                        ))

                        self.coords(self.lines['switch'], self.coord_smooth_rectangle(
                            (off_x + w_led + off_g,
                             0.5 * (h - h_swt),
                             off_x + w_led + off_g + w_swt,
                             0.5 * (h - h_swt) + h_swt), sharpness
                        ))

                        self.resize_cursor()

                        self.label.place(
                            x=off_x,
                            y=0.5 * (h - h_swt) - h_lab,
                            width=w_swt + w_led + off_g,
                            heigh=h_lab - 4

                        )
                        for count, widget in enumerate(self.buttons.values()):
                            widget.place(
                                x=w * (1 - 0.4),
                                y=off_b + count * h_butt,
                                width= 0.4 * w - off_g,
                                heigh=h_butt
                            )

            class Header(CanvasTemplate, DeviceLinkTemplate):

                class Template(CanvasTemplate, DeviceLinkTemplate):
                    def __init__(self, parent, board, sensor, **kwargs):
                        CanvasTemplate.__init__(self, parent, **kwargs)
                        DeviceLinkTemplate.__init__(self, board, sensor)
                        self.labels = dict()
                        self.icon = PictureTemplate(self, Icons.BOARD_RED())

                    def build(self, *args, **kwargs):
                        super(UiBoards.Devices.DeviceDisplay.Header.Template, self).build()
                        self.icon.build()
                        for key in ('name', 'status'):
                            self.labels.update(
                                {
                                    key:LabelTemplate(
                                        self,
                                        relief=tk.FLAT,
                                        anchor=tk.W,
                                        font=Fonts.monobold(12),
                                        padx=10,
                                        bg=Palette.generic.BLACK
                                    )
                                }
                            )
                        self.labels['status'].configure(font=Fonts.monobold(10))

                    def loop(self):
                        if Config.flags.update.DEVICES:
                            self.on_update()

                    def on_update(self, *args, **kwargs):

                        if self.exist():
                            device = self.get_device()

                            self.labels['name'].configure(
                                text='{}:{}'.format(device.boardname, device.name))
                            self.labels['status'].configure(
                                text='{} ({})'.format(device.devtype, device.status.std['status'].value))

                        else:
                            self.labels['name'].configure(text='DELETED')
                            self.labels['status'].configure(text='DELETED')

                    def on_resize(self, *args, **kwargs):
                        w, h=super(UiBoards.Devices.DeviceDisplay.Header.Template, self).on_resize()

                        h_name = 0.6 * h

                        self.icon.place(
                            x=0,
                            y=0,
                            width=h,
                            heigh=h
                        )
                        self.labels['name'].place(
                            x=h,
                            y=0,
                            width=w - h,
                            heigh=h_name
                        )
                        self.labels['status'].place(
                            x=h,
                            y=h_name,
                            width=w - h,
                            heigh=h - h_name
                        )
                        self.icon.on_resize()

                class Relay(Template):

                    def __init__(self, parent, board, sensor, **kwargs):
                        UiBoards.Devices.DeviceDisplay.Header.Template.__init__(self, parent, board, sensor, **kwargs)
                        self.icon = PictureTemplate(self, Icons.devices.diodes.OFFLINE())

                    def on_update(self):
                        super(UiBoards.Devices.DeviceDisplay.Header.Relay, self).on_update()
                        if self.exist():
                            device = self.get_device()

                            if device.is_connected() and System.io.is_connected():

                                if device.status.dev['relay_state'].data:
                                    self.icon.replace_image(Icons.devices.diodes.ON())
                                else:
                                    self.icon.replace_image(Icons.devices.diodes.OFF())
                            else:
                                self.icon.replace_image(Icons.devices.diodes.OFFLINE())
                        else:
                            self.icon.replace_image(Icons.devices.diodes.ERROR())

            class Container(PanedTemplate, DeviceLinkTemplate):

                def __init__(self, parent, board, sensor, context, **kwargs):
                    PanedTemplate.__init__(self, parent, **kwargs)
                    DeviceLinkTemplate.__init__(self, board, sensor)
                    self.labels = dict()
                    self.values = dict()
                    self.units = dict()
                    self._context = context

                def build(self, *args, **kwargs):
                    super(UiBoards.Devices.DeviceDisplay.Container, self).build()
                    device = self.get_device()

                    c = self._context.split(':')
                    p = device.__getattribute__(c[0]).__getattribute__(c[1])

                    for key, entry in p.items():
                        self.labels.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    font=Fonts.mono(9),
                                    anchor=tk.W,
                                    padx=10,
                                    text=entry.label,
                                    fg=Palette.generic.WHITE,
                                )
                            }
                        )
                        self.values.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    font=Fonts.mono(8),
                                    anchor=tk.W,
                                    padx=10,
                                    text=entry.value,
                                    fg=Palette.generic.WHITE,
                                )
                            }
                        )
                        self.units.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    font=Fonts.mono(9),
                                    anchor=tk.W,
                                    padx=10,
                                    text=entry.unit,
                                    fg=Palette.generic.WHITE,
                                )
                            }
                        )

                def on_update(self, *args, **kwargs):
                    if self.exist():
                        device = self.get_device()
                        c = self._context.split(':')
                        p = device.__getattribute__(c[0]).__getattribute__(c[1])
                        for key, entry in p.items():
                            self.values[key].configure(
                                text=entry.value,
                                fg=Palette.generic.FG_DEFAULT
                            )
                    else:
                        for entry in self.values.values():
                            entry.configure(
                                text='DELETED',
                                fg=Palette.generic.ERROR_GUI)

                def on_resize(self, *args, **kwargs):
                    w, h = super(UiBoards.Devices.DeviceDisplay.Container, self).on_resize()

                    h_label = max(20, 0.01 * h)
                    y_off = 5
                    w_label = max(5, 0.475 * w)
                    w_value = max(5, 0.38 * w)

                    for count, key in enumerate(self.labels.keys()):
                        self.labels[key].place(
                            x=0,
                            y=y_off + count * h_label,
                            width=w_label,
                            heigh=h_label
                        )
                        self.values[key].place(
                            x=w_label,
                            y=y_off + count * h_label,
                            width=w_value,
                            heigh=h_label
                        )
                        self.units[key].place(
                            x=w_label + w_value,
                            y=y_off + count * h_label,
                            width=w - (w_label + w_value) - 2,
                            heigh=h_label
                        )

            def __init__(self, parent, board, sensor, **kwargs):
                CanvasTemplate.__init__(self, parent, **kwargs)
                DeviceLinkTemplate.__init__(self, board, sensor)
                self.labels = dict()
                self.body = {
                    'cs':   self.Container(self, board, sensor, 'config:std'),
                    'ss':   self.Container(self, board, sensor, 'status:std'),
                    'cd': self.Container(self, board, sensor, 'config:dev'),
                    'sd': self.Container(self, board, sensor, 'status:dev'),
                }

                dev = self.get_device()
                if dev.devtype == 'RELAY':
                    self.devwidget=self.Widgets.Relay(self, board, sensor)
                    self.header = self.Header.Relay(self, board, sensor)
                else:
                    self.devwidget = PanedTemplate(self, bg=Palette.generic.ERROR_GUI)
                    self.header = self.Header.Template(self, board, sensor)

            def build(self, *args, **kwargs):
                super(UiBoards.Devices.DeviceDisplay, self).build()
                self.header.build()
                self.devwidget.build()
                for item in self.body.values():
                    item.build()
                    item.configure(relief=tk.RAISED)

            def loop(self):
                self.header.loop()
                self.devwidget.loop()
                for item in self.body.values():
                    item.loop()

            def on_update(self, *args, **kwargs):
                self.header.on_update()
                self.devwidget.on_update()
                for item in self.body.values():
                    item.on_update()

            def on_resize(self, *args, **kwargs):
                w,h=super(UiBoards.Devices.DeviceDisplay, self).on_resize()

                h_head = max(60, h * 0.1)
                y_off = h_head

                self.header.place(
                    x=0,
                    y=0,
                    width=w,
                    heigh=h_head
                )

                for count, item in enumerate(self.body.values()):

                    h_body = len(item.labels) * 20 + 10

                    item.place(
                        x=0,
                        y=y_off,
                        width=w - 2,
                        heigh=h_body
                    )
                    item.on_resize()
                    y_off += h_body

                self.devwidget.place(
                    x=2,
                    y=y_off + 2,
                    width=w - 50,
                    heigh=h - (y_off + 2)
                )

                self.header.on_resize()
                self.devwidget.on_resize()

        def __init__(self, parent, board):
            PanedTemplate.__init__(self, parent)
            self.devlist=self.DeviceList(self, board)
            self.widgets = dict()
            self.current = ''

        def build(self, *args, **kwargs):
            super(UiBoards.Devices, self).build()
            self.devlist.build()
            for dev in self.devlist.get_board().device.values():
                self.widgets.update(
                    {
                        dev.name : self.DeviceDisplay(self, self.devlist.board, dev.name)
                    }
                )
                self.widgets[dev.name].build()

        def loop(self):

            self.devlist.loop()
            if self.current in self.widgets.keys():
                self.widgets[self.current].loop()

        def show_device(self, device):
            if device in self.widgets.keys():
                if self.current in self.widgets.keys():
                    self.widgets[self.current].place_forget()
                self.current=device
                self.widgets[self.current].on_update()
                self.on_resize()

        def on_update(self, *args, **kwargs):
            self.devlist.on_update()
            if self.devlist.exist():
                for dev in self.devlist.get_board().device.values():
                    if dev.name not in self.widgets.keys():
                        self.widgets.update(
                            {
                                dev.name: self.DeviceDisplay(self, self.devlist.board, dev.name)
                            }
                        )
                        self.widgets[dev.name].build()
                    self.widgets[dev.name].on_update()

            if self.current in self.widgets.keys():
                self.widgets[self.current].on_update()

        def on_resize(self, *args, **kwargs):
            w,h = super(UiBoards.Devices, self).on_resize()
            w_list=max(260, 0.2*w)

            self.devlist.place(
                x=0,
                y=0,
                width=w_list,
                heigh=h
            )

            if self.current in self.widgets.keys():
                self.widgets[self.current].place(
                    x=w_list,
                    y=0,
                    width=w - w_list,
                    heigh=h
                )
                self.widgets[self.current].on_resize()

            self.devlist.on_resize()

    class BoardDisplay(PanedTemplate, BoardLinkTemplate):

        class Info(PanedTemplate):

            class Header(PanedTemplate, BoardLinkTemplate):

                def __init__(self, parent, boardname, **kwargs):
                    PanedTemplate.__init__(self, parent, **kwargs)
                    BoardLinkTemplate.__init__(self, boardname)
                    self.labels = dict()
                    self.buttons = dict()

                def build(self, *args, **kwargs):
                    super(UiBoards.BoardDisplay.Info.Header, self).build()
                    self.configure(bg=Palette.generic.BLACK)

                    board = System.boards()[self.board]
                    for key in ('name', 'type', 'board'):
                        self.labels.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    anchor=tk.W,
                                    font=Fonts.monobold(12),
                                    relief=tk.FLAT,
                                    text=board.config.std[key].value,
                                    bg=Palette.generic.BLACK,
                                )
                            }
                        )

                    for key in ('reset', 'update', 'configure'):
                        self.buttons.update(
                            {
                                key: ButtonTemplate(
                                    self,
                                    font=Fonts.mono(8),
                                    text=key.capitalize(),
                                    command=lambda c=key: self.callback(c),
                                )
                            }
                        )

                def callback(self, command):

                    if self.exist():
                        if command == 'reset':
                            System.io.send_server_request("RESET:{}".format(self.board))
                        elif command == 'update':
                            System.io.send_server_request("UPDATE:{}".format(self.board))
                            System.io.send_server_request("SYNC:{}".format(self.board))
                        if command == 'configure':
                            p=TopBoardEditor(self, self.board)
                            p.show()
                            self.master.master.master.on_update()

                    self.on_update()

                def on_update(self, *args, **kwargs):
                    if self.exist():
                        board = System.boards()[self.board]
                        for key in ('name', 'type', 'board'):
                            self.labels[key].configure(
                                text=board.config.std[key].value,
                                fg=Palette.generic.FG_DEFAULT
                            )
                    else:
                        for key in ('name', 'type', 'board'):
                            self.labels[key].configure(
                                text='DELETED',
                                fg=Palette.generic.ERROR_GUI)

                def on_resize(self, *args, **kwargs):
                    w, h = super(UiBoards.BoardDisplay.Info.Header, self).on_resize()

                    h_label = int(h / len(self.labels.keys()))
                    for count, widget in enumerate(self.labels.values()):
                        widget.place(
                            x=10,
                            y=3 + count * h_label,
                            width=w - 20,
                            heigh=h_label - 2
                        )
                    h_butt = max(0.2 * h, 20)
                    w_butt = max(0.1 * w, 90)
                    for count, widget in enumerate(self.buttons.values()):
                        widget.place(
                            x=w - w_butt * 1.25,
                            y=0.1 * h + count * h_butt,
                            width=w_butt,
                            heigh=h_butt
                        )

            class Container(PanedTemplate, BoardLinkTemplate):

                def __init__(self, parent, boardname, context, **kwargs):
                    PanedTemplate.__init__(self, parent, **kwargs)
                    BoardLinkTemplate.__init__(self, boardname)
                    self.labels = dict()
                    self.values = dict()
                    self.units = dict()
                    self._context = context

                def build(self, *args, **kwargs):
                    super(UiBoards.BoardDisplay.Info.Container, self).build()
                    board = System.boards()[self.board]

                    c = self._context.split(':')
                    p = board.__getattribute__(c[0]).__getattribute__(c[1])

                    for key, entry in p.items():
                        self.labels.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    font=Fonts.mono(9),
                                    anchor=tk.W,
                                    padx=10,
                                    text=entry.label,
                                    fg=Palette.generic.WHITE,
                                )
                            }
                        )
                        self.values.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    font=Fonts.mono(8),
                                    anchor=tk.W,
                                    padx=10,
                                    text=entry.value,
                                    fg=Palette.generic.WHITE,
                                )
                            }
                        )
                        self.units.update(
                            {
                                key: LabelTemplate(
                                    self,
                                    font=Fonts.mono(9),
                                    anchor=tk.W,
                                    padx=10,
                                    text=entry.unit,
                                    fg=Palette.generic.WHITE,
                                )
                            }
                        )

                def on_update(self, *args, **kwargs):
                    if self.exist():
                        board = System.boards()[self.board]
                        c = self._context.split(':')
                        p = board.__getattribute__(c[0]).__getattribute__(c[1])
                        for key, entry in p.items():
                            self.values[key].configure(
                                text=entry.value,
                                fg=Palette.generic.FG_DEFAULT
                            )
                    else:
                        for entry in self.values.values():
                            entry.configure(
                                text='DELETED',
                                fg=Palette.generic.ERROR_GUI)

                def on_resize(self, *args, **kwargs):
                    w, h = super(UiBoards.BoardDisplay.Info.Container, self).on_resize()

                    h_label = max(20, 0.01 * h)
                    y_off   = 5
                    w_label = max(5, 0.475 * w)
                    w_value = max(5, 0.38 * w)

                    for count, key in enumerate(self.labels.keys()):
                        self.labels[key].place(
                            x=0,
                            y=y_off + count * h_label,
                            width=w_label,
                            heigh=h_label
                        )
                        self.values[key].place(
                            x=w_label,
                            y=y_off + count * h_label,
                            width=w_value,
                            heigh=h_label
                        )
                        self.units[key].place(
                            x=w_label + w_value,
                            y=y_off + count * h_label,
                            width=w - (w_label + w_value) - 2,
                            heigh=h_label
                        )

            def __init__(self, parent, boardname, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)

                self.header = self.Header(self, boardname)
                self.labels = dict()
                self.body = {
                    'config_std':   self.Container(self, boardname, 'config:std'),
                    'config_board': self.Container(self, boardname, 'config:board'),
                    'status_std':   self.Container(self, boardname, 'status:std'),
                    'status_board': self.Container(self, boardname, 'status:board')
                }

            def build(self, *args, **kwargs):
                self.header.build()
                for item in self.body.values():
                    item.build()
                    item.configure(relief=tk.RAISED)

                for key in ('config', 'status'):
                    self.labels.update(
                        {
                            key: LabelTemplate(
                                self,
                                font=Fonts.monobold(12),
                                relief=tk.FLAT,
                                anchor=tk.W,
                                text=key.capitalize(),
                                fg=Palette.generic.R_TITLE
                            )
                        }
                    )

            def on_update(self, *args, **kwargs):
                self.header.on_update()
                for item in self.body.values():
                    item.on_update()

            def on_resize(self, *args, **kwargs):
                w, h = super(UiBoards.BoardDisplay.Info, self).on_resize()

                h_header = 80

                self.header.place(
                    x=0,
                    y=0,
                    width=w,
                    heigh=h_header
                )
                self.header.on_resize()

                h_label = 27
                y_off = h_header + h_label

                self.labels['config'].place(
                    x=0,
                    y=h_header,
                    width=w,
                    heigh=h_label
                )

                for count, item in enumerate(self.body.values()):

                    h_body = len(item.labels) * 20 + 10

                    item.place(
                        x=0,
                        y=y_off,
                        width=w - 2,
                        heigh=h_body
                    )
                    item.on_resize()

                    y_off += h_body
                    if count == 1:
                        self.labels['status'].place(
                            x=0,
                            y=y_off,
                            width=w,
                            heigh=h_label
                        )
                        y_off += h_label

        def __init__(self, parent, boardname, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            BoardLinkTemplate.__init__(self, boardname)
            self.boardinfo = self.Info(self, boardname)
            self.devlist = UiBoards.Devices(self, boardname)

        def build(self, *args, **kwargs):
            super(UiBoards.BoardDisplay, self).build()
            self.boardinfo.build()
            self.devlist.build()

        def loop(self):
            self.boardinfo.loop()
            self.devlist.loop()

        def on_update(self, *args, **kwargs):
            self.boardinfo.on_update()
            self.devlist.on_update()

        def on_resize(self, *args, **kwargs):
            w,h = super(UiBoards.BoardDisplay, self).on_resize()
            w_display = 350
            w_list = 260

            self.boardinfo.place(
                x=0,
                y=0,
                width=w_display,
                heigh=h
            )
            self.boardinfo.on_resize()

            if len(self.devlist.devlist.widgetlist) > 0:
                self.devlist.place(
                    x=w_display + 2,
                    y=0,
                    width=w - (w_display + 4),
                    heigh=0.5 * h
                )
                self.devlist.on_resize()

    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)

        self.boardlist = self.BoardList(self)
        self.frames=dict()
        self.current=''
        # self.devs=self.Devices(self, self.current)

    def build(self, *args, **kwargs):

        self.boardlist.build()
        self.on_update()

    def show_frame(self, name):
        if name in self.frames.keys():
            print('Showing frame {}'.format(name))

            if name != self.current:
                if self.current in self.frames.keys():
                    self.frames[self.current].place_forget()
                self.current = name

            self.frames[self.current].on_update()
            self.on_resize()
        else:
            warning_ui('No {} board UI found'.format(name))

    def loop(self):

        self.boardlist.loop()
        if Config.flags.update.BOARD:
            self.on_update()

        # c = self.current
        # if _local_status.board and _local_status.board != self.current:
        #     try:
        #         if self.current in self.frames.keys():
        #             self.frames[self.current].place_forget()
        #         self.current=_local_status.board
        #         self.frames[self.current].on_update()
        #     except KeyError:
        #         alert_os('Invalid board name <{}>'.format(self.current))
        #         self.current = c
        #         _local_status.board = c
        #     self.on_resize()


        if self.current in self.frames.keys():
            self.frames[self.current].loop()

    def on_update(self, *args, **kwargs):

        self.boardlist.on_update()
        for board in System.boards().keys():
            if not board in self.frames.keys():
                self.frames.update(
                    {
                        board:self.BoardDisplay(self, board,)
                    }
                )
                self.frames[board].build()
                alert_os('Added UI board {}'.format(board))

        if self.current in self.frames.keys():
            self.frames[self.current].on_update()

    def on_resize(self, *args, **kwargs):
        w,h=super(UiBoards, self).on_resize()

        print('resizing for {}'.format(self.current))

        w_list=350
        self.boardlist.place(
            x=5,
            y=5,
            width=w_list,
            heigh=h-10
        )
        self.boardlist.on_resize()

        if self.current in self.frames.keys():
            self.frames[self.current].place(
                x=w_list +10,
                y=5,
                width=w - w_list,
                heigh=h - 10
            )
            self.frames[self.current].on_resize()

        # try:
        #     z=self.Devices.Header.Relay(self, 'HOUSE.BEAMS', 'BEAM1', bg='blue')
        #     z.build()
        #     z.on_update()
        #     z.place(
        #         x=w_list + w_display + 20,
        #         y=5,
        #         width=300,
        #         heigh=100
        #     )
        #     z.on_resize()
        # except KeyError:
        #     pass




