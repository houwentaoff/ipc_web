#!/bin/sh
if [ -z "$1" ]; then
    echo "Usage:`basename $0` --[sensor]"
    echo "eg: `basename $0` --ov9710"
    exit 
fi

cd /;
init.sh $1
rtsp_server &
sleep 1
/usr/local/bin/ctlserver &
/webSvr/boa 

