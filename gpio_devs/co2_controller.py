import time
from typing import Dict, Union
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
import numpy as np
import signal
import sys
from datetime import datetime as dt

CO2_pin = 19
CO2_ON_THRESH = 6.2

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
    
    while True:
        if shared_data['ph'] < CO2_ON_THRESH:
            ctrl.on()
        else:
            ctrl.off()
        time.sleep(1)