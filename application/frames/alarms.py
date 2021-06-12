from common.templates import *
# from widgets.tools import AlarmGroupWidget

class UiAlarms(PanedTemplate):

    class AlarmGroupList(PanedTemplate):

        class AlarmWidget(PanedTemplate):

            def __init__(self, parent, group_name, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)
                self.group = group_name
                self.frame = PanedTemplate(self)
                self.button = LabelTemplate(self.frame)
                self.status = LabelTemplate(self)
                self.label_dev = LabelTemplate(self)
                self.label_count = LabelTemplate(self)

                self.devlist = list()

                self._enabled = False
                self._alarm = False
                self._acknowledge = False

                self._taskstate = True  # alternating alarm color in loop
                self._total_events = 0

            def build(self, *args, **kwargs):
                super(UiAlarms.AlarmGroupList.AlarmWidget, self).build()

                self.configure(
                    bg=Palette.generic.BLACK,
                    relief=tk.RAISED,
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
                    relief=tk.FLAT,
                    text='OFFLINE',
                    bg=Palette.generic.BLACK,
                )

                # self.status.bind("<ButtonPress-1>", self.on_press_status)
                # self.status.bind("<ButtonRelease-1>", self.on_release_status)

                self.label_dev.configure(
                    font=Fonts.monobold(9),
                    anchor=tk.W,
                    text=self.group.capitalize(),
                    bg=Palette.generic.BLACK,
                    fg=Palette.generic.WARNING,
                )

                self.label_count.configure(
                    font=Fonts.mono(8),
                    anchor=tk.E,
                    bg=Palette.generic.BLACK,
                    fg=Palette.generic.DISABLED,
                )

                self.loop()
                self.on_update()

                # if alarmtype == 'HEAT':
                #     self.option = {
                #         'alarmstring'   :   'FIRE',
                #         'alarmcolor'    :   'red',
                #         'warningstring' :   'OVERHEAT',
                #         'warningcolor'  :   'firebrick'
                #     }

            def loop(self):

                self.after(750, self.loop)

                if not self._alarm:
                    return

                if self._taskstate:
                    self.status.configure(
                        fg=Palette.generic.ERROR_GUI
                    )
                else:
                    self.status.configure(
                        fg=Palette.generic.WHITE
                    )
                self._taskstate = not self._taskstate

            def on_press_device(self, *event):
                self.button.configure(
                    relief=tk.SUNKEN
                )

            def on_release_device(self, *event):
                self.button.configure(
                    relief=tk.RAISED
                )
                if self._alarm:
                    ## send acknoledge to server
                    pass
                mainframe = self.master.master.master.master.master.master

                if mainframe.current in System.alarms.groups.keys():
                    mainframe.alarmframe.show(self.group)
                else:
                    mainframe.show(self.group)
                # if self.exist() and self.get_device().is_connected():
                #     if self._enabled:
                #         self._enabled = False
                #     else:
                #         self._enabled = True
                # self.on_update()

            def set_alarm(self):
                self._alarm = True
                self._total_events += 1
                self.status.configure(
                    text='ALARM'
                )
                self.loop()

            def parse(self, packet):

                if packet.command() == 10:
                    self._total_events += 1

            def on_update(self, *args, **kwargs):

                if System.alarms.groups[self.group].is_onalarm:
                    self.set_alarm()

                count_ex = 0
                count_cn = 0
                count_en = 0

                self.label_count.configure( text = 'Events: {}'.format(self._total_events))

                for entry in [item for item in System.alarms.groups.values() if item.name == self.group]:
                    for device in entry.devices.values():
                        if not any([c.board == device.boardname and c.device == device.devname for c in self.devlist]):
                            print('APPENDING {}:{} to ALARMS'.format(device.boardname, device))
                            self.devlist.append(DeviceLinkTemplate(device.boardname, device.devname))
                        if device.is_enabled:
                            count_en += 1

                # for key, entry in Config.layout.alarms.items():
                #     if self.group in entry.keys():
                #         for board, item in Config.layout.alarms[key][self.group].items():
                #             for device in item.keys():
                #                 if not any([ c.board == board and c.device == device for c in self.devlist ]):
                #                     print('APPENDING {}:{} to ALARMS'.format(board, device))
                #                     self.devlist.append(DeviceLinkTemplate(board, device))

                for entry in self.devlist:
                    if entry.exist():
                        count_ex += 1
                        if entry.get_device().is_connected():
                            count_cn += 1

                if count_ex == 0:
                    self.button.configure(
                        bg=Palette.generic.BG_DEFAULT,
                        fg=Palette.generic.ERROR_GUI,
                        text='ERROR'
                    )
                    self.status.configure(
                        fg=Palette.generic.ERROR_GUI,
                        text='NOT-FOUND',
                    )
                    self.frame.configure(
                        bg=Palette.generic.ERROR_GUI
                    )
                    return

                if not System.io.is_connected():
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
                    return

                if count_cn == 0:
                    self.button.configure(
                        bg=Palette.generic.BG_DEFAULT,
                        fg=Palette.generic.OFFLINE,
                        text='OFFLINE'
                    )
                    self.status.configure(
                        fg=Palette.generic.DISABLED,
                        text='DISCONNECTED',
                    )
                    self.frame.configure(
                        bg=Palette.generic.OFFLINE
                    )
                    return

                if count_en > 0:
                    self.status.configure(
                        fg=Palette.generic.WHITE,
                        text='ARMED',
                    )
                    self.frame.configure(
                        bg=Palette.generic.ONLINE
                    )

                else:
                    self.status.configure(
                        fg=Palette.generic.DISABLED,
                        text='DISABLED',
                    )
                    self.frame.configure(
                        bg=Palette.generic.DISABLED
                    )

                if self._alarm:
                    self.button.configure(
                        bg=Palette.generic.BLACK,
                        fg=Palette.generic.ERROR_GUI,
                        text='ALARM'
                    )
                else:
                    self.button.configure(
                        text='IDLE'
                    )
                    if count_ex < self.devlist.__len__():
                        self.button.configure(
                            bg=Palette.generic.BLACK,
                            fg=Palette.generic.WARNING,
                        )
                    elif count_cn < self.devlist.__len__():
                        self.button.configure(
                            bg=Palette.generic.BLACK,
                            fg=Palette.generic.R_TITLE,
                        )
                    else:
                        self.button.configure(
                            bg=Palette.generic.BLACK,
                            fg=Palette.generic.ONLINE,
                        )

            def on_resize(self, *args, **kwargs):
                w, h = super(UiAlarms.AlarmGroupList.AlarmWidget, self).on_resize()

                off = max(4, 0.045 * h)
                b_off = max(4, 0.065 * h)
                h_labels = 25

                self.frame.place(
                    x=off,
                    y=off + h_labels,
                    width=w - 2 * off,
                    heigh=0.5 * h - off
                )
                self.button.place(
                    x=b_off,
                    y=b_off,
                    width=(w - 2 * off) - 2 * b_off,
                    heigh=(0.5 * h - 2 * off) - 2 * b_off
                )

                self.status.place(
                    x=b_off,
                    y=0.5 * h + off + h_labels,
                    width=w - 2 * b_off,
                    heigh=0.5 * h - (off + b_off + h_labels)
                )

                self.label_dev.place(
                    x=5,
                    y=0,
                    width=0.65 * w - 7.5,
                    heigh=h_labels
                )
                self.label_count.place(
                    x=0.65 * w + 2.5,
                    y=0,
                    width=0.35 * w - 7.5,
                    heigh=h_labels
                )

                self.button.configure(
                    font=Fonts.monobold(int(0.1 * h))
                )
                self.status.configure(
                    font=Fonts.monobold(int(0.1 * h))
                )

        def __init__(self, parent, group_name, **kwargs): # group name is "FIRE" or "FLOOD"
            PanedTemplate.__init__(self, parent, **kwargs)
            self.alarmlist = ScrollableFrameTemplate(self)
            self.label=LabelTemplate(self)
            self.group=group_name

        def set_title(self, string):
            self.label.configure(text = string)

        def build(self, *args, **kwargs):
            super(UiAlarms.AlarmGroupList, self).build()
            self.alarmlist.build()
            self.alarmlist.configure(
                relief=tk.RAISED,
                bg='navy'
            )

            self.label.configure(
                anchor=tk.W,
                font=Fonts.monobold(12),
                bg=Palette.generic.BLACK,
                fg=Palette.generic.WHITE
            )

            self.on_update()

        def on_update(self, *args, **kwargs):
            flag_resize=False
            for group in System.alarms.groups.values():
                if group.alarmtype == self.group:
                    if not group.name in [ w.group for w in self.alarmlist.widgetlist ]:
                        self.alarmlist.add(self.AlarmWidget, group.name, bg='blue')
                        flag_resize=True

            self.alarmlist.on_update()
            if flag_resize:
                self.on_resize()

        def on_resize(self, *args, **kwargs):
            w,h=super(UiAlarms.AlarmGroupList, self).on_resize()

            h_title= max(30, 0.05 * h)

            self.label.place(
                x=3,
                y=3,
                width=w - 6,
                heigh=h_title,
            )

            self.alarmlist.place(
                x=3,
                y=6 + h_title,
                width=w - 6,
                heigh=h - (h_title + 9),
            )
            self.alarmlist.on_resize(150)

    class AlarmDisplay(CanvasTemplate):

        class AlarmDeviceList(PanedTemplate):

            class AlarmDeviceWidget(PanedTemplate, DeviceLinkTemplate):

                def __init__(self, parent, boardname, devname, group, **kwargs):
                    PanedTemplate.__init__(self, parent, **kwargs)
                    DeviceLinkTemplate.__init__(self, boardname, devname)
                    self._group=group
                    try:
                        self.icon =PictureTemplate(self, Icons.devices[self.get_device().devtype].OFFLINE())
                    except AttributeError:
                        self.icon = PictureTemplate(self, Icons.system.NOT_FOUND())
                    self.labels = dict()
                    self.buttons = dict()

                def build(self):
                    super(UiAlarms.AlarmDisplay.AlarmDeviceList.AlarmDeviceWidget, self).build()
                    self.icon.build()

                    self.configure(bg=Palette.generic.BLACK)
                    self.icon.configure(bg=Palette.generic.BLACK)

                    try:
                        dev = System.layout.devices[(self.board, self.device)]
                    except KeyError:
                        error_ui('alarms:display:list:widget: Invalid key {}:{}'.format(self.board, self.device))
                        dev = {
                            'area' : '',
                            'location' : '',
                        }

                    data = (
                        ( 'name',  '{}:{}'.format(self.board, self.device), ),
                        ( 'area',  '{}:{}'.format(dev.area, dev.location),),
                        ( 'state', 'DISABLED',),
                        ( 'conn',  'CONNECTED',),
                    )

                    for key in data:
                        self.labels.update(
                            {
                                key[0]:LabelTemplate(
                                    self,
                                    font=Fonts.monobold(10),
                                    anchor=tk.W,
                                    text=key[1],
                                    bg=Palette.generic.BLACK,
                                    fg=Palette.generic.FG_DEFAULT
                                )
                            }
                        )
                    for key in ('disable', 'update', 'acknowledge', 'configure'):
                        self.buttons.update(
                            {
                                key:ButtonTemplate(
                                    self,
                                    font=Fonts.monobold(10),
                                    text=key.capitalize(),
                                )
                            }
                        )

                def on_update(self, *args, **kwargs):

                    if System.alarms.groups[self._group].devices[(self.board, self.device)].is_onalarm:
                        self.labels['state'].configure(
                            text='ON ALARM',
                            fg=Palette.generic.WHITE
                        )
                    else:
                        if System.alarms.groups[self._group].devices[(self.board, self.device)].is_enabled:
                            self.labels['state'].configure(
                                text='ENABLED',
                                fg=Palette.generic.WHITE
                            )
                            self.buttons['disable'].configure(
                                text='Disable',
                            )
                        else:
                            self.labels['state'].configure(
                                text='DISABLED',
                                fg=Palette.generic.WARNING
                            )
                            self.buttons['disable'].configure(
                                text='Enable',
                            )

                    if self.exist() and 'status' in self.get_device().status.dev.packet.payload.keys():
                        status = self.get_device().status.dev.packet.payload['status']
                    else:
                        status = 5 ## generic undefined error

                    if not self.exist() or \
                            not System.io.is_connected() or \
                            not self.get_device().is_connected() \
                            or status > 0:
                        for entry in self.labels.values():
                            entry.configure( fg=Palette.generic.DISABLED )

                        self.disable_buttons()

                        if not self.exist():
                            self.icon.replace_image(Icons.system.NOT_FOUND())
                            self.labels['conn'].configure(
                                text='NOT FOUND',
                                fg=Palette.generic.ERROR_GUI
                            )
                        elif not System.io.is_connected():
                            try:
                                self.icon.replace_image(Icons.devices[self.get_device().devtype].DISCONNECTED())
                            except AttributeError:
                                self.icon.replace_image(Icons.system.NOT_FOUND())

                            self.labels['conn'].configure(
                                text='OFFLINE',
                                fg=Palette.generic.DISABLED
                            )
                        elif status > 0:
                            try:
                                self.icon.replace_image(Icons.devices[self.get_device().devtype].ERROR())
                            except AttributeError:
                                self.icon.replace_image(Icons.system.NOT_FOUND())

                            self.labels['conn'].configure(
                                text='ERROR',
                                fg=Palette.generic.WARNING
                            )
                        else:
                            try:
                                self.icon.replace_image(Icons.devices[self.get_device().devtype].OFFLINE())
                            except AttributeError:
                                self.icon.replace_image(Icons.system.NOT_FOUND())

                            self.labels['conn'].configure(
                                text='OFFLINE',
                                fg=Palette.generic.OFFLINE
                            )

                    else:
                        self.enable_buttons()
                        for entry in self.labels.values():
                            entry.configure( fg=Palette.generic.FG_DEFAULT )

                        try:
                            self.icon.replace_image(Icons.devices[self.get_device().devtype].ONLINE())
                        except AttributeError:
                            self.icon.replace_image(Icons.system.NOT_FOUND())

                        self.labels['conn'].configure(
                            text='CONNECTED',
                            fg=Palette.generic.ONLINE
                        )

                def disable_buttons(self):
                    for i in self.buttons.values():
                        i.configure(
                            state=tk.DISABLED
                        )

                def enable_buttons(self):
                    for i in self.buttons.values():
                        i.configure(
                            state=tk.NORMAL
                        )

                def on_resize(self, *args, **kwargs):
                    w,h = super(UiAlarms.AlarmDisplay.AlarmDeviceList.AlarmDeviceWidget, self).on_resize()

                    off=5
                    size = min(w,h) - 2 * off

                    try:
                        dev = System.layout.devices[(self.board, self.device)]
                        location = '{}:{}'.format(dev.area, dev.location)
                        w_label = 2 + 10 * max(len(location), len('{}:{}'.format(self.board, self.device)))
                    except KeyError:
                        w_label = 2 + 10 * len('{}:{}'.format(self.board, self.device))

                    w_status=100
                    h_button=30

                    self.icon.place(
                        x=off, y=off,
                        width=size,
                        heigh=size
                    )
                    self.icon.on_resize()

                    self.labels['name'].place(
                        x=size + 2 * off,
                        y=off,
                        width=w_label,
                        heigh=0.5 * h - 2 * off
                    )
                    self.labels['area'].place(
                        x=size + 2 * off,
                        y=0.5 * h + off,
                        width=w_label,
                        heigh=0.5 * h - 2 * off
                    )
                    self.labels['conn'].place(
                        x=size + w_label + 3 * off,
                        y=off,
                        width=w_status,
                        heigh=0.5 * h - 2 * off
                    )
                    self.labels['state'].place(
                        x=size + w_label + 3 * off,
                        y=0.5 * h + off,
                        width=w_status,
                        heigh=0.5 * h - 2 * off
                    )

                    data = list()
                    for key in self.buttons.keys():
                        data.append( 10 * len(key) + 10)

                    for count, entry in enumerate(self.buttons.values()):
                        entry.place(
                            x=size + 3 * off + w_label + w_status + sum(data[0:count]) + count * 15,
                            y=0.5 * (h - h_button),
                            width=data[count],
                            heigh=h_button
                        )

            def __init__(self, parent, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)
                self.devlist = ScrollableFrameTemplate(self)
                self.title = LabelTemplate(self)
                self._current='---'

            def build(self, *args, **kwargs):
                self.devlist.build()
                self.title.configure(
                    font=Fonts.monobold(13),
                    anchor=tk.W,
                    padx=20,
                    relief=tk.RAISED,
                    text='{} devices:'.format(self._current.capitalize())
                )

            def show(self, name):
                if name == self._current:
                    self.devlist.on_update()
                    return

                self.devlist.clear()
                for entry in System.alarms.groups[name].devices.values():
                    self.devlist.add(self.AlarmDeviceWidget, entry.boardname, entry.devname, name)
                self.title.configure( text='{} devices:'.format(name.capitalize()) )
                self._current = name

            def on_update(self, *args, **kwargs):
                self.devlist.on_update()

            def on_resize(self, *args, **kwargs):
                w,h=super(UiAlarms.AlarmDisplay.AlarmDeviceList, self).on_resize()
                off=3
                h_title=30
                self.title.place(
                    x=0,
                    y=0,
                    width=w - 2,
                    heigh=h_title
                )
                self.devlist.place(
                    x=0,
                    y=h_title + off,
                    width=w,
                    heigh=h - (h_title + 2 * off)
                )
                self.devlist.on_resize(65)

        class AlarmInfo(PanedTemplate):

            def __init__(self, parent, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)

                self._current=''
                self.labels = dict()
                self.values = dict()
                self.buttons= dict()

            def build(self, *args, **kwargs):
                super(UiAlarms.AlarmDisplay.AlarmInfo, self).build()

                for key in ('name','type','location', 'devices', 'enabled', 'state'):
                    self.labels.update(
                        {
                            key: LabelTemplate(
                                self,
                                font=Fonts.monobold(9),
                                anchor=tk.W,
                                padx=10,
                                text=key.capitalize(),
                            )
                        }
                    )
                    self.values.update(
                        {
                            key: LabelTemplate(
                                self,
                                font=Fonts.monobold(9),
                                anchor=tk.W,
                                padx=10,
                                text='###:###',
                            )
                        }
                    )

                for key in ('acknowledge', 'confgiure', 'disable'):
                    self.buttons.update(
                        {
                            key:ButtonTemplate(
                                self,
                                font=Fonts.monobold(10),
                                text=key.capitalize()
                            )
                        }
                    )

                self.labels['name'].configure(
                    font=Fonts.monobold(13),
                    bg=Palette.generic.BLACK
                )

            def show(self, name):

                if name == self._current:
                    return
                if name in System.alarms.groups.keys():
                    self._current=name
                else:
                    error_ui('alarminfo:show: invalid name {}'.format(name))

            def on_update(self, *args, **kwargs):

                if not self._current in System.alarms.groups.keys():
                    return

                self.labels['name'].configure(text=self._current.capitalize())
                self.values['type'].configure(text=System.alarms.groups[self._current].alarmtype.upper())

                for entry in System.alarms.groups[self._current].devices.values():
                    if (entry.boardname, entry.devname) in System.layout.devices.keys():
                        key = (entry.boardname, entry.devname)

                        self.values['location'].configure(text='{}:{}'.format(
                            System.layout.devices[key].area, System.layout.devices[key].location).upper())
                        break

                devnum = len(System.alarms.groups[self._current].devices)
                if devnum == 0:
                    self.values['devices'].configure(fg=Palette.generic.ERROR_GUI)
                else:
                    self.values['devices'].configure(fg=Palette.generic.FG_DEFAULT)
                self.values['devices'].configure(text='{}'.format(devnum))

                if any([i.is_enabled for i in System.alarms.groups[self._current].devices.values()]):
                    self.values['enabled'].configure(text='YES')
                else:
                    self.values['enabled'].configure(text='NO')

                if not System.io.is_connected():
                    self.values['state'].configure(text='OFFLINE')
                    self.values['state'].configure(fg=Palette.generic.DISABLED)
                elif System.alarms.groups[self._current].is_onalarm:
                    self.values['state'].configure(text='ON ALARM')
                    self.values['state'].configure(fg=Palette.generic.ERROR_GUI)
                else:
                    self.values['state'].configure(text='IDLE')
                    self.values['state'].configure(fg=Palette.generic.FG_DEFAULT)

            def on_resize(self, *args, **kwargs):
                w,h = super(UiAlarms.AlarmDisplay.AlarmInfo, self).on_resize()
                xoff=10
                yoff=20
                h_label = 30
                w_button= min(200, max(120, 0.3 * w))
                for count, entry in enumerate(self.labels.values()):
                    entry.place(
                        x=xoff,
                        y=yoff + count * h_label,
                        width=0.5 * w - 1.5 * xoff,
                        heigh=h_label - 5
                    )

                for count, entry in enumerate(self.values.values()):
                    if count == 0: continue
                    entry.place(
                        x=0.5 * (w + xoff),
                        y=yoff + count * h_label,
                        width=0.5 * w - 1.5 * xoff,
                        heigh=h_label - 5
                    )

                self.labels['name'].place(
                    x=xoff,
                    y=xoff,
                    width=w - 2 * xoff,
                    heigh=h_label
                )

                for count, entry in enumerate(self.buttons.values()):
                    entry.place(
                        x=0.5 * w - (xoff + 3 * w_button / 2) + count * (w_button + xoff),
                        y=2 * yoff + len(self.labels) * h_label,
                        width=w_button,
                        heigh=h_label + 5
                    )

        class AlarmEvents(PanedTemplate):

            def __init__(self, parent, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)

                self._current = ''
                self.label = LabelTemplate(self)
                self.button = ButtonTemplate(self)
                self.listbox = ListBoxTemplate(self)
                self._scroll = tk.Scrollbar(self.listbox)

            def build(self, *args, **kwargs):
                super(UiAlarms.AlarmDisplay.AlarmEvents, self).build()

                self.label.configure(
                    font=Fonts.monobold(13),
                    anchor=tk.W,
                    padx=20,
                    text='Events: ###'
                )

                self.listbox.configure(
                    font=Fonts.mono(9),
                    bg=Palette.generic.BG_DEFAULT,
                    fg=Palette.generic.WHITE,
                    selectbackground=Palette.generic.R_TITLE,
                    selectforeground=Palette.generic.BLACK,
                    selectmode=tk.SINGLE,
                )
                self.listbox.bind("<<ListboxSelect>>", self.callback_select)
                self.listbox.config(yscrollcommand=self._scroll.set)
                self._scroll.config(command=self.listbox.yview)
                self._scroll.configure(width=10)

                self.button.configure(
                    font=Fonts.monobold(10),
                    text='Clear',
                    command=self.callback_clear,
                )

            def callback_select(self, *event):
                index = self.listbox.curselection()
                if index:
                    print('Selected : {}'.format(index))

            def callback_clear(self, *event):
                for dev in System.alarms.groups[self._current].devices.values():
                    dev.clear_events()
                self.on_update()

            def show(self, name):

                if name == self._current:
                    return
                if name in System.alarms.groups.keys():
                    self._current = name
                else:
                    error_ui('alarminfo:show: invalid name {}'.format(name))

            def on_update(self, *args, **kwargs):
                if not self._current in System.alarms.groups.keys():
                    return

                self.listbox.select_clear(0)
                self.listbox.delete(0, tk.END)

                n_events = len(System.alarms.groups[self._current].events)

                for event in System.alarms.groups[self._current].events:
                    if event.command == IBACOM_ENV_ALARM:
                        entry = '{} @ {}:{}'.format(
                            event.payload['desc1'], event.payload['label1'], event.payload['label2'])
                        self.listbox.insert(tk.END, entry)
                        self.listbox.itemconfig(tk.END, fg=Palette.generic.OFFLINE)
                    else:
                        entry = '{}'.format(event.description())
                        self.listbox.insert(tk.END, entry)

                for dev in System.alarms.groups[self._current].devices.values():
                    n_events += len(dev.events)

                    for event in dev.events:
                        if event.command == IBACOM_ALARM_DEVSTAT:
                            entry = '{}:{} [{}:{}] '.format(
                                event.payload['board'], event.payload['devname'],
                                'enabled' if event.payload['status'] else 'disabled',
                                'on alarm' if event.payload['status'] else 'idle',
                            )
                            self.listbox.insert(tk.END, entry)
                        else:
                            entry = '{}'.format(event.description())
                            self.listbox.insert(tk.END, entry)

                self.label.configure(
                    text='Events: {}'.format(n_events)
                )

            def on_resize(self, *args, **kwargs):
                w, h = super(UiAlarms.AlarmDisplay.AlarmEvents, self).on_resize()
                off = 5
                h_label = 30
                w_button = min(120, max(70, 0.3 * w))
                self.label.place(
                    x=off,
                    y=off,
                    width=w - (w_button + 3 * off),
                    heigh=h_label
                )
                self.button.place(
                    x=w - (w_button + off),
                    y=off,
                    width=w_button,
                    heigh=h_label - 5
                )
                self.listbox.place(
                    x=off,
                    y=off + h_label,
                    width=w - (2 * off),
                    heigh=h - (2 * off + h_label)
                )
                self._scroll.pack(side=tk.RIGHT, fill=tk.Y)

        def __init__(self, parent, **kwargs):
            CanvasTemplate.__init__(self, parent, **kwargs)
            self.alarminfo = self.AlarmInfo(self)
            self.devices= self.AlarmDeviceList(self)
            self.events= self.AlarmEvents(self)

        def build(self, *args, **kwargs):
            super(UiAlarms.AlarmDisplay, self).build()
            self.alarminfo.build()
            self.devices.build()
            self.events.build()

        def on_update(self, *args, **kwargs):
            self.alarminfo.on_update()
            self.devices.on_update()
            self.events.on_update()

        def show(self, name):
            self.alarminfo.show(name)
            self.devices.show(name)
            self.events.show(name)

            self.on_update()
            self.on_resize()

        def on_resize(self, *args, **kwargs):
            w,h = super(UiAlarms.AlarmDisplay, self).on_resize()
            off = 30
            w_info = 400
            h_info = 270
            self.alarminfo.place(
                x=off,
                y=off,
                width= w_info,
                heigh= h_info,
            )
            self.alarminfo.on_resize()

            self.events.place(
                x=2 * off + w_info,
                y=off,
                width= w - (3 * off + w_info),
                heigh= h_info,
            )
            self.events.on_resize()

            self.devices.place(
                x=off,
                y=2 * off + h_info,
                width= w - 2 * off,
                heigh= h - (3 * off + h_info),
            )
            self.devices.on_resize()

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        self.alarmgroups=dict()
        self.alarmframe=self.AlarmDisplay(self, bg=Palette.generic.BLACK)
        self._current='all'

    @property
    def current(self):
        return self._current

    def build(self, *args, **kwargs):
        super(UiAlarms, self).build()
        self.alarmframe.build()
        self.on_update()

    def loop(self):
        if Config.flags.update.DEVICES or Config.flags.update.NETWORK:
            self.on_update()

    def show(self, arg):

        if arg in [group.alarmtype.lower() for group in System.alarms.groups.values()] or \
                arg in System.alarms.groups.keys() or \
                arg == 'all':
            print('Showing {} ALARMS'.format(arg))
            self.alarmframe.place_forget()
            for entry in self.alarmgroups.values():
                entry.place_forget()

            self._current = arg
            if arg in System.alarms.groups.keys():
                self.alarmframe.show(arg)
            self.on_resize()
        else:
            error_ui('alarms:show: invalid argument {}'.format(arg))

    def on_update(self, *args, **kwargs):

        resize_flag = False

        # for group, data in Config.layout.alarms.items():
        for group in System.alarms.groups.values():

            if not group.alarmtype in self.alarmgroups.keys():
                self.alarmgroups.update(
                    {
                        group.alarmtype: self.AlarmGroupList(self, group.alarmtype),
                    }
                )
                self.alarmgroups[group.alarmtype].build()
                self.alarmgroups[group.alarmtype].set_title('{} ALARMS'.format(group.alarmtype.capitalize()))
                resize_flag = True

        self.alarmframe.on_update()
        for entry in self.alarmgroups.values():
            entry.on_update()

        if resize_flag:
            self.on_resize()

    def on_resize(self, *args, **kwargs):
        w,h = super(UiAlarms, self).on_resize()

        w_alarms = 4 * w / 5

        try:
            ww = min(380, max(250, w_alarms / len(self.alarmgroups)))
        except ZeroDivisionError:
            ww=0

        if self._current == 'all':
            for count, entry in enumerate(self.alarmgroups.values()):
                entry.place(
                    x=count * ww,
                    y=0,
                    width=ww,
                    heigh=h
                )
                entry.on_resize()

        elif self._current in [group.alarmtype.lower() for group in System.alarms.groups.values()]:
            self.alarmgroups[self._current.upper()].place(
                    x=0,
                    y=0,
                    width=ww,
                    heigh=h
                )
            self.alarmgroups[self._current.upper()].on_resize()

        elif self._current in System.alarms.groups.keys():
            group = [group.alarmtype for group in System.alarms.groups.values() if group.name == self._current]
            try:
                self.alarmgroups[group[0]].place(
                        x=0,
                        y=0,
                        width=ww,
                        heigh=h
                    )
                self.alarmgroups[group[0]].on_resize()

                self.alarmframe.place(
                    x=ww + 5,
                    y=0,
                    width=w - (ww + 10),
                    heigh=h
                )
                self.alarmframe.show(self._current)
            except KeyError:
                pass

