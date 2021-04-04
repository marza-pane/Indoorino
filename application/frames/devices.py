from common.templates import *
from common.comtable import *
from indoorino.packet import IndoorinoPacket

import tkinter.messagebox

class DeviceTypeList(PanedTemplate):
    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)

        self._listbox=ListBoxTemplate(self)
        self._scroll=tk.Scrollbar(self._listbox)
        self._type=''
        self._source=(
            'ANALOG',
            'RELAY',
            'DHT22',
            'TIMER',
            'STEPPER',
            'SERVO'
        )
    
    @property
    def type(self):
        return self._type

    def clear(self):
        self._type=''
        self._listbox.selection_clear(0, tk.END)

    def build(self):
        super(DeviceTypeList, self).build()
        self._listbox.configure(
            font=Fonts.monobold(10),
            bg=Palette.generic.WHITE,
            fg=Palette.generic.BLACK,
            selectbackground=Palette.generic.R_TITLE,
            selectforeground=Palette.generic.BLACK,
            selectmode=tk.SINGLE,
        )
        self._listbox.bind("<<ListboxSelect>>", self.callback_select)
        self._listbox.config(yscrollcommand=self._scroll.set)
        self._scroll.config(command=self._listbox.yview)
        self._scroll.configure(width=10)

        for key in self._source:
            self._listbox.insert(tk.END, key)
    
    def callback_select(self, *evt):
        index = self._listbox.curselection()
        if index:
            self._type=self._source[index[0]]

    def on_resize(self):
        w, h = super(DeviceTypeList, self).on_resize()
        self._listbox.place(
            x=0,
            y=0,
            width=w,
            heigh=h,
        )
        self._scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self._listbox.on_resize()


class TopAddDevice(TopLevelTemplate):

    def __init__(self, parent, **kwargs):
        TopLevelTemplate.__init__(self, parent, **kwargs)
        self._frame = DeviceTypeList(self)
        self._buttons=dict()
        self._entries=dict()
        self._values=dict()
        self._labels=dict()

    def build(self):
        super(TopAddDevice, self).build()
        self.geometry('{}x{}+{}+{}'.format(
            300, 600,
            max(10, int(0.5 * Screen.CURRENT_SCREEN_W) - 150),
            max(10, int(0.5 * Screen.CURRENT_SCREEN_H) - 300),
        ))
        self._frame.build()
        self.title('Add device:')

        for key in ('cancel', 'apply'):
            self._buttons.update(
                {
                    key:ButtonTemplate(
                        self,
                        font=Fonts.mono(10),
                        text=key.capitalize(),
                        command=lambda c=key : self.callback(c)
                    )
                }
            )
        for key in ('name', 'pin'):
            self._values.update(
                {
                    key: tk.StringVar(self)
                }
            )
            self._entries.update(
                {
                    key:EntryTemplate(
                        self,
                        font=Fonts.mono(10),
                        textvariable=self._values[key],
                    )
                }
            )
            self._labels.update(
                {
                    key:LabelTemplate(
                        self,
                        font=Fonts.mono(8),
                        text=key.upper(),
                    )
                }
            )

        self._values['name'].set('<device-name>')
        self._values['pin'].set('<device-pin>')

    def show(self):
        super(TopAddDevice, self).show()
        return self._frame.type

    def callback(self, command):
        if command == 'cancel':
            self._frame.clear()
        self.destroy()

    def on_resize(self):
        w,h=super(TopAddDevice, self).on_resize()

        h_butt=25
        h_entry=25
        h_list=h - (h_butt + 2 * h_entry + 40)
        w_entry=0.6 * w
        self._frame.place(
            x=0,
            y=0,
            width=w,
            heigh=h_list,
        )

        self._frame.on_resize()

        for count, key in enumerate(self._labels.keys()):
            self._labels[key].place(
                x=0,
                y=h_list + count * h_entry,
                width=w - w_entry,
                heigh=h_entry,
            )
            self._entries[key].place(
                x=w - w_entry,
                y=h_list + count * h_entry,
                width=w_entry,
                heigh=h_entry,
            )


        self._buttons['cancel'].place(
            x=5,
            y=h_list + 2 * h_entry,
            heigh=h_butt,
            width= 0.5 * w - 8
        )
        self._buttons['apply'].place(
            x=0.5 * w + 3,
            y=h_list + 2 * h_entry,
            heigh=h_butt,
            width= 0.5 * w - 8
        )

class TopDeviceSelectionList(TopLevelTemplate, BoardLinkTemplate):

    def __init__(self, parent, board, **kwargs):
        TopLevelTemplate.__init__(self, parent, **kwargs)
        BoardLinkTemplate.__init__(self, board)

        self._devlist=ListBoxTemplate(self)
        self._scroll=tk.Scrollbar(self._devlist)
        self._buttons=dict()
        self.device=''

    def build(self):
        super(TopDeviceSelectionList, self).build()
        self.geometry('{}x{}+{}+{}'.format(
            300, 600,
            max(10, int(0.5 * Screen.CURRENT_SCREEN_W) - 150),
            max(10, int(0.5 * Screen.CURRENT_SCREEN_H) - 300),
        ))
        self.title('Choose:')
        self._devlist.configure(
            font=Fonts.monobold(10),
            bg=Palette.generic.WHITE,
            fg=Palette.generic.BLACK,
            selectbackground=Palette.generic.R_TITLE,
            selectforeground=Palette.generic.BLACK,
            selectmode=tk.SINGLE,
        )
        self._devlist.bind("<<ListboxSelect>>", self.callback_select)
        self._devlist.config(yscrollcommand=self._scroll.set)
        self._scroll.config(command=self._devlist.yview)
        self._scroll.configure(width=10)

        for key in self.get_board().device.keys():
            self._devlist.insert(tk.END, str(key))

        for key in ('cancel', 'apply'):
            self._buttons.update(
                {
                    key:ButtonTemplate(
                        self,
                        font=Fonts.mono(10),
                        text=key.capitalize(),
                        command=lambda c=key : self.callback(c)
                    )
                }
            )

    def show(self):
        super(TopDeviceSelectionList, self).show()
        return self.device

    def callback_select(self, *evt):
        index = self._devlist.curselection()
        if index:
            self.device=list(self.get_board().device.values())[index[0]].name

    def callback(self, command):
        if command == 'cancel':
            self.device=''
        self.destroy()

    def on_resize(self):
        w,h=super(TopDeviceSelectionList, self).on_resize()

        h_butt=30
        self._devlist.place(
            x=0,
            y=0,
            heigh=h - (h_butt + 10),
            width=w
        )
        self._buttons['cancel'].place(
            x=5,
            y=h - (h_butt + 5),
            heigh=h_butt,
            width= 0.5 * w - 8
        )
        self._buttons['apply'].place(
            x=0.5 * w + 3,
            y=h - (h_butt + 5),
            heigh=h_butt,
            width= 0.5 * w - 8
        )
        self._scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self._devlist.on_resize()


class DeviceEditorFrame(PanedTemplate, DeviceLinkTemplate):

    class Container(PanedTemplate, DeviceLinkTemplate):

        def __init__(self, parent, board, sensor, context, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            DeviceLinkTemplate.__init__(self, board, sensor)
            self.labels = dict()
            self.values = dict()
            self.units = dict()
            self.strings = dict()
            self._context = context

        def build(self, *args, **kwargs):
            super(DeviceEditorFrame.Container, self).build()
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
                self.strings.update(
                    {
                        key: tk.StringVar(self)
                    }
                )
                self.values.update(
                    {
                        key: EntryTemplate(
                            self,
                            relief=tk.RAISED,
                            font=Fonts.monobold(10),
                            textvariable=self.strings[key],
                            fg=Palette.generic.WHITE,
                            bg=Palette.generic.BLACK,
                            disabledbackground=Palette.generic.BLACK,
                            selectforeground='yellow',
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
            self.on_update()

        def on_update(self, *args, **kwargs):
            if self.exist():
                device = self.get_device()
                c = self._context.split(':')
                p = device.__getattribute__(c[0]).__getattribute__(c[1])
                for key, entry in p.items():
                    self.strings[key].set(entry.value)
                    self.units[key].configure(fg=Palette.generic.FG_DEFAULT)
                    self.labels[key].configure(fg=Palette.generic.FG_DEFAULT)
                    self.values[key].configure(
                        fg=Palette.generic.FG_DEFAULT,
                        state=tk.NORMAL
                    )
                    if key == 'boardname':
                        self.values[key].configure(state=tk.DISABLED)

            else:
                for key, entry in self.strings:
                    entry.set('DELETED')
                    self.units[key].configure(fg=Palette.generic.ERROR_GUI)
                    self.labels[key].configure(fg=Palette.generic.ERROR_GUI)
                    self.values[key].configure(
                        fg=Palette.generic.ERROR_GUI,
                        state=tk.DISABLED
                    )

        def on_resize(self, *args, **kwargs):
            w, h = super(DeviceEditorFrame.Container, self).on_resize()
            h_label = max(25, 0.01 * h)
            y_off = 5
            w_label = 10 + 9 * max([len(x.cget('text')) for x in self.labels.values()])
            w_value = max(50, 0.2 * w)

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

    def __init__(self, parent, board, device, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)
        DeviceLinkTemplate.__init__(self, board, device)
        self.body = {
            # 'cs': self.Container(self, board, device, 'config:std'),
            'cd': self.Container(self, board, device, 'config:dev'),
        }
        self.icon=PictureTemplate(self, Icons.BOARD_RED())
        self.label=LabelTemplate(self)
        self.buttons=dict()

    def build(self, *args, **kwargs):
        super(DeviceEditorFrame, self).build()

        device = self.get_device()
        if System.io.is_connected() and device.is_connected():
            self.icon.replace_image(Icons.devices[device.devtype].ONLINE())
        else:
            self.icon.replace_image(Icons.devices[device.devtype].OFFLINE())
        self.label.configure(
            anchor=tk.W,
            font=Fonts.monobold(14),
            text='[{}] {}:{}'.format(device.devtype, self.board, self.device)
        )

        self.icon.build()
        for entry in self.body.values():
            entry.build()

        for key in ('apply', 'restore', 'delete', 'exit'):
            self.buttons.update(
                {
                    key:ButtonTemplate(
                        self,
                        font=Fonts.mono(9),
                        text=key.capitalize(),
                        command=lambda c=key : self.callback(c)
                    )
                }
            )

        if not System.io.is_connected():
            self.buttons['apply'].configure(state=tk.DISABLED)

    def on_update(self, *args, **kwargs):
        super(DeviceEditorFrame, self).on_update()
        for entry in self.body.values():
            entry.update()

    def callback(self, command):

        if command == 'exit':
            self.master.destroy()
        elif command == 'delete':

            c = tk.messagebox.askyesnocancel('Confirm remove', 'Proceed re-configuring board?')
            if c is None:
                return
            if c is True:
                p = IndoorinoPacket()
                p.build(IBACOM_REM_DEVICE, self.board, { 'devname': self.device })
                System.io.send(p)
            self.get_board().remove_dev(self.device)
            self.master.destroy()

        elif command == 'restore':
            self.body['cd'].on_update()
        elif command == 'apply':
            self.apply_changes()

    def apply_changes(self):

        device = self.get_device()

        flag=False
        packet = device.config.dev.packet()

        for key, item in device.config.dev.items():
            if key in ('name', 'pin'):
                continue
            if str(item.data) != self.body['cd'].strings[key].get().rstrip():
                flag=True
                info_os('Editing parameter <{}> {} ==> {}'.format(
                    key,packet.payload[key],
                    self.body['cd'].strings[key].get().rstrip()
                ))
                if isinstance(packet.payload[key], str):
                    packet.payload[key] = self.body['cd'].strings[key].get().rstrip()

        if flag:
            p = IndoorinoPacket()
            p.build(packet.command, self.board, { packet.payload } )
            System.io.send(p)

        if device.pin != int(self.body['cd'].strings['pin'].get()):
            print('found different PIN')
            p = IndoorinoPacket()
            p.build(IBACOM_SET_DEVPIN, self.board, {
                'devname': self.device,
                'pin1': int(self.body['cd'].strings['pin'].get()) })
            System.io.send(p)

        if self.device != self.body['cd'].strings['name'].get():
            warning_boards('found different name')
            p = IndoorinoPacket()
            p.build(IBACOM_SET_DEVNAME, self.board, {
                'devname': self.device,
                'newname': self.body['cd'].strings['name'].get() })
            System.io.send(p)
            self.get_board().remove_dev(self.device)
            self.master.destroy()

    def on_resize(self, *args, **kwargs):
        w, h = super(DeviceEditorFrame, self).on_resize()

        s_icon = max( min(w,h) * 0.1, 100 )
        h_title = 40
        y_off = s_icon + 10
        self.icon.place(
            x=5,
            y=5,
            width=s_icon,
            heigh=s_icon
        )
        self.icon.on_resize()
        self.label.place(
            x= s_icon + 10,
            y= 0.5 * (s_icon - h_title + 5),
            width=w - (s_icon + 10),
            heigh=h_title
        )

        for count, item in enumerate(self.body.values()):
            h_body = len(item.labels) * 25 + 10

            item.place(
                x=0,
                y=y_off,
                width=w - 2,
                heigh=h_body
            )
            item.on_resize()
            y_off += h_body

        h_butt = max(0.05 * h, 25)
        w_butt = max(0.1 * w, 80)
        for count, item in enumerate(self.buttons.values()):

            item.place(
                x=5 + w_butt * count,
                y=h - (h_butt + 2),
                width=w_butt,
                heigh=h_butt
            )
            item.on_resize()

            self.buttons['exit'].place(
                x=w - (w_butt + 5),
            )

class TopDeviceEditor(TopLevelTemplate):

    def __init__(self, parent, board, device, **kwargs):
        TopLevelTemplate.__init__(self, parent, **kwargs)
        self.frame = DeviceEditorFrame(self, board, device)

    def build(self):
        super(TopDeviceEditor, self).build()
        self.geometry('680x440')
        self.title('device editor - {}'.format(self.frame.device))
        self.frame.build()
        self.frame.on_update()

    def show(self):
        super(TopDeviceEditor, self).show()
        return True

    def on_resize(self):
        super(TopDeviceEditor, self).on_resize()
        self.frame.pack(expand=tk.TRUE, fill=tk.BOTH)
        self.frame.on_resize()



class BoardEditorFrame(PanedTemplate, BoardLinkTemplate):

    class Container(PanedTemplate, BoardLinkTemplate):

        def __init__(self, parent, board, context, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            BoardLinkTemplate.__init__(self, board)
            self.labels = dict()
            self.values = dict()
            self.units = dict()
            self.strings = dict()
            self._context = context

        def build(self, *args, **kwargs):
            super(BoardEditorFrame.Container, self).build()
            board = self.get_board()

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
                self.strings.update(
                    {
                        key: tk.StringVar(self)
                    }
                )
                self.values.update(
                    {
                        key: EntryTemplate(
                            self,
                            relief=tk.RAISED,
                            font=Fonts.monobold(10),
                            textvariable=self.strings[key],
                            fg=Palette.generic.WHITE,
                            bg=Palette.generic.BLACK,
                            disabledbackground=Palette.generic.BLACK,
                            selectforeground='yellow',
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
            self.on_update()

        def on_update(self, *args, **kwargs):
            if self.exist():
                board = self.get_board()
                c = self._context.split(':')
                p = board.__getattribute__(c[0]).__getattribute__(c[1])

                for key, entry in p.items():
                    self.strings[key].set(entry.value)
                    self.units[key].configure(fg=Palette.generic.FG_DEFAULT)
                    self.labels[key].configure(fg=Palette.generic.FG_DEFAULT)
                    self.values[key].configure(
                        fg=Palette.generic.FG_DEFAULT,
                        state=tk.NORMAL
                    )

                    if key in ('boardtype',):
                        self.values[key].configure(state=tk.DISABLED)

            else:
                for key, entry in self.strings:
                    entry.set('DELETED')
                    self.units[key].configure(fg=Palette.generic.ERROR_GUI)
                    self.labels[key].configure(fg=Palette.generic.ERROR_GUI)
                    self.values[key].configure(
                        fg=Palette.generic.ERROR_GUI,
                        state=tk.DISABLED
                    )

        def on_resize(self, *args, **kwargs):
            w, h = super(BoardEditorFrame.Container, self).on_resize()
            h_label = max(25, 0.01 * h)
            y_off = 5
            try:
                w_label = 10 + 9 * max([len(x.cget('text')) for x in self.labels.values()])
            except ValueError:
                w_label = 30
            w_value = max(50, 0.2 * w)

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

    def __init__(self, parent, board, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        BoardLinkTemplate.__init__(self, board)
        self.body = {
            # 'cs': self.Container(self, board, device, 'config:std'),
            'cd': self.Container(self, board, 'config:board'),
        }
        self.icon=PictureTemplate(self, Icons.BOARD_RED())
        self.label=LabelTemplate(self)
        self.buttons=dict()

    def build(self, *args, **kwargs):
        super(BoardEditorFrame, self).build()

        board = self.get_board()
        if System.io.is_connected() and board.is_connected():
            self.icon.replace_image(Icons.BOARD_RED())
        else:
            self.icon.replace_image(Icons.BOARD_GREEN())
        self.label.configure(
            anchor=tk.W,
            font=Fonts.monobold(14),
            text='[{}] {}'.format(board.type, self.board)
        )

        self.icon.build()
        for entry in self.body.values():
            entry.build()

        for key in ('apply', 'delete', 'restore', 'factory', 'add device', 'remove device', 'exit'):
            self.buttons.update(
                {
                    key:ButtonTemplate(
                        self,
                        font=Fonts.mono(9),
                        text=key.capitalize(),
                        command=lambda c=key : self.callback(c)
                    )
                }
            )

        if not System.io.is_connected():
            self.buttons['apply'].configure(state=tk.DISABLED)

    def on_update(self, *args, **kwargs):
        super(BoardEditorFrame, self).on_update()
        for entry in self.body.values():
            entry.update()

    def callback(self, command):

        if command == 'exit':
            self.master.destroy()

        elif command == 'delete':

            # c = tk.messagebox.askyesnocancel('Confirm remove', 'Proceed re-configuring board?')
            # if c is None:
            #     return
            # if c is True:
            #     p = IndoorinoPacket()
            #     p.build(IBACOM_REM_DEVICE, self.board, { 'devname': self.device })
            #     System.io.send(p)
            #
            System.boards.remove(self.board)
            self.master.destroy()

        elif command == 'restore':
            self.body['cd'].on_update()

        elif command == 'apply':
            self.apply_changes()

        elif command == 'factory':
            p = IndoorinoPacket()
            p.build(IBACOM_FACTORY_RESET, self.board, { })
            System.io.send(p)

        elif command == 'remove device':
            dev = TopDeviceSelectionList(self, self.board).show()

            if dev in self.get_board().device.keys():
                p = IndoorinoPacket()
                p.build(IBACOM_REM_DEVICE, self.board, {'devname': dev})
                System.io.send(p)
                self.get_board().remove_dev(dev)

        elif command == 'add device':

            t = TopAddDevice(self).show()

            if t:
                print('Generating {} template'.format(t))
                HERE

    def apply_changes(self):
        pass
        # if self.device != self.body['cd'].strings['name'].get():
        #     print('found different name')
        #     p = IndoorinoPacket()
        #     p.build(IBACOM_SET_DEVNAME, self.board, {
        #         'devname': self.device,
        #         'newname': self.body['cd'].strings['name'].get() })
        #     System.io.send(p)
        #
        # device = self.get_device()
        #
        # if device.pin != int(self.body['cd'].strings['pin'].get()):
        #     print('found different PIN')
        #     p = IndoorinoPacket()
        #     p.build(IBACOM_SET_DEVPIN, self.board, {
        #         'devname': self.device,
        #         'pin1': int(self.body['cd'].strings['pin'].get()) })
        #     System.io.send(p)
        #
        # flag=False
        # packet = device.config.dev.packet()
        # if not isinstance(packet, IndoorinoPacket):
        #     return
        #
        # for key, item in device.config.dev.items():
        #     if str(item.data) != self.body['cd'].strings[key].get().rstrip():
        #         flag=True
        #         info_os('Editing parameter <{}> {} ==> {}'.format(
        #             key,packet.payload[key],
        #             self.body['cd'].strings[key].get().rstrip()
        #         ))
        #         if isinstance(packet.payload[key], str):
        #             packet.payload[key] = self.body['cd'].strings[key].get().rstrip()
        #
        # if flag:
        #     p = IndoorinoPacket()
        #     p.build(packet.command, self.board, { packet.payload } )
        #     System.io.send(p)

    def on_resize(self, *args, **kwargs):
        w, h = super(BoardEditorFrame, self).on_resize()

        s_icon = max( min(w,h) * 0.1, 100 )
        h_title = 40
        y_off = s_icon + 10
        self.icon.place(
            x=5,
            y=5,
            width=s_icon,
            heigh=s_icon
        )
        self.icon.on_resize()
        self.label.place(
            x= s_icon + 10,
            y= 0.5 * (s_icon - h_title + 5),
            width=w - (s_icon + 10),
            heigh=h_title
        )

        for count, item in enumerate(self.body.values()):
            h_body = len(item.labels) * 25 + 10

            item.place(
                x=0,
                y=y_off,
                width=w - 2,
                heigh=h_body
            )
            item.on_resize()
            y_off += h_body

        h_butt = max(0.05 * h, 25)
        w_butt = max(0.1 * w, 80)
        for count, item in enumerate(self.buttons.values()):

            item.place(
                x=5 + w_butt * count,
                y=h - (h_butt + 2),
                width=w_butt,
                heigh=h_butt
            )
            item.on_resize()

        self.buttons['exit'].place(
            x=w - (w_butt + 5),
        )

        h_butt = max(0.05 * h, 35)
        w_butt = max(0.2 * w, 130)

        for count, key in enumerate(('factory', 'add device', 'remove device',)):
            self.buttons[key].place(
                    x=w - (w_butt + 10),
                    y=s_icon + 10 + count * h_butt,
                    width=w_butt - 5,
                    heigh=h_butt
                )

class TopBoardEditor(TopLevelTemplate):

    def __init__(self, parent, board, **kwargs):
        TopLevelTemplate.__init__(self, parent, **kwargs)
        self.frame = BoardEditorFrame(self, board)

    def build(self):
        super(TopBoardEditor, self).build()
        self.geometry('680x440')
        self.title('board editor - {}'.format(self.frame.board))
        self.frame.build()
        self.frame.on_update()

    def show(self):
        super(TopBoardEditor, self).show()
        return True

    def on_resize(self):
        super(TopBoardEditor, self).on_resize()
        self.frame.pack(expand=tk.TRUE, fill=tk.BOTH)
        self.frame.on_resize()

