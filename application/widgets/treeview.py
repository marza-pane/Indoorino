from common.templates import *

class ResourceTreeView(TreeViewTemplate):

    def __init__(self, parent, **kwargs):
        TreeViewTemplate.__init__(self, parent, **kwargs)

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
        self["columns"] = ("prop",)

        self.column("#0", width=150, minwidth=150, stretch=tk.NO)
        self.column("prop", width=150, minwidth=150, stretch=tk.NO)

        self.heading("#0", text="Indoorino", anchor=tk.W)
        self.heading("prop", text="Status", anchor=tk.W)

        self._structure['home'] = self.insert("", 1, text="Home", values=('',))
        self._structure['boards'] = self.insert("", 2, text="Boards", values=('',))
        self._structure['alarms'] = self.insert("", 3, text="Alarms", values=('',))
        self._structure['reports'] = self.insert("", 4, text="Reports", values=('',))
        self._structure['resources'] = self.insert("", 5, text="Resources", values=('',))
        self._structure['map'] = self.insert("", 6, text="Map", values=('',))

        # self._structure['reports'] = self.insert(self._structure['signals'], 1, text="Reports", values=('',))
        # self._structure['warnings'] = self.insert(self._structure['signals'], 2, text="Warnings", values=('',))
        # self._structure['alarms'] = self.insert(self._structure['signals'], 3, text="Alarms", values=('',))

        self._structure['lights'] = self.insert(self._structure['resources'], 1, text="Lights", values=('',))
        self._structure['weather'] = self.insert(self._structure['resources'], 2, text="Weather", values=('',))

        self.on_update()
        self.selection_set(self._structure['boards'])

    def loop(self):
        if Config.flags.update.BOARD or Config.flags.update.DEVICES:
            self.on_update()

    def on_tv_select(self, *event):
        self.master.show_frame(super(ResourceTreeView, self).on_tv_select(*event).lower())

    def on_update(self, *args, **kwargs):

        for entry in self.get_children(self._structure['boards']):
            self.delete(entry)

        for i, board in enumerate(System.boards().values()):
            self.insert(self._structure['boards'], i, text=board.name, values=('ONLINE',))

