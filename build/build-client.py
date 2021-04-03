import os
from glob import glob

RED="\033[1;31m"
GRN="\033[1;32m"
YLW="\033[1;33m"
TRS="\033[0m"

os.system('clear')

sourcefolders = [
    'common',
    'devices',
    'network',
    'packets',
    'server',
    'AES/src',
]

compiler = 'gcc'
outputname = 'clientbuilt'


macros = [ 'INDOORINO_NETWORK', 'INDOORINO_CLIENT',]
options = [ 'Wall', 'pthread', 'std=c++17']
# options = [ 'Wall', 'pthread', 'fpermissive', 'std=c++17']
includes = [ '.', '/usr/include/openssl/', '/usr/include/boost/', '/usr/include/python3.9']
libraries = [ 'm', 'dl', 'stdc++', 'boost_system', 'crypto'] #ssl

localpath = os.path.dirname(os.path.abspath(__file__))
sourcepath = localpath.replace('build', 'libraries')

source = list()
print('\n\t{}Building Server{}'.format(GRN, TRS))

for i in sourcefolders:
    for j in os.walk(os.path.join(sourcepath, i)):
        for k in glob(os.path.join(j[0], '*.cpp')):
            print('found {}'.format('/'.join(k.split('/')[-3::])))
            source.append(k)

compilestring = '{} '.format(compiler)

for macro in macros:
    compilestring += '-D{} '.format(macro)

for include in includes:
    compilestring += '-I{} '.format(include)

for code in source:
    compilestring += '{} '.format(code)    

for lib in libraries:
    compilestring += '-l{} '.format(lib)

for option in options:
    compilestring += '-{} '.format(option)

compilestring += '-o {} '.format(outputname)

print ("{}copile command is : \n{}{}".format(YLW,compilestring,TRS))
os.chdir(sourcepath)

if os.path.isfile('./{}'.format(outputname)):
    print("{}Removing old server bin{}".format(YLW,TRS))
    os.system("/bin/rm -v {}".format(outputname))

os.system(compilestring)

if os.path.isfile('./{}'.format(outputname)):
    flag=True
    print('{}Compiled succesfully!{}'.format(GRN, TRS))
    os.system('cp -v {} {}'.format(outputname, os.path.join(sourcepath, '../client')))
else:
    flag=False
    print('{}Check compiling errors{}!'.format(RED, TRS))
    
os.chdir(localpath)
if flag:
    os.system(sourcepath+os.path.sep+outputname)
    
              
#clc
#cd ../libraries
#gcc -Wall -I. -DINDOORINO_SERVER indoorino/utils.cpp indoorino/benchmark.cpp AES/src/AESLib.cpp AES/src/AES.cpp AES/src/xbase64.cpp network/tcpserver.cpp network/serverutils.cpp network/tcpclient.cpp network/indoorino-server.cpp network/server-start.cpp -o server -lstdc++ && {
    #mv server ../build/server
    #cd ../build
    #./server } | cd ../build

