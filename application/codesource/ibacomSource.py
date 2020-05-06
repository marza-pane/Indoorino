
import pathlib

SRC_variables = (
    ( 'message',        'SERIAL_TX_BUFFER_SIZE',      'isPrint',                    '0',                          '0'),
    ( 'command',        'SERIAL_TX_BUFFER_SIZE',      'isPrint',                    '0',                          '0'),
    ( 'name',           'LEN_NAME',                   'isAlpha',                    '0',                          '0'),
    ( 'type',           'LEN_NAME',                   'isAlpha',                    '0',                          '0'),
    ( 'board',          'LEN_NAME',                   'isAlpha',                    '0',                          '0'),
    ( 'devname',        'LEN_DEVNAME',                'isAlpha',                    '0',                          '0'),
    ( 'epoch',          'sizeof(uint32_t)',           '',                           'SECONDS_FROM_1970_TO_2010',  'SECONDS_FROM_1970_TO_2100'),
    ( 'value',          'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'freemem',        'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'looptime',       'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'errors',         'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'totalrx',        'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'totaltx',        'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'serialrx',       'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'serialtx',       'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'param1',         'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'param2',         'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'param3',         'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'param4',         'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'temperature',    'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'humidity',       'sizeof(uint32_t)',           '',                           '0',                          '0'),
    ( 'stepday1',       'sizeof(uint32_t)',           '',                           '1',                          'SECONDS_PER_DAY'),
    ( 'stephour1',      'sizeof(uint32_t)',           '',                           '1',                          'SECONDS_PER_HOUR'),
    ( 'timeout',        'sizeof(uint32_t)',           '',                           '0',                          'SECONDS_PER_HOUR'),
    ( 'port',           'sizeof(uint32_t)',           '',                           'PORT_SOCKET_FIRST',          'PORT_SOCKET_LAST'),
    ( 'level',          'sizeof(uint8_t)',            '',                           '0',                          '0'),
    ( 'status',         'sizeof(uint8_t)',            '',                           '0',                          '0'),
    ( 'devnum',         'sizeof(uint8_t)',            '',                           '0',                          'MAX_ATTACHED_DEVICES'),
    ( 'pin',            'sizeof(uint8_t)',            '',                           '0',                          'NUM_DIGITAL_PINS'),
    ( 'ip',             'LEN_IPADDRESS',              'isPrint',                    '0',                          '0'),

)

SRC_packets = (
    ( 0,        'none',             'empty packet',         ()),
    ( 1,        'init',             'init byte',            ()),
    ( 2,        'reset',            'reset command',        ()),
    ( 3,        'request_config',   'config request',       ()),
    ( 4,        'request_status',   'status request',       ()),
    ( 5,        'command_string',   'command string',       ('command',)),
    ( 10,       'unix_epoch',       'timestamp',            ('epoch',)),
    ( 15,       'report',           'generic report',       ('message','name','epoch','level',)),
    ( 25,       'ack',              'acknowledge',          ('value',)),
    ( 30,       'reset_config',     'factory reset',        ()),

    #---#    [00301   - 00500]   :   boards commands

    ( 301,      'set_sd',           'sd card setting',      ('command',)),
    # ( 301,      'request_sddrop',   'drop request',         ()),
    # ( 302,      'request_sdclear',  'clear request sd',     ()),
    # ( 303,      'request_sdwipe',   'wipe request sd',      ()),

    # ---#    [00501   - 01000]   :   devices commands

    ( 501,      'set_device',       'device setting',       ('command','devname','pin','level')),
    # ( 501,      'set_devname',      'set devname',          ('devname','name',)),
    # ( 502,      'set_devpin',       'set devpin',           ('devname','pin',)),
    # ( 504,      'rem_device',       'rem device',           ('devname',)),
    # ( 600,      'actuator_set',     'set actuator',         ('devname','level',)),

    # ---#    [01001   - 01300]   :   boards config

    ( 1001,     'conf_std',         'standard conf',        ('name','type','board','devnum',)),
    ( 1003,     'conf_esp',         'esp conf',             ('ip', 'port', 'timeout', 'status')),
    ( 1005,     'conf_sampler',     'sampler conf',         ('stepday1','stephour1',)),


    # ---#    [01301   - 02000]   :   devices config

    ( 1301,     'conf_asensor',     'analog sensor conf',   ('devname','pin','param1','param2',)),
    ( 1302,     'conf_switch',      'switch conf',          ('devname','pin')),
    ( 1310,     'conf_ldr',         'ldr sensor conf',      ('devname','pin','param1','param2',)),
    ( 1522,     'conf_DHT22',       'DHT22 sensor conf',    ('devname','pin','param1','param2','param3','param4',)),
    ( 1700,     'conf_relay',       'relay conf',           ('devname','pin','status')),
    ( 2000,     'conf_devstd',      'device generic conf',  ('devname','pin',)),

    #---#    [02001   - 02300]   :   devices status

    ( 2001,     'status_std',       'standard status',      ('name','epoch','looptime','freemem','errors')),
    ( 2003,     'status_esp',       'esp status',           ('value', 'totaltx','totalrx','serialtx', 'serialrx')),
    ( 2005,     'status_sampler',   'sampler status',       ('name','stepday1','value',)),  # nextprobe & total probes

    #---#    [02301   - 03000]   :   devices status

    ( 2301,     'status_asensor',   'analog sensor stat',   ('devname','value','status',)),
    ( 2302,     'status_switch',    'switch sensor stat',   ('devname','value','status',)),
    ( 2310,     'status_ldr',       'ldr sensor stat',      ('devname','value','status',)),
    ( 2522,     'status_DHT22',     'DHT22 sensor stat',    ('devname','temperature','humidity','status',)),
    ( 2522,     'status_DHT22',     'DHT22 sensor stat',    ('devname','temperature','humidity','status',)),
    ( 2700,     'status_relay',     'relay stat',           ('devname','level',)),
    ( 2880,     'status_sd',        'sd statistics',        ('name','pin','param1','freemem',)), # size & free
    ( 3000,     'status_devstd',    'device generic stat',  ('devname','status',)),

    #---#    [03001   - 03200]   :   alarms

    ( 3001,     'allarm',           'allarm',               ('type','board','epoch','level',)),
    ( 3002,     'warnings',         'general warning',      ('type','board','epoch','level',)),
    # ( 3002,     'allarm_flood',     'flood allarm',         ('devname','epoch',)),
    # ( 3003,     'allarm_smoke',     'smoke allarm',         ('devname','epoch',)),
    # ( 3004,     'allarm_fire',      'fire allarm',          ('devname','epoch',)),
    # ( 3005,     'allarm_temp',      'temperature allarm',   ('devname','epoch',)),

    #---#    [03201   - 03500]   :   requests

    ( 3205,     'request_probe',    'probe request',        ()),

    #---#    [03501   - 04000]   :   payloads

    ( 3505,     'probe_ambient',    'ambient probe',        ('devname','type','epoch','value',)),

    #---#   [05001    - 06000]   :  esp commands

    ( 5001,     'set_addr',         'network address set',  ('command', 'devname', 'ip', 'port')),
    ( 5002,     'esp_address',      'esp address',          ('devname', 'ip', 'port')),

    # ( 5505,     'espaddr_add',      'add esp address',      ('devname','ip', 'port',)),
    # ( 65529,    'packetest',     'test packet',             ('name','devname', 'bash','type','epoch','value','ip',
    #                                                          'port','parameter1','parameter2','parameter3','parameter4',
    #                                                          'stepday1','stephour1')),

    ( 65530,    'doorbell',      'doorbell signal',         ('devname',)),
)

packetcomments = (
    (   0,  300, 'system command'),
    ( 301,  500, 'general command'),
    ( 501, 1000, 'devices command'),
    (1001, 1300, 'board config'),
    (1301, 2000, 'device config'),
    (2001, 2300, 'board status'),
    (2301, 3000, 'device status'),
    (3001, 3200, 'alarms'),
    (3201, 3500, 'requests'),
    (3501, 4000, 'payloads'),
    (5001, 6000, 'network'),
)

codepath = pathlib.Path(str(pathlib.Path(__file__).absolute()).split('application')[0])
