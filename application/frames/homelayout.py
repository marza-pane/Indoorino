from common.templates import *

layout_link = {
    'devices'   : System.layout.devices,
    'alarms'    : System.alarms.groups,
    'lights'    : System.layout.lights,
    'weather'   : System.layout.weather,
}



class UiLayout (PanedTemplate):

    class Group(PanedTemplate):

        class Entry(PanedTemplate, DeviceLinkTemplate):

            def __init__(self, parent, board, device, **kwargs):
                PanedTemplate.__init__(self, parent, **kwargs)
                DeviceLinkTemplate.__init__(self, board, device)

                self.labels=dict()

            def build(self, *args, **kwargs):
                super(UiLayout.Group.Entry, self).build()

                for key in ('device', 'location', 'type'):
                    self.labels.update({
                        key:LabelTemplate(
                            self,
                            font=Fonts.mono(11),
                            text=key.upper
                        )
                    })

            def on_resize(self, *args, **kwargs):
                w,h=super(UiLayout.Group.Entry, self).on_resize()


        def __init__(self, parent, group, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            self.group=group
            self.label = LabelTemplate(self)
            self.entrylist = ScrollableFrameTemplate(self)

        def build(self, *args, **kwargs):
            self.label.configure(
                font=Fonts.monobold(12),
                text=self.group.upper(),
            )

            self.entrylist.build()

        def on_update(self, *args, **kwargs):
            error_os('ON UPDATE FOR LAYOUT')

            data = layout_link[self.group]

            for key, item in data:
                if self.group == 'devices':
                    if not key in [ (i.board, i.device) for i in self.entrylist.widgetlist ]:
                        warning_devices('Building {}'.format(key))
                        self.entrylist.add(self.Entry, key[0], key[1])

        def on_resize(self, *args, **kwargs):
            w,h=super(UiLayout.Group, self).on_resize()
            h_label=30
            self.label.place(
                x=0,
                y=0,
                width=w,
                heigh=h_label,
            )
            self.entrylist.place(
                x=0,
                y=h_label,
                width=w,
                heigh=h - h_label,
            )
            self.entrylist.on_resize()

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        self.label = LabelTemplate(self)
        self.button = ButtonTemplate(self)
        self.widgets = dict()

    def build(self, *args, **kwargs):

        self.label.configure(
            font=Fonts.monobold(16),
            text='LAYOUT',
            anchor=tk.W,
            padx=30,
            fg=Palette.generic.R_TITLE,
            bg=Palette.generic.BLACK
        )

        self.button.configure(
            font=Fonts.monobold(13),
            text='Refresh',
            command=self.callback_refresh
        )

        for key in layout_link.keys():
            self.widgets.update({
                key: self.Group(self, key)
            })
            self.widgets[key].build()

    def on_update(self, *args, **kwargs):

        for item in self.widgets.values():
            item.on_update()

    def loop(self, *args, **kwargs):

        if Config.flags.update.DEVICES or Config.flags.update.NETWORK:
            self.on_update()

        for widget in self.widgets.values():
            widget.loop()

    def callback_refresh(self, *event):
        pass

    def on_resize(self, *args, **kwargs):
        w,h=super(UiLayout, self).on_resize()

        h_label = 35
        w_group = 300
        self.label.place(
            x=0,
            y=0,
            width=w,
            heigh=h_label,
        )

        self.button.place(
            x=2 * w / 3,
            y=3,
            width=150,
            heigh=h_label - 6,
        )

        for count, entry in enumerate(self.widgets.values()):
            entry.place(
                x=count * w_group,
                y=h_label,
                width=w_group,
                heigh=h - h_label,
            )
            entry.on_resize()