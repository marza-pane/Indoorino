#!/bin/bash

currentpath=$PWD
cd ../..

rm -r ./build/build/temp.*

find ./application/ | grep -E "(__pycache__|\.pyc|\.pyo$)" | xargs rm -rf
find ./code-gen/ | grep -E "(__pycache__|\.pyc|\.pyo$)" | xargs rm -rf

# git add . && git commit -m "version $(cat ./build/.version)" && git push -u origin master

cd $currentpath


