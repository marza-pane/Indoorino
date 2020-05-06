from codesource.ibacomClasses import ibacomPacketMap, ibacomVariableMap
from codesource.ibacomSource import packetcomments, codepath
from datetime import datetime

LOGOWIDTH=70

def composeIntro (name):
    code = '/*\n * {}\n *\n *  Created on: Apr 25, 2020\n *   Edited on: {}\n *      Author: n00b\n */'.format(
     name, datetime.now().strftime('%b %d, %Y - %H:%M:%S')
    )
    return code

def composeLogo(logo, mode='C'):

    if mode == 'C':
        np = '//'
    else:
        np = '#'

    l1='{}    _'.format(np)
    l2='{}    |'.format(np)
    l3='{}    |'.format(np)
    l4='{}    |'.format(np)
    w=len(l3)
    for n in range(LOGOWIDTH):
        l1 += '_'
        l2 += ' '
        l4 += '_'

    l3 += '    {}'.format(logo)
    for i in range(LOGOWIDTH - len(l3) + w):
        l3 += ' '

    l1 += '_'
    l2 += '|'
    l3 += '|'
    l4 += '|'

    return '\n{}\n{}\n{}\n{}\n{}\n'.format(l1, l2, l3, l4, np)

def genComtable():

    commap = ibacomPacketMap()

    c = composeIntro('comtable.h')
    c += '\n\n#ifndef SOURCE_COMTABLE_H_\n#define SOURCE_COMTABLE_H_\n'
    c +=composeLogo('IBACOM - command list')
    c += '\n#define TOTAL_IBACOM {}\n\n'.format(len(commap.keys()))

    p = '\n\n# This code has been written by Python/Generator.py script'
    p +=composeLogo('IBACOM - command numbers    list', 'P')
    p += '\nTOTAL_IBACOM = {}\n\n'.format(len(commap.keys()))

    temp = -1
    for key, item in commap.items():

        for m in packetcomments:
            if temp < m[0] <= item.command:
            # if temp <= m[1] and item.command > m[1]:
                code = '\n\n\t/***    [{0:<6} - '.format(str(m[0]).zfill(5)) + \
                       '{0:<6}] : '.format(str(m[1]).zfill(5)) + \
                       '{} ***/\n'.format(m[2])
                p += code.replace('/***','# --- #').replace('***/','# --- #')
                c += code

        temp = item.command

        c += '\n#define\t{0:<35}'.format(key) +' '+ str(item.command)
        p += '\n{0:<35}'.format(key) +' = '+ str(item.command)

    c += '\n\n\n#endif  /* SOURCE_COMTABLE_H_ */\n\n\n'
    return c,p

def genPGMstructs():


    code = composeIntro('packetPgm.h')
    code += '\n\n#ifndef PACKETPGM_H_\n#define PACKETPGM_H_\n\n#include "comtable.h"\n#include "definitions.h"\n'
    code += '#define VARMAP_NUMBER {}\n'.format(len(ibacomVariableMap().keys()))
    code += composeLogo('mapped variables')
    code += '\n/*\n * possible types for varmap_template are:\n *\n * c = char *\n * s = uint8_t' \
            '\n * l = uint32_t\n * u = uint16_t\n * n = int16\n *\n*/\n'
    code += '\nstruct varmap_template\n{'
    code += '\n\t{0:15}'.format('char') + '{}'.format('type') + ';'
    code += '\n\t{0:15}'.format('ibasize_t') + '{}'.format('size') + ';'
    code += '\n\t{0:15}'.format('char') + '{}'.format('name') + '[LEN_FIELD];\n};\n'

    code += '\nconst varmap_template VarTable[VARMAP_NUMBER] PROGMEM = {\n'
    for x, var in enumerate(ibacomVariableMap().items()):

        if var[1].size == 'sizeof(uint8_t)':
            t = 's'
        elif var[1].size == 'sizeof(uint16_t)':
            t = 'u'
        elif var[1].size == 'sizeof(uint32_t)':
            t = 'l'
        elif var[1].size == 'sizeof(int16_t)':
            t = 'u'
        else:
            t = 'c'

        code += '\n\t{ ' + '\'{}\','.format(t)
        code += '\t{0:22},'.format(var[1].size)
        code += ' {0:22} '.format( '\"' + var[1].name + '\"') + '}'
        if x < len(ibacomVariableMap().keys()) - 1:
            code += ','
        code += '\t/* {0:3} */'.format(x)

    code += '\n};\n'
    code += composeLogo('mapped packet')
    code += '\nstruct packetmap_template\n{'
    code += '\n\t{0:15}'.format('ibacomm_t') + '{0:6}'.format('comm') + ';'
    code += '\n\t{0:15}'.format('ibavar_t') + '{0:6}'.format('fields') + ';'
    code += '\n\t{0:15}'.format('char') + '{0:6}'.format('label') + '[LEN_LABEL];'
    code += '\n\t{0:15}'.format('ibavar_t') + '{0:6}'.format('var') + '[MAX_PACKET_ENTRIES];\n};\n'

    code += '\n\nconst packetmap_template PackeTable[TOTAL_IBACOM] PROGMEM = {'

    ibacommap = ibacomPacketMap()

    for x, packet in enumerate(ibacommap.items()):

        c = packet[1].command
        n = packet[1].name
        l = packet[1].label
        f = packet[1].vars

        logo = 'IBACOM - {} - {} - {}'.format(str(c).zfill(3), packet[1].ibacomstring(), l)
        code += '\n\n' + composeLogo(logo) + '\t{\n\t\t'
        code += 'IBACOM_{}, {},\n\t\t\"{}\",'.format(n.upper(), len(f), l)

        f = dict(f)
        code += ' {'
        if len(f.keys()) > 0:
            code += ' '
            for y, var in enumerate(f.keys()):
                for z, varmap in enumerate(ibacomVariableMap().keys()):
                    if var == varmap:
                        code += '{}'.format(z)
                        if y < len(f) - 1:
                            code +=', '
                        else:
                            code += ' '
        code += '}'

        code += '\n\t}'
        if x < len(ibacommap.keys()) - 1:
            code += ','

        # if x > 5:
        #     break

    code  += '\n};\n\n'
    code +='#endif  /* PACKETPGM_H_ */'
    return code

def genPacketStubs():

    head = ''

    code = composeIntro('packetStub.cpp')
    code += '\n\n#ifndef PACKETSTUB_H_\n#define PACKETSTUB_H_\n\n#include "packeTemplate.h"\n'
    code += composeLogo('payload shortcuts to variables')
    code += '\n'
    for key, var in ibacomVariableMap().items():

        if var.size == 'sizeof(uint8_t)':
            h0 = 'uint8_t'
        elif var.size == 'sizeof(uint32_t)':
            h0 = 'uint32_t'
        else:
            h0 = 'char'

        s = '{0:<10} * ipacket::'.format(h0) + 'p_{0:<16}'.format(key) + '(void)\n{\n\tswitch(command())\n\t{'
        head += '\t{0:<15}*    '.format(h0) + 'p_{0:<14}'.format(key) + '(void);\n'

        for k, p in ibacomPacketMap().items():

            if key in p.vars.keys():
                m = ''
                for w, v in p.vars.items():
                    if w == key:
                        break
                    m += ' + {}'.format(v.size)
                s += '\n\t\tcase {} :\n\t\t\treturn ({} *) (payload()'.format(p.ibacomstring(), h0) + m + ');'
                # s += '\n\t\tcase {} :\n\t\t\treturn payload()'.format(p.ibacomstring()) + m + ';'

            else:
                pass
        s += '\n\t\tdefault:\n\t\t\treturn nullptr;'
        s += '\n\t}\n}\n\n'
        code += s

    code +='\n#endif  /* PACKETSTUB_H_ */\n\n/*\nAdd the following headers to the <ipacket> class defintion\n\n'
    code += head
    code += '\n */'
    return code

def genEvals(*file):

    if file:
        try:
            with open(file[0]) as f:
                src= f.readlines()
                f.close()
        except FileNotFoundError:
            print('CodeGen: could not open {}'.format(file[0].absolute()))
            src=[]
    else:
        src = []

    code=composeIntro('packetEval.cpp')
    code += '\n\n#include "indoorino.h"\n\nvoid\t\tipacket::eval\t\t(void)\n{'
    code += '\n\tdebug(label());\n\tswitch (command())\n\t{\n'
    logohead = composeLogo("").split('\n')
    code += logohead[0] + '\n'
    code += logohead[1] + '\n'
    code += logohead[2] + '\n'

    ibacommap = ibacomPacketMap()

    src_index=0
    while not 'switch (command())' in src[src_index]:
        src_index += 1
        if src_index >= len(src):
            raise LookupError("INVALID FILE {}. Not a valid PacketEval.cpp file".format(file))

    src_index += 2

    # evalist=list()
    # for n, line in enumerate(src):
    #     if 'EVAL:' in line:
    #         currentlist=''
    #         currentline=''
    #         m = n - 2
    #         while not 'EVAL:' in currentline:
    #




    for x, packet in enumerate(ibacommap.items()):

        c = packet[1].command
        n = packet[1].name
        l = packet[1].label
        f = packet[1].vars

        logo = 'EVAL: {} - {} - {}'.format(str(c).zfill(3), packet[1].ibacomstring(), l)
        # initline = src[src_index + 1]
        # srclogo = ''
        # for i in range(6):
        #     srclogo +=src[src_index]
        #     src_index += 1

        # print('\n###{}'.format(initline))
        # print('\n###{}'.format(composeLogo(logo)))
        # print('\n###{}'.format(srclogo))
        # exit(0)

        # if composeLogo(logo) == srclogo:
        #     code += composeLogo(logo)
        #     while src[src_index] != initline:
        #         code += src[src_index]
        #         src_index +=1

        flag = False
        for m, w in enumerate(src):
            #if eval is found in file, copy text untill next eval
            if logo in w:
                flag = True
                for b, g in enumerate(src[m:-1]):
                    if b > 0 and ('EVAL:' in g or 'default:' in g):
                        break
                    code += g
                break

        if not flag:

            code += composeLogo(logo)

            code +='\n\t\tcase IBACOM_{}:'.format(n.upper())
            code +='\n\t\t{'
            code +='\n\t\t#if   defined (PYIMODULE)\n\t\t#elif defined (INDOORINO_ESP)\n\t\t#elif defined (INDOORINO_SAMPLER)'
            code +='\n\t\t#elif defined (INDOORINO_CAMERA)'
            code +='\n\t\t#endif\n\t\t\tbreak;\n\t\t}\n\n'

    code += composeLogo('EVAL: DEFAULT')
    code +='\n\t\tdefault:\n\t\t{\n\t\t\treturn;\n\t\t}\n'
    code +='\t}\n}'


    return(code)



if __name__ == '__main__':

    # c,p = genComtable()
    #
    # print(c)
    # print(p)

    # print(genPacketStubs())
    # print(genEvals('/home/n00b/Code/Develop/Indoorino3/libraries/indoorino/packetEval.cpp'))

    # print(codepath.joinpath('libraries/indoorino/packetMap.h') )
    # print(genPGMstructs())

    path = codepath.joinpath('libraries/indoorino/packetMap.h')

    print('\n\n\t\t ** INDOORINO CODE GENERATOR **')
    try:
        with open(path, 'w') as f:
            print('\nWriting {} ...'.format(path), end='')
            f.write(genPGMstructs())
            f.close()
        print('done!')
    except FileNotFoundError:
        print('WARNING! CodeGen: could not write {}'.format(path))

    path = codepath.joinpath('libraries/indoorino/comtable.h')

    c,p = genComtable()
    try:
        with open(path, 'w') as f:
            print('Writing {} ...'.format(path), end='')
            f.write(c)
            f.close()
        print('done!')
    except FileNotFoundError:
        print('WARNING! CodeGen: could not write {}'.format(path))

    path = codepath.joinpath('application/utils/comtable.py')

    try:
        with open(path, 'w') as f:
            print('Writing {} ...'.format(path), end='')
            f.write(p)
            f.close()
        print('done!')
    except FileNotFoundError:
        print('WARNING! CodeGen: could not write {}'.format(path))

    # path = codepath.joinpath('libraries/indoorino/packetEval.cpp')
    # c = genEvals(path)
    # try:
    #     with open(path, 'w') as f:
    #         print('Writing {} ...'.format(path), end='')
    #         f.write(c)
    #         f.close()
    #     print('done!')
    # except FileNotFoundError:
    #     print('WARNING! CodeGen: could not write {}'.format(path))

    path = codepath.joinpath('libraries/indoorino/packetStubs.cpp')
    try:
        with open(path, 'w') as f:
            print('Writing {} ...'.format(path), end='')
            f.write(genPacketStubs())
            f.close()
        print('done!')
    except FileNotFoundError:
        print('WARNING! CodeGen: could not write {}'.format(path))

    print('\n\nlibraries updated - all done')