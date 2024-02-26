#!/bin/bash

# generate timestamp for filename
timestamp=$(date +"%Y%m%d_%H_%M_%S")

# wait so video won't be so long
sleep 45

# run ffmpeg command with unique filename
ffmpeg -i /dev/video0 -t 30 "/home/cyano/CYANO/video/output_${timestamp}.mp4"
