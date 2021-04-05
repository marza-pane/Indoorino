from common.templates import *


class UiHomeMap(CanvasTemplate):

    def __init__(self, parent, **kwargs):
        CanvasTemplate.__init__(self, parent, **kwargs)
        self._imagepath=str(pathlib.Path(PATH_APP).joinpath('pictures/top-render.png'))
        self._scrollx=tk.Scrollbar(self, orient=tk.HORIZONTAL)
        self._scrolly=tk.Scrollbar(self, orient=tk.VERTICAL)
        self._source = None
        self._image=None
        self.size = [0,0]
        self.bitmap = 0

    def build(self, *args, **kwargs):
        super(UiHomeMap, self).build()
        self._source = ImagePil.open(self._imagepath)
        self._image = ImageTk.PhotoImage(self._source)
        self.bitmap = self.create_image((0, 0), image=self._image, anchor=tk.NW)
        self.size = self._source.size
        self._scrollx.configure(command=self.xview)
        self._scrolly.configure(command=self.yview)

        self.configure(
            xscrollcommand=self._scrollx.set,
            yscrollcommand=self._scrolly.set
        )

    def on_resize(self, *args, **kwargs):
        w,h = super(UiHomeMap, self).on_resize()
        self._scrollx.pack(side=tk.BOTTOM,fill=tk.X)
        self._scrolly.pack(side=tk.RIGHT,fill=tk.Y)
        return w, h
