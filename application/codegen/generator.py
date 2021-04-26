from classes import CodeSourceVariables, CodeSourcePacket, packet_comments
from source import codepath
from datetime import datetime

LOGOWIDTH=70

def composeIntro (name):
    code = '/*\n * {}\n *\n *  Created on: Apr 25, 2020\n *   Edited on: {}\n *      Author: n00b\n'.format(
     name, datetime.now().strftime('%b %d, %Y - %H:%M:%S')
    )
    code += ' *  This code is code-generated\n */'
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

    packmap = CodeSourcePacket()

    c = composeIntro('comtable.h')
    c += '\n\n#ifndef SOURCE_COMTABLE_H_\n#define SOURCE_COMTABLE_H_\n'
    c +=composeLogo('IBACOM - command list')

    c += '\n\n#if defined (__linux__)\n\t'

    mg = max(len(value.vars.keys()) for value in packmap.generic.values())
    mn = max(len(value.vars.keys()) for value in packmap.network.values())
    ms = max(len(value.vars.keys()) for value in packmap.server.values())

    c += '#define TOTAL_IBACOM {}\n\t#define MAX_PACKET_ENTRIES {}\n#elif defined (ESP8266)\n\t'.format(int(
        len(packmap.generic.keys()) +
        len(packmap.server.keys()) +
        len(packmap.network.keys())),int(
        max((mg, mn, ms))
    ))

    c += '#define TOTAL_IBACOM {}\n\t#define MAX_PACKET_ENTRIES {}\n#else\n\t'.format(int(
        len(packmap.generic.keys()) +
        len(packmap.network.keys())),
        max((mg,mn))
    )
    c += '#define TOTAL_IBACOM {}\n\t#define MAX_PACKET_ENTRIES {}\n#endif'.format(
        len(packmap.generic.keys()), mg)

    # MAX_PACKET_ENTRIES

    p = '\n\n# Automatically generated code by application/codesource/generator.py'
    p +=composeLogo('IBACOM - command numbers    list', 'P')
    p += '\nTOTAL_IBACOM = {}\n\n'.format(len(packmap.generic.keys()) + len(packmap.network.keys()))

    temp = -1
    for key, item in packmap.generic.items():

        for m in packet_comments:
            if temp < m[0] <= item.command:
                code = '\n\n\t/*** [ {0:<5} - '.format(str(m[0]).zfill(5)) + \
                       '{0:<5} ] : '.format(str(m[1]).zfill(5)) + \
                       '{0:<15} ***/\n'.format(m[2])
                p += code.replace('/***','# --- #').replace('***/','# --- #')
                c += code

        temp = item.command

        c += '\n#define\t{0:<35}'.format(key) +' '+ str(item.command)
        p += '\n{0:<35}'.format(key) +' = '+ str(item.command)

    p += '\n\n\t# --- # [ {0:<5} - '.format(str(packet_comments[-2][0]).zfill(5)) + \
         '{0:<5} ] : '.format(str(packet_comments[-2][1]).zfill(5)) + \
         '{0:<15} # --- #\n'.format(packet_comments[-2][2])

    c += '\n\n\t/*** [ {0:<5} - '.format(str(packet_comments[-2][0]).zfill(5)) + \
         '{0:<5} ] : '.format(str(packet_comments[-2][1]).zfill(5)) + \
         '{0:<15} ***/\n'.format(packet_comments[-2][2])

    for key, item in packmap.network.items():

        c += '\n#define\t{0:<35}'.format(key) +' '+ str(item.command)
        p += '\n{0:<35}'.format(key) +' = '+ str(item.command)


    p += '\n\n\t# --- # [ {0:<5} - '.format(str(packet_comments[-1][0]).zfill(5)) + \
         '{0:<5} ] : '.format(str(packet_comments[-1][1]).zfill(5)) + \
         '{0:<15} # --- #\n'.format(packet_comments[-1][2])

    c += '\n\n\t/*** [ {0:<5} - '.format(str(packet_comments[-1][0]).zfill(5)) + \
         '{0:<5} ] : '.format(str(packet_comments[-1][1]).zfill(5)) + \
         '{0:<15} ***/\n'.format(packet_comments[-1][2])

    for key, item in packmap.server.items():

        c += '\n#define\t{0:<35}'.format(key) +' '+ str(item.command)
        p += '\n{0:<35}'.format(key) +' = '+ str(item.command)

    c += '\n\n\n#endif  /* SOURCE_COMTABLE_H_ */\n\n\n'
    return c,p

def genPGMstructs():

    varmap = CodeSourceVariables()
    packmap = CodeSourcePacket()

    code = composeIntro('packetmap.h')
    code += '\n\n#ifndef PACKETPGM_H_\n#define PACKETPGM_H_\n\n'

    code += '\n#if defined (__linux__)\n\t#include "../common/comtable.h"\n\t#include "../common/definitions.h"\n\t'
    code += '#define VARMAP_NUMBER {}\n#else\n\t#include "comtable.h"\n\t#include "definitions.h"\n\t'.format(
        len(varmap.generic.keys()) +
        len(varmap.server.keys()) +
        len(varmap.network.keys()))
    code += '#if defined (ESP8266)\n\t\t'
    code += '#define VARMAP_NUMBER {}\n\t#else\n\t\t'.format(
        len(varmap.generic.keys()) +
        len(varmap.network.keys()))
    code += '#define VARMAP_NUMBER {}'.format(len(varmap.generic.keys()))
    code += '\n\t#endif /* ARDUINO */\n#endif /* __linux__ */\n\n'

    code += composeLogo('mapped variables')
    code += '\n/*\n * possible types for variable_fst_ are:\n' \
            ' * c = char\n' \
            ' * s = uint8_t\n' \
            ' * u = uint32_t\n' \
            ' * n = int32_t\n' \
            ' * h = uint8_t * (hashes)\n*/\n'

    code += '\nnamespace packet\n{'
    code += '\n\tstruct variable_fst_\n\t{'
    code += '\n\t\t{0:15}'.format('char') + '{}'.format('type') + ';'
    code += '\n\t\t{0:15}'.format('iSize_t') + '{}'.format('size') + ';'
    code += '\n\t\t{0:15}'.format('char') + '{}'.format('name') + '[LEN_FIELD];\n\t};\n'

    code += '\n\tstruct packet_fst_\n\t{'
    code += '\n\t\t{0:15}'.format('iCom_t') + '{0:6}'.format('comm') + ';'
    code += '\n\t\t{0:15}'.format('uint8_t') + '{0:6}'.format('count') + ';'
    code += '\n\t\t{0:15}'.format('char') + '{0:6}'.format('label') + '[LEN_LABEL];'
    code += '\n\t\t{0:15}'.format('uint8_t') + '{0:6}'.format('params') + '[MAX_PACKET_ENTRIES];\n\t};\n'

    code += '\n\tconst variable_fst_ VariableMap[VARMAP_NUMBER] PROGMEM = {\n'

    def sub_gen_varmap(current, pmap, off):

        for shift, variable in enumerate(pmap.items()):

            if variable[1].size == 'sizeof(uint8_t)':
                t = 's'
            elif variable[1].size == 'sizeof(uint32_t)':
                t = 'u'
            elif variable[1].size == 'sizeof(int32_t)':
                t = 'n'
            elif variable[1].size in ('SHA_DIGEST_LENGTH', 'LEN_AES_MASTERKEY'):
                t=  'h'
            else:
                t = 'c'

            current += '\n\t\t{ ' + '\'{}\','.format(t)
            current += '\t{0:22},'.format(variable[1].size)
            current += ' {0:22} '.format( '\"' + variable[1].name + '\"') + '},'
            current += '\t/* {0:3} */'.format(shift  + off)

        return current

    code = sub_gen_varmap(code, varmap.generic, 0)

    code += '\n\n#if defined (ESP8266) || defined (__linux__)\n'
    code = sub_gen_varmap(code, varmap.network, len(varmap.generic.keys()))
    # code = code[::-1].replace(',', '', 1)[::-1]
    code += '\n\n#endif /* ESP8266 or __linux__ */\n'

    code += '\n#if defined (__linux__)\n'
    code = sub_gen_varmap(code, varmap.server, len(varmap.generic.keys()) + len(varmap.network.keys()))
    # code = code[::-1].replace(',', '', 1)[::-1]
    code += '\n\n#endif /* __linux__ */\n'


    code += '\n\t};\n'
    # code += composeLogo('mapped packet')

    code += '\n\n\tconst packet_fst_ PacketMap[TOTAL_IBACOM] PROGMEM = {'

    def sub_gen_packmap(current_code, pmap, var, x):

        command = var[1].command
        name = var[1].name
        label = var[1].label
        fields = dict(var[1].vars)

        logo = 'IBACOM - {} - {} - {}'.format(str(command).zfill(3), var[1].ibacomstring(), label)
        current_code += '\n\n' + composeLogo(logo) + '\t{\n\t\t'
        current_code += 'IBACOM_{}, {},\n\t\t\"{}\",'.format(name.upper(), len(fields), label)

        labels=list()

        def sub_gen_variables(current, field):
            labels.clear()
            for variable in field.keys():

                for count, generic in enumerate(varmap.generic.keys()):
                    if variable == generic:
                        current += '{}, '.format(count)
                        labels.append(str(generic))
                for count, generic in enumerate(varmap.network.keys()):
                    if variable == generic:
                        current += '{}, '.format(count + len(varmap.generic.keys()))
                        labels.append(str(generic))

                for count, generic in enumerate(varmap.server.keys()):
                    if variable == generic:
                        current += '{}, '.format(count + len(varmap.generic.keys()) + len(varmap.network.keys()))
                        labels.append(str(generic))

            return current

        current_code += ' {'
        if len(fields.keys()) > 0:
            current_code += ' '

            current_code = sub_gen_variables(current_code, fields)

        if current_code.endswith(', '):
            current_code = current_code[::-1].replace(',','',1)[::-1]
        current_code += '}'

        if len(labels):
            current_code += ' // {}'.format(labels[0])
            for l in labels[1::]:
                current_code += ', {}'.format(l)

        current_code += '\n\t},'
        # if x < len(pmap.keys()) - 1:
        #     current_code += ','
        return current_code


    for offset, packet in enumerate(packmap.generic.items()):
        buffer = ''
        buffer = sub_gen_packmap(buffer, varmap.generic, packet, offset)
        code += buffer.replace('\n', '\n\t')

    code += '\n\n#if defined (ESP8266) || defined (__linux__)'

    for offset, packet in enumerate(packmap.network.items()):
        buffer = ''
        buffer = sub_gen_packmap(buffer, varmap.network, packet, offset)
        code += buffer.replace('\n', '\n\t')

    # code = code[::-1].replace(',', '', 1)[::-1]
    code += '\n\n#endif /* ESP8266 or __linux__ */\n'

    code += '\n#if defined (__linux__)'

    for offset, packet in enumerate(packmap.server.items()):
        buffer = ''
        buffer = sub_gen_packmap(buffer, varmap.server, packet, offset)
        code += buffer.replace('\n', '\n\t')

    # code = code[::-1].replace(',', '', 1)[::-1]
    code += '\n\n#endif /* __linux__ */\n'


    code  += '\n\t};\n}\n\n'
    code +='#endif  /* PACKETPGM_H_ */'
    return code

def genPacketStubs():

    head = ''

    varmap = CodeSourceVariables()
    packmap = CodeSourcePacket()

    code = composeIntro('packetStub.cpp')
    code += '\n\n#ifndef PACKETSTUB_H_\n#define PACKETSTUB_H_\n\n#include "ipacket.h"\n'
    code += '\nvoid error_packet_call (char * label)\n{\n\terror_mem("FATAL: invalid call for packet %s!", label);\n}\n'

    code += composeLogo('payload shortcuts to variables')
    code += '\nnamespace packet\n{\n'

    def sub_stubs(current, h_current, pmap):

        def sub_pointers(current_code, sheader, itemkey, cmap):

            for k, p in cmap.items():

                if itemkey in p.vars.keys():
                    m = ''
                    for w, v in p.vars.items():
                        if w == itemkey:
                            break
                        m += ' + {}'.format(v.size)
                    current_code += '\n\t\tcase {} :\n\t\t\treturn ({} *) (payload()'.format(p.ibacomstring(), sheader) + m + ');'
            return current_code

        for key, var in pmap.items():

            builtin_var_list = ('sizeof(int8_t)',
                                'sizeof(uint8_t)',
                                'sizeof(int16_t)',
                                'sizeof(uint16_t)',
                                'sizeof(int32_t)',
                                'sizeof(uint32_t)')

            if var.size in builtin_var_list:
                h0 = var.size.split('(')[1].strip(')')
            else:
                h0 = 'char'

            subcode = '\t{0:<10} * ipacket::'.format(h0) + 'p_{0:<16}'.format(key) + \
                      '(void)\n\t{\n\t\tswitch(command())\n\t\t{'
            h_current += '\t\t{0:<11}*    '.format(h0) + 'p_{0:<14}'.format(key) + '(void);\n'

            subcode = sub_pointers(subcode, h0, key, packmap.generic)
            subcode = sub_pointers(subcode, h0, key, packmap.network)
            subcode = sub_pointers(subcode, h0, key, packmap.server)

            subcode += '\n\t\tdefault:\n\t\t\terror_packet_call(label());\n\t\t\treturn nullptr;'
            subcode += '\n\t\t}\n\t}\n\n'
            current += subcode

        return current, h_current

    code, head = sub_stubs(code, head, varmap.generic)

    code += '#if defined(INDOORINO_NETWORK)\n\n'
    head += '\n#if defined(INDOORINO_NETWORK)\n\n'
    code, head = sub_stubs(code, head, varmap.network)
    code += '#endif /* INDOORINO_NETWORK */\n'
    head += '\n#endif // INDOORINO_NETWORK \n'


    code += '\n#if defined (__linux__)\n\n'
    head += '\n#if defined (__linux__)\n\n'
    code, head = sub_stubs(code, head, varmap.server)
    code += '#endif /* __linux__ */\n\n'
    head += '\n#endif // __linux__\n'


    code +='} /* packet */\n\n' \
           '#endif  /* PACKETSTUB_H_ */\n\n/*\nAdd the following headers to the <ipacket> class defintion\n\n'
    code += head
    code += '\n */'
    return code

def genEvals(*file):

    code=composeIntro('packetEval.cpp')
    code += '\n\n#include "indoorino.h"\n\nvoid\t\tipacket::eval\t\t(void)\n{'
    code += '\n\tdebug(label());\n\tswitch (command())\n\t{\n'
    head = composeLogo("").split('\n')
    code += head[0] + '\n'
    code += head[1] + '\n'
    code += head[2] + '\n'

    packmap = CodeSourcePacket()

    def sub_eval( pmap, cases=True):

        current=''
        for packet in pmap.items():
            command = packet[1].command
            name = packet[1].name
            label = packet[1].label

            logo = 'EVAL: {} - {} - {}'.format(str(command).zfill(3), packet[1].ibacomstring(), label)
            current += composeLogo(logo)

            current += '\n\t\tcase IBACOM_{}:'.format(name.upper())
            current += '\n\t\t{'
            if cases:
                current += '\n\t\t#if defined (ARDUINO)\n\t\t#else\n\t\t#endif'
            current += '\n\t\t\tbreak;\n\t\t}\n\n'

        return current

    code += sub_eval(packmap.generic)
    code += '#if defined (INDOORINO_NETWORK)\n'
    code += sub_eval(packmap.network, cases=True)
    code += '#endif /* INDOORINO_NETWORK */\n'

    code += '#if defined (__linux__)\n'
    code += sub_eval(packmap.server, cases=False)
    code += '#endif /* __linux__ */\n'

    code += composeLogo('EVAL: DEFAULT')
    code +='\n\t\tdefault:\n\t\t{\n\t\t\treturn;\n\t\t}\n'
    code +='\t}\n}'


    return(code)



if __name__ == '__main__':

    TABLE=1
    MAP=1
    STUBS=1
    EVALS=0

    SHOW=0

    print('\n\n\t\t ** INDOORINO CODE GENERATOR **\n\n\t\tgenerating file to {}'.format(codepath))

    if SHOW:
        if TABLE:
            path = codepath.joinpath('libraries/packets/comtable.h')
            print('\nWriting {} ...'.format(path))
            print(genComtable()[0])
        if MAP:
            path = codepath.joinpath('libraries/packets/packetmap.h')
            print('\nWriting {} ...'.format(path))
            print(genPGMstructs())
        if STUBS:
            path = codepath.joinpath('libraries/packets/packetstubs.cpp')
            print('\nWriting {} ...'.format(path))
            print(genPacketStubs())
        if EVALS:
            path = codepath.joinpath('libraries/packets/packetEval.template')
            print('\nWriting {} ...'.format(path))
            print(genEvals())

        exit(0)

    else:

        if MAP:
            path = codepath.joinpath('libraries/packets/packetmap.h')
            try:
                with open(path, 'w') as f:
                    print('\nWriting {} ...'.format(path), end='')
                    f.write(genPGMstructs())
                    f.close()
                print('done!')
            except FileNotFoundError:
                print('WARNING! CodeGen: could not write {}'.format(path))
        if TABLE:
            c,p = genComtable()

            path = codepath.joinpath('libraries/common/comtable.h')
            try:
                with open(path, 'w') as f:
                    print('Writing {} ...'.format(path), end='')
                    f.write(c)
                    f.close()
                print('done!')
            except FileNotFoundError:
                print('WARNING! CodeGen: could not write {}'.format(path))

            path = codepath.joinpath('application/common/comtable.py')
            try:
                with open(path, 'w') as f:
                    print('Writing {} ...'.format(path), end='')
                    f.write(p)
                    f.close()
                print('done!')
            except FileNotFoundError:
                print('WARNING! CodeGen: could not write {}'.format(path))
        if EVALS:
            path = codepath.joinpath('libraries/indoorino/packetEval.template')
            c = genEvals()
            try:
                with open(path, 'w') as f:
                    print('Writing {} ...'.format(path), end='')
                    f.write(c)
                    f.close()
                print('done!')
            except FileNotFoundError:
                print('WARNING! CodeGen: could not write {}'.format(path))
        if STUBS:
            path = codepath.joinpath('libraries/packets/packetstubs.cpp')
            try:
                with open(path, 'w') as f:
                    print('Writing {} ...'.format(path), end='')
                    f.write(genPacketStubs())
                    f.close()
                print('done!')
            except FileNotFoundError:
                print('WARNING! CodeGen: could not write {}'.format(path))

        print('\nlibraries updated - all done!')