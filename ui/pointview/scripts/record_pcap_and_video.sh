#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 need input <path>"
    exit 1
fi
SAVE_DIR=$1
echo "The value of abc is: $SAVE_DIR"

RECORD_TIME=10
TIME=$(date "+%Y_%m_%d_%H_%M_%S")

if [ ! -d ${SAVE_DIR} ];then
	mkdir ${SAVE_DIR}
fi

VIDEO_PATH="$SAVE_DIR/$TIME.mkv"
PCAP_PATH="$SAVE_DIR/$TIME.pcap"

sudo ifconfig
ffmpeg -f v4l2 -framerate 30 -video_size 1920x1080 -i /dev/video0 -t ${RECORD_TIME} -pix_fmt yuv420p ${VIDEO_PATH} &
sudo tcpdump -i any -f "udp port 2370 or udp port 2470 or udp port 2369" -W 1 -G ${RECORD_TIME} -w ${PCAP_PATH}
