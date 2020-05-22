import tkinter as tk
from gui.templates import GenericGuiTemplate

class MateCheckButton(GenericGuiTemplate, tk.PanedWindow):
    """ There's a glicth with Radiobutton and Checkbutton
        when running on Linux Mate"""
    def __init__(self, parent, **kwargs):

        GenericGuiTemplate.__init__(self, parent)
        tk.PanedWindow.__init__(self, parent, **kwargs)

        self._state = tk.BooleanVar()
        self._state.set(True)
        self.checkbutton = tk.Checkbutton(self,
            variable = self._state,
            command = self.callback)
        self.callback()

    @property
    def status(self):
        return self._state.get()

    def set_state(self,state):
        if state:
            if not self._state.get():
                self._state.set(True)
                self.callback()
        else:
            if self._state.get():
                self._state.set(False)
                self.callback()

    def callback(self):
        if self._state.get():
            self.configure(bg='SpringGreen4')
        else:
            self.configure(bg='firebrick4')

    def place(self, *args, **kwargs):
        super(MateCheckButton, self).place(*args, **kwargs)
        self.update()
        self.checkbutton.place(
            x=2,
            y=2,
            width=self.winfo_width() - 6,
            heigh=self.winfo_height() - 6
        )

