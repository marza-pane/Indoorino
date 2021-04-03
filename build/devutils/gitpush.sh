#!/bin/bash

clean.sh

currentpath=$PWD
cd ../..

git add . && git commit -m "version $(cat ./build/.version)" && git push -u origin master

cd $currentpath


