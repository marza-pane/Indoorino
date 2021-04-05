from common.templates import *

import tkinter.messagebox

class StatusBar(PanedTemplate):

    class MessageList:

        def __init__(self):
            self.messages = list()
            self.timeout = 2
            self.db = list()
            # [0] message, [1] color, [2] timeout

        def push(self, message, color=Palette.generic.FG_DEFAULT):

            for item in self.messages:
                if message == item[0] and color == item[1]:
                    return

            self.messages.append([message, color, 0])
            self.db.append('{} : {}'.format(datetime.datetime.now().strftime('%y-%m-%d %H:%M:%S'), message))

        def get_current(self):

            string = ''
            color = Palette.generic.FG_DEFAULT

            if self.messages:
                string = self.messages[0][0]
                color = self.messages[0][1]

                if self.messages[0][2] == 0:
                    self.messages[0][2] = time.perf_counter() + self.timeout

                elif time.perf_counter() > self.messages[0][2]:
                    if len(self.messages) > 1:
                        self.messages.pop(0)
                        return self.get_current()
            return string, color

        def get_all(self):
            string = '\n'
            for item in self.db:
                string += '  {}\n'.format(str(item))

            return string

    class TopClientLogin(TopLevelTemplate):

        def __init__(self, parent):
            TopLevelTemplate.__init__(self, parent)

            self.attributes('-type', 'dock')
            self.btn_exit = tk.Button(self,
                                      text='Exit',
                                      font=Fonts.mono(12),
                                      command=self.call_btn_exit)
            self.btn_login = tk.Button(self,
                                       text='Connect',
                                       font=Fonts.mono(12),
                                       command=self.call_btn_login)

            self.var_user = tk.StringVar()
            self.var_pass = tk.StringVar()
            self.var_stat = tk.StringVar()

            self.str_user = tk.Label(self,
                                     text='Username:',
                                     anchor=tk.W,
                                     padx=10,
                                     font=Fonts.mono(11))

            self.str_pass = tk.Label(self,
                                     text='Password:',
                                     anchor=tk.W,
                                     padx=10,
                                     font=Fonts.mono(11))

            self.var_stat.set('Initializing...')
            self.str_stat = tk.Label(self,
                                     textvariable=self.var_stat,
                                     anchor=tk.W,
                                     padx=10,
                                     font=Fonts.mono(10))

            self.ent_user = tk.Entry(self,
                                     textvariable=self.var_user,
                                     relief=tk.RAISED,
                                     font=Fonts.mono(11))

            self.ent_pass = tk.Entry(self,
                                     textvariable=self.var_pass,
                                     relief=tk.RAISED,
                                     font=Fonts.mono(11),
                                     show="*")

        def build(self):
            super(StatusBar.TopClientLogin, self).build()
            w, h = 400, 120
            self.resizable(False, False)
            self.minsize(width=w,
                         height=h)
            self.geometry('%dx%d%+d%+d' % (
                w, h,
                0.5 * Screen.CURRENT_SCREEN_W - 200,
                0.5 * Screen.CURRENT_SCREEN_H - 100))
            self.focus_force()
            self.on_resize()

        def on_resize(self):
            w, h = super(StatusBar.TopClientLogin, self).on_resize()

            soff = 22

            thick = 0.5 * (h - soff) - 7.5
            offset = 0.5 * (h - soff) + 2.5

            off1 = 100
            off2 = 100

            self.str_user.place(
                x=10,
                y=5,
                width=off1,
                heigh=thick
            )
            self.str_pass.place(
                x=10,
                y=offset,
                width=off1,
                heigh=thick
            )

            self.ent_user.place(
                x=off1 + 15,
                y=5,
                width=w - off1 - off2 - 30,
                heigh=thick
            )
            self.ent_pass.place(
                x=off1 + 15,
                y=offset,
                width=w - off1 - off2 - 30,
                heigh=thick
            )

            self.btn_login.place(
                x=w - off2 - 10,
                y=5,
                width=off2,
                heigh=thick
            )
            self.btn_exit.place(
                x=w - off2 - 10,
                y=offset,
                width=100,
                heigh=thick
            )

            self.str_stat.place(
                x=10,
                y=h - soff,
                width=w - 20,
                heigh=soff
            )

        def call_btn_exit(self, *event):
            self.on_closing(*event)

        def call_btn_login(self, *event):
            username = self.var_user.get()
            password = self.var_pass.get()

            if not len(username) or not len(password):
                self.var_stat.set('Please insert credentials to continue')
                self.str_stat.configure(fg=Palette.generic.WARNING)
                return

            """ Here I log myself in toward Indoorino server """

            Config.username = username
            Config.password = password
            if not System.clientstat():
                if not System.io.begin():
                    tkinter.messagebox.showinfo('NETWORK', 'Server is offline')
                    return
            debug_connection('Loggin in with User:<{}> Pass:<{}>'.format(username, ''.join(['*' * len(password)])))
            System.login(username, password)
            self.on_closing(*event)

        def catch_keystroke(self, char):
            super(StatusBar.TopClientLogin, self).catch_keystroke(char)

            if char == 'ENTER':
                self.call_btn_login()
            elif char == 'ESC':
                self.on_closing()

    def __init__(self, parent, **kwargs):
        PanedTemplate.__init__(self, parent, **kwargs)

        self._status_flags = [True, True, False]
        self._status_str = tk.StringVar()
        self._status_str.set('Initializing...')

        fnt_butts = Fonts.mono(10)

        self.btn_exit = ButtonTemplate(self,
                                       text='Exit',
                                       font=fnt_butts,
                                       command=self.clb_on_exit
                                       )
        self.btn_connect = ButtonTemplate(self,
                                          text='Connect',
                                          font=fnt_butts,
                                          command=self.clb_connect
                                          )
        self.btn_disconnect = ButtonTemplate(self,
                                             text='Stop',
                                             font=fnt_butts,
                                             command=self.clb_disconnect
                                             )
        self.btn_login = ButtonTemplate(self,
                                        text='Login',
                                        font=fnt_butts,
                                        command=self.clb_login
                                        )
        self.btn_option = ButtonTemplate(self,
                                         text='Options',
                                         font=fnt_butts,
                                         command=self.clb_option
                                         )
        self.btn_update = ButtonTemplate(self,
                                         text='Update',
                                         font=fnt_butts,
                                         command=self.clb_update
                                         )
        self.btn_refresh = ButtonTemplate(self,
                                          text='Refresh',
                                          font=fnt_butts,
                                          command=self.clb_refresh
                                          )
        self.lbl_status = LabelTemplate(self,
                                        textvariable=self._status_str,
                                        font=Fonts.monobold(11),
                                        anchor=tk.W,
                                        )

        self.message = self.MessageList()
        self.lbl_status.bind_mouse_motion()
        self.lbl_status.bind_mouse_buttons()

        if System.io.is_online():
            self.after(500, lambda: self.message.push('Connected to local network', Palette.generic.ONLINE))

    def loop(self):

        self.on_update()

        message, color = self.message.get_current()

        if self._status_str.get() != message:
            self._status_str.set(message)
            self.lbl_status.configure(fg=color)

    def on_update(self, *args, **kwargs):

        if System.io.is_online():

            if System.io.is_connected():

                if System.session():
                    self._status_flags[2] = True
                    self.message.push('Connected to server', Palette.generic.ONLINE)
                else:
                    self._status_flags[2] = False
                    self.message.push('Client connected. Please login', Palette.generic.WARNING)
            else:
                self._status_flags[1] = False
                self._status_flags[2] = False
                self.message.push('Ready to begin', Palette.generic.FG_DEFAULT)
                # self.message.push('Can not connect to server!', Palette.generic.OFFLINE)
        else:
            self._status_flags[0] = False
            self._status_flags[1] = False
            self._status_flags[2] = False
            self.message.push('No network!', Palette.generic.OFFLINE)

        self.lbl_status.settooltip(self.message.get_all())

    def clb_connect(self, *evt):

        if not System.io.is_online():
            tkinter.messagebox.showwarning('NETWORK', 'No network connection')
            return False

        elif not System.io.is_connected():
            System.io.begin()
            timeout = time.perf_counter() + 0.5
            while time.perf_counter() < timeout:
                System.io.loop()
                if System.io.is_connected():
                    return True
            tkinter.messagebox.showwarning('NETWORK', 'Server seems to be offline')
            return False

        return True

    def clb_disconnect(self, *evt):
        System.io.stop()

    def clb_login(self, *evt):
        if not Config.password or not Config.username:
            StatusBar.TopClientLogin(self).show()
            return False
        if self.clb_connect():
            tkinter.messagebox.showinfo('Sorry', 'Only connects, login not implemented yet')
            # client.login(Config.username, Config.password)

    def clb_option(self, *evt):
        pass

    def clb_update(self, *evt):
        info_network('Requesting update ### TO IMPLEMENT')
        # client.serverequest("UPDATE:ALL")

    def clb_refresh(self, *evt):
        self.master.on_update()

    def clb_on_exit(self, *args):
        self.on_closing()

    def on_resize(self, *args, **kwargs):
        w, h = super(PanedTemplate, self).on_resize()
        w_btn = 80
        widget_list = (self.btn_connect,
                       self.btn_disconnect,
                       self.btn_login,
                       self.btn_option,
                       self.btn_update,
                       self.btn_refresh)

        for off, item in enumerate(widget_list):
            item.place(
                x=off * w_btn,
                y=0,
                width=w_btn,
                heigh=h
            )

        self.lbl_status.place(
            x=len(widget_list) * w_btn + 5,
            y=0,
            width=w - (w_btn * (len(widget_list) + 1) + 10),
            heigh=h
        )
        self.btn_exit.place(
            x=w - w_btn,
            y=0,
            width=w_btn,
            heigh=h
        )

