#!/bin/bash

cd ..

rm -r ./build/build/*

git add . && git commit -m "version 3.0.6" && git push -u origin master

cd build/


