import time
from typing import Dict, Union
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
import numpy as np
import signal
import sys
from datetime import datetime as dt
from playsound import playsound

CO2_pin = 19
CO2_falling_thresh = 0.1

def co2_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    """
    Main loop for led process
    """
    
    # Define Relay
    ctrl = gpio_dev(CO2_pin, reverse_polarity=False)
    
    #For Proper Exit
    def cleanup(*args):
        print("co2_controller: Exiting cleanly")
        ctrl.off()
        gpio_dev.cleanup()
        sys.exit(0)

    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)
    
    shared_data['co2'] = ctrl.off()

    auto_mode = True # automatically control solenoid unless user takes control
    
    while True:
        if auto_mode:
            if shared_data['ph'] != -1 and shared_data['ph'] > shared_data['ph_upper']:
                if not shared_data['co2'] :
                    print(f"co2_controller: ph {shared_data['ph']}, turning on co2")
                shared_data['co2'] = ctrl.on()
                # playsound('/home/cyano/CYANO/sounds/beep-06.mp3') # crashes pi without power supply for screen
            else:
                # only turn off after being on if less than the upper bound minus a margin
                if shared_data['co2'] and shared_data['ph'] < shared_data['ph_upper'] - CO2_falling_thresh:
                    print(f"co2_controller: ph {shared_data['ph']}, turning off co2")
                    shared_data['co2'] = ctrl.off()
            if events['solenoid_on'].is_set() or events['solenoid_off'].is_set():
                auto_mode = False
        else:
            if events['solenoid_on'].is_set():
                shared_data['co2'] = ctrl.on()
                events['solenoid_on'].clear()
            elif events['solenoid_off'].is_set():
                shared_data['co2'] = ctrl.off()
                events['solenoid_off'].clear()

        time.sleep(1)