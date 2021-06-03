#!/bin/bash

# currentpath=$PWD
targetpath=$HOME/Code/Develop/Indoorino.3.1.0/

if [ -d "$targetpath" ]; then
    echo -e "Clening $targetpath";
else
	echo -e "\e[0;91mInvalid directory $targetpath\e[0m"
	exit
fi


if [ -d "$targetpath/build/build/" ]; then
{
    if [ -z "$(ls -A $targetpath/build/build/)" ]; then
    {
        echo -e "\e[0;31mNo python build files found\e[0m"
    } else {
        echo -e "\e[0;92mRemoving python build files\e[0m"
        find $targetpath/build/build/ -mindepth 1 | xargs rm -rf
    }
    fi
}   
else
	echo -e "\e[0;91mInvalid directory build/build\e[0m"
fi


if [ -f "$targetpath/server" ]; then
    echo -e "\e[0;92mRemoving server binary\e[0m"
    rm $targetpath/server
else
	echo -e "\e[0;91mNo server binary found\e[0m"
fi


if [ -f "$targetpath/libraries/serverbuilt" ]; then
    echo -e "\e[0;92mRemoving build binary\e[0m"
    rm $targetpath/libraries/serverbuilt
else
	echo -e "\e[0;91mNo build binary found\e[0m"
fi


if [ -d "$targetpath/application" ]; then
    echo -e "\e[0;92mCleaning python application caches\e[0m"
    find $targetpath/application/ | grep -E "(__pycache__|\.pyc|\.pyo$)" | xargs rm -rf
else
	echo -e "\e[0;91mNo application folder found\e[0m"
fi

# git add . && git commit -m "version $(cat ./build/.version)" && git push -u origin master

# cd $currentpath
