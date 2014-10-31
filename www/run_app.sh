#!/bin/sh
if [ -z "$1" ]; then
    echo "Usage:`basename $0` --[sensor]"
    echo "eg: `basename $0` --ov9710"
    exit 
fi

echo 1024 > /proc/sys/vm/min_free_kbytes
ulimit -s 4096

cd /;
init.sh $1
rtsp_server &
sleep 1
/usr/local/bin/ctlserver &
/webSvr/boa 

