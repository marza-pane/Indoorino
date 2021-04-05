from common.templates import *

class DeviceWidgetTemplate(CanvasTemplate, DeviceLinkTemplate):
    def __init__(self, parent, board, device, group, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        DeviceLinkTemplate.__init__(self, board, device)
        self._group = group

class UiHomeLights(CanvasTemplate):

    class Device(DeviceWidgetTemplate):
        def __init__(self, parent, board, device, group, **kwargs):
            DeviceWidgetTemplate.__init__(self, parent, board, device, group, **kwargs, bg='green')
            self.lines = dict()

        def build(self, *args, **kwargs):
            super(UiHomeLights.Device, self).build()
            for key in ('outer', 'inner'):
                self.lines.update(
                    {
                        key:self.create_polygon(
                            (0,0,0,0,0,0),
                            smooth=tk.TRUE,
                            outline=Palette.generic.BLACK,
                            width=4,
                            fill=Palette.generic.DISABLED
                        )
                    }
                )
        def on_resize(self, *args, **kwargs):
            w,h=super(UiHomeLights.Device, self).on_resize()

            size=min(w,h)
            width=min(0.5 * w, h)

            off=size * 0.05
            thick=size * 0.025


            size *= 0.95
            self.coords(
                self.lines['outer'],
                self.coord_smooth_rectangle( (
                    off,
                    off,
                    w - off,
                    off + width,
                ), 3.0)
            )
            self.coords(
                self.lines['inner'],
                self.coord_smooth_rectangle( (
                    off + thick,
                    off + thick,
                    w - (off + thick),
                    off + width - thick,
                ), 3.0)
            )



    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        self.frame=self.Device(self, 'BOARD', 'DEVICE', 'LIGHT:BEAMS',)
        self.scheme=dict()

    def build(self, *args, **kwargs):
        super(UiHomeLights, self).build()
        self.frame.build()

    def on_resize(self, *args, **kwargs):
        w,h=super(UiHomeLights, self).on_resize()

        self.frame.pack(fill=tk.BOTH, expand=tk.TRUE)
        self.frame.on_resize()













































"""   """

"""  """






from widgets.buttons import *
class CanvasBeamFrame(TransparentFrameContainer):

    def __init__(self, parent):
        TransparentFrameContainer.__init__(self, parent)
        
        """ TEST START """
        self.children = [
            ButtonCanvasPushableSign(self.master),
            CanvasDeviceWidget(self.master),
            CanvasWidgetLightBeam(self.master),
        ]

    def build(self):
        super(CanvasBeamFrame, self).build()
        self.bind()
        self.children[0].build()
        self.children[1].build()
        self.children[2].build()

    def on_resize(self):
        w0, h0, w, h = super(CanvasBeamFrame, self).on_resize()
        self.children[0].place(
            x=w0 + 0.5 * w,
            y=h0 + 0.1 * h,
            width=min(0.1 * w, 0.1 * h),
            heigh=min(0.1 * w, 0.1 * h)
        )
        self.children[0].on_resize()
        self.children[1].place(
            x=w0 + 0.2 * w,
            y=h0 + 0.5 * h,
            width=min(0.1 * w, 0.1 * h),
            heigh=min(0.1 * w, 0.1 * h)
        )
        self.children[1].on_resize()
        self.children[2].place(
            x=w0 + 0.6 * w,
            y=h0 + 0.6 * h,
            width=min(0.1 * w, 0.1 * h),
            heigh=min(0.1 * w, 0.1 * h)
        )
        self.children[2].on_resize()

        