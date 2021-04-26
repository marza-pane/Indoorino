import sys
from common.gutils import *

from indoorino.core import System

import matplotlib as mpl
mpl.use("TkAgg")

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure

from tkinter import ttk

""" Base Templates """
""" Context may be [all], [motion], [selection]"""

class   AppClassTemplate:
    """
        This is an application UI template.
        Note that the actual Tkinter widget is initialized in subclass <GenericUiTemplate>
    """
    def __init__(self, *args, **kwargs):
        debug_ui('INIT AppClassTemplate with {}|{}'.format(args, kwargs))
        pass

    def build(self, *args, **kwargs):
        debug_ui('BUILD with {}|{}'.format(args, kwargs))
        pass

    def loop(self):
        pass

    def on_update(self, *args, **kwargs):
        debug_ui('ON UPDATE with args {}'.format(args))

    def on_resize(self, *args, **kwargs):
        debug_ui('ON RESIZE with args {}'.format(args))
        return 0,0

    def on_motion(self, *event):
        pass

    def on_leave(self, *event):
        debug_ui('LEAVE {}'.format(event[0]))
        pass

    def on_enter(self, *event):
        debug_ui('ENTER {}'.format(event[0]))
        pass

    def on_press(self, *event):
        debug_ui('PRESS {}'.format(event))
        pass

    def on_release(self, *event):
        debug_ui('RELEASE {}'.format(event))
        pass

    def on_scroll(self, *event):
        debug_ui('SCROLL {}'.format(event))
        pass

    def on_closing(self):
        pass

class   GenericUiTemplate(AppClassTemplate):
    """
        Generic template class for application widgets. Declarations go brrrr
        IMPORTANT : parameter <widget> must be a tkinter widget class!
    """

    def __init__(self, parent, widget, *args, **kwargs):
        AppClassTemplate.__init__(self, *args, **kwargs)
        tool_tip = kwargs.pop('tooltip','')
        widget.__init__(self, parent, **kwargs)
        self._tooltip = ToolTip(self, tool_tip)
        if not isinstance(self, tk.BaseWidget):
            error_os('Can not initialize <{}> as widget'.format(format_type(widget)))

    def build(self, *args, **kwargs):
        super(GenericUiTemplate, self).build(*args, **kwargs)

    def bind_keys(self):
        if not isinstance(self, tk.BaseWidget):
            error_os('Can not initialize <{}> as widget'.format(format_type(self)))
            return
        self.bind("<Return>", lambda evt, key='ENTER': self.catch_keystroke(key))
        self.bind("<Delete>", lambda evt, key='DEL': self.catch_keystroke(key))
        self.bind("<Escape>", lambda evt, key='ESC': self.catch_keystroke(key))
        self.bind("<Cancel>", lambda evt, key='CANC': self.catch_keystroke(key))
        self.bind("<Key>",    lambda event: self.catch_keystroke(str(event.char).upper()))

    def bind_mouse_motion(self):
        if not isinstance(self, tk.BaseWidget):
            error_os('Can not initialize <{}> as widget'.format(format_type(self)))
            return
        self.bind("<Enter>", self.on_enter)
        self.bind("<Leave>", self.on_leave)
        self.bind("<Motion>", self.on_motion)

    def bind_mouse_buttons(self):
        if not isinstance(self, tk.BaseWidget):
            error_os('Can not initialize <{}> as widget'.format(format_type(self)))
            return
        self.bind("<ButtonPress-1>", self.on_press)
        self.bind("<ButtonRelease-1>", self.on_release)
        self.bind("<MouseWheel>", self.on_scroll)

    # def bind_default(self, context):
    #     if not isinstance(self, tk.BaseWidget):
    #         error_os('Can not initialize <{}> as widget'.format(format_type(self)))
    #         return
    #
    #     if context == 'all':
    #         self.bind_default('motion')
    #         self.bind_default('selection')
    #     elif context == 'motion':
    #         self.bind("<Enter>", self.on_enter)
    #         self.bind("<Leave>", self.on_leave)
    #         self.bind('<Motion>', self.on_motion)
    #     elif context == 'selection':
    #         self.bind("<ButtonPress-1>", self.on_press)
    #         self.bind("<ButtonRelease-1>", self.on_release)
    #     else:
    #         error_os('invalid bind {}: context can be motion, selection, all'.format(context))

    def on_resize(self, *args, **kwargs):
        super(GenericUiTemplate, self).on_resize(*args, **kwargs)
        if not isinstance(self, tk.BaseWidget):
            error_os('Can not resize <{}>, No tkinter class'.format(format_type(self)))
            return
        self.update()
        try:
            w = self.winfo_width()
            h = self.winfo_height()
            return w, h
        except tk.TclError as error:
            debug_ui('warning: can not place element [{}]'.format(error))
            return 0, 0

    def on_update(self, *args, **kwargs):
        super(GenericUiTemplate, self).on_update(*args, **kwargs)

    def on_motion(self, *event):
        super(GenericUiTemplate, self).on_motion(*event)
        self._tooltip.on_motion(*event)

    def on_leave(self, *event):
        super(GenericUiTemplate, self).on_leave(*event)
        self._tooltip.on_leave(*event)

    def on_enter(self, *event):
        super(GenericUiTemplate, self).on_enter(*event)
        self._tooltip.on_enter(*event)

    def on_press(self, *event):
        super(GenericUiTemplate, self).on_press(*event)
        pass

    def on_release(self, *event):
        super(GenericUiTemplate, self).on_release(*event)
        pass

    def on_scroll(self, *event):
        super(GenericUiTemplate, self).on_scroll(*event)
        pass

    def catch_keystroke(self, char):
        debug_ui('Pressed <{}>'.format(char))

    def settooltip(self, message=''):
        self._tooltip.text=message

    def on_closing(self):
        try:
            self.master.on_closing()
        except AttributeError:
            pass


class   BoardLinkTemplate:
    """ This template is used to link a class to a board """
    def __init__(self, name):
        self.board = name
        if not self.exist():
            warning_os('Can not init BoardLink with board name <{}>'.format(name))

    def get_board(self):
        if self.exist():
            return System.boards()[self.board]
        return None

    def exist(self):
        if self.board in System.boards().keys():
            return True
        return False

class   DeviceLinkTemplate(BoardLinkTemplate):
    """ This template is used to link a class to a device """
    def __init__(self, board, device):
        self.device = device
        BoardLinkTemplate.__init__(self, board)
        if not self.exist():
            warning_os('Can not init DeviceLink with device name <{}>'.format(device))

    def exist(self):
        if super(DeviceLinkTemplate, self).exist():
            board = System.boards()[self.board]
            if self.device in board.device.keys():
                return True
        return False

    def get_device(self):
        if self.exist():
            return System.boards()[self.board].device[self.device]
        return None


""" Base Widget Templates """

class   ApplicationWindowTemplate(tk.Tk, AppClassTemplate):

    def __init__(self):

        tk.Tk.__init__(self)
        AppClassTemplate.__init__(self)

        self.attributes('-alpha', 1)
        Palette.generic.BG_DEFAULT = self.cget("bg")

    def build(self):
        # self.geometry('1440x900')
        # self.win.geometry('{}x{}'.format(int(0.666 * self.win.winfo_screenwidth()), int(0.9 * self.win.winfo_screenheight())))
        # self.resizable(True, True)
        # self.attributes('-zoomed', True)
        # self.attributes('-alpha', 0.5)

        self.update()
        Screen.CURRENT_SCREEN_W = self.winfo_screenwidth()
        Screen.CURRENT_SCREEN_H = self.winfo_screenheight()
        self.bind("<Configure>", self.catch_resize)
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.minsize(width=Screen.MIN_WINDOW_W,
                     height=Screen.MIN_WINDOW_H)

    def show(self):
        self.build()
        self.on_update()
        self.on_resize()

        self.after(5, self.loop)
        self.mainloop()

    def loop(self):
        if Screen.ON_RESIZE_FLAG and not Mouse.ON_PRESS:
            self.resize()

    def bind_keys(self):
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.bind("<Return>", lambda evt, key='ENTER': self.catch_keystroke(key))
        self.bind("<Delete>", lambda evt, key='DEL': self.catch_keystroke(key))
        self.bind("<Escape>", lambda evt, key='ESC': self.catch_keystroke(key))
        self.bind("<Cancel>", lambda evt, key='CANC': self.catch_keystroke(key))
        self.bind("<Key>",    lambda event: self.catch_keystroke(str(event.char).upper()))

    def bind_default(self, context):

        if context == 'all':
            pass
        elif context == 'motion':
            self.bind("<Enter>", self.on_enter)
            self.bind("<Leave>", self.on_leave)
            self.bind('<Motion>', self.on_motion)
        elif context == 'selection':
            self.bind("<ButtonPress-1>", self.on_press)
            self.bind("<ButtonRelease-1>", self.on_release)

    def on_resize(self):
        self.update()
        try:
            w = self.winfo_width()
            h = self.winfo_height()
            return w, h
        except tk.TclError as error:
            debug_ui('warning: can not call winfo width/heigh [{}]'.format(error))
            return 0, 0

    def on_update(self, *args, **kwargs):
        pass

    def on_closing(self, *evt, code=0):
        self.unbind_all('<Configure>')
        print('\nGoodbye Space Cowboy\n')
        self.destroy()
        sys.exit(code)

    def on_motion(self, *evt):
        Screen.update()

    def resize(self):
        w = self.winfo_width()
        h = self.winfo_height()

        if Screen.CURRENT_WINDOW_H != h or Screen.CURRENT_WINDOW_W != w:
            print('Catch resize!')
            Screen.update(str(self.geometry()))
            debug_ui('Resized to {}x{}'.format( Screen.CURRENT_WINDOW_W, Screen.CURRENT_WINDOW_H ))
            Screen.ON_RESIZE_ID = self.after(5, self.on_resize)
            Screen.ON_RESIZE_FLAG=False

    def catch_resize(self, *evt):
        Screen.ON_RESIZE_FLAG=True
        while Mouse.ON_PRESS:
            return
        else:
            self.resize()

    def catch_keystroke(self, char):
        debug_ui('Pressed <{}>'.format(char))

class   TopLevelTemplate(tk.Toplevel, AppClassTemplate):

    def __init__(self, parent, **kwargs):

        tk.Toplevel.__init__(self, parent, **kwargs)
        AppClassTemplate.__init__(self)
        self._win_height = 0
        self._win_width  = 0

    def build(self):
        self.update()
        self.bind("<Configure>", self.catch_resize)
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.geometry('480x320')

    def bind_keys(self):
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.bind("<Return>", lambda evt, key='ENTER': self.catch_keystroke(key))
        self.bind("<Delete>", lambda evt, key='DEL': self.catch_keystroke(key))
        self.bind("<Escape>", lambda evt, key='ESC': self.catch_keystroke(key))
        self.bind("<Cancel>", lambda evt, key='CANC': self.catch_keystroke(key))
        self.bind("<Key>",    lambda event: self.catch_keystroke(str(event.char).upper()))

    def bind_events(self):
        self.bind('<Motion>', self.on_motion)
        self.bind("<Enter>", self.on_enter)
        self.bind("<Leave>", self.on_leave)
        self.bind('<Motion>', self.on_motion)
        self.bind("<ButtonPress-1>", self.on_press)
        self.bind("<ButtonRelease-1>", self.on_release)

    def on_closing(self, *evt):
        self.unbind_all('<Configure>')
        self.destroy()

    def show(self):
        self.build()
        self.on_resize()
        # self.mainloop()
        try:
            self.wait_window()
        except tk.TclError:
            pass
    def on_press(self, *evt):
        pass

    def on_release(self, *evt):
        pass

    def on_enter(self, *evt):
        pass

    def on_leave(self, *evt):
        pass

    def on_motion(self, *evt):
        Screen.update()

    def resize(self):
        w = self.winfo_width()
        h = self.winfo_height()

        if Screen.CURRENT_WINDOW_H != h or Screen.CURRENT_WINDOW_W != w:
            print('Catch resize!')
            Screen.update(str(self.geometry()))
            debug_ui('Resized to {}x{}'.format( Screen.CURRENT_WINDOW_W, Screen.CURRENT_WINDOW_H ))
            Screen.ON_RESIZE_ID = self.after(5, self.on_resize)
            Screen.ON_RESIZE_FLAG=False

    def catch_resize(self, *evt):
        Screen.ON_RESIZE_FLAG=True
        while Mouse.ON_PRESS:
            return
        else:
            self.resize()

    def catch_keystroke(self, char):
        debug_ui('Pressed <{}>'.format(char))

    def on_resize(self):
        self.update()
        try:
            w = self.winfo_width()
            h = self.winfo_height()
            return w, h
        except tk.TclError as error:
            warning_ui('warning: can not call winfo width/heigh [{}]'.format(error))
            return 0, 0

class   PanedTemplate(GenericUiTemplate, tk.PanedWindow):

    """ Generic Paned Frame (container). One of the most used"""

    def __init__(self, parent, *args, **kwargs):
        GenericUiTemplate.__init__(self, parent, tk.PanedWindow, *args, **kwargs)

class   ButtonTemplate(GenericUiTemplate, tk.Button):
    def __init__(self, parent, **kwargs):
        GenericUiTemplate.__init__(self, parent, tk.Button, **kwargs)

class   LabelTemplate(GenericUiTemplate, tk.Label):
    def __init__(self, parent, **kwargs):
        GenericUiTemplate.__init__(self, parent, tk.Label, **kwargs)

class   EntryTemplate(GenericUiTemplate, tk.Entry):
    def __init__(self, parent, **kwargs):
        GenericUiTemplate.__init__(self, parent, tk.Entry, **kwargs)

class   ListBoxTemplate(GenericUiTemplate, tk.Listbox):
    def __init__(self, parent, **kwargs):
        GenericUiTemplate.__init__(self, parent, tk.Listbox, **kwargs)

class   TextBoxTemplate(GenericUiTemplate, tk.Text):
    def __init__(self, parent, **kwargs):
        GenericUiTemplate.__init__(self, parent, tk.Text, **kwargs)

    def text(self, string):
        state = self.cget('state')
        if not state == tk.NORMAL:
            self.configure(state=tk.NORMAL)
        self.delete('1.0', tk.END)
        self.insert('1.0', string)
        self.configure(state=state)

class   TreeViewTemplate(GenericUiTemplate, ttk.Treeview):

    def __init__(self, parent, **kwargs):
        GenericUiTemplate.__init__(self, parent, ttk.Treeview, selectmode ='browse', **kwargs)
        self.scrollbar = ttk.Scrollbar(self,
                           orient="vertical",
                           command=self.yview)
        self.configure(xscrollcommand=self.scrollbar.set)
        self.bind("<<TreeviewSelect>>", self.on_tv_select)
        self.bind("<<TreeviewOpen>>", self.on_tv_open)
        self.bind("<<TreeviewClose>>", self.on_tv_close)

        self._structure = dict()

    def _get_tree_path_(self):
        string = ''
        names = list()
        names.append(self.item(self.focus(), 'text'))
        parent = self.parent(self.focus())
        while len(parent) > 0:
            names.append(self.item(parent, 'text'))
            parent = self.parent(parent)

        names = list(reversed(names))
        for item in names:
            string += '{}:'.format(item)
        string = string[:-1]
        return string

    def on_tv_select(self, *event):
        string=self._get_tree_path_()
        info_os('Treeview item select <{}>'.format(string))
        return string


    def on_tv_open(self, *event):
        string=self._get_tree_path_()
        debug_ui('Treeview item open <{}>'.format(string))
        return string

    def on_tv_close(self, *event):
        string=self._get_tree_path_()
        debug_ui('Treeview item close <{}>'.format(string))
        return string

    def on_resize(self, *args, **kwargs):
        w,h = super(TreeViewTemplate, self).on_resize()
        self.scrollbar.pack(side='right', fill='y')
        return w,h

class   CanvasTemplate(GenericUiTemplate, tk.Canvas):

    def __init__(self, parent, *args, **kwargs):

        GenericUiTemplate.__init__(self, parent, tk.Canvas, *args, **kwargs)

        self.current_id = self.find_withtag(tk.CURRENT)

    def on_motion(self, *event):
        super(CanvasTemplate, self).on_motion()
        fid = self.find_withtag(tk.CURRENT)
        if not fid == self.current_id:
            self.current_id = fid
            debug_ui('Canvas:current item ID <{}:{}>'.format(tk.CURRENT, fid))

    def coord_smooth_polygon(self, x, y, sharpness, **kwargs):

        # The sharpness here is just how close the sub-points
        # are going to be to the vertex. The more the sharpness,
        # the more the sub-points will be closer to the vertex.
        # (This is not normalized)
        #
        # use 1 < sharpness < 10
        # smooth=tk.TRUE on polygon you wanna smooth

        if sharpness < 2:
            sharpness = 2

        ratioMultiplier = sharpness - 1
        ratioDividend = sharpness

        # Array to store the points
        points = []

        # Iterate over the x points
        for i in range(len(x)):
            # Set vertex
            points.append(x[i])
            points.append(y[i])

            # If it's not the last point
            if i != (len(x) - 1):
                # Insert submultiples points. The more the sharpness, the more these points will be closer to the vertex.
                points.append((ratioMultiplier * x[i] + x[i + 1]) / ratioDividend)
                points.append((ratioMultiplier * y[i] + y[i + 1]) / ratioDividend)
                points.append((ratioMultiplier * x[i + 1] + x[i]) / ratioDividend)
                points.append((ratioMultiplier * y[i + 1] + y[i]) / ratioDividend)
            else:
                # Insert submultiples points.
                points.append((ratioMultiplier * x[i] + x[0]) / ratioDividend)
                points.append((ratioMultiplier * y[i] + y[0]) / ratioDividend)
                points.append((ratioMultiplier * x[0] + x[i]) / ratioDividend)
                points.append((ratioMultiplier * y[0] + y[i]) / ratioDividend)
                # Close the polygon
                points.append(x[0])
                points.append(y[0])

        return points
        # return self.create_polygon(points, **kwargs, smooth=tk.TRUE)

    def coord_smooth_rectangle(self, coords, sharpness, **kwargs):
        return self.coord_smooth_polygon(
            [coords[0], coords[2], coords[2], coords[0]],
            [coords[1], coords[1], coords[3], coords[3]],
            sharpness, **kwargs
        )

""" Widget Templates """

class   PictureTemplate(CanvasTemplate):

    def __init__(self, parent, path, **kwargs):

        CanvasTemplate.__init__(self, parent, **kwargs)

        self.path = path

        self.source = None
        self.buffer = None
        self.image = None
        self.current_size= [1, 1]
        self.bitmap = self.create_image((0,0))

    def build(self, *args, **kwargs):
        super(PictureTemplate, self).build(*args, **kwargs)
        if self.source is None:
            self.source = ImagePil.open(self.path)

    def resize_image(self, off=2):
        w = self.winfo_width()
        h = self.winfo_height()
        size = [ max(1, w - 2 * off), max(1, h - 2 * off) ]
        if not size == self.current_size:
            self.current_size=size
            self.buffer = self.source.resize(tuple(self.current_size), ImagePil.ANTIALIAS)
            self.image = ImageTk.PhotoImage(self.buffer)
            return True
        return False

    def replace_image(self, path):

        if self.path != path:
            self.path = path
            self.source = ImagePil.open(self.path)
            self.current_size = [1, 1]
            self.on_resize()

    def place_forget(self):
        self.delete(self.bitmap)
        super(PictureTemplate, self).place_forget()

    def on_resize(self, *args, **kwargs):
        w, h = super(PictureTemplate, self).on_resize()
        off=kwargs.pop('offset', 2)
        size = [max(1, w - 2 * off), max(1, h - 2 * off)]
        if self.source is None:
            self.build()
        if not size == self.current_size:
            self.current_size = size
            self.buffer = self.source.resize(tuple(self.current_size), ImagePil.ANTIALIAS)
            self.image = ImageTk.PhotoImage(self.buffer)
            self.delete(self.bitmap)
            self.bitmap = self.create_image((off, off), image=self.image, anchor=tk.NW)
        return w,h

class   DefaultEmptyFrame(PanedTemplate):

    def __init__(self, parent, **kwargs):

        string = kwargs.pop('text', IndorinoTitle().capitalize().replace(' ', ''))
        PanedTemplate.__init__(self, parent, **kwargs)
        self.configure(bg=Palette.generic.BLACK)
        self.label= tk.Label(self,
                             font=Fonts.monobold(60),
                             text=string,
                             bg=Palette.generic.BLACK,fg='black').pack(expand=tk.TRUE, fill=tk.BOTH)

class   PanedPushTemplate(PanedTemplate):
    """ Used in Status bar """
    def __init__(self, parent, *args, **kwargs):
        PanedTemplate.__init__(self, parent, *args, **kwargs)
        self._last_bg_color_ = ''
        self.configure(relief=tk.RAISED)

    def on_press(self, *event):
        super(PanedPushTemplate, self).on_press(*event)
        self._last_bg_color_ = self.cget('bg')
        self.configure(bg=Palette.generic.WHITE)
        self.configure(relief=tk.SUNKEN)

    def on_release(self, *event):
        super(PanedPushTemplate, self).on_release(*event)
        self.configure(bg=self._last_bg_color_)
        self.configure(relief=tk.RAISED)

class   ScrollableFrameTemplate(GenericUiTemplate, tk.PanedWindow):

    """A pure Tkinter scrollable frame that actually works!
        * Use the 'frame' attribute to place widgets inside the scrollable frame
        * Construct and pack/place/grid normally
        * This frame only allows vertical scrolling
        * root->paned->frame->canvas->frame
        https://gist.github.com/bakineugene/76c8f9bcec5b390e45df
        http://tkinter.unpythonic.net/wiki/VerticalScrolledFrame
        """

    def __init__(self, parent, **kwargs):

        bg = kwargs.pop('bg', Palette.generic.BG_DEFAULT)
        GenericUiTemplate.__init__(self, parent, tk.PanedWindow, bg=bg, **kwargs)

        self.widgetlist = list()

        self._layer  = tk.Frame(self, bg=bg)
        self._scroll = tk.Scrollbar(self._layer)
        self._canvas = tk.Canvas(self._layer, bg=bg)

        self.frame   = tk.Frame(self._canvas, bg=bg)
        self._frameid = self._canvas.create_window(0, 0, anchor=tk.NW, window=self.frame)
        self._scroll_width = 15

    @property
    def scroll(self):
        # this is used to bind scroll on other widget
        return self._scroll

    def clear(self):
        for widget in self.widgetlist:
            widget.destroy()
        self.widgetlist.clear()

    def add(self, widget, *args, **kwargs):
        self.widgetlist.append(
            widget(self.frame, *args, **kwargs)
        )
        self.widgetlist[-1].build()
        self.widgetlist[-1].on_update()
        # try:
        #
        # except AttributeError:
        #     error_ui('Could not build {}'.format(widget))

        self.widgetlist[-1].bind("<MouseWheel>", self._scroll.set)

    def build(self):
        super(ScrollableFrameTemplate, self).build()
        self.bind("<MouseWheel>", self._scroll.set)
        self._canvas.bind("<MouseWheel>", self._scroll.set)
        self._layer.bind("<MouseWheel>", self._scroll.set)

        self._canvas.configure(
            bd=0,
            # bg=Palette.generic.BG_DEFAULT,
            highlightthickness=0,
            yscrollcommand=self._scroll.set)

        self._scroll.configure(
            command=self._canvas.yview,
            width=self._scroll_width,
            orient=tk.VERTICAL
        )
        self._canvas.xview_moveto(0)
        self._canvas.yview_moveto(0)

    def on_update(self, *args, **kwargs):
        super(ScrollableFrameTemplate, self).on_update()
        for entry in self.widgetlist:
            entry.on_update()

    def on_resize(self, *args, **kwargs):
        w,h = super(ScrollableFrameTemplate, self).on_resize(*args)

        """ INPUNT: an array with all lengths or a const int """
        """ This class should be overridden to place actual widgets from self.widgetlist """
        """ (remember to copy the boring lines) """
        """ Sometimes needed 2 calls to actual draw widgets"""

        self._layer.update()
        self._canvas.update()
        self.frame.update()

        wign=len(self.widgetlist)

        if wign > 0:
            wigh = [100] * wign
            if args:
                if isinstance(args[0], (int, float)):
                    wigh = [args[0]] * wign
                elif isinstance(args[0], (list, tuple)):
                    if len(args[0]) == len(self.widgetlist):
                        wigh=args[0]
                    else:
                        error_ui('input array [{}] has different size from len(widgetlist) [{}]'.format(
                            len(args[0]), len(self.widgetlist)))
                else:
                    error_ui('input array has invalid type <{}>'.format( format_type(args[0])))
            else:
                warning_ui('setting widget heigh by default [100]')

            try:
                for count, item in enumerate(self.widgetlist):
                    item.configure(
                        width=w - 1.5 * self._scroll_width,
                        heigh=wigh[count],
                    )
                    item.grid(
                        row=count,
                        column=0,
                        # columnspan=1 - w/self._scroll_width
                        # ipadx=-self._scroll_width
                    )
                    count += 1
            except tk.TclError:
                pass

        size = (self.frame.winfo_width(), self.frame.winfo_height())
        # size = (self._frame.winfo_reqwidth(), self._frame.winfo_reqheight())
        self._canvas.config(scrollregion="0 0 %s %s" % size)
        self._canvas.itemconfigure(self._frameid, width=self._canvas.winfo_width())
        self._canvas.config(width=self.frame.winfo_reqwidth())

        self._layer.pack(fill=tk.BOTH, expand=tk.TRUE)
        self._scroll.pack(fill=tk.Y, side=tk.RIGHT, expand=tk.FALSE)
        self._canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=tk.TRUE)


        for item in self.widgetlist:
            item.on_resize()

        return w,h


class TransparentFrameTemplate(AppClassTemplate):

    def __init__(self, parent, *args, **kwargs):
        AppClassTemplate.__init__(self, *args, **kwargs)
        self._position = list((0, 0, 0, 0))
        self.master = parent
        self.children = list()

    def unbind(self):
        self.master.unbind("<Enter>", self.on_enter)
        self.master.unbind("<Leave>", self.on_leave)
        self.master.unbind('<Motion>', self.on_motion)
        self.master.unbind("<ButtonPress-1>", self.on_press)
        self.master.unbind("<ButtonRelease-1>", self.on_release)

    def bind(self):
        self.master.bind("<Enter>", self.on_enter)
        self.master.bind("<Leave>", self.on_leave)
        self.master.bind('<Motion>', self.on_motion)
        self.master.bind("<ButtonPress-1>", self.on_press)
        self.master.bind("<ButtonRelease-1>", self.on_release)

    def on_motion(self, *event):
        debug_ui('TRANSPARENT CANVAS ON MOTION')
        for widget in self.children:
            widget.on_motion(*event)

    def on_enter(self, *event):
        debug_ui('TRANSPARENT CANVAS ON ENTER')
        for widget in self.children:
            widget.on_enter(*event)

    def on_leave(self, *event):
        debug_ui('TRANSPARENT CANVAS ON LEAVE')
        for widget in self.children:
            widget.on_leave(*event)

    def on_press(self, *event):
        debug_ui('TRANSPARENT CANVAS on_press')
        for widget in self.children:
            widget.on_press(*event)

    def on_release(self, *event):
        debug_ui('TRANSPARENT CANVAS on_release')
        for widget in self.children:
            widget.on_release(*event)

    def place_forget(self):
        for index in range(len(self._position)):
            self._position[index] = 0

    def place(self, **kwargs):
        self._position.clear()
        self._position.append(kwargs.pop('x', 0))
        self._position.append(kwargs.pop('y', 0))
        self._position.append(kwargs.pop('width', 0))
        self._position.append(kwargs.pop('heigh', 0))

    def on_resize(self, *args, **kwargs):
        if not len(self._position) == 4:
            return 0, 0, 0, 0
        return tuple(self._position)

    def turn_waiting(self):
        pass

    def turn_on(self):
        pass

    def turn_off(self):
        pass

    def turn_offline(self):
        pass

class TransparentFrameContainer(TransparentFrameTemplate):

    def __init__(self, parent, *args, **kwargs):
        TransparentFrameTemplate.__init__(self, parent, *args, **kwargs)

    def on_motion(self, *event):
        self.master.on_motion(*event)
        super(TransparentFrameContainer, self).on_motion(*event)

    def on_enter(self, *event):
        self.master.on_enter(*event)
        super(TransparentFrameContainer, self).on_enter(*event)

    def on_leave(self, *event):
        self.master.on_leave(*event)
        super(TransparentFrameContainer, self).on_leave(*event)

    def on_press(self, *event):
        self.master.on_press(*event)
        super(TransparentFrameContainer, self).on_press(*event)

    def on_release(self, *event):
        self.master.on_release(*event)
        super(TransparentFrameContainer, self).on_release(*event)

    def place_forget(self):
        self.unbind()
        super(TransparentFrameContainer, self).place_forget()

# class   ShadowedFrameTemplate(TransparentFrameTemplate):
#
#     def __init__(self, parent, *args, **kwargs):
#         TransparentFrameTemplate.__init__(self, parent, *args, **kwargs)
#         self.___positions = (0,0,0,0) # this is made to avoid excessive resizing while using mouse
#         self.shadowcolor=kwargs.pop('color', 'black')
#         self.background = None
#
#
#     def place_forget(self):
#         super(ShadowedFrameTemplate, self).place_forget()
#         if self.background:
#             self.master.delete(self.background)
#             self.background=None
#
#     # def place(self, **kwargs):
#     #     super(ShadowedFrameTemplate, self).place(**kwargs)
#
#     def destroy(self, *args):
#         self.place_forget()
#
#     def on_resize_alpha(self, x,y,w,h):
#         if w > 0 and h > 0:
#             alpha=0.5
#             alpha = int(alpha * 255)
#             fill = self.master.winfo_rgb('black') + (alpha,)
#             image = ImagePil.new('RGBA', ( int(w), int(h)), fill)
#             self.master.alpha.append(ImageTk.PhotoImage(image))
#             self.background = self.master.create_image( x, y, image=self.master.alpha[-1], anchor='nw')
#         return x, y, w, h
#
#     def on_resize(self, *args, **kwargs):
#         positions = super(ShadowedFrameTemplate, self).on_resize(*args, **kwargs)
#
#         if positions != self.___positions:
#             self.___positions = positions
#             return self.on_resize_alpha(
#                 positions[0],
#                 positions[1],
#                 positions[2],
#                 positions[3],
#             )
#         elif self.background:
#             self.master.tag_raise(self.background)
#
#         # return self.on_resize_alpha(
#         #     positions[0],
#         #     positions[1],
#         #     positions[2],
#         #     positions[3],
#         # )
#
#         # if w > 0 and h > 0:
#         #     alpha=0.5
#         #     alpha = int(alpha * 255)
#         #     fill = self.master.winfo_rgb('black') + (alpha,)
#         #     image = ImagePil.new('RGBA', ( int(w), int(h)), fill)
#         #     self.master.alpha.append(ImageTk.PhotoImage(image))
#         #     self.background = self.master.create_image( x, y, image=self.master.alpha[-1], anchor='nw')
#         # if self._resize_id:
#         #     self.master.after_cancel(self._resize_id)
#         # self._resize_id = self.master.after(300, lambda : self.on_resize_alpha(x,y,w,h))
#
#         return positions[0], positions[1], positions[2], positions[3]

# class   CanvasWidgetTemplate(TransparentFrameTemplate):
#     """ Transparent frame to use on resources map """
#     def __init__(self, parent, ):
#         TransparentFrameTemplate.__init__(self, parent)
#         self.lines = list()
#
#     def place_forget(self):
#         super(CanvasWidgetTemplate, self).place_forget()
#         for line in self.lines:
#             self.master.delete(line)
#         self.lines.clear()

class   PlotTemplate(GenericUiTemplate, tk.Frame):

    class PlotTemplateLine:

        def __init__(self, x, y, label, color):

            self.x = x
            self.y = y
            self.label=label
            self.color=color
            self.status=True

    def __init__(self, parent, **kwargs):

        # tk.Frame.__init__(self, parent, **kwargs)
        GenericUiTemplate.__init__(self, parent, tk.Frame, **kwargs)

        self._lines = list()
        self._flag_hold=False
        self._flag_grid=False

        self._figure = mpl.figure.Figure(figsize = (5, 5), dpi = 100,
                                  facecolor='#2d2d2d',
                                  # facecolor='red',
                                  # frameon=False,
                                  # facecolor=[i/65280 for i in self.winfo_rgb(Palette.generic.BG_DEFAULT)],
                                  edgecolor='red')
        self._plot = self._figure.add_subplot(111)
        self._canvas = FigureCanvasTkAgg(self._figure, self)

        axes = self._figure.gca()
        # axes.set_facecolor('xkcd:salmon')
        axes.set_facecolor('#070d0d') # plot background color (almost black)
        # axes.xaxis.label.set_color(Palette.generic.WHITE)
        # axes.yaxis.label.set_color(Palette.generic.WHITE)
        axes.tick_params(axis='x', colors=Palette.generic.WHITE)
        axes.tick_params(axis='y', colors=Palette.generic.WHITE)

        try:
            toolbar = NavigationToolbar2Tk(self._canvas, self, pack_toolbar=False)
            toolbar.pack(side=tk.BOTTOM, fill=tk.X)
        except TypeError as error:
            debug_ui('Could not pack toolbar [{}]'.format(error))
            toolbar = NavigationToolbar2Tk(self._canvas, self)

        toolbar.update()
        # self.canvas.show()

    def build(self):
        self._figure.set_facecolor(Palette.generic.BG_DEFAULT)

    def hold(self, *args):
        if args:
            self._flag_hold=args[0]
        else:
            return self._flag_hold

    def grid(self, *args):
        if args:
            self._flag_grid=args[0]
            self.draw()
        else:
            return self._flag_grid

    def clean(self):
        self._plot.clear()
        self._lines.clear()
        self._canvas.draw()
        self._canvas.flush_events()

    def plot(self, x, y, label='', **kwargs):

        if not len(x) == len(y):
            error_os('x and y have different sizes ({}:{})'.format(
                len(x), len(y)
            ))

        if len(label) == 0:
            label = 'data{}'.format(len(self._lines))

        for index, item in enumerate(self._lines):
            if item.y == y:
                print('Plot Already in list')
                return
            if item.label == label and len(x) > len(item.x):
                self._lines[index].x = x
                self._lines[index].y = y
                return

        if not self._flag_hold:
            self.clean()

        self._lines.append(
            self.PlotTemplateLine(
                x, y, label, Palette.plot.new()))


        # lineid = self._plot.plot(x, y, label=label)
        # lineid.set_label(label)


    def draw(self):

        self._plot.clear()

        if len(self._lines) == 0:
            return

        for line in self._lines:
            self._plot.plot(
                line.x, line.y,
                label=line.label,
                color=line.color,
                # marker='.',
                # markersize = 5,
                linestyle = '-',
                linewidth = 2,
            )


        if self._flag_grid:
            self._plot.grid()

        self._plot.legend()
        self._canvas.draw()
        # self._canvas.flush_events()

        # if len(self._x_values) == len(self._y_values):
        #     if not self._flag_hold:
        #         self.clean()
        #     self._plot.plot(self._x_values, self._y_values)
        #     self._canvas.draw()
        #     self._canvas.flush_events()
        # else:
        #     error_os('x and y have different sizes ({}:{})'.format(
        #         len(self._x_values), len(self._y_values)
        #     ))

    def on_update(self, query=False):
        self.update()
        # self.draw()

    def on_resize(self):
        self.update()
        try:
            w=self.winfo_width()
            h=self.winfo_height()
            self._canvas.get_tk_widget().place(
                x = 2,
                y = 2,
                width = w - 4,
                heigh = h - 4
            )

        except tk.TclError as error:
            debug_ui('warning: can not place element [{}]'.format(error))

        self.draw()


print('Loaded common.templates')
