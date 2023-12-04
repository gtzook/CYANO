import time
from typing import Dict, Union
import util.time_formatting
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
from .gpio_dev import pwm_dev
import random

LED_pin = 26
BLED = 19
GLED = 20
RLED = 16

def led_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    """
    Main loop for led process
    """
    ctrl = gpio_dev(LED_pin, reverse_polarity=False)
    blue_ctrl = pwm_dev(BLED, )
    green_ctrl = pwm_dev(GLED, )
    red_ctrl = pwm_dev(RLED, )
    shared_data['state'] = ctrl.on()
    
    # time to toggle between day and night
    toggle_time = shared_data['period']/2.0
    start_t = time.time()
    while True:
        # Time elapsed since start of this state
        shared_data['elapsed'] = time.time() - start_t

        # If time elapsed, toggle LED
        if shared_data['elapsed'] > toggle_time:
            if debug_mode:
                print("led_controller: Toggling...")
        
            # Update time measure
            start_t = time.time()
            
            # Toggle
            shared_data['state'] = ctrl.toggle()
            events['new_light'].set()
            
        else:       
            # Calculate time remaining
            rm_t = toggle_time - shared_data['elapsed']
            shared_data['remaining'] = rm_t
            if debug_mode:
                # Get formatted time string
                time_str = util.time_formatting.time_string_from_sec(int(rm_t))
                
                state_str = "night" if shared_data['state'] else "day"
                print("led_controller: Time to " + state_str + " is " + time_str)  
            
            blue_ctrl.set_duty(random.randint(0,100))
            green_ctrl.set_duty(random.randint(0,100))
            red_ctrl.set_duty(random.randint(0,100))
            time.sleep(1)
            