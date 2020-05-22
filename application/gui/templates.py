import tkinter as tk

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
        pass

    def on_update(self, query=False):
        pass

    def loop(self):
        pass

    def on_resize(self):
        pass


class FrameTemplate(GenericGuiTemplate, tk.Frame):

    def __init__(self, parent, *args):
        GenericGuiTemplate.__init__(self, parent)
        tk.Frame.__init__(self, parent, *args)

    def on_resize(self):
        super(FrameTemplate, self).on_resize()
        self.update()

class PanedTemplate(GenericGuiTemplate, tk.PanedWindow):

    def __init__(self, parent, *args):

        GenericGuiTemplate.__init__(self, parent)
        tk.PanedWindow.__init__(self, parent, *args)

    def on_resize(self):
        super(PanedTemplate, self).on_resize()
        self.update()


if __name__ == '__main__':

    app = tk.Tk()
    app.mainloop()