#!/bin/bash

ttl_5=""
pin_cnt=0

ping_IP='10.20.30.40'
ping_ret=""

mkdir log -p
nowTime=$(date "+%Y%m%d_%H%M%S")

echo -e ">>>>>>>> flidar_ui ./start.sh >>>>>>>>\n"

echo -e "nowTime: $(date "+%Y-%m-%d_%H-%M-%S") \n"

if [ $# -eq 1 ] && [ "noping" == $1 ]; then
    echo "============== Not ping =============="
else
    echo "========== ping ${ping_IP} =========="

    ping -c 1 ${ping_IP}
    ping_ret=$?
  # echo 'ping: $?:' "${ping_ret}"

    while [ 0 -ne ${ping_ret} ]; do
        ping -c 1 ${ping_IP}
        ping_ret=$?
      # echo 'ping: $?:' "${ping_ret}"

        ping_cnt=`expr ${ping_cnt} + 1`
        if (("${ping_cnt}" >= 100)); then
    	    echo 'ping ${IP_PING} --> failed!'
    	    exit 1
        fi

        sleep 1
    done
fi

echo -e "\n>>>>>>>>>>> flidar_ui boot >>>>>>>>>>>\n"

# exit 1

source ./devel/setup.bash 
roslaunch autox_ui lightning_local.launch

exit 0

