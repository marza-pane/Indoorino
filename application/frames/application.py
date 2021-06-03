from common.templates import *
from widgets.treeview import ResourceTreeView
from widgets.topbar import TopDashboardBar
from widgets.statusbar import StatusBar
from indoorino.core import System

from frames.ioverview import UiIOverview
from frames.boards import UiBoards
from frames.homemap import UiHomeMap
from frames.homelights import UiHomeLights
from frames.homelayout import UiLayout
from frames.weather import UiWeather
from frames.alarms import UiAlarms

import tkinter.messagebox

class ApplicationWindow(ApplicationWindowTemplate):

    def __init__(self):
        ApplicationWindowTemplate.__init__(self)
        self.main_frame = ApplicationMainFrame(self, bg='blue')

    def build(self):
        super(ApplicationWindow, self).build()
        self.title(IndorinoTitle().upper())
        self.geometry('1600x900')

        self.main_frame.build()

    def loop(self):
        super(ApplicationWindow, self).loop()
        System.loop()

        """ Put here mail GUI loop """
        self.main_frame.loop()

        Config.flags.clear()
        self.after(10, self.loop)

    def on_resize(self, *args, **kwargs):
        w,h=super(ApplicationWindow, self).on_resize()
        self.main_frame.place(
            x=0,
            y=0,
            width=w,
            heigh=h
        )
        self.main_frame.on_resize()

    def on_closing(self, *evt):
        System.on_exit()
        self.main_frame.on_closing()
        super(ApplicationWindow, self).on_closing(*evt)

class ApplicationMainFrame(PanedTemplate):

    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)
        self.frames = {
            'overview'          : UiIOverview(self),
            'boards'            : UiBoards(self),
            'resources:alarms'  : UiAlarms(self),
            'layout'            : UiLayout(self),
            'map'               : UiHomeMap(self),
            'resources:lights'  : UiHomeLights(self),
            'resources:weather' : UiWeather(self),
        }

        self.status_bar = StatusBar(self)
        self.tree_view = ResourceTreeView(self)
        self.top_bar = TopDashboardBar(self)
        self.current='resources:lights'
        self.current='resources:alarms'
        self.current='overview'
        self.current='layout'
        self.current='resources:alarms'
        self.current='boards'
        # self.after(4500, self.tempdrop)

    def tempdrop(self):
        self.show_frame('resources:alarms:fire:attic')

    def build(self, *args, **kwargs):
        super(ApplicationMainFrame, self).build(*args)

        for entry in self.frames.values():
            entry.build()
        self.status_bar.build()
        self.tree_view.build()
        self.top_bar.build()

    def loop(self):
        if self.current in self.frames.keys():
            self.frames[self.current].loop()

        self.status_bar.loop()
        self.tree_view.loop()

    def on_update(self, *args, **kwargs):
        if self.current in self.frames.keys():
            self.frames[self.current].on_update()
        self.status_bar.on_update()
        self.tree_view.on_update()

    def show_frame(self, name):

        if not len(name): return
        info_os('Showing [{}]'.format(name))

        if name == 'treeview':
            if 'disabled' in self.tree_view.state():
                self.tree_view.state(('!disabled',))
                self.top_bar.buttons['treeview'].replace_image(Icons.system.TREEVIEW_ON())
                self.on_resize()
            else:
                self.tree_view.state(('disabled',))
                self.top_bar.buttons['treeview'].replace_image(Icons.system.TREEVIEW_OFF())
                self.on_resize()
            return

        if 'resources:alarms' in name:

            key = 'resources:alarms'
            arg='all'

            chunk = name.split(':')
            if len(chunk) > 2 :
                if chunk[2] in [group.alarmtype.lower() for group in System.alarms.groups.values()]:
                    if len(chunk) == 3:
                        arg=chunk[2]
                    else:
                        data = [
                            group.name for
                                group in System.alarms.groups.values() if
                                group.alarmtype.lower() == chunk[2] and
                                group.name.split(' ')[0] == chunk[3]]

                        if data:
                            arg=data[0]
                        else:
                            error_ui('application:show:alarms: {} not a valid group'.format(chunk[3]))
                else:
                    error_ui('application:show:alarms: {} not a valid type'.format(chunk[2]))


            if not self.current == key:
                if self.current in self.frames.keys():
                    self.frames[self.current].place_forget()
                self.current = key
                self.on_resize()
            self.frames[self.current].show(arg)
            self.frames[self.current].on_update()

        if 'boards:' in name:

            key = 'boards'
            chunk = name.split(':')
            if len(chunk) == 2:
                if chunk[1].upper() in System.boards().keys():
                    if not self.current == key:
                        if self.current in self.frames.keys():
                            self.frames[self.current].place_forget()
                        self.current = key
                        self.on_resize()
                    self.frames[self.current].show_frame(chunk[1].upper())
                    self.frames[self.current].on_update()



        elif name in self.frames.keys():
            if self.current == name:
                self.frames[self.current].on_update()
                return
            if self.current in self.frames.keys():
                self.frames[self.current].place_forget()
            self.current=name
            self.frames[self.current].on_update()
            self.on_resize()
            return

        else:
            warning_ui('Invalid show_frame command {}'.format(name))

    def on_resize(self, *args, **kwargs):
        w,h=super(ApplicationMainFrame, self).on_resize()

        h_topbar=50
        h_statbar=35

        self.top_bar.place(
            x=0,
            y=0,
            width=w,
            h=h_topbar
        )

        self.status_bar.place(
            x=0,
            y=h - h_statbar,
            width=w,
            heigh=h_statbar
        )

        if 'disabled' in self.tree_view.state():
            self.tree_view.place_forget()
            self.frames[self.current].place(
                x=0,
                y=h_topbar,
                width=w - 2,
                heigh=h - (h_topbar + h_statbar + 2)
            )
        else:
            w_tv=max(250, 0.1 * w)
            self.tree_view.place(
                x=0,
                y=h_topbar,
                width=w_tv,
                heigh=h - (h_topbar + h_statbar + 2)
            )
            self.frames[self.current].place(
                x=w_tv + 2,
                y=h_topbar,
                width=w - (w_tv + 4),
                heigh=h - (h_topbar + h_statbar + 2)
            )

        self.frames[self.current].on_resize()
        self.tree_view.on_resize()
        self.top_bar.on_resize()
        self.status_bar.on_resize()





























from frames.homelights import CanvasBeamFrame


class BackgroundFrame(CanvasTemplate):

    def __init__(self, parent, path):
        CanvasTemplate.__init__(self, parent)
        self.source = ImagePil.open(path)
        self.buffer = ImageTk.PhotoImage(self.source)
        self.background=self.create_image((0,0))
        # self.lines=dict()

    def build(self, *args, **kwargs):
        super(BackgroundFrame, self).build(*args)
        # self.lines['top'] = list()
        # for i in range(8):
        #     self.lines['top'].append(
        #         self.create_polygon(
        #             (0,0,0,0,0,0),
        #             fill='',
        #             width=5,
        #             outline=Palette.generic.BG_DEFAULT))

    def on_update(self, *args, **kwargs):
        super(BackgroundFrame, self).on_update(*args)

    def on_motion(self, *event):
        super(BackgroundFrame, self).on_motion(*event)

        # if self.current_id:
        #     for entry in self.lines['top']:
        #         if self.current_id[0] == entry:
        #             self.itemconfig(entry, outline='lawn green')
        #         else:
        #             self.itemconfig(entry, outline=Palette.generic.BG_DEFAULT)
    def on_press(self, *event):
        super(BackgroundFrame, self).on_press(*event)

    def on_release(self, *event):
        super(BackgroundFrame, self).on_release(*event)

    def on_resize(self, *args, **kwargs):
        w, h = super(BackgroundFrame, self).on_resize()
        off=kwargs.pop('offset', 2)
        self.buffer = ImageTk.PhotoImage(self.source.resize((w - 2 * off, h - 2 * off), ImagePil.ANTIALIAS))
        self.delete(self.background)
        self.background = self.create_image((off, off), image=self.buffer, anchor=tk.NW)

        # self.coords(
        #     self.lines['top'][0],
        #     self.coord_smooth_rectangle(
        #         (0.335*w,
        #          0.1*h,
        #          0.735*w,
        #          0.535*h), 5)
        # )


        self.tag_lower(self.background)
        return w,h


class ApplicationWindowOld(ApplicationWindowTemplate):

    def __init__(self):
        ApplicationWindowTemplate.__init__(self)

        bgpath=os.path.join(PATH_APP, 'pictures/top-render.png')

        self.background=BackgroundFrame(self, bgpath)
        self.treeview=None
        self.current = ['',0]

        self.res=CanvasBeamFrame(self.background)

    def build(self):
        super(ApplicationWindowOld, self).build()
        self.geometry('1024x768')
        self.background.build()
        self.res.build()

    def on_resize(self):
        w,h=super(ApplicationWindowOld, self).on_resize()
        self.background.place(
            x=0,
            y=0,
            width=w,
            heigh=h
        )
        self.background.on_resize()
        self.res.place(
            x=0,
            y=0,
            width=w,
            heigh=h
        )
        self.res.on_resize()

# root = Tk()
# root.bind("<<Foo>>", doFoo)
#
# # some time later, inject the "<<Foo>>" virtual event at the
# # tail of the event queue
# root.event_generate("<<Foo>>", when="tail")



        # if tk.messagebox.askyesno('Indoorino Start', 'Do you want to load last session?'):
        #     System.load_session()

        # Config.options.SAVE_ON_EXIT = False
        # if len(System.boards()):
        #     r = tk.messagebox.askyesnocancel('Exit Application', 'Do you want to save session?')
        #     if r is None:
        #         return
        #     elif r:
        #         Config.options.SAVE_ON_EXIT = True
