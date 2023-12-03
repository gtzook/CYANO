from .gpio_dev import gpio_dev
import time

laser_pin = 13

def od_loop(od_data, debug_mode):
    """
    Main loop for od process
    
    Control laser and process adc data
    """
    ctrl = gpio_dev(laser_pin)
    ctrl.off() # turn on laser
    if debug_mode:
        print("od: laser on!")
    while (True):
        time.sleep(1) # stay awake
            