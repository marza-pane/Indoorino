1)  SOME PACKET'S PAYLOAD CRASH SERVER WHEN SENT:
    Creating new packets's payload will crash the server.
    Current solution: change packet payload in /application/codegen/source.py
    Hints:
        NO  ( 7153, 'lgt_timer_stat', 'light timer status', ('board', 'devname', 'desc1', 'level', 'status', 'value1')) # 70:128
        NO  ( 7153, 'lgt_timer_stat', 'light timer status', ('board', 'devname', 'desc1', 'level', 'status', 'value1', 'value2')) # 74:132
        YES ( 7153, 'lgt_timer_stat', 'light timer status', ('board', 'devname', 'desc1', 'level', 'status', 'value1', 'value2', 'value3')), # 78:136
        
        
2) ERROR AT STARTUP:
    Once in a while at services init thread it aborts.
    Maybe when booting with devices waiting for connection ?
    - Added mutex when reading System.services._threadlist
    Current solution: none
    Hints:
        2021 Sep 08 02.07.07:ALERT:OS:SERVICES: starting services...
        2021 Sep 08 02.07.07:INFO:OS:SERVICE:reading layout...
        2021 Sep 08 02.07.07:ALERT:OS:SERVICE:ALARM:begin KITCHEN.FIRE.ALARM @ [home:kitchen]
        2021 Sep 08 02.07.07:INFO:OS:SERVICE:ALARM:FIRE: KITCHEN.FIRE.ALARM @ [home:kitchen] thread start!
        2021 Sep 08 02.07.07:ALERT:OS:SERVICE:ALARM:begin ATTIC.FIRE.ALARM @ [home:attic]
        2021 Sep 08 02.07.07:INFO:OS:SERVICE:ALARM:FIRE: ATTIC.FIRE.ALARM @ [home:attic] thread start!
        terminate called after throwing an instance of 'std::bad_alloc'
        what():  std::bad_alloc
        Aborted

        2021 Sep 08 02.10.13:ALERT:OS:SERVICES: starting services...
        2021 Sep 08 02.10.13:INFO:OS:SERVICE:reading layout...
        2021 Sep 08 02.10.13:ALERT:OS:SERVICE:ALARM:begin KITCHEN.FIRE.ALARM @ [home:kitchen]
        2021 Sep 08 02.10.13:INFO:OS:SERVICE:ALARM:FIRE: KITCHEN.FIRE.ALARM @ [home:kitchen] thread start!
        2021 Sep 08 02.10.13:ALERT:OS:SERVICE:ALARM:begin ATTIC.FIRE.ALARM @ [home:attic]
        2021 Sep 08 02.10.13:INFO:OS:SERVICE:ALARM:FIRE: ATTIC.FIRE.ALARM @ [home:attic] thread start!
        2021 Sep 08 02.10.13:ALERT:OS:SERVICE:ALARM:begin PRESCOTT.FLOOD.ALARM @ [home:heating]
        2021 Sep 08 02.10.13:INFO:OS:SERVICE:ALARM:FLOOD: PRESCOTT.FLOOD.ALARM @ [home:heating] thread start!
        terminate called after throwing an instance of 'std::bad_alloc'
        what():  std::bad_alloc2021 Sep 08 02.10.13:ALERT:OS:SERVICE:LIGHTS:begin PERIMETER.BEAMS @ [perimeter:entrance]
        2021 Sep 08 02.10.13:INFO:OS:SERVICE:LIGHTS:OUTDOOR: PERIMETER.BEAMS @ [perimeter:entrance] thread start!
        Aborted

        2021 Sep 08 21.54.12:INFO:OS:SERVICE:reading layout...
        2021 Sep 08 21.54.12:ALERT:OS:SERVICE:ALARM:begin KITCHEN.FIRE.ALARM @ [home:kitchen]
        2021 Sep 08 21.54.12:INFO:OS:SERVICE:ALARM:FIRE: KITCHEN.FIRE.ALARM @ [home:kitchen] thread start!
        2021 Sep 08 21.54.12:ALERT:OS:SERVICE:ALARM:begin ATTIC.FIRE.ALARM @ [home:attic]
        2021 Sep 08 21.54.12:INFO:OS:SERVICE:ALARM:FIRE: ATTIC.FIRE.ALARM @ [home:attic] thread start!
        terminate called after throwing an instance of 'std::bad_alloc'
        what():  std::bad_alloc
        Aborted
        
        2021 Sep 09 00.59.08:ALERT:OS:SERVICE:ALARM:begin PRESCOTT.FLOOD.ALARM @ [home:heating]
        2021 Sep 09 00.59.08:INFO:OS:SERVICE:ALARM:FLOOD: PRESCOTT.FLOOD.ALARM @ [home:heating] thread start!
        2021 Sep 09 00.59.08:ALERT:OS:SERVICE:LIGHTS:begin PERIMETER.BEAMS @ [perimeter:entrance]
        Segmentation fault
