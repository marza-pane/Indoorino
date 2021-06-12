from distutils.core import setup, Extension
from glob import glob
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
#      |       Generating source paths                                     |
#      |___________________________________________________________________|
#

sourcepath = localpath.replace('build', 'libraries')
sourcefolders = [
    'common',
    'devices',
    'network',
    'packets',
    'AES/src',
    'python-module',
]

from distutils.core import setup, Extension
import os

filesource=list()
source = list()
print('\n\t{}Building Server {}'.format(GRN, TRS))


for i in sourcefolders:
    for j in os.walk(os.path.join(sourcepath, i)):
        for k in glob(os.path.join(j[0], '*.cpp')):
            print('found ./{}'.format('/'.join(k.split('/')[-3::])))
            filesource.append(k)

# source= [
#         sourcepath + '/indoorino/utils.cpp',
#         sourcepath + '/indoorino/benchmark.cpp',
#         sourcepath + '/indoorino/packeTemplate.cpp',
#         sourcepath + '/indoorino/packetStubs.cpp',
#         sourcepath + '/indoorino/packetParse.cpp',
#         sourcepath + '/indoorino/packetUtils.cpp',
#         sourcepath + '/indoorino/paths.cpp',
#         sourcepath + '/indoorino/schedule.cpp',
#         sourcepath + '/devices/boards.cpp',
#         sourcepath + '/AES/src/AESLib.cpp',
#         sourcepath + '/AES/src/AES.cpp',
#         sourcepath + '/AES/src/xbase64.cpp',
#         sourcepath + '/network/useraccess.cpp',
#         sourcepath + '/network/tcpserver.cpp',
#         sourcepath + '/network/serverutils.cpp',
#         sourcepath + '/network/tcpclient.cpp',
#         sourcepath + '/network/indoorino-client.cpp',
#
#         sourcepath + '/pymodule/pyimodule.cpp',
#         sourcepath + '/pymodule/pyipacket.cpp',
# ]

print("\n\t{}Python module - compiling with:{}\n".format(YLW,TRS))
for code in filesource:
    if os.path.isfile(code):
        if code.split('/')[-1] == 'client-start.cpp':
            continue
        source.append(code)
        print(GRN, end='')
    else:
        print(RED, end='')
    print('{} {}'.format('/'.join(code.split('/')[-3::]), TRS))

#      _____________________________________________________________________
#      |                                                                   |
#      |       Compiling modules                                           |
#      |___________________________________________________________________|
#

print('\n\t{}Now building modules{}\n'.format(YLW, TRS))

Indoorino_module    = Extension('IndoorinoModule',
                                define_macros=[('INDOORINO_CLIENT', None), ],
                                include_dirs=['/usr/include/python3.9/',
                                              '/usr/include/openssl/',
                                              sourcepath ,
                                              #sourcepath + 'AESLib/src',
                                              ],
                                library_dirs=['/usr/lib',
                                              '/usr/lib/x86_64-linux-gnu/',],
                                libraries=['m', 'dl', 'stdc++', 'boost_system', 'crypto',],
                                # libraries=['m', 'dl', 'stdc++', 'boost_system', 'boost_thread', 'ssl', 'crypto'],

                                # language='c++',
                                language='gnu++17',
                                sources=source,
                                extra_compile_args=['-std=gnu++17'],
                                )

#ipstools_module = Extension('tools',
                      #define_macros=[('PYIMODULE', '')],
                      #include_dirs=['/usr/include/python3.5/',
                                    #src_path+'indoorino/', ],
                      #library_dirs=['/usr/lib'],
                      #sources=s)

setup (
    name            ='Indoorino',
    version         ='3.1.0',
    author          ="Ann Satsu",
    author_email    ="annsatsu@yandex.com",
    description     ="Indoorino packet module",
    ext_modules     =[Indoorino_module]
)
