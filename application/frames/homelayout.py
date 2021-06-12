import time
import tkinter.messagebox

from common.templates import *
from indoorino.packet import IndoorinoPacket

class UiLayout (PanedTemplate):

    class TopDevice(TopLevelTemplate):
        def __init__(self, parent, board, device, **kwargs):
            TopLevelTemplate.__init__(self, parent, **kwargs)
            self._buttons = dict()
            self._labels  = dict()
            self._values  = dict()
            self._entries = dict()
            self._key = (board, device)
            self._menu = tk.Menu(self, tearoff=0)
            self._menu.items=list()
            self.geometry('{}x{}+{}+{}'.format(
                500, 350,
                max(10, int(0.5 * Screen.CURRENT_SCREEN_W) - 150),
                max(10, int(0.5 * Screen.CURRENT_SCREEN_H) - 300),
            ))

        def build(self):
            super(UiLayout.TopDevice, self).build()
            self.title('D E V I C E     L A Y O U T')
            self.bind_keys()

            for key in ('close', 'apply'):
                self._buttons.update(
                    {
                        key: ButtonTemplate(
                            self,
                            font=Fonts.mono(10),
                            text=key.capitalize(),
                            command=lambda c=key: self.callback(c)
                        )
                    }
                )
            for key in ('board', 'device', 'area', 'location', 'type'):
                self._values.update(
                    {
                        key: tk.StringVar(self)
                    }
                )
                self._entries.update(
                    {
                        key: EntryTemplate(
                            self,
                            font=Fonts.mono(10),
                            textvariable=self._values[key],
                            fg=Palette.generic.WHITE,
                            bg=Palette.generic.BLACK,
                            disabledbackground=Palette.generic.BLACK,
                            selectforeground='yellow',
                        )
                    }
                )
                self._entries[key].bind('<ButtonPress-1>', lambda event, c=key:self.on_entry_press(event,c) )
                self._labels.update(
                    {
                        key: LabelTemplate(
                            self,
                            font=Fonts.mono(8),
                            text=key.upper(),
                            anchor=tk.W,
                            padx=10,
                        )
                    }
                )

            if self._key in System.layout.devices.keys():
                d = System.layout.devices[self._key]
                self._values['board'].set(d.boardname)
                self._values['device'].set(d.devname)
                self._values['area'].set(d.area)
                self._values['location'].set(d.location)
                self._values['type'].set(d.devtype)
            else:
                self._values['board'].set('<board-name>')
                self._values['device'].set('<device-name>')
                self._values['area'].set('<device-area>')
                self._values['location'].set('<device-location>')
                self._values['type'].set('<device-type>')

        def show(self):
            super(UiLayout.TopDevice, self).show()
            return {}

        def callback(self, command, x=0, y=0):
            if command in self._entries.keys():
                self._menu.tk_popup(x,y)
                return
            if command == 'apply':
                self._apply_clb()

            self.destroy()

        def _create_device(self):
            d = {
                'devname': self._values['device'].get(),
                'board': self._values['board'].get(),
                'label1': self._values['area'].get(),
                'label2': self._values['location'].get(),
                'type': self._values['type'].get(),
            }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_DEVICE, '', d)
            System.io.send2server(p)

        def _apply_clb(self):
            if not System.io.is_connected():
                tkinter.messagebox.showinfo('', 'Please connect first!')
                self.destroy()
                return

            key = (self._values['board'].get(), self._values['device'].get())
            for k, item in self._values.items():

                if not item.get()[0].isalpha():
                    tk.messagebox.showwarning(None, 'Invalid {} name'.format(k))
                    return

            if key in System.layout.devices.keys():
                lyt = System.layout.devices[key]

                l1 = (
                    self._values['area'].get(),
                    self._values['location'].get(),
                    self._values['type'].get()
                )

                l2 = (
                    lyt.area,
                    lyt.location,
                    lyt.devtype,
                )
                if l1 == l2:
                    self.destroy()
                    return
            else:
                if not tk.messagebox.askyesno('L A Y O U T',
                                              'Do you want to add new device {}:{}'.format(key[0], key[1])):
                    self.destroy()
                    return

            self._create_device()

        def _fill_menu(self, command):
            self._menu.delete('0', tk.END)
            self._menu.items.clear()

            if command == 'board':
                blist=list()
                for key in System.layout.devices.keys():
                    if not key[0] in blist:
                        blist.append(key[0])
                for key in blist:
                    self._menu.add_command(
                        label=key,
                        command=lambda c=key: self._values[command].set(c)
                    )

            elif command == 'device':
                dlist=list()
                for key in System.layout.devices.keys():
                    if key[0] == self._values['board'].get() and not key[1] in dlist:
                        dlist.append(key[1])

                for key in dlist:
                    self._menu.add_command(
                        label=key,
                        command=lambda c=key: self._values[command].set(c)
                    )

            elif command == 'area':
                for key in Config.layout.areas:
                    self._menu.add_command(
                        label=key,
                        command=lambda c=key: self._values[command].set(c)
                    )

            elif command == 'location':
                try:
                    for key in Config.layout.location[self._values['area'].get()]:
                        self._menu.add_command(
                            label=key,
                            command=lambda c=key: self._values[command].set(c)
                        )
                except (KeyError, ):
                    return

            elif command == 'type':
                for key in Config.layout.devtypes:
                    self._menu.add_command(
                        label=key,
                        command=lambda c=key: self._values[command].set(c)
                    )

        def catch_keystroke(self, char):
            self._menu.delete('0', tk.END)
            self._menu.items.clear()

        def on_entry_press(self, event, command):
            self._fill_menu(command)
            self.callback(command, event.x_root, event.y_root)

        def on_resize(self):
            w, h = super(UiLayout.TopDevice, self).on_resize()

            h_butt = 28
            h_entry = 28
            h_list = h - (h_butt + 2 * h_entry + 40)
            w_entry = 0.75 * w

            for count, key in enumerate(self._labels.keys()):
                self._labels[key].place(
                    x=0,
                    y= 5 + count * h_entry,
                    width=w - w_entry,
                    heigh=h_entry,
                )
                self._entries[key].place(
                    x=w - w_entry,
                    y= 5 + count * h_entry,
                    width=w_entry - 10,
                    heigh=h_entry,
                )

            self._buttons['close'].place(
                x=5,
                y=h - (h_butt + 5),
                heigh=h_butt,
                width=0.5 * w - 8
            )
            self._buttons['apply'].place(
                x=0.5 * w + 3,
                y=h - (h_butt + 5),
                heigh=h_butt,
                width=0.5 * w - 8
            )

    class TopAlarm(TopDevice):
        def __init__(self, parent, board, device, **kwargs):
            UiLayout.TopDevice.__init__(self, parent, board, device, **kwargs)

        def build(self):
            super(UiLayout.TopAlarm, self).build()
            self.title('A L A R M     L A Y O U T')

            for key in ('group', 'alarm type'):
                self._values.update(
                    {
                        key: tk.StringVar(self)
                    }
                )
                self._entries.update(
                    {
                        key: EntryTemplate(
                            self,
                            font=Fonts.mono(10),
                            textvariable=self._values[key],
                            fg=Palette.generic.WHITE,
                            bg=Palette.generic.BLACK,
                            disabledbackground=Palette.generic.BLACK,
                            selectforeground='yellow',
                        )
                    }
                )
                self._entries[key].bind('<ButtonPress-1>', lambda event, c=key:self.on_entry_press(event,c) )
                self._labels.update(
                    {
                        key: LabelTemplate(
                            self,
                            font=Fonts.mono(8),
                            text=key.upper(),
                            anchor=tk.W,
                            padx=10,
                        )
                    }
                )

            if self._key in System.alarms.devices().keys():
                d = System.alarms.devices()[self._key]
                self._values['group'].set(d.group)
                self._values['alarm type'].set(d.alarmtype)
            else:
                self._values['group'].set('<group-name>')
                self._values['alarm type'].set('<alarm-type>')

        def _apply_clb(self):
            if not System.io.is_connected():
                tkinter.messagebox.showinfo('L A Y O U T', 'Please connect first!')
                self.destroy()
                return

            key = (self._values['board'].get(), self._values['device'].get())
            for k, item in self._values.items():

                if not item.get()[0].isalpha():
                    tk.messagebox.showwarning('L A Y O U T', 'Invalid {} name'.format(k))
                    return

            if not key in System.layout.devices.keys():
                if tk.messagebox.askyesno('L A Y O U T','Do you want to add new device {}:{} ?'.format(
                    self._values['board'].get(), self._values['device'].get())):
                    self._create_device()
                else:
                    return


            if key in System.alarms.devices().keys():
                lyt = System.alarms.devices()[key]

                l1 = (
                    self._values['board'].get(),
                    self._values['device'].get(),
                    self._values['group'].get(),
                    self._values['alarm type'].get(),
                )

                l2 = (
                    lyt.boardname,
                    lyt.devname,
                    lyt.group,
                    lyt.alarmtype,
                )
                if l1 == l2:
                    self.destroy()
                    return
            else:
                if not tk.messagebox.askyesno('L A Y O U T',
                                              'Do you want to add new alarm {}:{}'.format(key[0], key[1])):
                    self.destroy()
                    return

            d = {
                'devname': self._values['device'].get(),
                'board': self._values['board'].get(),
                'label1': self._values['group'].get(),
                'type': self._values['alarm type'].get(),
            }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_ALARMS, '', d)
            System.io.send2server(p)

        def _fill_menu(self, command):
            super(UiLayout.TopAlarm, self)._fill_menu(command)

            if command == 'group':
                blist=list()
                for key in System.alarms.groups.values():
                    if not key.name in blist:
                        blist.append(key.name)
                for key in blist:
                    self._menu.add_command(
                        label=key,
                        command=lambda c=key: self._values[command].set(c)
                    )
            elif command == 'alarm type':
                blist=list()
                for key in System.alarms.groups.values():
                    if not key.alarmtype in blist:
                        blist.append(key.alarmtype)
                for key in blist:
                    self._menu.add_command(
                        label=key,
                        command=lambda c=key: self._values[command].set(c)
                    )

    class TopLight(TopDevice):
        def __init__(self, parent, board, device, **kwargs):
            UiLayout.TopDevice.__init__(self, parent, board, device, **kwargs)

        def build(self):
            super(UiLayout.TopLight, self).build()
            self.title('L I G H T     L A Y O U T')

            for key in ('group', 'light type'):
                self._values.update(
                    {
                        key: tk.StringVar(self)
                    }
                )
                self._entries.update(
                    {
                        key: EntryTemplate(
                            self,
                            font=Fonts.mono(10),
                            textvariable=self._values[key],
                            fg=Palette.generic.WHITE,
                            bg=Palette.generic.BLACK,
                            disabledbackground=Palette.generic.BLACK,
                            selectforeground='yellow',
                        )
                    }
                )
                self._entries[key].bind('<ButtonPress-1>', lambda event, c=key:self.on_entry_press(event,c) )
                self._labels.update(
                    {
                        key: LabelTemplate(
                            self,
                            font=Fonts.mono(8),
                            text=key.upper(),
                            anchor=tk.W,
                            padx=10,
                        )
                    }
                )

            if self._key in System.layout.lights.keys():
                d = System.layout.lights[self._key]
                self._values['group'].set(d.group)
                self._values['light type'].set(d.lightype)
            else:
                self._values['group'].set('<group-name>')
                self._values['light type'].set('<light-type>')

        def _apply_clb(self):
            if not System.io.is_connected():
                tkinter.messagebox.showinfo('L A Y O U T', 'Please connect first!')
                self.destroy()
                return

            key = (self._values['board'].get(), self._values['device'].get())
            for k, item in self._values.items():

                if not item.get()[0].isalpha():
                    tk.messagebox.showwarning('L A Y O U T', 'Invalid {} name'.format(k))
                    return

            if not key in System.layout.devices.keys():
                if tk.messagebox.askyesno('L A Y O U T','Do you want to add new device {}:{} ?'.format(
                    self._values['board'].get(), self._values['device'].get())):
                    self._create_device()
                else:
                    return


            if key in System.layout.lights.keys():
                lyt = System.layout.lights[key]

                l1 = (
                    self._values['board'].get(),
                    self._values['device'].get(),
                    self._values['group'].get(),
                    self._values['light type'].get(),
                )

                l2 = (
                    lyt.boardname,
                    lyt.devname,
                    lyt.group,
                    lyt.lightype,
                )
                if l1 == l2:
                    self.destroy()
                    return
            else:
                if not tk.messagebox.askyesno('L A Y O U T',
                                              'Do you want to add new light {}:{}'.format(key[0], key[1])):
                    self.destroy()
                    return

            d = {
                'devname': self._values['device'].get(),
                'board': self._values['board'].get(),
                'label1': self._values['group'].get(),
                'type': self._values['light type'].get(),
            }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_LIGHTS, '', d)
            System.io.send2server(p)


        def _fill_menu(self, command):
            super(UiLayout.TopLight, self)._fill_menu(command)

            if command == 'group':
                for item in System.layout.lights.values():
                    if not item.group in self._menu.items:
                        self._menu.items.append(item.group)
                        self._menu.add_command(
                            label=item.group,
                            command=lambda c=item.group: self._values[command].set(c)
                        )
            elif command == 'light type':
                for item in System.layout.lights.values():
                    if not item.lightype in self._menu.items:
                        self._menu.items.append(item.lightype)
                        self._menu.add_command(
                            label=item.lightype,
                            command=lambda c=item.lightype: self._values[command].set(c)
                        )

    class TopWeather(TopDevice):

        def __init__(self, parent, board, device, **kwargs):
            UiLayout.TopDevice.__init__(self, parent, board, device, **kwargs)

        def build(self):
            super(UiLayout.TopWeather, self).build()
            self.title('W E A T H E R     L A Y O U T')

            for key in ('group',):
                self._values.update(
                    {
                        key: tk.StringVar(self)
                    }
                )
                self._entries.update(
                    {
                        key: EntryTemplate(
                            self,
                            font=Fonts.mono(10),
                            textvariable=self._values[key],
                            fg=Palette.generic.WHITE,
                            bg=Palette.generic.BLACK,
                            disabledbackground=Palette.generic.BLACK,
                            selectforeground='yellow',
                        )
                    }
                )
                self._entries[key].bind('<ButtonPress-1>', lambda event, c=key:self.on_entry_press(event,c) )
                self._labels.update(
                    {
                        key: LabelTemplate(
                            self,
                            font=Fonts.mono(8),
                            text=key.upper(),
                            anchor=tk.W,
                            padx=10,
                        )
                    }
                )

            if self._key in System.layout.weather.keys():
                d = System.layout.weather[self._key]
                self._values['group'].set(d.group)
            else:
                self._values['group'].set('<group-name>')

        def _apply_clb(self):
            if not System.io.is_connected():
                tkinter.messagebox.showinfo('L A Y O U T', 'Please connect first!')
                self.destroy()
                return

            key = (self._values['board'].get(), self._values['device'].get())
            for k, item in self._values.items():

                if not item.get()[0].isalpha():
                    tk.messagebox.showwarning('L A Y O U T', 'Invalid {} name'.format(k))
                    return

            if not key in System.layout.devices.keys():
                if tk.messagebox.askyesno('L A Y O U T','Do you want to add new device {}:{} ?'.format(
                    self._values['board'].get(), self._values['device'].get())):
                    self._create_device()
                else:
                    return


            if key in System.layout.weather.keys():
                lyt = System.layout.weather[key]

                l1 = (
                    self._values['board'].get(),
                    self._values['device'].get(),
                    self._values['group'].get(),
                )

                l2 = (
                    lyt.boardname,
                    lyt.devname,
                    lyt.group,
                )
                if l1 == l2:
                    self.destroy()
                    return
            else:
                if not tk.messagebox.askyesno('L A Y O U T',
                                              'Do you want to add new weather {}:{}'.format(key[0], key[1])):
                    self.destroy()
                    return

            d = {
                'devname': self._values['device'].get(),
                'board': self._values['board'].get(),
                'label1': self._values['group'].get(),
            }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_WEATHER, '', d)
            System.io.send2server(p)

        def _fill_menu(self, command):
            super(UiLayout.TopWeather, self)._fill_menu(command)

            if command == 'group':
                for item in System.layout.weather.values():
                    if not item.group in self._menu.items:
                        self._menu.items.append(item.group)
                        self._menu.add_command(
                            label=item.group,
                            command=lambda c=item.group: self._values[command].set(c)
                        )

    class Group(PanedTemplate):

        class Entry(PanedTemplate, DeviceLinkTemplate):

            def __init__(self, parent, board, device, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)
                DeviceLinkTemplate.__init__(self, board, device)
                self.buttons=dict()
                self.labels=dict()
                self._disabled=False

            def build(self, *args, **kwargs):
                super(UiLayout.Group.Entry, self).build()
                self.configure(relief=tk.RAISED)

                for key in ('device', 'location'):
                    self.labels.update({
                        key:LabelTemplate(
                            self,
                            font=Fonts.mono(9),
                            anchor=tk.W,
                            padx=10,
                            text=key.upper()
                        )
                    })
                for key in ('edit', 'delete'):
                    self.buttons.update({
                        key:ButtonTemplate(
                            self,
                            font=Fonts.mono(9),
                            text=key.capitalize(),
                            command=lambda c=key : self.callback(c)
                        )
                    })

            def set_disabled(self):
                self._disabled=True
                for i in self.buttons.values():
                    i.configure(state=tk.DISABLED)

                for i in self.labels.values():
                    i.configure(
                        state=tk.DISABLED,
                        text='DELETED'
                    )

            def callback(self, command):

                if command == 'edit':
                    pass

                elif command == 'delete':
                    pass
                    # self.destroy()
                    # self.master.master.master.master.master.master.callback('refresh')

            def on_update(self, *args, **kwargs):
                if self._disabled:
                    return False
                if self.exist():
                    fg=Palette.generic.FG_DEFAULT
                else:
                    fg=Palette.generic.WARNING

                self.labels['device'].configure(
                    text='device   ==> {}:{}'.format(self.board,self.device),
                    fg=fg
                )

                if (self.board, self.device) in System.layout.devices.keys():
                    d = System.layout.devices[ (self.board, self.device) ]
                    self.labels['location'].configure(
                        text='location ==> {}:{}'.format(d.area, d.location),
                        fg=fg
                    )
                else:
                    self.labels['location'].configure(
                        text='location UNKNOWN',
                        fg=Palette.generic.ERROR_GUI
                    )

                return True

            def on_resize(self, *args, **kwargs):
                w,h=super(UiLayout.Group.Entry, self).on_resize()

                off=5
                h_label=22
                w_butt=80
                h_butt=25

                for count, item in enumerate(self.labels.values()):
                    item.place(
                        x=off,
                        y=off + count * h_label,
                        width=w - 2*off,
                        heigh=h_label
                    )

                for count, item in enumerate(self.buttons.values()):
                    item.place(
                        x=count * w_butt + (count + 1) * ((w - 2 * w_butt) / 3),
                        y=h - (h_butt + off),
                        width=w_butt,
                        heigh=h_butt
                    )

        class DeviceEntry(Entry):

            def __init__(self, parent, board, device, **kwargs):
                UiLayout.Group.Entry.__init__(self, parent, board, device, **kwargs)

            def build(self, *args, **kwargs):
                super(UiLayout.Group.DeviceEntry, self).build()

                for key in ('type',):
                    self.labels.update({
                        key:LabelTemplate(
                            self,
                            font=Fonts.mono(9),
                            anchor=tk.W,
                            padx=10,
                            text=key.upper()
                        )
                    })

            def on_update(self):
                if not super(UiLayout.Group.DeviceEntry, self).on_update():
                    return

                if self.exist():
                    fg=Palette.generic.FG_DEFAULT
                else:
                    fg=Palette.generic.WARNING

                if (self.board, self.device) in System.layout.devices.keys():
                    d = System.layout.devices[ (self.board, self.device) ]
                    self.labels['type'].configure(
                        text='type     ==> {}'.format(d.devtype),
                        fg=fg
                    )
                else:
                    self.labels['type'].configure(
                        text='type UNKNOWN',
                        fg=Palette.generic.ERROR_GUI
                    )

            def callback(self, command):
                super(UiLayout.Group.DeviceEntry, self).callback(command)

                if command == 'edit':
                    t = UiLayout.TopDevice(self, self.board, self.device)
                    t.show()

                elif command == 'delete':
                    if not System.io.is_connected():
                        tkinter.messagebox.showinfo('', 'Please connect first!')
                        return

                    d = {
                        'command': 'REM:DEVICES:{}:{}'.format(self.board, self.device),
                    }
                    p = IndoorinoPacket()
                    p.build(IBACOM_LYT_CONF, '', d)
                    System.io.send2server(p)
                    System.layout.devices.pop( (self.board, self.device,) )
                    self.set_disabled()

        class AlarmEntry(Entry):

            def __init__(self, parent, board, device, **kwargs):
                UiLayout.Group.Entry.__init__(self, parent, board, device, **kwargs)

            def build(self, *args, **kwargs):
                super(UiLayout.Group.AlarmEntry, self).build()

                for key in ('type', 'group'):
                    self.labels.update({
                        key:LabelTemplate(
                            self,
                            font=Fonts.mono(9),
                            anchor=tk.W,
                            padx=10,
                            text=key.upper()
                        )
                    })

            def on_update(self):
                super(UiLayout.Group.AlarmEntry, self).on_update()
                if self.exist():
                    fg=Palette.generic.FG_DEFAULT
                else:
                    fg=Palette.generic.WARNING

                for group in System.alarms.groups.values():
                    if (self.board, self.device) in group.devices.keys():

                        self.labels['type'].configure(
                            text='type     ==> {}'.format(group.alarmtype),
                            fg=fg
                        )
                        self.labels['group'].configure(
                            text='group    ==> {}'.format(group.name),
                            fg=fg
                        )
                        return

                self.labels['type'].configure(
                    text='type UNKNOWN',
                    fg=Palette.generic.ERROR_GUI
                )
                self.labels['group'].configure(
                    text='group UNKNOWN',
                    fg=Palette.generic.ERROR_GUI
                )

            def callback(self, command):
                super(UiLayout.Group.AlarmEntry, self).callback(command)

                if command == 'edit':
                    t = UiLayout.TopAlarm(self, self.board, self.device)
                    t.show()

                elif command == 'delete':
                    if not System.io.is_connected():
                        tkinter.messagebox.showinfo('', 'Please connect first!')
                        return

                    d = {
                        'command': 'REM:ALARMS:{}:{}'.format(self.board, self.device),
                    }
                    p = IndoorinoPacket()
                    p.build(IBACOM_LYT_CONF, '', d)
                    System.io.send2server(p)
                    System.layout.devices.pop( (self.board, self.device,) )
                    self.set_disabled()

        class LightEntry(Entry):

            def __init__(self, parent, board, device, **kwargs):
                UiLayout.Group.Entry.__init__(self, parent, board, device, **kwargs)

            def build(self, *args, **kwargs):
                super(UiLayout.Group.LightEntry, self).build()

                for key in ('type', 'group'):
                    self.labels.update({
                        key:LabelTemplate(
                            self,
                            font=Fonts.mono(9),
                            anchor=tk.W,
                            padx=10,
                            text=key.upper()
                        )
                    })

            def on_update(self):
                super(UiLayout.Group.LightEntry, self).on_update()
                if self.exist():
                    fg=Palette.generic.FG_DEFAULT
                else:
                    fg=Palette.generic.WARNING

                if (self.board, self.device) in System.layout.lights.keys():
                    d = System.layout.lights[ (self.board, self.device) ]
                    self.labels['type'].configure(
                        text='type     ==> {}'.format(d.lightype),
                        fg=fg
                    )
                    self.labels['group'].configure(
                        text='group    ==> {}'.format(d.group),
                        fg=fg
                    )
                else:
                    self.labels['type'].configure(
                        text='type UNKNOWN',
                        fg=Palette.generic.ERROR_GUI
                    )
                    self.labels['group'].configure(
                        text='group UNKNOWN',
                        fg=Palette.generic.ERROR_GUI
                    )

            def callback(self, command):
                super(UiLayout.Group.LightEntry, self).callback(command)

                if command == 'edit':
                    t = UiLayout.TopLight(self, self.board, self.device)
                    t.show()

                elif command == 'delete':
                    if not System.io.is_connected():
                        tkinter.messagebox.showinfo('', 'Please connect first!')
                        return

                    d = {
                        'command': 'REM:LIGHTS:{}:{}'.format(self.board, self.device),
                    }
                    p = IndoorinoPacket()
                    p.build(IBACOM_LYT_CONF, '', d)
                    System.io.send2server(p)
                    System.layout.devices.pop( (self.board, self.device,) )
                    self.set_disabled()

        class WeatherEntry(Entry):

            def __init__(self, parent, board, device, **kwargs):
                UiLayout.Group.Entry.__init__(self, parent, board, device, **kwargs)

            def build(self, *args, **kwargs):
                super(UiLayout.Group.WeatherEntry, self).build()

                for key in ('type', 'group'):
                    self.labels.update({
                        key:LabelTemplate(
                            self,
                            font=Fonts.mono(9),
                            anchor=tk.W,
                            padx=10,
                            text=key.upper()
                        )
                    })

            def on_update(self):
                super(UiLayout.Group.WeatherEntry, self).on_update()
                if self.exist():
                    fg=Palette.generic.FG_DEFAULT
                else:
                    fg=Palette.generic.WARNING

                if (self.board, self.device) in System.layout.weather.keys():
                    d = System.layout.weather[ (self.board, self.device) ]
                    self.labels['type'].configure(
                        text='type     ==> {}'.format(d.weathertype),
                        fg=fg
                    )
                    self.labels['group'].configure(
                        text='group    ==> {}'.format(d.group),
                        fg=fg
                    )
                else:
                    self.labels['type'].configure(
                        text='type UNKNOWN',
                        fg=Palette.generic.ERROR_GUI
                    )
                    self.labels['group'].configure(
                        text='group UNKNOWN',
                        fg=Palette.generic.ERROR_GUI
                    )

            def callback(self, command):
                super(UiLayout.Group.WeatherEntry, self).callback(command)

                if command == 'edit':
                    t = UiLayout.TopWeather(self, self.board, self.device)
                    t.show()

                elif command == 'delete':
                    if not System.io.is_connected():
                        tkinter.messagebox.showinfo('', 'Please connect first!')
                        return

                    d = {
                        'command': 'REM:WEATHER:{}:{}'.format(self.board, self.device),
                    }
                    p = IndoorinoPacket()
                    p.build(IBACOM_LYT_CONF, '', d)
                    System.io.send2server(p)
                    System.layout.devices.pop( (self.board, self.device,) )
                    self.set_disabled()

        def __init__(self, parent, group, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            self.group=group
            self.label = LabelTemplate(self)
            self.button_add = ButtonTemplate(self)
            self.button_cla = ButtonTemplate(self)
            self.compact = PictureTemplate(self, Icons.system.COLLAPSE_SIDE(), bg=Palette.generic.DISABLED)
            self.entrylist = ScrollableFrameTemplate(self)
            self._visible = True

        @property
        def visible(self):
            return self._visible

        def build(self, *args, **kwargs):
            super(UiLayout.Group, self).build()
            self.configure(
                relief=tk.RAISED,
                bg=Palette.generic.DISABLED,
            )
            self.label.configure(
                font=Fonts.monobold(12),
                text=self.group.upper(),
                bg=Palette.generic.DISABLED,
                fg=Palette.generic.BLACK,
            )

            self.button_add.configure(
                text = 'Add (+)',
                font=Fonts.mono(10),
                command=lambda c='add' : self.callback(c)
            )
            self.button_cla.configure(
                text = 'Clear',
                font=Fonts.mono(10),
                command=lambda c='cla' : self.callback(c)
            )

            self.compact.build()
            self.compact.configure(
                relief=tk.RAISED,
            )
            self.compact.bind("<ButtonPress-1>", self.on_press)
            self.compact.bind("<ButtonRelease-1>", self.on_release)

            self.entrylist.build()
            self.on_update()
            self.on_release()

        def callback(self, command):
            if command == 'add':
                if self.group == 'devices':
                    t=UiLayout.TopDevice(self, '', '')
                elif self.group == 'alarms':
                    t=UiLayout.TopAlarm(self, '', '')
                elif self.group == 'lights':
                    t=UiLayout.TopLight(self, '', '')
                elif self.group == 'weather':
                    t=UiLayout.TopWeather(self, '', '')
                else:
                    return
                t.show()
            elif command == 'cla':
                self.entrylist.clear()
                self.on_resize()

        def on_press(self, *event):
            super(UiLayout.Group, self).on_press()
            self.compact.configure( relief=tk.SUNKEN)

        def on_release(self, *event):
            super(UiLayout.Group, self).on_release(*event)
            self.compact.configure( relief=tk.RAISED)
            if self._visible:
                self._visible=False

                if self.group == 'alarms':
                    self.compact.replace_image(Icons.layout.ALARM())
                elif self.group == 'lights':
                    self.compact.replace_image(Icons.layout.LIGHTS())
                elif self.group == 'weather':
                    self.compact.replace_image(Icons.layout.WEATHER())
                elif self.group == 'devices':
                    self.compact.replace_image(Icons.BOARD_GREEN())
                else:
                    self.compact.replace_image(Icons.system.NOT_FOUND())

            else:
                self._visible=True
                self.compact.replace_image(Icons.system.COLLAPSE_SIDE())
            self.master.on_resize()
            self.on_resize()

        def on_update(self, *args, **kwargs):
            flag_resize=False
            if self.group == 'devices':
                for index, entry in enumerate(self.entrylist.widgetlist):
                    if not (entry.board, entry.device) in System.layout.devices.keys():
                        flag_resize = True
                        self.entrylist.widgetlist.pop(index)

                for key, item in System.layout.devices.items():
                    if not key in [(i.board, i.device) for i in self.entrylist.widgetlist]:
                        warning_devices('Building {}'.format(key))
                        flag_resize=True
                        self.entrylist.add(self.DeviceEntry, key[0], key[1])

            if self.group == 'alarms':
                for dev in System.alarms.devices().values():
                    if not (dev.boardname, dev.devname) in [(i.board, i.device) for i in self.entrylist.widgetlist]:
                        warning_devices('Building {}:{}'.format(dev.boardname, dev.devname))
                        flag_resize=True
                        self.entrylist.add(self.AlarmEntry, dev.boardname, dev.devname)

            if self.group == 'lights':
                for key, item in System.layout.lights.items():
                    if not key in [(i.board, i.device) for i in self.entrylist.widgetlist]:
                        warning_devices('Building {}'.format(key))
                        flag_resize=True
                        self.entrylist.add(self.LightEntry, key[0], key[1])

            if self.group == 'weather':
                for key, item in System.layout.weather.items():
                    if not key in [(i.board, i.device) for i in self.entrylist.widgetlist]:
                        warning_devices('Building {}'.format(key))
                        flag_resize=True
                        self.entrylist.add(self.WeatherEntry, key[0], key[1])



            self.entrylist.on_update()
            if flag_resize:
                self.on_resize()

        def on_resize(self, *args, **kwargs):
            w,h=super(UiLayout.Group, self).on_resize()

            if not self._visible:
                w_icon = 60
                self.label.place_forget()
                self.button_add.place_forget()
                self.entrylist.place_forget()
                self.compact.place(
                    x=2,
                    y=2,
                    width=w_icon - 4,
                    heigh=w_icon - 4,
                )
                self.compact.on_resize()
                return

            off= 5
            h_label=30
            h_button=25
            h_sublabel=22
            self.label.place(
                x=h_label,
                y=0,
                width=w - 2 * h_label,
                heigh=h_label,
            )
            self.button_add.place(
                x=off,
                y=h - (h_button + 2),
                width=w - off,
                heigh=h_button,
            )
            self.compact.place(
                x=2,
                y=2,
                width=h_label - 4,
                heigh=h_label - 4,
            )
            self.compact.on_resize()
            self.entrylist.place(
                x=0,
                y=h_label,
                width=w,
                heigh=h - (h_label + h_button + off),
            )

            if len(self.entrylist.widgetlist) > 0:
                self.entrylist.on_resize(len(self.entrylist.widgetlist[0].labels) * h_sublabel + h_button + 3 * off)
            else:
                self.entrylist.on_resize(100)

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        self.label = LabelTemplate(self)
        self.button = dict()
        self.widgets = dict()

    def build(self, *args, **kwargs):

        self.label.configure(
            font=Fonts.monobold(16),
            text='LAYOUT',
            anchor=tk.W,
            padx=30,
            fg=Palette.frames.LAYOUT,
            bg=Palette.generic.BLACK
        )

        for key in ('refresh', 'reset'):
            self.button.update({
                key:ButtonTemplate(
                    self,
                    font=Fonts.mono(8),
                    text=key.upper(),
                    command=lambda c=key : self.callback(c)
                )
            })


        for key in ('devices',  'alarms',  'lights',  'weather',):
            self.widgets.update({
                key: self.Group(self, key)
            })
            self.widgets[key].build()

    def on_update(self, *args, **kwargs):

        for item in self.widgets.values():
            item.on_update()

    def loop(self, *args, **kwargs):

        if Config.flags.update.DEVICES or Config.flags.update.SYSTEM:
            self.on_update()

        for widget in self.widgets.values():
            widget.loop()

    def callback(self, command):
        if not System.io.is_connected():
            tkinter.messagebox.showinfo('', 'Please connect first!')
            return
        if command == 'refresh':
            System.layout.clear()
            System.alarms.clear()
            for entry in self.widgets.values():
                entry.entrylist.clear()
            d = {
                'command': 'GET',
            }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_CONF, '', d)
            System.io.send2server(p)

        if command == 'reset':
            d = {
                'command': 'RESET',
            }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_CONF, '', d)
            System.io.send2server(p)
            time.sleep(1)
            self.callback('refresh')

    def on_resize(self, *args, **kwargs):
        w,h=super(UiLayout, self).on_resize()

        h_label = 35
        w_group = 380
        w_butt = 100
        off_butt = 8
        self.label.place(
            x=0,
            y=0,
            width=w,
            heigh=h_label,
        )

        for count, entry in enumerate(self.button.values()):
            entry.place(
                x=w - (5 + w_butt) * (count + 1),
                y=off_butt,
                width=w_butt,
                heigh=h_label - 2 * off_butt,
            )

        w_curr = 0
        for count, entry in enumerate(self.widgets.values()):
            w_delta = w_group
            if not entry.visible:
                w_delta = 64
            entry.place(
                x=w_curr,
                y=h_label,
                width=w_delta,
                heigh=h - h_label,
            )
            w_curr += w_delta
            entry.on_resize()
