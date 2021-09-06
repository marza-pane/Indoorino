import tkinter.messagebox

from common.templates import *
from indoorino.packet import IndoorinoPacket

class UiLayout (PanedTemplate):

    class ServiceView(PanedTemplate):

        def __init__(self, parent, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            self.labels = dict()
            self.strings = dict()
            self.buttons=dict()
            self.devlist = ListBoxTemplate(self)
            self._current=''

        @property
        def current(self):
            return self._current

        @current.setter
        def current(self, value):
            print('Current set to {}'.format(value))
            self._current = str(value)
            self.on_update()

        def build(self, *args, **kwargs):
            super(UiLayout.ServiceView, self).build()
            self.configure(
                relief=tk.SUNKEN,
                bg=Palette.generic.DISABLED
            )
            self.devlist.build()
            for key in ('name', 'type', 'location', 'description', 'state'):
                self.strings.update({
                    key:tk.StringVar()
                })

                self.labels.update({
                    key:LabelTemplate(
                        self,
                        anchor=tk.W,
                        padx=10,
                        font=Fonts.monobold(10),
                        fg=Palette.generic.BLACK,
                        bg=Palette.generic.DISABLED,
                        textvariable=self.strings[key]
                    )
                })
                # self.strings[key].set('{}:{}'.format(key, key.upper()))
            self.on_update()

        def on_update(self, *args, **kwargs):

            if self._current in System.layout.services.keys():

                data = {
                    'name' : System.layout.services[self._current].name,
                    'type' : System.layout.services[self._current].service_type,
                    'location' : '{}:{}'.format(
                    System.layout.services[self._current].area, System.layout.services[self._current].location),
                    'description' : System.layout.services[self._current].description,
                    'state' : 'Running' # TODO here
                }

                for key, d in data.items():
                    self.strings[key].set('{0:<12}'.format('name') + ': {}'.format(d))
                # update devlist
            else:
                for key, item in self.strings.items():
                    item.set(
                        '{0:<12}'.format(key.capitalize()) + ':{}'.format(key.upper())
                    )


        def on_resize(self, *args, **kwargs):
            w,h = super(UiLayout.ServiceView, self).on_resize()

            off = 5
            h_label = 30
            for count, item in enumerate(self.labels.values()):
                item.place(
                    x=off,
                    y=off + count * h_label,
                    width=w - 2 * off,
                    heigh=h_label,
                )
            self.devlist.place(
                    x=off,
                    y=2 * off + len(self.labels) * h_label,
                    width=w - 2 * off,
                    heigh=h - (2 * off + (2 + len(self.labels)) * h_label),
                )
            self.devlist.on_resize()

    def __init__(self, parent, **kwargs):

        PanedTemplate.__init__(self, parent, **kwargs)
        self.title = LabelTemplate(self)
        self.button = dict()
        self.labels = dict()

        self.servicelist = ListBoxTemplate(self)
        self.serviceview = self.ServiceView(self)

        # for i in range(100):
        #     listbox.insert(END, i)

        # attach listbox to scrollbar

        self.widgets = dict()

    def build(self, *args, **kwargs):
        super(UiLayout, self).build()

        self.serviceview.build()
        self.title.configure(
            font=Fonts.monobold(16),
            text='LAYOUT',
            anchor=tk.W,
            padx=30,
            fg=Palette.frames.LAYOUT,
            bg=Palette.generic.BLACK
        )

        for key in ('refresh', 'reset', 'new service (+)'):
            self.button.update({
                key:ButtonTemplate(
                    self,
                    font=Fonts.mono(8),
                    text=key.upper(),
                    command=lambda c=key : self.callback(c)
                )
            })

        self.button['new service (+)'].configure(font=Fonts.mono(10))

        self.labels.update({
            'service' : LabelTemplate(
                self,
                font=Fonts.monobold(11),
                fg=Palette.generic.R_TITLE,
                text='AVAILABLE SERVICES'
            )
        })
        self.servicelist.configure(
            bd=0,
            bg=Palette.generic.WHITE,
            fg=Palette.generic.BLACK,
            selectbackground=Palette.generic.R_TITLE,
            selectforeground=Palette.generic.BLACK,
            selectmode=tk.SINGLE,
            font=Fonts.monobold(9)
        )
        self.servicelist.build()
        self.servicelist.bind("<<ListboxSelect>>", self.callback_servicelist)

        self.on_update()

    def loop(self):

        if Config.flags.update.SYSTEM or Config.flags.update.DEVICES:
            self.on_update()

    def on_update(self, *args, **kwargs):

        self.serviceview.on_update()

        self.servicelist.select_clear(0)
        self.servicelist.delete(0, tk.END)

        for entry in System.layout.services.keys():
            self.servicelist.insert(tk.END, '==> {}'.format(entry))

    def callback(self, command):

        if not System.io.is_connected():
            tkinter.messagebox.showinfo('', 'Please connect first!')
            return
        if command == 'refresh':
            System.layout.clear()
            System.alarms.clear()
            for entry in self.widgets.values():
                entry.entrylist.clear()
            d = { 'command': 'GET', }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_CONF, '', d)
            System.io.send2server(p)

        if command == 'reset':
            d = { 'command': 'RESET', }
            p = IndoorinoPacket()
            p.build(IBACOM_LYT_CONF, '', d)
            System.io.send2server(p)
            time.sleep(1)
            self.callback('refresh')

        if command == 'new service (+)':
            pass

    def callback_servicelist(self, *event):

        service = str(self.servicelist.selection_get()).replace('==> ','')
        self.serviceview.current = service

    def on_resize(self, *args, **kwargs):
        w,h=super(UiLayout, self).on_resize()

        h_label = 35
        off = 10
        off_butt = 8
        w_listbox = 0.5 * max(300, w / 2)
        w_butt = 100

        w_group = 380

        self.title.place(
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

        self.labels['service'].place(
            x=off,
            y=h_label + off,
            width=w_listbox,
            heigh=h_label,
        )

        self.servicelist.place(
            x=off,
            y=2 * (h_label + off),
            width=w_listbox,
            heigh=h - 4 * h_label,
        )
        self.servicelist.on_resize()

        self.button['new service (+)'].place(
            x=off + 0.5 * (w_listbox - 2 * w_butt),
            y=h - h_label - off,
            width=2 * w_butt,
            heigh=h_label,
        )

        self.serviceview.place(
            x=2 * off + w_listbox,
            y=2 * (h_label + off),
            width=1.2 * w_listbox,
            heigh=h - 4 * h_label,
        )
        self.serviceview.on_resize()

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

print('Loaded gui.frame.homelayout')
