#--------------------------------------------------------------#
#           Full imports                                       #
#--------------------------------------------------------------#

import os
import re
import pwd  # getpwuid
import sys
import math
import time
import numpy as np
import pickle
import random
import signal
import serial
import shutil
import socket
import string
import pathlib
import calendar
import datetime
import operator # itemgetter
import threading
import matplotlib as mpl
import subprocess
import collections

import tkinter as tk

import utils.IndoorinoModule    ### these module must be compiled!

#--------------------------------------------------------------#
#           Sub imports                                        #
#--------------------------------------------------------------#

import tkinter.messagebox

from serial.tools                       import list_ports
from tkinter.font                       import Font
from matplotlib                         import use as mpluse
from matplotlib.figure                  import Figure
from matplotlib.backends.backend_tkagg  import FigureCanvasTkAgg
from matplotlib.backends.backend_tkagg  import NavigationToolbar2Tk

# COLOR_TKINTER_BG   = root.cget("background")

#--------------------------------------------------------------#
#           ************** TEST  ZONE **************           #
#--------------------------------------------------------------#

if __name__ == "__main__":

    print('\n Indoorino folder is {}'.format(DEFAULT_FOLDER_PATH))

    loaded_modules=len(sys.modules.keys())

    for n, module in enumerate(sys.modules.keys()):
        print(module)
        pass

    print('\nTotal module loaded {}'.format(loaded_modules))

    print('\t\t*** DEBUGGING MODULE ***\n\n\tInitializing a ipacket:')
