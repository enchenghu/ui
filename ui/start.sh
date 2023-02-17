#!/bin/bash
ttl_5=""
while [ -z "$ttl_5" ]; do
    rut_ping=`ping -c 1 10.20.30.40 | grep "icmp_seq"`
    echo ${rut_ping}
    ttl_5=`echo $rut_ping | awk '{print $5}'`   # 如果ping通则非空，如果ping失败则为空。
    sleep 1
done
source ./devel/setup.bash 
#sudo ifconfig $1 $2
roslaunch autox_ui lightning.launch
