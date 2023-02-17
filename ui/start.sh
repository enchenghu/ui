#!/bin/bash

# $ ping -c 1 192.168.186.2
# PING 192.168.186.2 (192.168.186.2) 56(84) bytes of data.
# 64 字节，来自 192.168.186.2: icmp_seq=1 ttl=128 时间=0.441 毫秒
# --- 192.168.186.2 ping 统计 ---
# 已发送 1 个包， 已接收 1 个包, 0% 包丢失, 耗时 0 毫秒
# rtt min/avg/max/mdev = 0.441/0.441/0.441/0.000 ms
#----------------------------
# ttl_1: 64
# ttl_2: 字节，来自
# ttl_3: 192.168.186.1:
# ttl_4: icmp_seq=1
# ttl_5: ttl=128
# ttl_6: 时间=0.243
# ttl_7: 毫秒
#----------------------------

# qh-inn@ubuntu:~/Desktop/Project/Flidar_ui/ui/ui/shortcut$ ping -c 1 192.168.186.3
# PING 192.168.186.3 (192.168.186.3) 56(84) bytes of data.
# 来自 192.168.186.130 icmp_seq=1 目标主机不可达
# --- 192.168.186.3 ping 统计 ---
# 已发送 1 个包， 已接收 0 个包, +1 错误, 100% 包丢失, 耗时 0 毫秒
#----------------------------
# ttl_1: 来自
# ttl_2: 10.20.30.60
# ttl_3: icmp_seq=1
# ttl_4: 目标主机不可达
#----------------------------

ttl_5=""
ping_cnt=0
echo '========== ping 10.20.30.40 =========='
while [ -z "$ttl_5" ]; do
    rut_ping=`ping -c 1 10.20.30.40 | grep "icmp_seq"`
    echo ${rut_ping}
    ttl_5=`echo $rut_ping | awk '{print $5}'`   # 如果ping通则非空，如果ping失败则为空。

    ping_cnt=`expr ${ping_cnt} + 1`
    if (("${ping_cnt}" >= 100)); then
    	echo 'ping 10.20.30.40 --> failed!'
    	exit 1
    fi
    
    sleep 1
done

echo -e ">>>>>>>>>> flidar_ui boot >>>>>>>>>> \n"
source ./devel/setup.bash 
#sudo ifconfig $1 $2
roslaunch autox_ui arbe.launch
