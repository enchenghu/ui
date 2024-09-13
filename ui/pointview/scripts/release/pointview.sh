#!/bin/bash

# 16M: 128 * 1024 * 1024= 16777216
if [[ $(cat /proc/sys/net/core/rmem_max) != "16777216" ]]
then
    # set net.core.rmem_max
    sudo sysctl -w net.core.rmem_max=16777216
fi
echo "default buffer size: "
cat /proc/sys/net/core/rmem_max

cd "$( dirname "${BASH_SOURCE[0]}" )"
LIB=libs
LIB_3rd=libs/third_party
export LD_LIBRARY_PATH=${LIB}:${LIB_3rd}:${LD_LIBRARY_PATH}
export QT_PLUGIN_PATH=qt/plugins
./bin/pointview
