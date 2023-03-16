#!/bin/bash
cd /home/encheng/code_ui/ui/ui/../
mkdir log -p
nowTime=$(date "+%Y%m%d_%H%M%S")
if [ -z $1 ] || [ $1 != "noping" ]; then
  ./start.sh | tee log/flidar.${nowTime}.log
elif [ $1 == "noping" ]; then
  ./start.sh noping | tee log/flidar.${nowTime}.log
fi
