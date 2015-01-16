#!/bin/sh
sensor=""
rtsp=1
rtsp_size=""
rtsp_num=""
argv=""

do_help ()
{
    SELF=`basename $0`
    echo "usage:"
    echo "$SELF --sensor [sensor] --rtsp [num=2 size=4M,1M]"
    echo "eg: $SELF --sensor ov9710"
    exit 
}
if [ $# -eq 0 ]
then
    do_help
fi

until [ $# -eq 0 ]
do
    argv=$1
    case $argv in
        "--nortsp")
            rtsp=0;
            ;;
        "--sensor")
            sensor=$2;
            shift
            ;;
        "--rtsp")
            rtsp=1;
            rtsp_num=$2
            rtsp_size=$3
            if [ -z "`echo $rtsp_num | grep 'num='`" -o -z "`echo $rtsp_size |grep 'size='`" ]
            then
                do_help
            fi
            shift 2
            ;;
        *)
            echo "unknow command $argv"
            ;;
    esac
    shift
done
#check argument
if [ -z $sensor ]
then
    do_help
fi

#
echo 1024 > /proc/sys/vm/min_free_kbytes
ulimit -s 4096

sensor="--"$sensor
cd /;
init.sh $sensor
if [ $rtsp -eq 1 ]
then
    rtsp_server $rtsp_num $rtsp_size &
fi
sleep 1
/usr/local/bin/ctlserver &
/webSvr/boa 

