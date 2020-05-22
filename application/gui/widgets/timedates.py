import calendar
import datetime
import tkinter as tk
from gui.templates import GenericGuiTemplate

class EntryTime(tk.PanedWindow):

    def __init__(self, parent, *args, **kwargs):

        tk.PanedWindow.__init__(self, parent, *args, **kwargs)

        self._hour = tk.StringVar()
        self._minute = tk.StringVar()
        self._second = tk.StringVar()

        self._l_hour = tk.Entry(self)
        self._l_minute = tk.Entry(self)
        self._l_second = tk.Entry(self)

        self.d1 = tk.Label(self)
        self.d2 = tk.Label(self)

        self.set(datetime.datetime.fromtimestamp(0))
        self._flag=False

        self._l_hour.bind('<Up>', self._catch_hour_up)
        self._l_hour.bind('<Down>', self._catch_hour_down)
        self._l_minute.bind('<Up>', self._catch_minute_up)
        self._l_minute.bind('<Down>', self._catch_minute_down)
        self._l_second.bind('<Up>', self._catch_second_up)
        self._l_second.bind('<Down>', self._catch_second_down)

        self._build()
        self._loop()

    @property
    def hour(self):
        return int(self._hour.get())
    @property
    def minute(self):
        return int(self._minute.get())
    @property
    def second(self):
        return int(self._second.get())

    def _catch_hour_up(self, *evt):
        if self.hour == 23:
            self._hour.set('00')
        else:
            self._hour.set(str(int(self.hour + 1)).zfill(2))

    def _catch_hour_down(self, *evt):
        if self.hour == 0:
            self._hour.set('23')
        else:
            self._hour.set(str(int(self.hour - 1)).zfill(2))

    def _catch_minute_up(self, *evt):
        if self.minute == 59:
            self._minute.set('00')
        else:
            self._minute.set(str(int(self.minute + 1)).zfill(2))

    def _catch_minute_down(self, *evt):
        if self.minute == 0:
            self._minute.set('59')
        else:
            self._minute.set(str(int(self.minute - 1)).zfill(2))

    def _catch_second_up(self, *evt):
        if self.second == 59:
            self._second.set('00')
        else:
            self._second.set(str(int(self.second + 1)).zfill(2))

    def _catch_second_down(self, *evt):
        if self.second == 0:
            self._second.set('59')
        else:
            self._second.set(str(int(self.second - 1)).zfill(2))

    def _build(self):
        self._l_hour.configure(
            textvariable=self._hour,
            relief=tk.FLAT,
            justify=tk.CENTER
        )
        self._l_minute.configure(
            textvariable=self._minute,
            relief=tk.FLAT,
            justify=tk.CENTER,
        )
        self._l_second.configure(
            textvariable=self._second,
            relief=tk.FLAT,
            justify=tk.CENTER
        )

        self.d1.configure(
            text=':',
            relief=tk.FLAT)
        self.d2.configure(
            text=':',
            relief=tk.FLAT)

    def place(self, *args, **kwargs):
        super(EntryTime, self).place(*args, **kwargs)
        self.update()
        w = self.winfo_width()
        h = self.winfo_height()
        dot = 5
        self._l_hour.place(
            x=0,
            y=0,
            width= w / 3 - dot,
            heigh=h - 2
        )
        self.d1.place(
            x=w / 3 - dot,
            y=0,
            width= dot,
            heigh=h
        )
        self._l_minute.place(
            x= w / 3,
            y=0,
            width= w / 3 - dot,
            heigh=h - 2
        )
        self.d2.place(
            x=2 * w / 3 - dot,
            y=0,
            width= dot,
            heigh=h
        )
        self._l_second.place(
            x=2 * w / 3 ,
            y=0,
            width= w / 3 - dot,
            heigh=h - 2
        )

    def get(self):

        return datetime.time(hour=self.hour, minute=self.minute, second=self.second)

    def set(self, settime):

        if isinstance(settime, (int, float)):
            return self.set(datetime.datetime.fromtimestamp(settime))

        self._hour.set(str(int(settime.time().hour)).zfill(2))
        self._minute.set(str(int(settime.time().minute)).zfill(2))
        self._second.set(str(int(settime.time().second)).zfill(2))

    def start(self):
        self._flag=True

    def stop(self):
        self._flag=False

    def _loop(self):
        if self._flag:
            self.set(datetime.datetime.now())
        self.after(225, self._loop)

class EntryDate(tk.PanedWindow):

    def __init__(self, parent, *args, **kwargs):

        tk.PanedWindow.__init__(self, parent, *args, **kwargs)

        self._year = tk.StringVar()
        self._month = tk.StringVar()
        self._day = tk.StringVar()

        self._l_year = tk.Entry(self)
        self._l_month = tk.Entry(self)
        self._l_day = tk.Entry(self)

        self.d1 = tk.Label(self)
        self.d2 = tk.Label(self)

        self._flag=False

        self._build()
        self._loop()

    @property
    def year(self):
        return int(self._year.get())
    @property
    def month(self):
        s=self._month.get()
        for n, i in enumerate(calendar.month_abbr):
            if i.lower() == s.lower():
                return n
        return 0

    @property
    def day(self):
        return int(self._day.get())

    def _build(self):
        self._l_year.configure(
            textvariable=self._year,
            relief=tk.FLAT,
            justify=tk.CENTER
        )
        self._l_month.configure(
            textvariable=self._month,
            relief=tk.FLAT,
            justify=tk.CENTER,
        )
        self._l_day.configure(
            textvariable=self._day,
            relief=tk.FLAT,
            justify=tk.CENTER
        )

        self.d1.configure(
            text=':',
            relief=tk.FLAT)
        self.d2.configure(
            text=':',
            relief=tk.FLAT)

    def place(self, *args, **kwargs):
        super(EntryDate, self).place(*args, **kwargs)
        self.update()
        w = self.winfo_width()
        h = self.winfo_height()

        self._l_year.place(
            x=0,
            y=0,
            width= 4 * w / 9,
            heigh=h - 2
        )
        self._l_month.place(
            x= 4 * w / 9,
            y=0,
            width= w / 3,
            heigh=h - 2
        )
        self._l_day.place(
            x=7 * w / 9 ,
            y=0,
            width= 2 * w / 9 - 2,
            heigh=h - 2
        )

    def get(self):

        return datetime.date(year=self.year, month=self.month, day=self.day)

    def set(self, settime):

        if isinstance(settime, (int, float)):
            return self.set(datetime.datetime.fromtimestamp(settime))

        self._year.set(str(settime.date().year))
        self._month.set(settime.strftime("%b"))
        self._day.set(str(int(settime.date().day)).zfill(2))

    def start(self):
        self._flag=True

    def stop(self):
        self._flag=False

    def _loop(self):
        if self._flag:
            self.set(datetime.datetime.now())
        self.after(1000, self._loop)

class EntryDatetime(tk.PanedWindow):

    def __init__(self, parent, *args, **kwargs):

        tk.PanedWindow.__init__(self, parent, *args, **kwargs)

        self.date = EntryDate(self)
        self.time = EntryTime(self)

    def place(self, *args, **kwargs):
        super(EntryDatetime, self).place(*args, **kwargs)
        self.update()

        self.date.place(
            x=2,
            y=0,
            width=self.winfo_width()/2,
            heigh=self.winfo_height() - 2
        )
        self.time.place(
            x=self.winfo_width()/2 + 4,
            y=0,
            width=self.winfo_width() / 2 - 6,
            heigh=self.winfo_height() - 2
        )

    def set(self, value):
        self.date.set(value)
        self.time.set(value)

    def get(self):

        return datetime.datetime(year=self.date.year, month=self.date.month, day=self.date.day,
                                 hour=self.time.hour, minute=self.time.minute, second=self.time.second)

if __name__ == '__main__':

    from gui.application import ApplicationWindowTemplate

    app = ApplicationWindowTemplate()
    datetimewidget =  EntryDatetime(app.win)

    datetimewidget.configure(bg='blue')
    datetimewidget.set(datetime.datetime.now())
    datetimewidget.date.start()
    datetimewidget.time.start()
    datetimewidget.place(x=10, y=10, width=280, heigh=50)
    app.show()


