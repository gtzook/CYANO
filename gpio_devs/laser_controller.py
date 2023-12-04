from .gpio_dev import pwm_dev
from typing import Dict, Union
import multiprocessing as mp
import time
laser_pin = 13

def laser_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, mp.Event], 
             debug_mode: bool) -> None:
    """
    Main loop for laser
    
    Turn on laser
    """
    ctrl = pwm_dev(laser_pin)
    val = 100
    recordings = []
    if debug_mode:
        print("laser_controller: laser on!")
    while (True):
        ctrl.set_duty(val)
        time.sleep(5)
        print(shared_data["od"])
        val -= 1
            