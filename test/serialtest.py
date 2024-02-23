#!/usr/bin/python
import os, sys
import serial
import time

def close():
    print('closing')
    ser.write(b'E')

ser = serial.Serial('/dev/ttyACM0',9600, timeout = 5)
# listen for the input, exit if nothing received in timeout period
print("---")
try:
   while True:
      line = ""
      ser.write(b'S')
      print("Sending...")
      line = ser.readline() # bits from board
      print(f"Reading: {len(line)}")
      time.sleep(0.1)
except KeyboardInterrupt:
   close()
