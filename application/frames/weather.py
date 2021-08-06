from common.templates import *
from widgets.homemap import HomeMap

# class WeatherWidgetTemplate(PanedTemplate, BoardLinkTemplate):
#
#     def __init__(self, parent, boardname, **kwargs):
#         PanedTemplate.__init__(self, parent, **kwargs)
#         BoardLinkTemplate.__init__(self, boardname)
#         self.labels = dict()
#
#     def on_update(self, *args, **kwargs):
#
#         if not self.exist():
#             return
#         #
#         #
#         #
#         # for dev in self.get_board().device.values():
#         #
#         #     for key, value in System.layout.weather.items():
#         #         if self.board == key[0] and dev.name == key[1]:
#         #             if not key in self.labels.keys():
#         #                 s = '{0:<16}'.format(dev.status.)
#
# class DataViewer(PanedTemplate):
#     def __init__(self, parent, **kwargs):
#         PanedTemplate.__init__(self, parent, **kwargs)


class UiWeather(PanedTemplate):

    class StationListEntry(PanedTemplate, BoardLinkTemplate):

        def __init__(self, parent, boardname, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            BoardLinkTemplate.__init__(self, boardname)

            self.title  = LabelTemplate(self)
            self.icon   = PictureTemplate(self, Icons.BOARD_RED())
            self.status_widget=None

        def build(self, *args, **kwargs):
            super(UiWeather.StationListEntry, self).build()
            self.configure(relief=tk.RAISED)
            self.title.configure(
                font=Fonts.monobold(10),
                # anchor=tk.W,
                # padx=20,
                relief=tk.FLAT,
                text=self.board,
            )

            self.bind_mouse_buttons()

            self.title.bind("<ButtonPress-1>", self.on_press)
            self.title.bind("<ButtonRelease-1>", self.on_release)

            self.icon.bind("<ButtonPress-1>", self.on_press)
            self.icon.bind("<ButtonRelease-1>", self.on_release)

        def on_press(self, *event):
            super(UiWeather.StationListEntry, self).on_press(event)
            self.configure( bg=Palette.generic.WHITE )
            self.title.configure( bg=Palette.generic.WHITE, fg=Palette.generic.BLACK)
            self.icon.configure( bg=Palette.generic.WHITE )

        def on_release(self, *event):
            super(UiWeather.StationListEntry, self).on_release(event)
            self.configure( bg=Palette.generic.BG_DEFAULT )
            self.title.configure( bg=Palette.generic.BG_DEFAULT, fg=Palette.generic.WHITE)
            self.icon.configure( bg=Palette.generic.BG_DEFAULT )
            self.master.show(self.board)

        def on_update(self, *args, **kwargs):

            if self.exist():

                for dev in self.get_board().device.values():
                    for key, lyt in System.layout.weather.items():
                        if self.board == key[0] and dev.name == key[1]:
                            self.title.configure(
                                text='{} '.format(lyt.group.split('climate')[0]).rstrip(' ').upper(),
                                anchor=tk.CENTER
                            )


                            for entry in dev.status.dev.values():
                                # <entry> is output value of dev
                                pass

        def on_resize(self, *args, **kwargs):
            w,h = super(UiWeather.StationListEntry, self).on_resize()

            h_title = 25
            size=min(w, h - h_title)
            self.title.place(
                x=2,
                y=h - (h_title + 2),
                width=w - 4,
                heigh=h_title
            )
            self.icon.place(
                x=0.5 *  (w - size),
                y=2,
                width=size,
                heigh=size
            )
            self.icon.on_resize()

    class StationWidget(PanedTemplate, BoardLinkTemplate):

        class StationDataLine(PanedTemplate, BoardLinkTemplate):
            def __init__(self, parent, boardname, valuetype, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)
                BoardLinkTemplate.__init__(self, boardname)
                self._type=valuetype
                self.labels=dict()
                self.buttons=dict()
                self.color=Palette.generic.BG_DEFAULT
                self.show=False

            def build(self, *args, **kwargs):
                super(UiWeather.StationWidget.StationDataLine, self).build()

                for key in ('label', 'value', 'unit'):
                    self.labels.update({
                        key : LabelTemplate(
                            self,
                            font=Fonts.monobold(12),
                            anchor=tk.W,
                            padx=10,
                            # relief=tk.RAISED,
                            # bg='snow',
                            # fg='black'
                        )
                    })


                for key in ('update', 'show', 'color'):
                    self.buttons.update({
                        key : ButtonTemplate(
                            self,
                            text=key.capitalize(),
                            font=Fonts.monobold(10),
                            command=lambda c=key : self.callback(c)
                        )
                    })

                self.labels['label'].configure(text=self._type)
                self.labels['value'].configure(font=Fonts.monobold(20), anchor=tk.SW)

                self.buttons['color'].configure(
                    text='',
                )

            def callback(self, command):

                if command == 'update':
                    print('REQUESTING UPDATE TO IMPLEMENT :(')
                elif command == 'show':
                    self.buttons['color'].configure(bg=self.color)
                    self.master.on_plot(self._type.split(' ')[1]) #### QUI!!! QUI deve combaciare la desc che board mette in probe con la description di Parameters
                    self.show = True
                    self.callback('color')
                elif command == 'color':
                    self.color = Palette.plot.new()
                    self.buttons['color'].configure(bg=self.color)
                    # self.master.plot.changecolrotoline()

            def on_update(self, *args, **kwargs):
                self.buttons['color'].configure(bg=self.color)

                if self.exist():

                    value = 0
                    count = 0
                    limits = list()
                    unit = str()

                    for dev in self.get_board().device.values():

                        if not (self.board, dev.name,) in System.layout.weather.keys():
                            # error_ui('{}:{} not in Layout!'.format(self.board, dev.name))
                            continue

                        for entry in [ item for item in dev.status.dev.values() if item.desc == self._type ]:

                            unit = entry.unit
                            limits = entry.limits
                            if dev.is_connected():
                                value += entry.data / Config.macros.FLOAT2INT
                                count += 1


                    self.labels['unit'].configure(text=unit)
                    if count > 0:
                        self.labels['value'].configure(text='{}'.format('%2.2f' % (value/count) ))
                    else:
                        self.labels['value'].configure(text='{}'.format('%2.2f' % value ))


                    if self.get_board().is_connected() and \
                            any([dev.is_connected() for dev in self.get_board().device.values()]):
                        if len(limits) > 1:
                            if limits[0] > value > limits[1]:
                                self.labels['value'].configure(bg=Palette.generic.WARNING)

                    else:
                        self.labels['label'].configure(fg=Palette.generic.DISABLED)
                        self.labels['value'].configure(fg=Palette.generic.DISABLED)
                        self.labels['unit'].configure(fg=Palette.generic.DISABLED)

            def on_resize(self, *args, **kwargs):
                w,h=super(UiWeather.StationWidget.StationDataLine, self).on_resize()

                split=0.5 * w
                h_label = min(30, 0.5 * h)
                off = 5
                w_butt = min(100, split/3 + off)

                self.labels['label'].place(
                    x=0,
                    y=h - h_label,
                    width=0.5 * split,
                    heigh=h_label
                )
                self.labels['value'].place(
                    x=0.5 * split,
                    y=0,
                    width=0.3 * split,
                    heigh=h
                )
                self.labels['unit'].place(
                    x=0.8 * split,
                    y=h - h_label,
                    width=0.2 * split,
                    heigh=h_label
                )

                for count, item in enumerate(self.buttons.values()):
                    item.place(
                    x=split + (off + w_butt) * count,
                    y=h - h_label,
                    width=w_butt,
                    heigh=h_label
                )
                self.buttons['color'].place(width=h_label)



        def __init__(self, parent, boardname, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            BoardLinkTemplate.__init__(self, boardname)
            self.title  = LabelTemplate(self)
            self.plot   = PlotTemplate(self)
            self.labels = dict()

        def build(self, *args, **kwargs):
            super(UiWeather.StationWidget, self).build()
            self.plot.build()
            self.title.configure(
                font=Fonts.monobold(13),
                anchor=tk.W,
                # relief=tk.RAISED,
                # bg='blue',
                fg='snow'
            )

            # self.plot.plot([1,2,3,4,5],[5,2,3,8,4])

        def on_plot(self, probetype):
            if self.exist():
                for dev in self.get_board().device.values():
                    if not (self.board, dev.name,) in System.layout.weather.keys():
                        continue
                    if probetype in dev.probes().keys():
                        x, y = dev.probes(probetype)
                        self.plot.plot(x, y, label='{}:{}'.format(dev.name, probetype))
                self.plot.draw()
        def on_update(self, *args, **kwargs):

            if self.exist():

                for dev in self.get_board().device.values():
                    for key, lyt in System.layout.weather.items():
                        if self.board == key[0] and dev.name == key[1]:
                            # self.title.configure(text='{} {}:{}'.format(
                            #     lyt.area, lyt.location, lyt.group
                            # ).capitalize())
                            self.title.configure(text='{}'.format(lyt.group).capitalize())

                            # for pkey in dev.probes().keys():
                            #     x,y = dev.probes(pkey)
                            #     self.plot.plot(x, y, label='{}:{}'.format(dev.name, pkey))

                            for entry in dev.status.dev.values():

                                if 'system' in entry.desc:
                                    continue
                                if not entry.desc in self.labels.keys():
                                    self.labels.update({
                                        entry.desc:self.StationDataLine(
                                            self,
                                            self.board,
                                            entry.desc
                                        )
                                    })
                                    self.labels[entry.desc].build()

            for entry in self.labels.values():
                entry.on_update()

                                # s = '{0:<16}'.format(entry.desc)
                                # s += ': {} '.format(entry.value)
                                # s += entry.unit
                                #
                                # self.labels[entry.desc].configure(text=s)

        def on_resize(self, *args, **kwargs):
            w, h=super(UiWeather.StationWidget, self).on_resize()
            h_title = 30
            h_label = 50
            h_plot  = 400
            off=10
            self.title.place(
                x=off,
                y=off,
                width=w - 2 * off,
                heigh=h_title
            )
            for count, entry in enumerate(self.labels.values()):
                entry.place(
                    x=off,
                    y=2 * off + h_title + h_label * count,
                    width=w - 2 * off,
                    heigh=h_label
                )
                entry.on_resize()

            self.plot.place(
                x=off,
                y=h - h_plot,
                width=w - 2 * off,
                heigh=h_plot
            )
            self.plot.on_resize()

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        self.title=LabelTemplate(self)
        self.background=HomeMap(self)

        self.buttons        = dict()

        self.frames         = dict()
        self.list_indoor    = dict()
        self.list_outdoor   = dict()

        self._current   = ''
        # self._current   = 'KITCHEN.CLIMATE'

    def build(self, *args, **kwargs):
        self.background.build()

        self.title.configure(
            font=Fonts.monobold(16),
            text='WEATHER',
            anchor=tk.W,
            padx=30,
            fg=Palette.frames.WEATHER,
            bg=Palette.generic.BLACK
        )
        self.on_update()

    def loop(self):
        if Config.flags.update.DEVICES or Config.flags.update.NETWORK or Config.flags.update.SYSTEM:
            self.on_update()

    def show(self, name):
        if name in self.frames.keys():

            self.frames[name].on_update()
            if self._current == name:
                return

            if self._current in self.frames.keys():
                self.frames[self._current].place_forget()
            self._current=name
            self.on_resize()

    def on_update(self, *args, **kwargs):

        _flag=False
        for key, lyt in System.layout.weather.items():
            if not key[0] in self.list_indoor.keys():

                if not lyt.weathertype:
                    self.list_indoor.update({
                        key[0]: UiWeather.StationListEntry(self, key[0])
                    })
                    self.list_indoor[key[0]].build()
                else:
                    self.list_outdoor.update({
                        key[0]: UiWeather.StationListEntry(self, key[0])
                    })
                    self.list_outdoor[key[0]].build()

                self.frames.update({
                    key[0]:UiWeather.StationWidget(self, key[0])
                })

                self.frames[key[0]].build()

                _flag=True

        for item in self.list_indoor.values():
            item.on_update()
        for item in self.list_outdoor.values():
            item.on_update()

        if self._current in self.frames.keys():
            self.frames[self._current].on_update()

        if _flag:
            self.on_resize()

    def on_resize(self, *args, **kwargs):
        w,h = super(UiWeather, self).on_resize()

        w_list = max(0.2 * w, 450)
        w_entry = 150
        h_entry = 80
        h_title = 35
        off=10
        self.title.place(
            x=0,
            y=0,
            width=w,
            heigh=h_title,
        )
        self.background.place(
            x=0,
            y=h_title,
            width=w,
            heigh=(h - h_title)
        )


        row = 0
        ndx = 0
        for count, item in enumerate(self.list_indoor.values()):

            # if count > 3 and  count % 4 == 0:
            if off + (ndx + 1) * (w_entry + off) > w_list:
                row +=1
                ndx = 0

            item.place(
                x=off + ndx * (w_entry + off),
                y=h_title + off + row * (h_entry + off),
                width=w_entry,
                heigh=h_entry
            )
            item.on_resize()
            ndx += 1

        if self._current in self.frames.keys():
            self.frames[self._current].place(
                x=2 * off + w_list,
                y=off + h_title,
                width=w - (3 * off + w_list),
                heigh=h - (h_title + 2 * off),
            )
            self.frames[self._current].on_resize()

        return w,h