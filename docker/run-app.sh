#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
## ulimit -c unlimited

cd /usr/local/src/telephone/build
./telephone-app

echo telephone-app exited with code $?

while :
do
    sleep 10
done
