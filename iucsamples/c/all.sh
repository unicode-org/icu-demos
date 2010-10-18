#!/bin/sh
# Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.

make -C data

for dir in `ls`;
do
    if [ -f "${dir}/Makefile" ];
    then
        make -C "${dir}" 
    fi
done
