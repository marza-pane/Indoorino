from common.templates import *
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

        