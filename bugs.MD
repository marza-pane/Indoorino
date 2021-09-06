1)  SOME PACKET'S PAYLOAD CRASH SERVER WHEN SENT:
    Creating new packets's payload will crash the server.
    Current solution: change packet payload in /application/codegen/source.py
    Hints:
        NO  ( 7153, 'lgt_timer_stat', 'light timer status', ('board', 'devname', 'desc1', 'level', 'status', 'value1')) # 70:128
        NO  ( 7153, 'lgt_timer_stat', 'light timer status', ('board', 'devname', 'desc1', 'level', 'status', 'value1', 'value2')) # 74:132
        YES ( 7153, 'lgt_timer_stat', 'light timer status', ('board', 'devname', 'desc1', 'level', 'status', 'value1', 'value2', 'value3')), # 78:136
        
        

