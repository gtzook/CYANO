import time
from typing import Dict, Union
from util.time_formatting import isDay, seconds_until
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
from .gpio_dev import pwm_dev
import numpy as np
import signal
import sys
from datetime import datetime as dt

LED_pin = 26
BLED = 19
GLED = 21
RLED = 16

def led_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    """
    Main loop for led process
    """
    
    # Relay
    ctrl = gpio_dev(LED_pin, reverse_polarity=False)

    # RGB
    rgb_ctrls = [pwm_dev(RLED), pwm_dev(GLED), pwm_dev(BLED)]
    def set_rgb(rgb: list[int]):
        for i in range(3):
            rgb_ctrls.set_duty(rgb[i])
    rainbow = np.load('gpio_devs/light_patterns/interpolated_rainbow.npy')
    pattern_index = 0
    
    def cleanup(*args):
        print("light_controller: Exiting cleanly")
        ctrl.off()
        gpio_dev.cleanup()
        sys.exit(0)
    
    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)
    
    # timing setup
    night_time = shared_data['to_night']
    day_time = shared_data['to_day']
    
    toggle_time = 0
    
    # initial state
    if isDay(night_time,day_time):
        shared_data['state'] = ctrl.on()
        print("light_controller: Starting daytime...")
        toggle_time = seconds_until(night_time)
    else:
        shared_data['state'] = ctrl.off()
        print("light_controller: Starting nighttime...")
        toggle_time = seconds_until(day_time)
    """
    shared_data['state'] = ctrl.on()
    toggle_time = 180
    #toggle_time = shared_data['period']/2.0
    """
    start_t = time.time()
    
    while True:
        # Time elapsed since start of this state
        shared_data['elapsed'] = time.time() - start_t

        # If time elapsed, toggle LED
        if shared_data['elapsed'] > toggle_time:
            # Update time measure
            start_t = time.time()
            
            # Toggle
            shared_data['state'] = ctrl.toggle()
            events['new_light'].set()
            
            # update toggle time
            if shared_data['state']: # it is day
                toggle_time = seconds_until(night_time)
            else: 
                toggle_time = seconds_until(day_time)
            if debug_mode:
                print(f"led_controller: State is now {'day' if shared_data['state'] else 'night'}")
                print(f"led_controller: time to next toggle {toggle_time}")            
        else:       
            # Calculate time remaining
            rm_t = toggle_time - shared_data['elapsed']
            shared_data['remaining'] = rm_t
            if shared_data['demo'] and shared_data['state']: # If demo mode and on, rainbow!
                if debug_mode:
                    print(f'current RGB: {rainbow[pattern_index]}')
                set_rgb(rainbow[pattern_index])
                pattern_index = pattern_index + 1 if pattern_index < len(rainbow) - 1 else 0
                time.sleep(.01)
                