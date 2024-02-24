import time
from typing import Dict, Union
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
import numpy as np
import signal
import sys
from datetime import datetime as dt

AGI_pin = 13

def agi_loop(shared_data: Dict[str, Union[int,float,bool]], 
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
    timer_mode = True
    
    # Wait for settings to be processed
    while not events['new_agi'].is_set():
        time.sleep(1)
        
    while True:
        if events['new_agi'].is_set():
            period = get_period(shared_data['agi_freq'])
            duty = shared_data['agi_duty']/100.0
            if period == 0:
                timer_mode = False
            else:
                timer_mode = True
            on_time = duty * period
            start = time.time()
            elapsed = 0
            if debug_mode:
                print(f"agi_controller: time update:\n \
                      period {period}s\n \
                      on_time {on_time}s\n \
                      duty {duty}")
            events['new_agi'].clear()
        
        if timer_mode:
            elapsed = time.time() - start
            if elapsed < on_time:
                ctrl.on()
            else:
                if elapsed > period:
                    start = time.time()
                else:
                    ctrl.off()
        else:
            if shared_data['agi_duty'] == 0:
                ctrl.off()
            else:
                ctrl.on()
                
        time.sleep(1)
                