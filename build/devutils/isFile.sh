PATH0="/home/n00b/.arduino15/"
PATH1="/opt/arduino-1.8.13/"
PATH2="/usr/include/"
PATH3="$(pwd | sed 's#\/build/devutils##g')"

echo -e "\n\t ** Checking $PATH0 **\n"
find $PATH0 -f | grep $1
echo -e "\n\t ** Checking $PATH1 **\n"
find $PATH1 -f | grep $1
echo -e "\n\t ** Checking $PATH2 **\n"
find $PATH2 -f | grep $1
echo -e "\n\t ** Checking $PATH3 **\n"
find $PATH3 -f | grep $1

echo
echo

