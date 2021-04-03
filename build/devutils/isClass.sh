PATH0="/home/n00b/.arduino15/"
PATH1="/opt/arduino-1.8.13/"
PATH2="/usr/include/"
PATH3="$(pwd | sed 's#\/build/devutils##g')"

echo -e "\n\t ** Checking $PATH0 **\n"
grep -rn $PATH0 -e $1 | grep -w 'class'

echo -e "\n\t ** Checking $PATH1 **\n"
grep -rn $PATH1 -e $1 | grep -w 'class'

echo -e "\n\t ** Checking $PATH2 **\n"
grep -rn $PATH2 -e $1 | grep -w 'class'

echo -e "\n\t ** Checking $PATH3 **\n"
grep -rn $PATH3 -e $1 | grep -w 'class'

echo
echo

