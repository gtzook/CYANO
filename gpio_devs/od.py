from .gpio_dev import gpio_dev
import time

laser_pin = 13

def led_loop(light_data, debug_mode):
    """
    Main loop for od process
    
    Control laser and process adc data
    """
    ctrl = gpio_dev(laser_pin)
    ctrl.on() # turn on laser
    while (True):
        time.sleep(1) # stay awake
            