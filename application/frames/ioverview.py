from common.comtable import *
from indoorino.packet import IndoorinoPacket
from common.templates import *

import tkinter.messagebox

class CurrentUiOverviewStatus:
    def __init__(self):
        self.board=''
        self.device=''
        self.packet=0

_current_state = CurrentUiOverviewStatus()

class UiIOverview(PanedTemplate):

    class Device(PanedTemplate, DeviceLinkTemplate):

        def __init__(self, parent, board, name, **kwargs):

            PanedTemplate.__init__(self, parent, **kwargs)
            DeviceLinkTemplate.__init__(self, board, name)

            self.labels = dict()
            self.buttons = dict()

        def build(self, *args, **kwargs):

            self.configure(relief=tk.RAISED)

            for key in ('name', 'label', 'status'):
                self.labels.update(
                    {
                        key : LabelTemplate(
                            self,
                            font=Fonts.mono(9),
                            anchor=tk.W,
                            padx=20,
                        )
                    }
                )

            for key in ('reset', 'update'):
                self.buttons.update(
                    {
                        key: ButtonTemplate(
                            self,
                            font=Fonts.mono(10),
                            text=key.capitalize(),
                            command=lambda *evt, command=key.upper(): self.callback(command)
                        )
                    }
                )

            self.build_dev()
            self.on_update()

            self.bind_mouse_buttons()
            for widget in self.labels.values():
                widget.bind("<ButtonPress-1>", self.on_press)
                widget.bind("<ButtonRelease-1>", self.on_release)
                # widget.bind("<MouseWheel>", self.master.scroll.set)

        def build_dev(self):

            if self.exist():
                device = System.boards()[self.board].device[self.device]
                self.labels['name'].configure(
                    font=Fonts.monobold(11),
                    text='{}:{}'.format(
                        device.config.std['devtype'].data,
                        device.config.std['name'].data)
                )
                self.labels['label'].configure(
                    text='{}:pin {}'.format(self.board, device.pin)
                )
                self.labels['status'].configure(
                    textvariable=device.status.std['status'].value
                )

            else:
                error_ui('ioverview:devicelist: board {} or device {} do not exist'.format(self.board, self.device))
                self.labels['name'].configure(
                    font=Fonts.monobold(11),
                    bg=Palette.generic.ERROR_GUI,
                    text='NOT FOUND'
                )
                self.labels['label'].configure(
                    bg=Palette.generic.ERROR_GUI,
                    text='{}:{}'.format(self.board, self.device)
                )
                self.labels['status'].configure(
                    bg=Palette.generic.ERROR_GUI,
                    text='NOT FOUND',
                )

        def on_update(self, *args, **kwargs):
            super(UiIOverview.Device, self).on_update(*args, **kwargs)

            try:
                device = System.boards()[self.board].device[self.device]
                if device.is_connected():
                    self.labels['name'].configure(fg=Palette.generic.ONLINE)
                    self.labels['status'].configure(fg=Palette.generic.FG_DEFAULT)
                else:
                    self.labels['name'].configure(fg=Palette.generic.OFFLINE)
                    self.labels['status'].configure(fg=Palette.generic.WARNING)
            except KeyError:
                alert_os('Updating gui after clean up')
                self.build_dev()
                self.on_update()

        def on_press(self, *event):
            super(UiIOverview.Device, self).on_press(*event)
            self.configure(relief=tk.SUNKEN)
            self.configure(bg=Palette.generic.WARNING)
            for entry in self.labels.values():
                entry.configure(bg=Palette.generic.WARNING,
                                fg=Palette.generic.BLACK)
            for entry in self.buttons.values():
                entry.configure(bg=Palette.generic.WARNING,
                                fg=Palette.generic.BLACK)

        def on_release(self, *event):
            # warning_os('setting current board device {}:{}'.format(self.board, self.device))
            self.configure(relief=tk.RAISED)
            self.configure(bg=Palette.generic.BG_DEFAULT)
            for entry in self.labels.values():
                entry.configure(bg=Palette.generic.BG_DEFAULT,
                                fg=Palette.generic.FG_DEFAULT)
            for entry in self.buttons.values():
                entry.configure(bg=Palette.generic.BG_DEFAULT,
                                fg=Palette.generic.FG_DEFAULT)

            _current_state.board=self.board
            _current_state.device=self.device
            self.after(100, self.master.master.master.master.master.deviceview.on_update)
            self.on_update()

        def callback(self, command):
            alert_os('called {}'.format(command))
            if not self.exist():
                error_os('<{}:{}> does not exist'.format(self.board, self.device))
                return


            alert_os('Requesting {} for {}:{}'.format(command, self.board, self.device))

            p = IndoorinoPacket()

            if command == 'UPDATE':
                p.build(IBACOM_REQ_DEV_STATUS, self.board, { 'devname' : self.device } )
            if command == 'RESET':
                p.build(IBACOM_REQ_DEV_STATUS, self.board, { 'devname' : self.device } )

            System.io.send2board(self.board, p)

        def on_resize(self, *args, **kwargs):
            w, h = super(UiIOverview.Device, self).on_resize(*args, **kwargs)

            hn = 25
            wn = w - 120

            for count, item in enumerate(self.labels.values()):
                item.place(
                    x=2,
                    y=2 + count * (2 + hn),
                    width=w - 4,
                    heigh=hn
                )

            for count, item in enumerate(self.buttons.values()):
                item.place(
                    x=wn + 4,
                    y=0.5 * hn + count * (hn + 2),
                    width=w - (wn + 4),
                    heigh=hn
                )

    class DeviceViewer(TextBoxTemplate):

        def __init__(self, parent, **kwargs):

            TextBoxTemplate.__init__(self, parent, **kwargs)

            self.configure(
                state=tk.DISABLED,
                font=Fonts.monobold(8),
                relief=tk.FLAT,
                bg=Palette.generic.BLACK,
            )
            self.text('Initializing...')
            self.after(1000, lambda : self.text(''))

        def loop(self):
            if Config.flags.update.DEVICES:
                self.on_update()

        def on_update(self):
            brd=_current_state.board
            dev=_current_state.device
            if brd in System.boards().keys() and \
                dev in System.boards()[brd].device.keys():
                device = System.boards()[brd].device[dev]
                string = '\n {}:{} ({})'.format(device.boardname, device.name, device.devtype)
                string += '\n\t{0:<18}'.format("device status")
                string += ':{}\n'.format(device.status.std['status'].data)

                for key, entry in device.config.dev.packet.payload.items():
                    string += '\n\t{0:<18}'.format(key)
                    string += ':{}'.format(entry)
                string += '\n'
                for key, entry in device.status.dev.packet.payload.items():
                    string += '\n\t{0:<18}'.format(key)
                    string += ':{}'.format(entry)

                self.text(string)

                self.tag_add("start", "3.20", "3.30")
                if device.is_connected():
                    self.tag_config("start", foreground=Palette.generic.ONLINE)
                else:
                    self.tag_config("start", foreground=Palette.generic.OFFLINE)
            else:
                self.text('')

    class DeviceList(ScrollableFrameTemplate):

        def __init__(self, parent, **kwargs):
            ScrollableFrameTemplate.__init__(self, parent, **kwargs)

        def show_board(self, boardname):

            if not boardname in System.boards().keys():
                return

            self.clear()

            for dev in System.boards()[boardname].device.values():
                debug_ui('ADDING {}:{}'.format(boardname, str(dev.name)))
                self.add(UiIOverview.Device, boardname, str(dev.name))
                self.on_resize()

        def on_resize(self, *args, **kwargs):
            widget_width = 80
            return super(UiIOverview.DeviceList, self).on_resize(widget_width)

        def loop(self):
            if Config.flags.update.DEVICES:
                self.on_update()

    class PacketList(PanedTemplate):

        def __init__(self, parent, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)

            self.buttons = dict()
            self.packetlist = list()

            self.label = LabelTemplate(self)
            self.listbox = ListBoxTemplate(self)
            self._scroll = tk.Scrollbar(self.listbox)

        def build(self, *args, **kwargs):
            super(UiIOverview.PacketList, self).build(*args)
            for key in ('filter', 'clear'):
                self.buttons.update(
                    { key : ButtonTemplate(
                        self,
                        font=Fonts.monobold(9),
                        text=key.capitalize(),
                        command=lambda *evt, command=key.upper(): self.callback(command)

                    )}
                )

            self.label.configure(
                font=Fonts.mono(8),
                relief=tk.FLAT,
                anchor=tk.W,
            )

            self.listbox.configure(
                font=Fonts.mono(9),
                bg=Palette.generic.WHITE,
                fg=Palette.generic.BLACK,
                selectbackground=Palette.generic.R_TITLE,
                selectforeground=Palette.generic.BLACK,
                selectmode=tk.SINGLE,
            )
            self.listbox.bind("<<ListboxSelect>>", self.callback_select)
            self.listbox.config(yscrollcommand=self._scroll.set)
            self._scroll.config(command=self.listbox.yview)
            self._scroll.configure(width=10)

        def callback_select(self, *evt):
            index = self.listbox.curselection()
            if index:
                _current_state.packet = self.packetlist[index[0]].pid
                self.master.packetview.on_update()

        def callback(self, command):
            if command == 'CLEAR':
                _current_state.packet=0
                System.io.clear_rx()
                self.on_update()
                self.callback_select()

        def loop(self):
            if Config.flags.update.PACKET:
                self.on_update()

        def on_update(self):
            super(UiIOverview.PacketList, self).on_update()
            self.packetlist = list(reversed(System.io.rx_packets()))
            if len(self.packetlist) == self.listbox.size():
                return

            self.listbox.select_clear(0)
            self.listbox.delete(0, tk.END)

            for packet in self.packetlist:
                entry = '{}:{}'.format(packet.source, packet.label)
                self.listbox.insert(tk.END, entry)

            count = len(System.io.rx_packets())
            if count > 0:
                self.label.configure(
                    text='Total/Shown {}:{}'.format(count, len(self.packetlist)).rjust(12))
            else:
                self.label.configure(text='Total/Shown {0:<12}'.format('---:---'.rjust(12)))

            """ This part is needed to keep cursor selection"""
            if _current_state.packet != 0:
                for count, entry in enumerate(self.packetlist):
                    if entry.pid == _current_state.packet:
                        self.listbox.selection_set(count, count)
                        return
                """ ^^^ ^^^ """

        def on_resize(self):
            w, h = super(UiIOverview.PacketList, self).on_resize()
            bh = 30
            self.listbox.place(
                x=2,
                y=2,
                width=w - 4,
                heigh=h - (bh + 6)
            )
            self._scroll.pack(side=tk.RIGHT, fill=tk.Y)

            self.label.place(
                x=2,
                y=h - (bh + 2),
                width=(0.5 * w),
                heigh=bh
            )
            self.buttons['filter'].place(
                x=(0.5 * w) + 2,
                y=h - (bh + 2),
                width=(0.25 * w) - 4,
                heigh=bh
            )
            self.buttons['clear'].place(
                x=(0.75 * w) + 2,
                y=h - (bh + 2),
                width=(0.25 * w) - 4,
                heigh=bh
            )
            return w, h

    class BoardList(PanedTemplate):

        def __init__(self, parent, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)

            self.buttons = dict()

            self.listbox = ListBoxTemplate(self)
            self._scroll = tk.Scrollbar(self.listbox)

        def build(self, *args, **kwargs):
            super(UiIOverview.BoardList, self).build(*args)
            for key in ('filter', 'clear'):
                self.buttons.update(
                    { key : ButtonTemplate(
                        self,
                        font=Fonts.monobold(9),
                        text=key.capitalize(),
                        command=lambda *evt, command=key.upper(): self.callback(command)

                    )}
                )

            self.listbox.configure(
                font=Fonts.mono(9),
                bg=Palette.generic.WHITE,
                fg=Palette.generic.BLACK,
                selectbackground=Palette.generic.R_TITLE,
                selectforeground=Palette.generic.BLACK,
                selectmode=tk.SINGLE,
            )
            self.listbox.bind("<<ListboxSelect>>", self.callback_select)
            self.listbox.config(yscrollcommand=self._scroll.set)
            self._scroll.config(command=self.listbox.yview)
            self._scroll.configure(width=10)

        def callback_select(self, *evt):
            index = self.listbox.curselection()
            if index:
                _current_state.board = self.listbox.selection_get() #self.boardlist[index[0]].pid
                _current_state.device = ''
                error_os('Current board set to {}'.format(_current_state.board))
                self.master.boardview.on_update()
                self.master.deviceview.on_update()
                self.master.devicelist.show_board(_current_state.board)

        def callback(self, command):
            if command == 'CLEAR':
                _current_state.board=''
                _current_state.packet=''
                System.boards.clear()
                self.on_update()
                self.callback_select()
            else:
                warning_ui('board:list: invalid callback {}'.format(command))

        def loop(self):
            if Config.flags.update.BOARD:
                self.on_update()

        def on_update(self):
            super(UiIOverview.BoardList, self).on_update()

            self.listbox.select_clear(0)
            self.listbox.delete(0, tk.END)

            for key in System.boards().keys():
                self.listbox.insert(tk.END, str(key))

        def on_resize(self):
            w, h = super(UiIOverview.BoardList, self).on_resize()
            bh = 30
            self.listbox.place(
                x=2,
                y=2,
                width=w - 4,
                heigh=h - (bh + 6)
            )
            self._scroll.pack(side=tk.RIGHT, fill=tk.Y)


            self.buttons['filter'].place(
                x=2,
                y=h - (bh + 2),
                width=(0.5 * w) - 4,
                heigh=bh
            )
            self.buttons['clear'].place(
                x=(0.5 * w) + 2,
                y=h - (bh + 2),
                width=(0.5 * w) - 4,
                heigh=bh
            )
            return w, h

    class PacketViewer(TextBoxTemplate):

        def __init__(self, parent, **kwargs):

            TextBoxTemplate.__init__(self, parent, **kwargs)

            self.configure(
                state=tk.DISABLED,
                font=Fonts.monobold(8),
                relief=tk.FLAT,
                bg=Palette.generic.BLACK,
            )
            self.text('Initializing...')
            self.after(2000, lambda : self.text(''))

        def loop(self):
            if Config.flags.update.PACKET:
                self.on_update()

        def on_update(self):
            for packet in System.io.rx_packets():
                if _current_state.packet == packet.pid:
                    string = '\n PACKET {}:{}'.format(packet.command, packet.label)
                    for key, entry in packet.payload.items():
                        string += '\n\t{0:<18}'.format('<{}>'.format(key))
                        string += ':<{}>'.format(entry)
                    self.text(string)

    class BoardViewer(PanedTemplate):

        def __init__(self, parent, **kwargs):

            PanedTemplate.__init__(self, parent, **kwargs)

            self.labels = dict()
            self.buttons = dict()
            self.displays = dict()

            # self.lbl_name = LabelTemplate(self, tooltip='Board unique name')
            # self.lbl_board = LabelTemplate(self, tooltip='Board configuration and hardware')
            #
            # self.lbl_conf = LabelTemplate(self, text='Configuration')
            # self.lbl_stat = LabelTemplate(self, text='Status')
            #
            # self.btn_reset = tk.Button(self)
            # self.btn_update = tk.Button(self)
            #
            # self.txt_conf = TextBoxTemplate(self, relief=tk.FLAT, )
            # self.txt_stat = TextBoxTemplate(self, relief=tk.FLAT, )

            # self.txt_conf   =TextBoxTemplate(self, relief=tk.FLAT, bg='blue', fg='yellow')
            # self.txt_stat   =TextBoxTemplate(self, relief=tk.FLAT, bg='green', fg='snow')
            # self.current_board = ''

        def build(self, *args, **kwargs):
            self.bind_mouse_motion()
            self.configure(bg=Palette.generic.BLACK)

            for key in ('name', 'board', 'configuration', 'status'):
                self.labels.update(
                    {
                        key : LabelTemplate(
                            self,
                            padx=10,
                            anchor=tk.W,
                            font=Fonts.monobold(11),
                            bg=Palette.generic.BLACK,
                            relief=tk.FLAT,
                            text=key.capitalize(),
                        )
                    }
                )

            for key, callback in ( ('update', self.clb_update), ('reset', self.clb_reset),):
                self.buttons.update(
                    {
                        key: ButtonTemplate(
                            self,
                            font=Fonts.mono(10),
                            bg=Palette.generic.BLACK,
                            text=key.capitalize(),
                            command=callback,
                        )
                    }
                )

            for key in ('config', 'status',):
                self.displays.update(
                    {
                        key: TextBoxTemplate(
                            self,
                            state=tk.DISABLED,
                            font=Fonts.monobold(8),
                            wrap=tk.WORD,
                            bg=Palette.generic.BLACK,
                            relief=tk.FLAT,
                        )
                    }
                )

            self.labels['name'].configure(
                font=Fonts.monobold(14),
            )
            self.labels['board'].configure(
                font=Fonts.mono(11),
            )
            self.on_update()

        def clb_reset(self, *event):
            if _current_state.board and _current_state.board in System.boards().keys():
                alert_os('Requesting RESET for {}'.format(_current_state.board))
                System.io.send_server_request("RESET:{}".format(_current_state.board))

        def clb_update(self, *event):
            if _current_state.board and _current_state.board in System.boards().keys():
                info_os('Requesting UPDATE for {}'.format(_current_state.board))
                System.io.send_server_request("UPDATE:{}".format(_current_state.board))
                System.io.send_server_request("SYNC:{}".format(_current_state.board))

        def on_update(self, *args, **kwargs):

            if _current_state.board and _current_state.board in System.boards().keys():

                board = System.boards()[_current_state.board]

                self.labels['name'].configure(text='{}'.format(board.name))
                self.labels['board'].configure(text='{}:{}'.format(board.type, board.board))

                if board.is_connected():
                    self.labels['name'].configure(fg=Palette.generic.ONLINE)
                else:
                    self.labels['name'].configure(fg=Palette.generic.OFFLINE)

                status_string = ''

                for entry in board.status.std.values():
                    status_string += '\n'
                    status_string += '{0:<20}'.format(entry.label)
                    status_string += '{}'.format(entry.value)
                    if entry.unit:
                        status_string += '{}'.format(entry.unit)
                for entry in board.status.board.values():
                    status_string += '\n'
                    status_string += '{0:<20}'.format(entry.label)
                    status_string += '{}'.format(entry.value)
                    if entry.unit:
                        status_string += '{}'.format(entry.unit)

                self.displays['status'].text(status_string)

                config_string = ''

                for entry in board.config.std.values():
                    config_string += '\n'
                    config_string += '{0:<20}'.format(entry.label)
                    config_string += '{}'.format(entry.value)
                    if entry.unit:
                        config_string += '{}'.format(entry.unit)
                for entry in board.config.board.values():
                    config_string += '\n'
                    config_string += '{0:<20}'.format(entry.label)
                    config_string += '{}'.format(entry.value)
                    if entry.unit:
                        config_string += '{}'.format(entry.unit)

                self.displays['config'].text(config_string)

            else:
                self.labels['name'].configure(text='---')
                self.labels['board'].configure(text='')
                self.displays['status'].text('---')
                self.displays['config'].text('---')

            self.on_resize_btn()

        def loop(self):
            if Config.flags.update.BOARD:
                self.on_update()
            # if self.current_board and self.current_board in System.boards().keys():
            # timestring = System.boards()[self.current_board].boardtime.strftime('%b %d %Y - %H:%M:%S')
            # self.lbl_epoch.configure(text='{}'.format(timestring))

        def on_resize_btn(self):

            if _current_state.board and _current_state.board in System.boards().keys():
                w, h = super(UiIOverview.BoardViewer, self).on_resize()
                nmh = 25
                tph = 22

                bth = 25
                btw = 100
                spl = 0.5 * w
                bow = int((spl - 2 * btw) / 3)

                self.buttons['update'].place(
                    x=bow,
                    y=nmh + tph + 6,
                    width=btw,
                    heigh=bth
                )
                self.buttons['reset'].place(
                    x=0.5 * (spl + bow),
                    y=nmh + tph + 6,
                    width=btw,
                    heigh=bth
                )
            else:
                self.buttons['update'].place_forget()
                self.buttons['reset'].place_forget()

        def on_resize(self):
            w, h = super(UiIOverview.BoardViewer, self).on_resize()

            # name heigh
            nmh = 25
            # type heigh
            tph = 22
            # DX labels heigh
            adh = 18

            # buttons heigh and width
            bsh = 30
            # split lines
            spl = 0.5 * w
            csh = 4 * adh + 10

            self.labels['name'].place(
                x=2,
                y=2,
                width=spl - 3,
                heigh=nmh
            )
            self.labels['board'].place(
                x=2,
                y=nmh + 4,
                width=spl - 3,
                heigh=tph
            )

            self.on_resize_btn()

            txh = 0.5 * (h - csh - bsh - 2 * tph)
            self.labels['configuration'].place(
                x=2,
                y=csh,
                width=w - 4,
                heigh=tph
            )

            self.displays['config'].place(
                x=2,
                y=csh + tph,
                width=w - 4,
                heigh=txh
            )

            self.labels['status'].place(
                x=2,
                y=csh + tph + txh + 2,
                width=w - 4,
                heigh=tph
            )
            self.displays['status'].place(
                x=2,
                y=csh + 2 * tph + txh + 2,
                width=w - 4,
                heigh=txh + adh
            )

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)

        self.labels = dict()

        self.rxlist = self.PacketList(self)
        self.boardlist = self.BoardList(self)
        self.devicelist = self.DeviceList(self, bg=Palette.generic.BLACK)

        self.packetview = self.PacketViewer(self)
        self.boardview = self.BoardViewer(self)
        self.deviceview = self.DeviceViewer(self)

        # self.statusbar = self.StatusBar(self)

    def build(self):

        for key in ('received packets', 'boards', 'current board', 'devices'):
            self.labels.update(
                { key : LabelTemplate(
                    self,
                    font=Fonts.monobold(14),
                    fg=Palette.generic.R_TITLE,
                    relief=tk.FLAT,
                    text=key,
                ) }
            )

        self.rxlist.build()
        self.boardlist.build()
        self.devicelist.build()

        self.packetview.build()
        self.boardview.build()
        self.deviceview.build()

        # self.statusbar.build()

    def loop(self):
        self.rxlist.loop()
        self.boardlist.loop()
        self.devicelist.loop()

        self.packetview.loop()
        self.boardview.loop()
        self.deviceview.loop()

        # self.statusbar.loop()

    def on_update(self):
        self.rxlist.on_update()
        self.boardlist.on_update()
        self.devicelist.on_update()

        self.packetview.on_update()
        self.boardview.on_update()
        self.deviceview.on_update()

        # self.statusbar.on_update()

    def on_resize(self):
        w,h = super(UiIOverview, self).on_resize()

        w_rxpacket=max(285, 0.14*w)
        w_boardlist=max(200, 0.11*w)
        w_board=max(500, 0.4*w)
        w_devices=w - int(sum( (w_rxpacket,w_boardlist,w_board) ))
        h_titles=30
        # h_bar=35
        h_forge=100

        w_splits=(w_rxpacket,w_boardlist,w_board,w_devices)

        for count, widget in enumerate(self.labels.values()):
            widget.place(
                x=int(sum(w_splits[0:count])),
                y=0,
                width=w_splits[count],
                heigh=h_titles
            )

        # h_widegts=max(350, (h - h_bar - h_titles))
        h_widegts=max(350, (h - h_titles))

        h_rxpacket= 0.7 * h_widegts

        self.rxlist.place(
            x=0,
            y=h_titles,
            width=w_rxpacket,
            heigh=h_rxpacket
        )
        self.boardlist.place(
            x=w_rxpacket,
            y=h_titles,
            width = w_boardlist,
            heigh = h_rxpacket
        )

        self.packetview.place(
            x=0,
            y=h_titles + h_rxpacket,
            width=w_boardlist + w_rxpacket,
            heigh=h_widegts - h_rxpacket
        )

        self.boardview.place(
            x=w_rxpacket + w_boardlist,
            y=h_titles,
            width = w_board,
            heigh = h_widegts - h_forge
        )

        self.devicelist.place(
            x=w_rxpacket + w_boardlist + w_board,
            y=h_titles,
            width=w_devices,
            heigh=h_rxpacket
        )

        self.deviceview.place(
            x=w_rxpacket + w_boardlist + w_board,
            y=h_titles + h_rxpacket,
            width=w_devices,
            heigh=h_widegts - h_rxpacket
        )

        # self.statusbar.place(
        #     x=0,
        #     y=h - h_bar,
        #     width=w,
        #     heigh=h_bar
        # )

        self.rxlist.on_resize()
        self.boardlist.on_resize()
        self.devicelist.on_resize()

        self.packetview.on_resize()
        self.boardview.on_resize()
        self.deviceview.on_resize()

        # self.statusbar.on_resize()

        return w,h


print('Loaded gui.frame.network')



