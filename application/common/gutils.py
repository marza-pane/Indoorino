from common.utils import *

import pynput
import tkinter.font
import tkinter as tk

from PIL import Image as ImagePil
import PIL.ImageTk as ImageTk

#_______________________________________________________________#
#                                                               #
#           GUI utils                                           #
#_______________________________________________________________#
#                                                               #

class Fonts:

    @staticmethod
    def mono(size):
        return tkinter.font.Font(family='mono', size=size)

    @staticmethod
    def monobold(size):
        return tkinter.font.Font(family='mono', weight="bold", size=size)

class PaletteManager:

    class GenericPalette:
        def __init__(self):
            self.R_TITLE = 'light sky blue'
            self.ERROR_GUI = 'red4'
            self.DISABLED = 'grey35'
            self.WARNING = 'dark orange'
            self.ONLINE = 'green3'
            self.OFFLINE = 'firebrick3'
            self.FG_DEFAULT = 'white'
            self.BG_DEFAULT = 'red'
            self.BLACK = 'grey5'
            self.WHITE = 'snow'
            self.OFF = 'firebrick4'
            self.ON = 'chartreuse'

    class ReportPalette:
        def __init__(self):
            self.BG_DEBUG = 'snow'
            self.BG_INFO = 'light sky blue'
            self.BG_ALERT = 'dark orange'
            self.BG_WARNING = 'orange red'
            self.BG_ERROR = 'firebrick1'
            self.BG_FATAL = 'red4'
            self.BG_UNKNOWN = 'purple4'

            self.FG_DEBUG = 'black'
            self.FG_INFO = 'black'
            self.FG_ALERT = 'navy'
            self.FG_WARNING = 'navy'
            self.FG_ERROR = 'navy'
            self.FG_FATAL = 'snow'
            self.FG_UNKNOWN = 'snow'

    class BoardPalette:

        class BoardProjectPalette:
            def __init__(self):
                self.SAMPLER    ='SpringGreen4'
                self.ESPROUTER  ='DeepSkyBlue4'
                self.CONTROLLER ='MediumOrchid4'
                self.CAMERA     ='SlateBlue1'
                self.PHONEDOOR  ='RoyalBlue1'

        class BoardHardwarePalette:
            def __init__(self):
                self.UNO    ='blue4'
                self.DUE    ='firebrick4'
                self.MEGA   = 'dark violet4'
                self.ESP    ='dark green'

        def __init__(self):
            self.projects = self.BoardProjectPalette()
            self.hardware = self.BoardHardwarePalette()

    class DevicePalette:
        def __init__(self):
            self.GENERIC='aquamarine2'
            self.DHT22='goldenrod1'
            self.LDR='gold'
            self.RELAY='cornflower blue'

    def __init__(self):
        self.generic=self.GenericPalette()
        self.report=self.ReportPalette()
        self.board=self.BoardPalette()
        self.device=self.DevicePalette()

class ScreenMonitor:

    def __init__(self):

        self.CURRENT_SCREEN_W = 0
        self.CURRENT_SCREEN_H = 1

        self.MIN_WINDOW_W = 600
        self.MIN_WINDOW_H = 480

        self.CURRENT_WINDOW_X = 0
        self.CURRENT_WINDOW_Y = 0

        self.CURRENT_WINDOW_W = 0
        self.CURRENT_WINDOW_H = 0

        self.ON_RESIZE_FLAG = False

    def update(self, size=''):
        if size:
            geometry = size.replace('x', '+').split('+')
            self.CURRENT_WINDOW_W = int(geometry[0])
            self.CURRENT_WINDOW_H = int(geometry[1])
            self.CURRENT_WINDOW_X = int(geometry[2])
            self.CURRENT_WINDOW_Y = int(geometry[3])

class MouseMonitor:

    def __init__(self):

        self._mouse_controller = pynput.mouse.Controller()
        self._mouse_listen = pynput.mouse.Listener(
            on_move=self._update_mouse_position,
            on_click=self._update_pressed,
            on_scroll=self._update_scroll)
        self._mouse_listen.start()

        self.CURRENT_MOUSE_X = 0
        self.CURRENT_MOUSE_Y = 0

        self.ON_MOVE=False
        self.ON_PRESS=False
        self.ON_SCROLL=0

    def stop(self):
        self._mouse_listen.stop()

    def _update_mouse_position(self, x, y):
        self._mouse_controller.ON_MOVE=True
        self.CURRENT_MOUSE_X = x
        self.CURRENT_MOUSE_Y = y

    def _update_pressed(self,x, y, button, pressed):
        if pressed:
            debug_ui('mouse button {} PRESSED at {}x{}!'.format(button, x,y))
            self.ON_PRESS=True
        else:
            debug_ui('mouse button {} RELEASED at {}x{}!'.format(button, x,y))
            self.ON_PRESS=False

    def _update_scroll(self,x, y, dx, dy):
        self.ON_SCROLL=max(dx, dy)

class IconPaths:

    class _System:

        @staticmethod
        def NOT_FOUND():
            return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/not-found.png'))

        @staticmethod
        def IOVERVIEW():
            return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/ioverview.png'))\

        @staticmethod
        def TREEVIEW_ON():
            return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/treeview-on.png'))

        @staticmethod
        def TREEVIEW_OFF():
            return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/treeview-off.png'))

        @staticmethod
        def COLLAPSE():
            return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/collapse-close.png'))

        @staticmethod
        def RESTORE():
            return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/collapse-open.png'))

    class _Devices:

        class _Default:
            @staticmethod
            def ERROR():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/board-red.png'))
            @staticmethod
            def ONLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/board-red.png'))
            @staticmethod
            def OFFLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/board-red.png'))


        class _Diodes:
            @staticmethod
            def ON():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/diod-on.png'))
            @staticmethod
            def OFF():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/diod-off.png'))
            @staticmethod
            def ERROR():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/diod-error.png'))
            @staticmethod
            def ONLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/diod-on.png'))
            @staticmethod
            def OFFLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/diod-offline.png'))

        class _Thermometer:
            @staticmethod
            def ERROR():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/thermometer-error.png'))
            @staticmethod
            def ONLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/thermometer-online.png'))
            @staticmethod
            def OFFLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/thermometer-offline.png'))


        def __init__(self):
            self.diodes = self._Diodes()
            self.thermometer = self._Thermometer()
            self.default = self._Default()

        def __getitem__(self, item):
            if item.lower() == 'relay':
                return self.diodes
            else:
                return self.default

    class _Lights:

        class _BeamSpot:

            @staticmethod
            def OFF():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/beam-spot-FF.png'))

            @staticmethod
            def ON_SENSOR():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/beam-spot-NF.png'))

            @staticmethod
            def ON():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/beam-spot-NN.png'))

            @staticmethod
            def ERROR():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/beam-spot-error.png'))

            @staticmethod
            def OFFLINE():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/beam-spot-offline.png'))

            @staticmethod
            def DISCONNECTED():
                return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/beam-spot-disconnected.png'))

        def __init__(self):
            self.beamspot = self._BeamSpot()


    @staticmethod
    def BOARD_GREEN():
        return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/board-green.png'))

    @staticmethod
    def BOARD_RED():
        return str(pathlib.Path(PATH_APP).joinpath('pictures/icons/board-red.png'))

    def __init__(self):

        self.system = self._System()
        self.devices = self._Devices()
        self.lights = self._Lights()
    # class _IconHelper:
    #
    #     class Icon:
    #         def __init__(self, path):
    #             self.source = ImagePil.open(path)
    #             self.size = [10, 10]
    #             self.buffer = self.source.resize(tuple(self.size), ImagePil.ANTIALIAS)
    #             self.image = ImageTk.PhotoImage(self.buffer)
    #
    #         def __call__(self, w, h):
    #             debug_ui('Icon resizing to {}x{}'.format(w, h))
    #             size = [w, h]
    #             if all(size) and size != self.size:
    #                 self.size = [w, h]
    #                 self.buffer = self.source.resize(self.size, ImagePil.ANTIALIAS)
    #                 self.image = ImageTk.PhotoImage(self.buffer)
    #             return self.image
    #
    #     def __init__(self, data):  # data may be {'BOARD_RED':'/path/to/icon.png' ...}
    #         self._data = dict()
    #
    #         for key, path in data.items():
    #             path = str(pathlib.Path(PATH_APP).joinpath('pictures' + path))
    #             if not os.path.isfile(path):
    #                 error_ui('Can not read image file {}'.format(path))
    #                 path = DEFAULT_PATH_ICON
    #             try:
    #                 self._data[key] = self.Icon(path)
    #                 self.__setattr__(key, lambda w, h, k=key: self._image_get(k, w, h))
    #                 print('loading image {}'.format(path))
    #             except (FileNotFoundError, AttributeError):
    #                 error_ui('Can not find file image {}'.format(path))
    #
    #     def _image_get(self, key, w=0, h=0):
    #         return self._data[key](w, h)

# class IconManager:
#
#     class Template(_IconHelper):
#         def __init__(self, data):
#             _IconHelper.__init__(self, data)
#
#     def __init__(self):
#         """ must be built becuase of Tk init"""
#         self.boards=None
#         self.system=None
#
#     def build(self):
#         boards_data = {
#             'BOARD_RED': '/icons/board-red.png',
#             'BOARD_GREEN': '/icons/board-green.png',
#         }
#
#         system_data = {
#             'NOT_FOUND': '/icons/not-found.png',
#             'TREEVIEW_ON': '/icons/treeview-on.png',
#             'TREEVIEW_OFF': '/icons/treeview-off.png',
#             'IOVERVIEW': '/icons/ioverview.png',
#         }
#         self.boards = self.Template(boards_data)
#         self.system = self.Template(system_data)

"""
    <Screen> is a global class monitor. Remember to update it in the main loop() and in catch_resize()
    <Palette> is a color manager
    <Fonts> return tkinter fonts
"""

Palette     = PaletteManager()
Screen      = ScreenMonitor()
Mouse       = MouseMonitor()
Icons       = IconPaths()

class ToolTip(object):

    """ create a tooltip for a given widget """

    def __init__(self, widget, text=''):
        self.text=text
        self.widget=widget
        self.tip=None

    def show(self):
        if self.tip:
            self.tip.destroy()
        self.tip = tk.Toplevel(self.widget)
        self.tip.wm_overrideredirect(True)
        self.tip.label = tk.Text(self.tip,
                        relief=tk.FLAT,
                        wrap=tk.WORD,
                        bd=0,
                        padx=5,
                        font=Fonts.mono(8),
                        )
        self.tip.label.insert(tk.END, self.text)
        self.tip.label.pack(expand=True, fill=tk.BOTH)
        self.place()

    def place(self):
        chunks = str(self.text).split('\n')
        if len(chunks) == 1:
            w = 8 * len(self.text)
            h = 16

        else:
            h = 15 * len(chunks)
            w = 0
            for entry in chunks:
                size = len(entry) * 8
                if size > w:
                    w = size

        self.tip.wm_geometry('%dx%d%+d%+d' % (
            w + 5, h + 5,
            Mouse.CURRENT_MOUSE_X,
            Mouse.CURRENT_MOUSE_Y + 30))

    def on_enter(self, *event):
        if len(self.text) == 0:
            return
        if Mouse.CURRENT_MOUSE_X == 0 and Mouse.CURRENT_MOUSE_Y == 0:
            return
        self.show()

    def on_motion(self, *event):

        if len(self.text) == 0:
            return

        if self.tip:
            if Mouse.CURRENT_MOUSE_X == 0 and Mouse.CURRENT_MOUSE_Y == 0:
                return

            if len(self.text) == 0:
                self.on_leave()
                return

            if not self.tip.label.get('1.0', tk.END).strip('\n') == self.text:
                self.tip.label.delete('1.0', tk.END)
                self.tip.label.insert(tk.END, self.text)

            self.place()

        else:
            self.show()

    def on_leave(self, *event):
        if self.tip:
            self.tip.destroy()
            self.tip=None

print('Loaded common.gutils')

# print( Icons.BOARD_GREEN() )
