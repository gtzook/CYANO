#!/usr/bin/python
import serial
import time
from datetime import datetime as dt
import signal
import sys
from typing import Dict, Union
from multiprocessing.synchronize import Event
from .signal_processing.ph_filter import ph_filter
from .signal_processing.od_filter import od_filter

class ADC():
    TIMEOUT = 5000 # How long to wait before giving up on response
    def __init__(self,num_sensors=1,debug_mode=False):
        self._set_handlers()
        self.num_sensors = num_sensors
        self.debug_mode = debug_mode
        # open serial interface
        self.ser = serial.Serial('/dev/ttyACM0',9600, timeout = 0.01)
        
        # send received messages and flush out buffers
        for i in range(5):
            self._next()
    @staticmethod
    def val_to_ph(val : int) -> float:
        voltage = 3.3 * (val / 4096)
        # equation from https://files.atlas-scientific.com/Gravity-pH-datasheet.pdf
        return (-5.6548 * voltage) + 14.509

    def val_to_od(val : int)-> float:
        # TODO: this
        return val
        
    def get_sense_vals(self):
        vals = self.get()
        converted = [vals[0], vals[1]] # convert values
        return converted
    
    def get(self) -> list[int]:
        line=self.get_line()
        line_vals = line.split(',')
        vals = list()
        try:
            for sense in range(self.num_sensors):
                vals.append(int(line_vals[sense]))
        except ValueError:
            print(f"{dt.now()}: no value from ADC")
            return [0,0]
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
        
def ADC_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    adc = ADC(num_sensors=2,debug_mode=debug_mode)
    ph_filt = ph_filter(filter_len=10)
    od_filt = od_filter(filter_len=10)
    while True:
        vals = adc.get_sense_vals()
        ph = vals[0]
        od = vals[1]
        
        ph_filt.add(ph)
        ph_f = ph_filt.filtered()
        
        od_filt.add(od)
        od_f = od_filt.filtered()
        
        shared_data['ph'] = ADC.val_to_ph(ph_f)
        shared_data['od'] = ADC.val_to_od(od_f)
                
        # Signal new data exists 
        events['new_adc'].set()
        
        if debug_mode: # print in debug mode
            print(f"\nadc: ph_raw is {ph}")
            print(f"adc: ph_raw_filtered is {ph_f}")
            print(f"adc: ph is {shared_data['ph']}")
            print(f"adc: od_raw is {od}")
            print(f"adc: od_raw_filtered is {od_f}\n")
        time.sleep(0.1)
