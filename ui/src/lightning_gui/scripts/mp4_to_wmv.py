#!/usr/bin/env python

import sys, getopt
import os
import subprocess

in_bag_file = sys.argv[1]
in_mp4_file = in_bag_file.replace(".bag", ".mp4")
out_wmp_file = in_bag_file.replace(".bag", "-wmp.mp4")
subprocess.Popen(['ffmpeg','-i',in_mp4_file,'-pix_fmt','yuv420p','-c:a' ,'copy' ,'-movflags', '+faststart', out_wmp_file])
