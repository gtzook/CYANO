#!/usr/bin/python
import serial
import time
import signal
import sys
from ctypes import c_float
class ADC():
    TIMEOUT = 5000
    def __init__(self,num_sensors=1,debug_mode=False):
        self._set_handlers()
        self.num_sensors = num_sensors
        self.debug_mode = debug_mode
        # open serial interface
        self.ser = serial.Serial('/dev/ttyACM0',9600, timeout = 0.01)
        # send received messages and flush out buffers
        for i in range(5):
            self._next()
    
    def val_to_ph(val):
        voltage = 3.3 * (val / 4096)
        # equation from https://files.atlas-scientific.com/Gravity-pH-datasheet.pdf
        return (-5.6548 * voltage) + 14.509
        
    def get_sense_vals(self):
        vals = self.get()
        vals[0] = ADC.val_to_ph(vals[0]) # convert value to ph
        return vals
    
    def get(self):
        line=self.get_line()
        line_vals = line.split(',')
        vals = list()
        for sense in range(self.num_sensors):
            vals.append(int(line_vals[sense]))
        return vals
        
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
        
        if self.debug_mode:
            print(f"adc buffer: {buffer.decode('utf8').strip()}")
        return buffer.decode('utf8').strip()
        
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
        
def ADC_loop(adc_data, new_ph_event, debug_mode):
    # adc_data should be an mp.Array (shared memory)
    adc = ADC(num_sensors=2,debug_mode=debug_mode)
    while True:
        vals = adc.get_sense_vals()
        ph = vals[0]
        od = vals[1]
        
        adc_data['ph'] = ph
        adc_data['od'] = od
                
        # Let all know new ph data exists 
        new_ph_event.set()
        
        # print if debugging
        if debug_mode:
            print(f"adc: ph is {ph}")
            print(f"adc: od is {od}")
        time.sleep(0.5)

if __name__ == "__main__":
    adc     = ADC()
    while True:
        print(f"ph: {adc.get_ph()}")
        time.sleep(0.5)
        
