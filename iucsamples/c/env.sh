#!/bin/sh
export DYLD_LIBRARY_PATH=/tmp/ICU/lib:${DYLD_LIBRARY_PATH}
export PATH=/tmp/ICU/bin:${PATH}
export PS1="i ${PS1}"
export ICU_DATA=/iuc34samples/c/data
