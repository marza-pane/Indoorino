
PATH0="/home/n00b/.arduino15/"
PATH1="/opt/arduino-1.8.12/"
PATH2="/usr/include/"
PATH3="$(pwd | sed 's#\/build##g')"

echo -e "\n\t ** Checking $PATH0 **"
grep -rwn $PATH0 -e $1

echo -e "\n\t ** Checking $PATH1 **"
grep -rwn $PATH1 -e $1

echo -e "\n\t ** Checking $PATH2 **"
grep -rwn $PATH2 -e $1

echo -e "\n\t ** Checking $PATH3 **"
grep -rwn $PATH3 -e $1


echo $PATH3
