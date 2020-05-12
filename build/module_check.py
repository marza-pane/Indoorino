from shutil import copyfile
import datetime
import sysconfig
import sys
import os
import gc

RED="\033[1;31m"
GRN="\033[1;32m"
BLU="\033[1;34m";
YLW="\033[1;33m"
TRS="\033[0m"

def distutils_path():
    f = "{dirname}.{platform}-{version[0]}.{version[1]}"
    return [f.format(dirname='temp',
                     platform=sysconfig.get_platform(),
                     version=sys.version_info),
            f.format(dirname='lib',
                     platform=sysconfig.get_platform(),
                     version=sys.version_info)]


modulepath = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    'build',
    distutils_path()[1],
)
modulepath = os.path.join(modulepath, os.listdir(modulepath)[0])

if os.path.isfile(modulepath):
    print(" Copying new module")
    copyfile(modulepath, './IndoorinoModule.so')
else:
    print('{} ERROR: can not find updated module file at {}{}'.format(RED, modulepath, TRS))
    exit(1)

try:
    import IndoorinoModule
except ImportError as error :
    print('{} ERROR: can not load mdoule:\n{}{}{}'.format(RED, BLU, error, TRS));
    exit(1)

print("{} Module loaded!{}".format(GRN, TRS))

IndoorinoModule.benchmark()

print("{} Testing module functionality{}".format(GRN, TRS))

print("{} Module constants{}".format(YLW, TRS))
for key, item in IndoorinoModule.appconfig().items():
    print('{} : {}'.format(key, item));
    
packetdict = {
    'command':15,
    'name':'SERVER',
    'data':
        {'message': 'This is a test message',
         'name':'APPLICATION',
         'epoch':int(datetime.datetime.now().timestamp()),
         'level':3
         },
    }

print("{} building packet from dict{}".format(YLW, TRS))
buildict = IndoorinoModule.build(packetdict);

print("{} Built test packet dictionary{}".format(YLW, TRS))
for key, item in buildict.items():
    print('{} : {}'.format(key, item)); 
    
print("{} forging packet from dict{}".format(YLW, TRS))
packetarray = IndoorinoModule.forge(packetdict);

print("{} Now Checksum : ".format(YLW), end='')
checksum = IndoorinoModule.checksum(packetdict);
print("{} {}".format(checksum, TRS))

print("{} now parsing packet by bytes {}".format(YLW, TRS))
for byte in packetarray:
    IndoorinoModule.parsebyte(byte)
    obtained_packet = IndoorinoModule.obtain()
    if len(obtained_packet) > 0:
        for rcvpacket in obtained_packet:
            for key, item in rcvpacket.items():
                print('{} : {}'.format(key, item));

print("{} now parsing packet by array {}".format(YLW, TRS))
IndoorinoModule.parse(packetarray)
obtained_packet = IndoorinoModule.obtain()
if len(obtained_packet) > 0:
    for rcvpacket in obtained_packet:
        for key, item in rcvpacket.items():
            print('{} : {}'.format(key, item));

