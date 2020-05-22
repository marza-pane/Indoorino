
from tkinter import font
import tkinter as tk


MAIN_WINDOW_MIN_WIDTH               =600
MAIN_WINDOW_MIN_HEIGH               =800

class Fonts:

    @staticmethod
    def mono(size):
        return tk.font.Font(family='mono', size=size)

    @staticmethod
    def monobold(size):
        return tk.font.Font(family='mono', weight="bold", size=size)

def Splashscreen(title):
    root = tk.Tk()
    w = root.winfo_screenwidth()
    h = root.winfo_screenheight()
    c = root.cget("background")

    root.wm_attributes('-alpha', 1)
    root.attributes('-type', 'dock')
    root.resizable(False, False)

    root.geometry('{}x{}+{}+{}'.format(
        500, 100,
        int(w / 2 - 250),
        int(h / 2 - 50)
    ))

    tk.Label(root, text=str(title), font=Fonts.monobold(30), bg=c, fg='green').pack(fill=tk.BOTH, expand=True)
    root.after(3000, root.destroy)
    root.mainloop()

if __name__ == "__main__":

    Splashscreen('Ind00rino')