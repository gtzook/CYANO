import time
from typing import Dict, Union
import util.time_formatting
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev

LED_pin = 26

def led_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    """
    Main loop for led process
    """
    ctrl = gpio_dev(LED_pin, reverse_polarity=True)
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
            
            time.sleep(1)
            