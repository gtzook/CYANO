import time
from typing import Dict, Union
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
import numpy as np
import signal
import sys
from datetime import datetime as dt

AGI_pin = 13

def agiloop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    """
    Main loop for led process
    """
    
    # Define Relay
    ctrl = gpio_dev(AGI_pin, reverse_polarity=False)
    
    #For Proper Exit
    def cleanup(*args):
        print("agi_controller: Exiting cleanly")
        ctrl.off()
        gpio_dev.cleanup()
        sys.exit(0)

    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)
    
    def get_period(i):
        periods = [0,60,300,600,1800,3600]
        return periods[i]
    
    on_time = -1
    period = 0
    start = time.time()
    elapsed = -1
    
    def update_times():
        global period, on_time, start, elapsed
        period = get_period(shared_data['agi_freq'])
        if period == 0:
            duty = 100
        else:
            duty = shared_data['agi_duty']
        duty /= 100.0
        on_time = duty * period
        start = time.time()
        elapsed = 0
        
    while True:
        if events['new_agi'].is_set():
            update_times()
            if debug_mode:
                print(f"agi_controller: time update: 
                      period {period}s 
                      on_time {on_time}s")
            
        elapsed = time.time() - start
        if elapsed < on_time:
            ctrl.on()
        else:
            if elapsed > period:
                start = time.time()
            else:
                ctrl.off()
                
        time.sleep(1)
                