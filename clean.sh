#!/bin/sh

for file in gmon.out proflog.txt water
do
    if [ -e $file ]
    then
        rm -v $file
    fi
done

