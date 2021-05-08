from common.templates import *
from widgets.tools import AlarmWidget

class UiAlarms(PanedTemplate):

    class AlarmEntry(AlarmWidget):

        def __init__(self, parent, board, device, opt, **kwargs):
            AlarmWidget.__init__(self, parent, board, device, **kwargs)

            # if alarmtype == 'HEAT':
            #     self.option = {
            #         'alarmstring'   :   'FIRE',
            #         'alarmcolor'    :   'red',
            #         'warningstring' :   'OVERHEAT',
            #         'warningcolor'  :   'firebrick'
            #     }


    class AlarmList(PanedTemplate):

        def __init__(self, parent, group_name, **kwargs):
            PanedTemplate.__init__(self, parent, **kwargs)
            self.alarmlist = ScrollableFrameTemplate(self)
            self.label=LabelTemplate(self)
            self.group=group_name

        def set_title(self, string):
            self.label.configure( text = string)

        def build(self, *args, **kwargs):
            super(UiAlarms.AlarmList, self).build()
            self.alarmlist.build()

            if self.group in Config.layout.sensors.keys():
                for board, item in Config.layout.sensors[self.group].items():
                    for device, opt in item.items():
                        self.alarmlist.add(UiAlarms.AlarmEntry, board, device, opt)
            else:
                warning_os('Alarms:List: Could not find group <{}>'.format(self.group))

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

        def on_update(self, *args, **kwargs):
           self.alarmlist.on_update()

        def on_resize(self, *args, **kwargs):
            w,h=super(UiAlarms.AlarmList, self).on_resize()

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
            self.alarmlist.on_resize(120)




    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)
        self.alarmgroups=list()


    def build(self, *args, **kwargs):
        super(UiAlarms, self).build()

        for key, value in Config.layout.alarmgroups.items():
            self.alarmgroups.append(
                self.AlarmList(self, value)
            )
            self.alarmgroups[-1].build()
            self.alarmgroups[-1].set_title('{} ALARMS'.format(key.upper()))

    def loop(self):
        if Config.flags.update.DEVICES or Config.flags.update.NETWORK:
            self.on_update()

    def on_update(self, *args, **kwargs):
        for entry in self.alarmgroups:
            entry.on_update()

    def on_resize(self, *args, **kwargs):
        w,h = super(UiAlarms, self).on_resize()

        w_alarms = 4 * w / 5

        for count, entry in enumerate(self.alarmgroups):
            entry.place(
                x=count * w_alarms / len(self.alarmgroups),
                y=0,
                width=w_alarms / len(self.alarmgroups),
                heigh=h

            )
            entry.on_resize()

