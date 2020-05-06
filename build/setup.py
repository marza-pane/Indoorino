from distutils.core import setup, Extension
import sysconfig
import sys
import os

RED="\033[1;31m"
GRN="\033[1;32m"
YLW="\033[1;33m"
TRS="\033[0m"


localpath = os.path.dirname(os.path.abspath(__file__))

#      _____________________________________________________________________
#      |                                                                   |
#      |       Removing old build files                                    |
#      |___________________________________________________________________|
#

def distutils_path():
    f = "{dirname}.{platform}-{version[0]}.{version[1]}"
    return [f.format(dirname='temp',
                    platform=sysconfig.get_platform(),
                    version=sys.version_info),
            f.format(dirname='lib',
                    platform=sysconfig.get_platform(),
                    version=sys.version_info) ]

for buildpath in distutils_path():
    full_buildpath = os.path.join(localpath,'build',buildpath)
    if os.path.isdir(full_buildpath):
        print('\n\t{}Removing old modules from {}... {}\n'.format(YLW, full_buildpath, TRS))
        os.system('rm -vr {}'.format(full_buildpath))
    else:
        print('\n\t{}{} not found - skipping{}\n'.format(YLW, full_buildpath, TRS))

#      _____________________________________________________________________
#      |                                                                   |
#      |       Generating source paths|
#      |___________________________________________________________________|
#

sourcepath = localpath.replace('build', 'libraries')

from distutils.core import setup, Extension
import os

source= [
         sourcepath + '/indoorino/utils.cpp',
         sourcepath + '/indoorino/packeTemplate.cpp',
         sourcepath + '/indoorino/packetStubs.cpp',
         sourcepath + '/indoorino/packetUtils.cpp',
         sourcepath + '/indoorino/packetParse.cpp',
         sourcepath + '/pymodule/server_module.cpp',
         sourcepath + '/indoorino/benchmark.cpp',
]

print("\n\t{}Python module - compiling with:{}\n".format(YLW,TRS))
for code in source:
    if os.path.isfile(code):
        print(GRN, end='')
    else:
        print(RED, end='')
    print(code + TRS)

#      _____________________________________________________________________
#      |                                                                   |
#      |       Compiling modules                                           |
#      |___________________________________________________________________|
#

print('\n\t{}Now building modules{}\n'.format(YLW, TRS))

Indoorino_module    = Extension('IndoorinoModule',
                      define_macros=[('INDOORINO_PYLIB', '')],
                      include_dirs=['/usr/include/python3.5/',
                                    sourcepath, ],
                      library_dirs=['/usr/lib'],
                      #language='gnu++17',
                      sources=source)

#ipstools_module = Extension('tools',
                      #define_macros=[('PYIMODULE', '')],
                      #include_dirs=['/usr/include/python3.5/',
                                    #src_path+'indoorino/', ],
                      #library_dirs=['/usr/lib'],
                      #sources=s)

setup (
    name            ='Indoorino',
    version         ='3.0.6',
    author          ="Ann Satsu",
    author_email    ="annsatsu@yamdex.com",
    description     ="Indoorino packet module",
    ext_modules     =[Indoorino_module]
)
