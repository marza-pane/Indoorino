from common.templates import *

class AnalogController(CanvasTemplate):

    def __init__(self, parent):
        CanvasTemplate.__init__(self, parent)

        self.___ratio=0.7
        self.___value=tk.IntVar()
        self.___status=False
        self.___flag_drag=False

        self.buttons = dict()

        self.lines = list()
        self.marks = list()
        self.l_value = 0

    def build(self, *args, **kwargs):
        super(AnalogController, self).build(*args)
        self.bind_mouse_motion()
        self.bind_mouse_buttons()
        labels = (
            ('max', self.clb_max),
            ('min', self.clb_min),
            ('inc', self.clb_inc),
            ('dec', self.clb_dec),
        )

        for key, callback in labels:
            self.buttons[key] = ButtonTemplate(
                self,
                text=key,
                font=Fonts.monobold(9),
                command=callback)

        self.lines.append(
            self.create_rectangle(
                (0, 0, 0, 0),
                fill='blue'))

        self.lines.append(
            self.create_rectangle(
                (0, 0, 0, 0),
                fill='orange',
                width=4,
                outline='gold'))

        for i in range(100):
            self.marks.append(self.create_line( (0,0,0,0), width=2))

        self.l_value = self.create_text((0, 0), justify=tk.CENTER, font=Fonts.monobold(14), text='---')
        self._draw_button(0, 0)

    def clb_min(self, *event):
        self.___value.set(0)
        self._draw_button(self.winfo_width(), self.winfo_height())

    def clb_max(self, *event):
        self.___value.set(1000)
        self._draw_button(self.winfo_width(), self.winfo_height())

    def clb_dec(self, *event):
        self.___value.set(max(self.___value.get() - 10, 0))
        self._draw_button(self.winfo_width(), self.winfo_height())

    def clb_inc(self, *event):
        self.___value.set(min(self.___value.get() + 10, 1000))
        self._draw_button(self.winfo_width(), self.winfo_height())

    def on_press(self, *event):
        super(AnalogController, self).on_press(*event)
        if len(self.current_id) and self.current_id[0] in self.lines:
            w=self.winfo_width()
            h=self.winfo_height()
            if not self.___flag_drag:
                self.___flag_drag = True
            self._set_value_from_position(w, event[0].x)
            self._draw_button(w,h)

            # print(format_event(event, True))
            # print('mouse abs @ {} : {}'.format(Mouse.CURRENT_MOUSE_X, Mouse.CURRENT_MOUSE_Y))
            # print('mouse rel @ {} : {}'.format(event[0].x, event[0].y))
            # print('position rel @ {} : {}'.format(event[0].x - 0.25 * w, event[0].y))

    def on_release(self, *event):
        super(AnalogController, self).on_release(*event)
        if self.___flag_drag:
            self.___flag_drag=False

    def on_motion(self, *event):
        super(AnalogController, self).on_motion(*event)
        if self.___flag_drag:
            w=self.winfo_width()
            h=self.winfo_height()
            self._set_value_from_position(w, event[0].x)
            self._draw_button(w, h)

    def _set_value_from_position(self,w, x):
        try:
            self.___value.set(
                min( max( 1e3 * (x - 0.25 * w) / (0.5 * w), 0 ), 1000 )
            )
        except ZeroDivisionError:
            pass

    def _draw_button(self, w, h):
        size = 0.5 * w * (1 - self.___ratio) + 5 + \
               min(max(1e-3 * self.___value.get(), 0.05), 0.975) * (self.___ratio * w - 5)
        self.itemconfig(self.l_value, text = '{}%'.format(int(1e-1 * self.___value.get())))
        self.coords( self.lines[1], (0.5 * w * (1 - self.___ratio) + 5 , 15, size, h - 15) )

    def on_resize(self, *args, **kwargs):
        w,h=super(AnalogController, self).on_resize()

        self.coords(
            self.lines[0],(
                0.5 * w * (1 - self.___ratio),
                5,
                # 0.5 * w * (1 - self.___ratio) + w * self.___ratio,
                w * (0.5 + 0.5 * self.___ratio),
                h - 5)
        )

        d_marks = w * self.___ratio / 49
        for i in range(1, 49):
            if i % 5 == 0:
                h_mark=40
            else:
                h_mark=20
            self.coords(
                self.marks[i],
                (0.5 * w * (1 - self.___ratio) + i * d_marks, 5,
                 0.5 * w * (1 - self.___ratio) + i * d_marks, h_mark),
            )
            self.coords(
                self.marks[-i],
                (0.5 * w * (1 - self.___ratio) + i * d_marks, h - h_mark,
                 0.5 * w * (1 - self.___ratio) + i * d_marks, h - 5),
            )

        self.buttons['max'].place(
            x=w * (0.5 + 0.5 * self.___ratio) + 5,
            y=(2/3) * h,
            width = 0.5 * w * (1 - self.___ratio) - 10,
            heigh = (1/3) * h - 5
        )
        self.buttons['min'].place(
            x=5,
            y=(2/3) * h,
            width = 0.5 * w * (1 - self.___ratio) - 10,
            heigh = (1/3) * h - 5
        )
        self.buttons['inc'].place(
            x=w * (0.5 + 0.5 * self.___ratio) + 5,
            y=5,
            width = 0.5 * w * (1 - self.___ratio) - 10,
            heigh = (2/3) * h - 10
        )
        self.buttons['dec'].place(
            x=5,
            y=5,
            width = 0.5 * w * (1 - self.___ratio) - 10,
            heigh = (2/3) * h - 10
        )
        self._draw_button(w,h)
        self.coords(self.l_value, (0.5 * w, 0.5 * h))
        # self.lines.append(self.create_smooth_rectangle((0,0,100,100), 5, fill='blue'))
