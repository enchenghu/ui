#!/usr/bin/env python

"""
rosbagto_bin.py
rosbag to Arbe native binary file
Extract Arbe native binary data from a rosbag recording (.rosbag)
eyal.r@arberobotics.com
"""

import roslib
roslib.load_manifest('rosbag')
import rosbag
import sys, getopt
import os
import numpy as np

import shlex, subprocess

opt_topic = ""
opt_files = []
opt_verbose = True;

def print_help():
    print 'rosbagto_bin.py [-o outputfile] [-v (verbose messages)] [-t topic] bagfile1 [bagfile2] ...'
    print
    print 'Extract arbe native binary data from a rosbag recording (.rosbag)'
    print '-h:     Displays this help.'
    print '-o:     sets output filename.'
    print '        If no output file (-o) is given the filename \'<topic>.mp4\' is used and default output codec is h264.'
    print '        Multiple image topics are supported only when -o option is _not_ used.'
    print '        avconv/ffmpeg will guess the format according to given extension.'
    print '        Compressed and raw image messages are supported with mono8 and bgr8/rgb8/bggr8/rggb8 formats.'
    print '-t:     Only the images from topic "topic" are used for the video output.'
    print '-v:     Verbose messages are displayed.'
   
print
print 'rosbag_to_bin, Eyal Reizer, 2020'
print

if len(sys.argv) < 2:
    print 'Please specify ros bag file(s)!'
    print_help()
    sys.exit(1)
else :
   try:
      opts, opt_files = getopt.getopt(sys.argv[1:],"hv:o:t:",["ofile=","topic="])
   except getopt.GetoptError:
      print_help()
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print_help()
         sys.exit(0)
      elif opt in ("-o", "--ofile"):
         opt_out_file = arg
      elif opt in ("-t", "--topic"):
         opt_topic = arg
      else:
          print "opz:", opt,'arg:', arg

for files in range(0,len(opt_files)):
    #First arg is the bag to look at
    bagfile = opt_files[files]
    if opt_verbose :
        print "Bagfile: {}".format(bagfile)
       
    #Go through the bag file
    bag = rosbag.Bag(bagfile)
    if opt_verbose :
        print "Bag opened."

    unityfilename = bagfile.replace(".bag", ".bin")
    unitybinfile = open(unityfilename, 'w')
    for topic, msg, t in bag.read_messages(topics=['/arbe_bin_pointcloud','/arbe/raw/radar_tx']):
        unitybinfile.write(msg.data)

    unitybinfile.close()
    bag.close();
    print "created bin file file: {}".format(unityfilename)

