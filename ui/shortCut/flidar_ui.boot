#!/bin/bash
cd /home/encheng/code_ui/ui/ui/shortCut/../
mkdir log -p
nowTime=$(date "+%Y%m%d_%H%M%S")
./start.sh | tee log/flidar.${nowTime}.log
