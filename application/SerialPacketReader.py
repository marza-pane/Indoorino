
import serial
import signal
import serial.tools.list_ports
import screeninfo

from common.templates import *
from tkinter import messagebox
from indoorino.packet import IndoorinoPacket
from common.comtable import *

class SerialPacketReader:

    class Parser:
        def __init__(self):

            self._buffer=b''
            self._preamble_buffer=b''
            self._flag_preamble=True
            self._flag_body=False
            self._timeout_buffer=0.0
            self._timeout_preamble=0.0
            self._body_size=0
            self._ready=False
            self._boardname='UNDEFINED'
            self.packets=list()

        @property
        def boardname(self):
            return str(self._boardname)

        def ready(self):
            if self._ready:
                self._ready=False
                return True
            return False

        def forge(self):
            packet = dict()
            data = self._buffer
            Client.fromSerial(packet, data)
            return  packet

        def clear(self):
            self._buffer = b''
            self._preamble_buffer = b''
            self._flag_preamble = True
            self._flag_body = False

        def add(self, bit):

            # if time.perf_counter() > self._timeout_preamble:
            #     self._preamble_buffer = b''
            time.perf_counter_ns()
            if self._flag_preamble:
                if int().from_bytes(bit, byteorder='little') == Config.macros.PREAMBLE[len(self._preamble_buffer)]:
                    # self._timeout_preamble = time.perf_counter() + 1
                    print('>')
                    self._preamble_buffer += bit
                    if self._preamble_buffer == Config.macros.PREAMBLE:
                        self._flag_preamble=False
                        self._buffer = b''
                        self._buffer += self._preamble_buffer
                        print('PASSED!')

                    if len(self._preamble_buffer) >= len(Config.macros.PREAMBLE):
                        self._preamble_buffer = b''
                else:
                    self._preamble_buffer = b''

            else:

                # if time.perf_counter() > self._timeout_buffer:
                #     self.clear()
                #     self.add(bit)
                #     return

                self._buffer += bit
                self._timeout_buffer = time.perf_counter() + 1


                if self._flag_body:
                    # print('#', end='')
                    if len(self._buffer) == Config.macros.SIZEOF_PACKET_HEADER + self._body_size:
                        packet = self.forge()
                        if 'command' in packet.keys():
                            if packet['command'] > 0:
                                self._ready=True
                                c = IndoorinoPacket()
                                c.from_client(packet)
                                self._boardname=c.source
                                self.packets.append(c)
                                print('Got packet! {}'.format(packet['label']))
                                self.clear()

                else:

                    # print('|', end='')
                    if len(self._buffer) == Config.macros.SIZEOF_PACKET_HEADER:

                        packet = self.forge()

                        if 'command' in packet.keys():
                            if packet['command'] > 0:

                                self._body_size = packet['data_size']
                                if self._body_size > 0:
                                    self._flag_body=True

                                else:
                                    self._ready = True
                                    c = IndoorinoPacket()
                                    c.from_client(packet)
                                    self.packets.append(c)
                                    print('Got packet! {}'.format(packet['label']))
                                    self.clear()
                            else:
                                error_os('Invalid packet with command=0')
                                self.clear()
                                self.add(bit)
                        else:
                            error_os('Invalid packet dictionary')
                            self.clear()
                            self.add(bit)

    def __init__(self):

        self.win = tk.Tk()

        self.screen = ScreenMonitor()

        self.baudrates = list(serial.Serial.BAUDRATES)
        self.baudrates.append(76800)
        self.baudrates.sort()

        self.dev_list = []
        self.status = False
        self.device = None
        self.baud = self.baudrates[17]
        self.board = serial.Serial()

        # self.hist_pos = 0
        # self.history = list()
        self.buttons = dict()
        self.labels = dict()
        self.labels_strings = dict()
        self.flag_scroll = False ## build() will call its callback() and turn it on
        self.flag_decode = 0 ## 0=clear. 1=clear+hex 2=hex
        self.show_bytes = False ## build() will call its callback() and turn it on
        self.parser=self.Parser()
        self.baud_entry = tk.Spinbox(self.win)
        # self.serial_string = tk.StringVar()
        # self.serial_entry = tk.Entry(self.win)

        self.terminal = tk.Text(self.win)
        self.listbox = tk.Listbox(self.win)
        self.scrollbar = tk.Scrollbar(self.listbox, orient="vertical")
        self.scroltext = tk.Scrollbar(self.terminal, orient="vertical")
        # self.scrollswitch = tk.Button(self.win)

        self.status_string = tk.StringVar()

        self.prop_list = ['Name', 'Path', 'Description', 'Product', 'Hardware ID', 'Interface', 'Location',
                          'Manufacturer', 'PID', 'Serial Number', 'Subsystem', 'USB info']
        self.prop_methods = ['name', 'device_path', 'description', 'product', 'hwid', 'interface', 'location',
                             'manufacturer', 'pid', 'serial_number', 'subsystem', 'usb_info']

        self.build()
        self.on_update()
        self.on_resize()

        self.loop()
        self.usbloop()
        self.win.mainloop()

    def build(self):

        # font_text = tk.font.Font(family='mono', size=9)
        # font_select = tk.font.Font(family='mono', size=10)

        ### Main window ###
        self.win.title('S E R I A L   P A C K E T   R E A D E R   {}'.format(Config.version()))
        self.win.resizable(True, True)

        screen = screeninfo.get_monitors()[0]
        if screen.width > (2 * screen.height):
            w = int(screen.width / 3)
        else:
            w = int(screen.width / 2)

        self.win.geometry('{}x{}+0+0'.format(w, screen.height))
        self.win.minsize(width=420, height=500)
        self.win.update()

        self.win.bind("<Configure>", self.catch_resize)
        self.win.protocol("WM_DELETE_WINDOW", self.on_closing)
        signal.signal(signal.SIGINT, lambda *args: self.on_closing())

        ### Listbox ###

        self.listbox.configure(bg='white',
                               fg='black',
                               selectforeground='white',
                               selectbackground='blue',
                               selectmode='single',
                               font=tkinter.font.Font(family='mono', size=10))

        self.listbox.bind('<<ListboxSelect>>', self.select_port)
        self.scrollbar.config(command=self.listbox.yview)
        self.scroltext.config(command=self.terminal.yview)
        self.scrollbar.pack(side="right", fill="y")
        self.scroltext.pack(side="right", fill="y")

        ### Buttons ###

        for key in ('close', 'reset', 'connect', 'send', 'clear', 'exit', 'scroll', 'bytes'):
            self.buttons.update(
                {
                    key : tk.Button(
                        self.win,
                        text=key.capitalize(),
                        font=Fonts.mono(10),
                        command=lambda c=key : self.callback(c),
                    )
                }
            )

        for key in ('req:rst', 'req:conf', 'pck:epoch'):
            self.buttons.update(
                {
                    key : tk.Button(
                        self.win,
                        text=key.upper(),
                        font=Fonts.mono(10),
                        command=lambda c=key : self.callback_send(c),
                    )
                }
            )


        self.callback('scroll')
        self.callback('bytes')

        ### BAUD ###

        baud_hint = 'Common value: ['

        for n in self.baudrates:
            baud_hint += '{},'.format(n)
        baud_hint = baud_hint[:-1] + ']'

        self.labels['baud'] = tk.Label(self.win, text='BAUD:', anchor=tk.E, font=Fonts.mono(10))
        self.labels['hint'] = tk.Text(self.win, relief=tk.FLAT, fg='grey70', font=Fonts.mono(7))
        self.labels['hint'].insert(tk.INSERT, baud_hint)
        default_baud = tk.StringVar(value='9600')

        self.baud_entry.configure(values=self.baudrates,
                                  textvariable=default_baud,
                                  justify=tk.RIGHT,
                                  font=Fonts.mono(10),
                                  bg='grey90',
                                  fg='black')

        while int(self.baud_entry.get()) < self.baud:
            self.baud_entry.invoke('buttonup')

        ### USB Properties LABELS ###

        for tag in self.prop_list:
            self.labels_strings[tag] = tk.StringVar()
            self.labels_strings[tag].set('{}:'.format(tag))
            self.labels[tag] = tk.Label(self.win, textvariable=self.labels_strings[tag], anchor=tk.W, font=Fonts.mono(9))

        self.terminal.configure(font=Fonts.mono(8))
        self.labels['status'] = tk.Label(
            self.win,
            textvariable=self.status_string,
            anchor=tk.W,
            fg='black',
            font=Fonts.mono(9)
        )

        self.status_string.set('Initializing...')

    def loop(self):
        self.read_serial()
        if self.parser.ready():
            print('ready')
            while len(self.terminal.get("1.0", tk.END)) > 1e6:
                self.terminal.delete('1.0', '2.0')
            for item in self.parser.packets:
                self.terminal.insert(tk.END, '{}'.format(item))
            if self.flag_scroll:
                self.terminal.yview_moveto(1)
        self.parser.packets.clear()
        self.win.after(5, self.loop)

    def usbloop(self):
        try:
            if self.dev_list != serial.tools.list_ports.comports():
                self.on_update()
        except TypeError:
            pass
        self.win.after(500, self.usbloop)

    def generate_string(self, array):

        string=''

        # if self.flag_decode < 2:
        #
        #     try:
        #
        # for byte in array:
        #
        #     try
        #     char = str(chr(ord(int(byte).to_bytes(1, 'little'))))
        #
        #     if char.isprintable() and self.flag_decode < 2:
        #
        #     if self.flag_decode < 2:
        #         try:
        #             c = char.decode()
        #             if c.isprintable():
        #                 string += c
        #             elif self.flag_decode:
        #                 print('Unprintable char')
        #                 i = int().from_bytes(char, byteorder='little')
        #                 string += '|{}|'.format(str(i).zfill(3))
        #         except UnicodeDecodeError:
        #             print('Unreadable char')
        #             if self.flag_decode:
        #                 i = int().from_bytes(char, byteorder='little')
        #                 string += '|{}|'.format(str(i).zfill(3))
        #     else:
        #         i = int().from_bytes(char, byteorder='little')
        #         string += '|{}|'.format(str(i).zfill(3))

    def read_serial(self):

        try:
            if self.status and self.board.in_waiting > 0:
                string = ''
                while self.board.in_waiting > 0:
                    byte = self.board.read(1)
                    self.parser.add(byte)
                    if self.show_bytes:
                        try:
                            c = byte.decode()
                            if c.isprintable() or c in (' ','\n','\t','\r'):
                                string += c
                            else:
                                i = int().from_bytes(byte, byteorder='little')
                                if string and string[-1] != '|': string += '|'
                                string += '{}|'.format(str(i).zfill(3))
                        except UnicodeDecodeError:
                            i = int().from_bytes(byte, byteorder='little')
                            if string and string[-1] != '|': string += '|'
                            string += '{}|'.format(str(i).zfill(3))

                if string:
                    # print(len(buf))
                    # print(buf)
                    while len(self.terminal.get("1.0",tk.END)) > 1e6:
                        self.terminal.delete('1.0', '2.0')

                    self.terminal.insert(tk.END, '{}'.format(string.replace('\r', '')))
                    if self.flag_scroll:
                        self.terminal.yview_moveto(1)

        except OSError:
            # except serial.serialutil.SerialException:
            tk.messagebox.showwarning('Device Error', 'Serial Device IO Error')
            self.callback('close')

    def write_serial(self, packet):

        try:
            self.board.write(packet)
        except OSError:
            # except serial.serialutil.SerialException:
            tk.messagebox.showwarning('Device Error', 'Serial Device IO Error')
            self.callback('close')

    def catch_resize(self, *evt):

        self.win.update()
        if self.screen.CURRENT_WINDOW_H != self.win.winfo_height() or self.screen.CURRENT_WINDOW_W != self.win.winfo_width():
            self.screen.update(str(self.win.geometry()))
            self.on_resize()

    # def catch_butt_up(self, *evt):
    #     if self.hist_pos != len(self.history):
    #         self.hist_pos += 1
    #         self.serial_string.set(self.history[- self.hist_pos])
    #
    # def catch_butt_down(self, *evt):
    #     if self.hist_pos > 1:
    #         self.hist_pos -= 1
    #         self.serial_string.set(self.history[- self.hist_pos])
    #     elif self.hist_pos == 1:
    #         self.hist_pos -= 1
    #         self.serial_string.set('')

    def on_resize(self):

        self.win.update()
        try:
            w, h = self.win.winfo_width(),self.win.winfo_height()
        except tk.TclError as error:
            print('warning: can not place element [{}]'.format(error))
            w, h = 0, 0


        self.listbox.place(x=10,
                           y=10,
                           width=max(180, (w / 5 + 20)),
                           heigh=150)

        self.buttons['close'].place(x=10,
                                    y=170,
                                    heigh=40,
                                    width=max((70, (w / 15))))

        self.buttons['reset'].place(x=(max(70, (w / 15)) + 20),
                                    y=170,
                                    heigh=40,
                                    width=max(70, (w / 15)))

        self.buttons['connect'].place(x=(2 * max(70, (w // 15)) + 30),
                                      y=170,
                                      heigh=40,
                                      width=max(70, (w / 15)))

        self.labels['baud'].place(x=(3 * max(70, (w / 15)) + 40),
                                  y=177.5,
                                  heigh=25,
                                  width=60)

        self.labels['hint'].place(x=(3 * max(70, (w / 15)) + max(90, (w / 7)) + 120),
                                  y=(max(150, h / 9) + 25),
                                  heigh=30,
                                  width=max(70, w - (
                                          4 * max(70, (w / 15)) + max(90, (w / 7)) + 60)))

        self.baud_entry.place(x=(3 * max(70, (w // 15)) + 110),
                              y=175,
                              heigh=30,
                              width=max(90, (w / 7)))

        data = ('req:rst', 'req:conf', 'pck:epoch')
        w_bb=(w - 180) / len(data)
        for count, key in enumerate(data):
            self.buttons[key].place(
                x=10 + count * w_bb,
                y=(max(150, h / 9) + 75),
                heigh=30,
                width=w_bb
            )
        # self.serial_entry.place(x=10,
        #                         y=(max(150, h / 9) + 75),
        #                         heigh=30,
        #                         width=(w - 180))

        self.buttons['send'].place(x=(w - 160),
                                   y=(max(150, h / 9) + 70),
                                   heigh=40,
                                   width=70)

        self.buttons['clear'].place(x=(w - 80),
                                    y=(max(150, h / 9) + 70),
                                    heigh=40,
                                    width=70)

        self.terminal.place(x=10,
                            y=(max(150, h / 9) + 125),
                            heigh=h - (max(150, h / 9) + 180),
                            width=w - 20
                            )

        self.buttons['exit'].place(x=(w - 80),
                                   y=(h - 50),
                                   heigh=40,
                                   width=70)

        self.labels['status'].place(x=10,
                                    y=(h - 45),
                                    heigh=30,
                                    width=(w - 350))
        self.buttons['scroll'].place(
            x=w - 205,
            y=(h - 45),
            heigh=30,
            width=120
        )
        self.buttons['bytes'].place(
            x=w - 330,
            y=(h - 45),
            heigh=30,
            width=120
        )
        for n, tag in enumerate(self.prop_list):

            if n < (len(self.prop_list) / 2):
                x_pos = max(180, ( int(0.2 * w + 20)) ) + 20
                y_pos = (10 + 25 * n)
            else:
                x_pos = max(320, (w + max(180, (w / 5 + 20))) / 2) + 5
                y_pos = (10 + 25 * (n - (len(self.prop_list) / 2)))
            self.labels[tag].place(x=x_pos,
                                   y=y_pos,
                                   heigh=23,
                                   width=max(120, (w - max(180, (w / 5 + 20)) - 40) / 2))

    def on_update(self):

        self.dev_list = serial.tools.list_ports.comports()

        self.listbox.delete(0, tk.END)
        for dev in self.dev_list:
            self.listbox.insert(tk.END, dev.device)

        if len(self.dev_list) > 0:
            if self.device is None or self.device not in self.dev_list:
                self.listbox.select_set(0)
                self.device = self.dev_list[0]
                # print('Device has been set to ' + self.device.device)
            else:
                self.listbox.select_set(
                    [n for n, dev in enumerate(self.dev_list) if dev.device == self.device.device][0])
        else:
            # print('BOARD(S) NOT FOUND\nPlease plug a board')
            tk.Tk.messagebox.showwarning('No boards found', 'Please plug a board')
            self.device = None

        if self.status:
            self.listbox.configure(state=tk.DISABLED)
            self.baud_entry.configure(state=tk.DISABLED)
            self.buttons['connect'].configure(state=tk.DISABLED)
            self.buttons['close'].configure(state=tk.NORMAL)
            self.buttons['send'].configure(state=tk.NORMAL)
            self.labels['status'].configure(bg='lawn green')
            self.status_string.set('Connected')
        else:
            self.baud_entry.configure(state=tk.NORMAL)
            self.listbox.configure(state=tk.NORMAL)
            self.buttons['close'].configure(state=tk.DISABLED)
            self.buttons['send'].configure(state=tk.DISABLED)
            if len(self.dev_list) > 0:
                self.buttons['connect'].configure(state=tk.NORMAL)
                self.labels['status'].configure(bg='orange')
                self.status_string.set('Not connected')
            else:
                self.buttons['connect'].configure(state=tk.DISABLED)
                self.labels['status'].configure(bg='orange red')
                self.status_string.set('No device(s) found')

        for n, tag in enumerate(self.prop_list):

            if callable(getattr(self.device, self.prop_methods[n])):
                self.labels_strings[tag].set('{}:{}'.format(tag, getattr(self.device, self.prop_methods[n])()))
            else:
                self.labels_strings[tag].set('{}:{}'.format(tag, getattr(self.device, self.prop_methods[n])))

    def callback(self, command):

        if command == 'clear':
            self.terminal.delete('1.0', tk.END)
        elif command == 'exit':
            self.on_closing()

        elif command == 'close':
            try:
                self.board.close()
            except OSError:
                self.board = serial.Serial()

            self.status = False
            self.on_update()

        elif command == 'reset':
            self.on_update()
            if self.status:
                self.callback('close')
                self.callback('connect')
            else:
                self.callback('clear')

        elif command == 'connect':
            try:
                self.baud = int(self.baud_entry.get())
            except ValueError:
                tk.messagebox.showwarning('Invalid baud rate', 'Must be positive integer')
                self.baud_entry.delete(0, tk.END)
                self.baud_entry.configure(values=self.baudrates)
                while int(self.baud_entry.get()) < self.baud:
                    self.baud_entry.invoke('buttonup')
                return

            if self.baud <= 0 or self.baud > max(self.baudrates):
                self.baud = self.baudrates[15]
                self.baud_entry.delete(0, tk.END)
                self.baud_entry.configure(values=self.baudrates)
                while int(self.baud_entry.get()) < self.baud:
                    self.baud_entry.invoke('buttonup')
            try:
                self.board = serial.Serial(self.device.device, baudrate=self.baud)
            except (serial.serialutil.SerialException, OSError):
                tk.messagebox.showwarning('Device Busy', 'Device is used by another program')
                return
            # except OSError:
            #     tk.Tk.messagebox.showwarning('Device Busy', 'Device is used by another program')
            #     return

            self.status = True
            self.callback('clear')
            self.on_update()

        elif command == 'scroll':
            if self.flag_scroll:
                self.flag_scroll = False
                self.buttons['scroll'].configure(
                    text='Scroll OFF',
                    bg='DarkGoldenrod4',
                    activebackground='DarkGoldenrod4')
            else:
                self.flag_scroll = True
                self.buttons['scroll'].configure(
                    text='Scroll ON',
                    bg='chartreuse4',
                    activebackground='chartreuse4',
                )
        elif command == 'bytes':
            if self.show_bytes:
                self.show_bytes = False
                self.buttons['bytes'].configure(
                    text='Bytes OFF',
                    bg='DarkGoldenrod4',
                    activebackground='DarkGoldenrod4')
            else:
                self.show_bytes = True
                self.buttons['bytes'].configure(
                    text='Bytes ON',
                    bg='chartreuse4',
                    activebackground='chartreuse4',
                )

    def callback_send(self, command):

        packetlist = list()

        if command == 'req:rst':
            packetlist.append(
                {
                    'command':IBACOM_RESET,
                    'source':'SHELL',
                    'target':self.parser.boardname,
                    'data':{}
                }
            )
        elif command == 'req:conf':
            packetlist.append(
                {
                    'command':IBACOM_REQUEST_CONFIG,
                    'source':'SHELL',
                    'target':self.parser.boardname,
                    'data':{}
                }
            )
            packetlist.append(
                {
                    'command':IBACOM_REQUEST_STATUS,
                    'source':'SHELL',
                    'target':self.parser.boardname,
                    'data':{}
                }
            )
            packetlist.append(
                {
                    'command': IBACOM_UNIX_EPOCH,
                    'source': 'SHELL',
                    'target': self.parser.boardname,
                    'data': {
                        'epoch':int(datetime.datetime.now().timestamp())
                    }
                }
            )
        elif command == 'pck:epoch':
            packetlist.append(
                {
                    'command': IBACOM_UNIX_EPOCH,
                    'source': 'SHELL',
                    'target': self.parser.boardname,
                    'data': {
                        'epoch':int(datetime.datetime.now().timestamp())
                    }
                }
            )
        else:
            error_os('Invalid command {}'.format(command))
            return

        for entry in packetlist:

            data = Client.toSerialBytes(entry)
            if data:
                self.write_serial(data)
            #     for i in data:
            #         self.parser.add(bytes([i]))

            else:
                error_os('Can not send {}:{}'.format(entry, data))



    def select_port(self, evt):

        if evt.x_root == -1 and evt.y_root == -1:
            positions = self.listbox.curselection()
            if len(positions) == 0:
                try:
                    self.device = self.dev_list[0]
                except IndexError:
                    pass
            else:
                try:
                    self.device = self.dev_list[positions[0]]
                except IndexError:
                    pass
            # print('Device has been set to ' + self.device.device)
            self.on_update()

    def on_closing(self, *evt):
        print('\nSee you in space cowboy\n')
        self.board.close()
        self.win.destroy()
        sys.exit(0)

if __name__ == "__main__":
    # import cProfile
    # cProfile.run('R = SerialReader()')

    R = SerialPacketReader()