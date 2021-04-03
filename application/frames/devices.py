from common.templates import *
from common.comtable import *
from indoorino.packet import IndoorinoPacket

import tkinter.messagebox


class BoardEditorFrame(PanedTemplate, BoardLinkTemplate):
    pass


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
                if key == 'boardname':
                    self.values[key].configure(state=tk.DISABLED)

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
            self.icon.replace_image(Icons.devices[self.get_device().devtype].ONLINE())
        else:
            self.icon.replace_image(Icons.devices[self.get_device().devtype].OFFLINE())
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
        if self.device != self.body['cd'].strings['name'].get():
            print('found different name')
            p = IndoorinoPacket()
            p.build(IBACOM_SET_DEVNAME, self.board, {
                'devname': self.device,
                'newname': self.body['cd'].strings['name'].get() })
            System.io.send(p)

        device = self.get_device()

        if device.pin != int(self.body['cd'].strings['pin'].get()):
            print('found different PIN')
            p = IndoorinoPacket()
            p.build(IBACOM_SET_DEVPIN, self.board, {
                'devname': self.device,
                'pin1': int(self.body['cd'].strings['pin'].get()) })
            System.io.send(p)

        flag=False
        packet = device.config.dev.packet()
        if not isinstance(packet, IndoorinoPacket):
            return

        for key, item in device.config.dev.items():
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

