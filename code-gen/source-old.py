
import pathlib

#_______________________________________________________________#
#                                                               #
#           VARIABLE MAP                                        #
#_______________________________________________________________#
#

variable_source_standard = (
    ( 'message',        'SERIAL_TX_BUFFER_SIZE',      'isPrint',                    '0',                            '0'),
    ( 'command',        'SERIAL_TX_BUFFER_SIZE',      'isPrint',                    '0',                            '0'),
    ( 'name',           'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'type',           'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'board',          'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'devnum',         'sizeof(uint8_t)',            '',                           '0',                            'MAX_ATTACHED_DEVICES'),
    ( 'devname',        'LEN_DEVNAME',                'isAlpha',                    '0',                            '0'),
    ( 'newname',        'LEN_DEVNAME',                'isAlpha',                    '0',                            '0'),
    ( 'desc1',          'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'desc2',          'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'desc3',          'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'desc4',          'LEN_NAME',                   'isAlpha',                    '0',                            '0'),
    ( 'epoch',          'sizeof(uint32_t)',           '',                           'SECONDS_FROM_1970_TO_2010',    'SECONDS_FROM_1970_TO_2100'),
    ( 'freemem',        'sizeof(uint32_t)',           '',                           '0',                            '0'),
    ( 'looptime',       'sizeof(uint32_t)',           '',                           '0',                            '0'),
    ( 'errors1',        'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'errors2',        'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'errors3',        'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'errors4',        'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'totalrx',        'sizeof(uint32_t)',           '',                           '0',                            '0'),
    ( 'totaltx',        'sizeof(uint32_t)',           '',                           '0',                            '0'),
    ( 'value1',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value2',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value3',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value4',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value5',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value6',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value7',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'value8',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param1',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param2',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param3',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param4',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param5',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param6',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param7',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'param8',         'sizeof(int32_t)',            '',                           '0',                            '0'),
    ( 'stepday1',       'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_DAY'),
    ( 'stepday2',       'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_DAY'),
    ( 'stepday3',       'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_DAY'),
    ( 'stepday4',       'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_DAY'),
    ( 'stephour1',      'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_HOUR'),
    ( 'stephour2',      'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_HOUR'),
    ( 'stephour3',      'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_HOUR'),
    ( 'stephour4',      'sizeof(uint32_t)',           '',                           '1',                            'SECONDS_PER_HOUR'),
    ( 'timeout1',       'sizeof(uint32_t)',           '',                           '0',                            'SECONDS_PER_HOUR'),
    ( 'timeout2',       'sizeof(uint32_t)',           '',                           '0',                            'SECONDS_PER_HOUR'),
    ( 'timeout3',       'sizeof(uint32_t)',           '',                           '0',                            'SECONDS_PER_HOUR'),
    ( 'timeout4',       'sizeof(uint32_t)',           '',                           '0',                            'SECONDS_PER_HOUR'),
    ( 'level',          'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'status',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count1',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count2',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count3',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count4',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count5',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count6',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count7',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'count8',         'sizeof(uint8_t)',            '',                           '0',                            '0'),
    ( 'pin1',           'sizeof(uint8_t)',            '',                           '0',                            'NUM_DIGITAL_PINS'),
    ( 'pin2',           'sizeof(uint8_t)',            '',                           '0',                            'NUM_DIGITAL_PINS'),
    ( 'pin3',           'sizeof(uint8_t)',            '',                           '0',                            'NUM_DIGITAL_PINS'),
    ( 'pin4',           'sizeof(uint8_t)',            '',                           '0',                            'NUM_DIGITAL_PINS'),
)

variable_source_network = (
    ( 'hash1',          'SHA_DIGEST_LENGTH',            'isAlpha',                  '0',                          '0'),
    ( 'hash2',          'SHA_DIGEST_LENGTH',            'isAlpha',                  '0',                          '0'),
    ( 'hash3',          'SHA_DIGEST_LENGTH',            'isAlpha',                  '0',                          '0'),
    ( 'hash4',          'SHA_DIGEST_LENGTH',            'isAlpha',                  '0',                          '0'),
    ( 'tcprx',          'sizeof(uint32_t)',             '',                         '0',                          '0'),
    ( 'tcptx',          'sizeof(uint32_t)',             '',                         '0',                          '0'),
    ( 'serialrx',       'sizeof(uint32_t)',             '',                         '0',                          '0'),
    ( 'serialtx',       'sizeof(uint32_t)',             '',                         '0',                          '0'),
    ( 'ip1',            'INET_ADDRSTRLEN',              'IPAddress().isValid',      '0',                          '0'),
    ( 'ip2',            'INET_ADDRSTRLEN',              'IPAddress().isValid',      '0',                          '0'),
    ( 'ip3',            'INET_ADDRSTRLEN',              'IPAddress().isValid',      '0',                          '0'),
    ( 'ip4',            'INET_ADDRSTRLEN',              'IPAddress().isValid',      '0',                          '0'),
    ( 'port1',          'sizeof(uint32_t)',             '',                         'PORT_SOCKET_FIRST',          'PORT_SOCKET_LAST'),
    ( 'port2',          'sizeof(uint32_t)',             '',                         'PORT_SOCKET_FIRST',          'PORT_SOCKET_LAST'),
    ( 'port3',          'sizeof(uint32_t)',             '',                         'PORT_SOCKET_FIRST',          'PORT_SOCKET_LAST'),
    ( 'port4',          'sizeof(uint32_t)',             '',                         'PORT_SOCKET_FIRST',          'PORT_SOCKET_LAST'),
    ( 'aeskey1',        'LEN_AES_MASTERKEY',            '',                         '',                           ''),
    ( 'aeskey2',        'LEN_AES_MASTERKEY',            '',                         '',                           ''),
    # ( 'aesiv1',         'LEN_AES_MASTERKEY',            '',                         '',                           ''),
    # ( 'aesiv2',         'LEN_AES_MASTERKEY',            '',                         '',                           ''),
)

variable_source_server = (
    # ( 'whole',          'MAX_PAYLOAD_SIZE',             '',                         '0',                          '0'),
    ( 'table1',         'SERIAL_TX_BUFFER_SIZE',        '',                         '0',                          '0'),
    ( 'table2',         'SERIAL_TX_BUFFER_SIZE',        '',                         '0',                          '0'),
    ( 'table3',         'SERIAL_TX_BUFFER_SIZE',        '',                         '0',                          '0'),
    ( 'table4',         'SERIAL_TX_BUFFER_SIZE',        '',                         '0',                          '0'),

)
#_______________________________________________________________#
#                                                               #
#           PACKET MAP                                          #
#_______________________________________________________________#
#

# type update:
# first is hashable (1 bit) <true:false>
# second is type (3bits)
#   - 1 = command
#   - 2 = update
#   - 3 = config
#   - 4 = message

packet_source_standard = (

    # ---#    [0000   - 00500]   :   boards commands

    ( 0,        'none',             'empty packet',         (False, 1), ()),
    ( 1,        'init',             'init byte',            (False, 1), ()),
    ( 2,        'reset',            'reset command',        (True, 1),  ()),
    ( 3,        'request_config',   'config request',       (False, 1), ()),
    ( 4,        'request_status',   'status request',       (False, 1), ()),
    ( 5,        'command_string',   'command string',       (True, 1),  ('command',)),
    ( 6,        'factory_reset',    'factory reset',        (True, 1),  ()),
    ( 10,       'unix_epoch',       'timestamp',            (False, 1), ('epoch',)),
    ( 15,       'report',           'generic report',       (True, 1),  ('message','name','epoch','level',)),


    ( 301,      'set_sd',           'sd card setting',      (True, 1),  ('command',)),
    ( 302,      'request_sddrop',   'drop request',         (True, 1),  ()),
    ( 303,      'request_sdclear',  'clear request sd',     (True, 1),  ()),
    ( 304,      'request_sdwipe',   'wipe request sd',      (True, 1),  ()),

    # ---#    [00501   - 01000]   :   devices commands

    ( 501,      'set_device',       'device setting',       (True, 1),  ('name','devname','command','value1')),
    ( 502,      'set_devname',      'set devname',          (True, 1),  ('name','devname','newname',)),
    ( 503,      'set_devpin',       'set devpin',           (True, 1),  ('name','devname','pin1',)),
    ( 504,      'rem_device',       'rem device',           (True, 1),  ('name','devname',)),

    # ---#    [01001   - 01300]   :   boards config

    ( 1001,     'conf_std',         'standard conf',        (True, 1),  ('name','type','board','devnum',)),

    ( 1005,     'conf_sampler',     'sampler conf',         (False, 1), ('name','stepday1','stephour1',)),                                 # probe timestep & cooling time


    # ---#    [01301   - 02000]   :   devices config

    ( 1301,     'conf_asensor',     'analog sensor conf',   (False, 1), ('name', 'devname','pin1','param1','param2',)),
    ( 1302,     'conf_switch',      'switch conf',          (False, 1), ('name', 'devname','pin1')),
    ( 1310,     'conf_ldr',         'ldr sensor conf',      (False, 1), ('name', 'devname','pin1','param1','param2',)),
    ( 1522,     'conf_DHT22',       'DHT22 sensor conf',    (False, 1), ('name', 'devname','pin1','param1','param2','param3','param4',)),
    ( 1700,     'conf_relay',       'relay conf',           (False, 1), ('name', 'devname','pin1')),
    ( 2000,     'conf_devstd',      'device generic conf',  (False, 1), ('name', 'devname','pin1')),

    #---#    [02001   - 02300]   :   boards status

    ( 2001,     'status_std',       'standard status',      ('name', 'epoch','looptime','freemem',
                                                             'errors1','errors2','errors3','errors4')),                 # invalid packets, low ram, mem fatals
    ( 2005,     'status_sampler',   'sampler status',       ('name', 'stepday1','value1',)),                                    # nextprobe & total probes
    ( 2270,     'status_sd',        'sd statistics',        ('name', 'devname','pin1','param1','param2',)),                        # size & free

    #---#    [02301   - 03000]   :   devices status

    ( 2301,     'status_asensor',   'analog sensor stat',   ('name', 'devname','value1','status',)),
    ( 2302,     'status_switch',    'switch sensor stat',   ('name', 'devname','value1','status',)),
    ( 2310,     'status_ldr',       'ldr sensor stat',      ('name', 'devname','value1','status',)),
    ( 2522,     'status_DHT22',     'DHT22 sensor stat',    ('name', 'devname','value1','value2','status',)),
    ( 2700,     'status_relay',     'relay stat',           ('name', 'devname','status','level',)),
    ( 3000,     'status_devstd',    'device generic stat',  ('name', 'devname','status',)),

    #---#    [03001   - 03200]   :   alarms

    ( 3001,     'allarm',           'allarm',               ('board','type','epoch','level',)),
    ( 3002,     'warnings',         'general warning',      ('board','type','epoch','level',)),

    #---#    [03201   - 03500]   :   requests

    ( 3205,     'request_probe',    'probe request',        ()),

    #---#    [03501   - 04000]   :   payloads

    ( 3505,     'probe_ambient',    'ambient probe',        ('board', 'devname', 'epoch',
                                                             'desc1', 'desc2', 'desc3', 'desc4',
                                                             'value1','value2','value3','value4')),
)

packet_source_network = (

    #---#   [05001    - 07000]   :  net packets

    ( 5001,     'conf_esp',         'esp conf',             ('ip1', 'ip2', 'port1', 'port2', 'aeskey1',
                                                             'timeout1', 'timeout2', 'timeout3', 'timeout4',            # timeout client, packet resend delay,
                                                             'count1', 'count2', 'count3', 'count4')),                  # packet attemps
    ( 5002,     'conf_aes',         'aes conf',             ('command', 'aeskey1', 'aeskey2')),
    ( 5003,     'conf_net',         'network conf',         ('command', 'ip1', 'port1', 'timeout1', 'timeout2')),
    ( 5025,     'ack',              'acknowledge',          ('hash1',)),
    ( 5050,     'net_address',      'network address',      ('command', 'ip1', 'port1')),
    ( 6001,     'status_esp',       'esp status',           ('serialtx', 'serialrx',
                                                             'tcptx', 'tcprx',
                                                             'totaltx', 'totalrx',
                                                             'errors1','errors2','errors3','errors4')),                 # missing ack, unsent packets, disconnections, net overflows
)

packet_source_server = (

    #---#   [07001    - 08000]   :  client packets

    ( 7001,     'req_login',         'login request',       ('name', 'command')),
    ( 7002,     'session_stat',      'session status',      ('name', 'status', 'epoch')),
    ( 7003,     'session_end',       'session end',         ()),
    ( 7010,     'srv_req',           'server request',      ('command',)),

    ( 7012,     'srv_conf',          'server config',       ('ip1', 'port1', 'port2', 'status', 'table1')),
    # ( 7020,     'srv_board_list',    'board list',          ('whole',)),
    ( 7021,     'srv_board',         'board data',          ('name','type','board','devnum','epoch','ip1','port1',)),
    ( 7022,     'srv_board_conn',    'board connection',    ('name','status',)),

    # ( 7575,     'pack_route',        'route packet',        ('whole',)),

)

packet_source_global = packet_source_standard + packet_source_network + packet_source_server

codepath = pathlib.Path(str(pathlib.Path(__file__).absolute()).split('code-gen')[0])

if __name__ == '__main__':
    for item in packet_source_standard + packet_source_network + packet_source_server:
        if len(item[3]) > 0:
            print('IBACOM_{} : '.format(item[1]).upper() + '{')
            for j in item[3]:
                print('\t\'{0:<16}'.format(j + '\'') + ' : ( \'{}\'),'.format(j))
            print('},')
