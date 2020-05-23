import sys
import tkinter as tk

from gui.utils import *
from utils.utils import debug_gui
from gui.templates import GenericGuiTemplate

class ApplicationWindowTemplate(GenericGuiTemplate):

    def __init__(self):

        self.win = tk.Tk()
        GenericGuiTemplate.__init__(self, self.win)

        self._win_height = 0
        self._win_width = 0

        self.build()

    def show(self):
        self.loop()
        self.win.mainloop()

    def build(self):
        self.win.geometry('{}x{}'.format(int(0.666 * self.win.winfo_screenwidth()), self.win.winfo_screenheight()))
        self.win.resizable(True, True)
        self.win.update()

        self.win.bind("<Configure>", self.catch_resize)
        self.win.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.win.minsize(width=MAIN_WINDOW_MIN_WIDTH, height=MAIN_WINDOW_MIN_HEIGH)


    def on_resize(self):
        self.win.update()

    def on_update(self, query=False):
        self.win.update()

    def on_closing(self, *evt):
        self.win.unbind_all('<Configure>')
        self.win.destroy()
        print('\nGoodbye Space Cowboy\n')
        sys.exit(0)

    def catch_resize(self, *evt):
        if self._win_height != self.win.winfo_height() or self._win_width != self.win.winfo_width():
            self._win_width = self.win.winfo_width()
            self._win_height = self.win.winfo_height()
            debug_gui('Resized to {}x{}'.format(self._win_width, self._win_height))
            self.on_resize()