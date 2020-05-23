import tkinter as tk
from utils.utils import debug_gui

class GenericGuiTemplate:
    """Generic template class for tkinter widgets. Declarations go in __init__
        * 'parent' attribute is the master widget hosting the class
        * 'build()' method configure all the widgets
        * 'on_update()' sets all gui visual variables
        * 'on_resize()' is invoked on.. resize
        """

    def __init__(self, parent):
        self.parent = parent

    def build(self):
        debug_gui('building from template')

    def on_update(self, query=False):
        debug_gui('value update')

    def loop(self):
        pass

    def on_resize(self):
        debug_gui('graphic update')


class FrameTemplate(GenericGuiTemplate, tk.Frame):

    def __init__(self, parent, *args, **kwargs):
        GenericGuiTemplate.__init__(self, parent)
        tk.Frame.__init__(self, parent, *args, **kwargs)

    def on_resize(self):
        super(FrameTemplate, self).on_resize()
        self.update()

class PanedTemplate(GenericGuiTemplate, tk.PanedWindow):

    def __init__(self, parent, *args, **kwargs):

        GenericGuiTemplate.__init__(self, parent)
        tk.PanedWindow.__init__(self, parent, *args, **kwargs)

    def on_resize(self):
        super(PanedTemplate, self).on_resize()
        self.update()


if __name__ == '__main__':
    # it's not working as we try to import utils.utils
    app = tk.Tk()
    app.geometry('300x300')
    panel = PanedTemplate(app, bg='blue')
    panel.configure(bg='blue')
    panel.place(x=10,y=10,width=100, heigh=100)
    app.mainloop()