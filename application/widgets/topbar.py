from common.templates import *

class TopDashboardBar(PanedTemplate):

    class Button(ButtonTemplate):
        def __init__(self, parent, path, **kwargs):
            ButtonTemplate.__init__(self, parent, **kwargs)
            self.path=path
            self.source = ImagePil.open(self.path)
            self.buffer = None
            self.image = None
            self.current_size = 1

        def replace_image(self, path):

            if self.path != path:
                self.path = path
                self.source = ImagePil.open(self.path)
                self.current_size = [1, 1]
                self.on_resize()

        def on_resize(self, *args, **kwargs):
            w, h = super(TopDashboardBar.Button, self).on_resize()
            size = max(1, min(w, h) - 2)
            if not size == self.current_size:
                self.current_size = size
                self.buffer = self.source.resize((self.current_size, self.current_size), ImagePil.ANTIALIAS)
                self.image = ImageTk.PhotoImage(self.buffer)
                self.configure(image=self.image)

    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)
        self.buttons=dict()
        self.scheme=(
            ('treeview', Icons.system.TREEVIEW_ON()),
            ('overview', Icons.system.IOVERVIEW())
        )

    def build(self, *args, **kwargs):
        for item in self.scheme:
            self.buttons[item[0]] = self.Button(
                self,
                item[1],
                command=lambda c=item[0]: self.master.show_frame(c),
                tooltip=item[0].capitalize()
            )

            self.buttons[item[0]].bind_default('motion')

    def on_resize(self, *args, **kwargs):
        w,h=super(TopDashboardBar, self).on_resize(*args, **kwargs)

        if len(self.buttons.keys()) > 0:
            w_button = min(220, w / len(self.buttons.keys()))
        else:
            w_button=220

        for count, widget in enumerate(self.buttons.values()):
            widget.place(
                x=count * w_button,
                y=0,
                width=w_button,
                heigh=h
            )
            widget.on_resize()