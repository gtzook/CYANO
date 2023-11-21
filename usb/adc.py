#!/usr/bin/python
import serial
import time
import signal
import sys

class ADC():
    TIMEOUT = 5000
    def __init__(self,num_sensors=1):
        self._set_handlers()
        self.num_sensors = num_sensors
        # open serial interface
        self.ser = serial.Serial('/dev/ttyACM0',9600, timeout = 0.01)
        # send received messages and flush out buffers
        for i in range(5):
            self._next()
    
    def get_ph(self):
        val = self.get()
        voltage = 3.3 * (val / 255)
        # equation from https://files.atlas-scientific.com/Gravity-pH-datasheet.pdf
        return (-5.6548 * voltage) + 15.509
    
    def get(self):
        line=self.get_line()
        if self.num_sensors > 1:
            vals = list()
            for sense in range(self.num_sensors):
                vals.append(line[sense])
            return vals
        else:
            return line[0]
        
    def get_line(self):
        line = ""
        self._next()
        # buffer to store last line
        buffer = ""
        line = self.ser.readline() # bits from board
        startTime = time.time()
        while (buffer == "" or 
                len(line) > 0 or
                time.time() - startTime > ADC.TIMEOUT): # keep reading to the end
            buffer = line
            line = self.ser.readline() # bits from board
        return buffer
        
    def close(self):
        self.ser.write(b'E')
    
    def _next(self):
        self.ser.write(b'S')
        
    def _set_handlers(self):
        # set handlers for exit to close cleanly
        signal.signal(signal.SIGINT, self._exit)
        signal.signal(signal.SIGTERM, self._exit)
        
    def _exit(self, signum, frame):
        # handle closing
        print("adc: Exiting cleanly")
        self.ser.close()
        sys.exit(0)
        
def ADC_loop():
    adc = ADC()
    while True:
        print(f"adc: ph is {adc.get_ph()}")
        time.sleep(3)

if __name__ == "__main__":
    adc = ADC()
    while True:
        print(f"ph: {adc.get_ph()}")
        time.sleep(0.5)
        