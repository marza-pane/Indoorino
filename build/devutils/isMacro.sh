
grep -rwn /home/n00b/.arduino15/ -e $1 | grep '#define'
grep -rwn /opt/arduino-1.8.12/ -e $1 | grep '#define'