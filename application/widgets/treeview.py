from common.templates import *

class ResourceTreeView(TreeViewTemplate):

    def __init__(self, parent, **kwargs):
        TreeViewTemplate.__init__(self, parent, **kwargs)

        self.alarmgroups=dict()

    def build(self, *args, **kwargs):

        # style=ttk.Style(self.master)
        # style.configure('new.TFrame', settings={
        #             'background': [("selected", "blue"), ("!selected", "white")],
        #             'foreground': [("selected", "black"), ("!selected", "red")]
        #         }
        # )

        style = ttk.Style()
        style.configure("new.Treeview", highlightthickness=0, bd=0,
                        font=('Calibri', 11))  # Modify the font of the body
        style.configure("new.Treeview.Heading", font=('Calibri', 13, 'bold'))  # Modify the font of the headings
        style.layout("new.Treeview", [('new.Treeview.treearea', {'sticky': 'nswe'})])  # Remove the borders


        self.configure(style='new.Treeview')
        self["columns"] = ()

        self.column("#0", width=300, minwidth=300, stretch=tk.NO)
        # self.column("prop", width=150, minwidth=150, stretch=tk.NO)

        self.heading("#0", text="Indoorino", anchor=tk.W)
        # self.heading("prop", text="Status", anchor=tk.W)

        self._structure['home'] = self.insert("", 1, text="Home", values=('',))
        self._structure['boards'] = self.insert("", 2, text="Boards", values=('',))
        self._structure['server'] = self.insert("", 3, text="Server", values=('',))
        self._structure['layout'] = self.insert("", 4, text="Layout", values=('',))
        self._structure['reports'] = self.insert("", 5, text="Reports", values=('',))
        self._structure['resources'] = self.insert("", 7, text="Resources", values=('',))

        self._structure['map'] = self.insert("", 8, text="Map", values=('',))

        # self._structure['reports'] = self.insert(self._structure['signals'], 1, text="Reports", values=('',))
        # self._structure['warnings'] = self.insert(self._structure['signals'], 2, text="Warnings", values=('',))
        # self._structure['alarms'] = self.insert(self._structure['signals'], 3, text="Alarms", values=('',))

        self._structure['alarms'] = self.insert(self._structure['resources'], 1, text="Alarms", values=('',))
        self._structure['lights'] = self.insert(self._structure['resources'], 2, text="Lights", values=('',))
        self._structure['weather'] = self.insert(self._structure['resources'], 3, text="Weather", values=('',))

        # for key, value in self._structure.items():
            # print('treeview:struct:{}:{}'.format(key,value))

        self.on_update()
        self.selection_set(self._structure['boards'])

    def loop(self):
        if Config.flags.update.SYSTEM or Config.flags.update.PACKET:
            self.on_update()

    def on_tv_select(self, *event):
        self.master.show_frame(super(ResourceTreeView, self).on_tv_select(*event).lower())

    def on_update(self, *args, **kwargs):

        for entry in self.get_children(self._structure['boards']):
            self.delete(entry)

        for i, board in enumerate(System.boards().values()):
            self.insert(self._structure['boards'], i, text=board.name, values=('ONLINE',))


        for group in self.alarmgroups.values():
            for entry in self.get_children(group):
                self.delete(entry)
        # for entry in self.get_children(self._structure['alarms']):
        #     self.delete(entry)

        for alarm in System.alarms.groups.values():
            if not alarm.alarmtype in self.alarmgroups.keys():
                self.alarmgroups.update(
                    {
                        alarm.alarmtype : self.insert(
                            self._structure['alarms'],
                            len(self.alarmgroups) + 1,
                            text=alarm.alarmtype, values=('',))
                    }
                )

                print('treeview:alarm:add:{}:{}'.format(alarm.alarmtype,self.alarmgroups[alarm.alarmtype]))

            self.insert(
                self.alarmgroups[alarm.alarmtype],
                len(self.alarmgroups[alarm.alarmtype]) + 1,
                text=alarm.name.split(' ')[0].capitalize(), values=('',))

        # for alarm in System.alarms.groups.values():
        #     self.insert(
        #         self._structure['alarms'],
        #         len(self.get_children(self._structure['alarms'])) + 1,
        #         text=alarm.name.capitalize(), values=('',))


