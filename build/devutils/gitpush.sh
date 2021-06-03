#!/bin/bash


targetpath=$HOME/Code/Develop/Indoorino.3.1.0/
size=$(du -sbh $targetpath | cut -d'/' -f1)

${targetpath}build/devutils/clean.sh

if [ -d "$targetpath" ]; then
    echo -e "Pushing $targetpath to github: total $size\n\n";
else
	echo -e "\e[0;91mInvalid directory $targetpath\e[0m"
	exit
fi


# currentpath=$PWD
# cd ../..
# 
git add $targetpath && git commit -m "version $(cat $targetpath/build/.version)" && git push -u origin master
# 
# cd $currentpath


