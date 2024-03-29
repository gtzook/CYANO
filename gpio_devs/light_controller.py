import time
from typing import Dict, Union
from util.time_formatting import isDay, seconds_until, isTimeFormat
from multiprocessing.synchronize import Event
from .gpio_dev import gpio_dev
import numpy as np
import signal
import sys
from datetime import datetime as dt
import neopixel
import board
LED_pin = 26

def led_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    """
    Main loop for led process
    """
    
    # Define Relay
    ctrl = gpio_dev(LED_pin, reverse_polarity=False)

    #Setup Light Strip
    ORDER = neopixel.RGB
    pixels = neopixel.NeoPixel(
        board.D18, 104, brightness=.5, auto_write=False, pixel_order=ORDER
    )
    #Initialize to White
    def to_white(pixels):
        for i in range(len(pixels)): 
            pixels[i] = (255,255,255)
    to_white(pixels)
    #For rainbow patterns
    rainbow = np.load('gpio_devs/light_patterns/interpolated_rainbow.npy')
    rainbow[:,[1,0]] = rainbow[:,[0,1]]
    rainbow[:,[2,0]] = rainbow[:,[0,2]]
    pattern_index = 0
    
    #For Proper Exit
    def cleanup(*args):
        print("light_controller: Exiting cleanly")
        ctrl.off()
        gpio_dev.cleanup()
        sys.exit(0)

    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)
    
    toggle_time = 0
    night_time = 0
    day_time = 0
    start_t = 0
    
    wait_time = 1
    
    update_pixels = True
    
    ctrl.off()
    
    # Wait for settings to be processed
    while not events['new_settings'].is_set():
            time.sleep(1)
    while True:
        if events['new_settings'].is_set():
            print("light_controller: new settings received")
            events['new_agi'].set() # notify agi to update with settings
            toggle_time = 0 # immediately toggle
            night_time = isTimeFormat(shared_data['to_night'])
            day_time = isTimeFormat(shared_data['to_day'])
            # set to not because we will toggle right after
            shared_data['state'] = not isDay(night_time, day_time)
            start_t = time.time()
            events['new_settings'].clear()
            print(night_time)
            
        # Time elapsed since start of this state
        shared_data['elapsed'] = time.time() - start_t
        # If time elapsed, toggle LED
        if shared_data['elapsed'] > toggle_time:
            # Update time measure
            start_t = time.time()
            
            # Toggle
            events['new_light'].set()
            
            # update toggle time
            if not shared_data['state']: # it is night, turn to day
                print("light_controller: Starting daytime...")  
                shared_data['state'] = ctrl.on() 
                toggle_time = seconds_until(night_time)
            else: # it is day, turn to night
                toggle_time = seconds_until(day_time)
                shared_data['state'] = ctrl.off()   
                print("light_controller: Starting nighttime...")
            if debug_mode:
                print(f"light_controller: State is now {'day' if shared_data['state'] else 'night'}")
                print(f"light_controller: time to next toggle {toggle_time}")            
        else:       
            # Calculate time remaining
            rm_t = toggle_time - shared_data['elapsed']
            shared_data['remaining'] = rm_t
            if shared_data['state']:
                if shared_data['demo']: # If demo mode and on, rainbow!
                    wait_time = 0.01
                    if debug_mode:
                        print(f'current RGB: {rainbow[pattern_index]}')
                    for i in range(len(pixels)): 
                        ind = pattern_index + i
                        while ind >= len(rainbow):
                            ind -= len(rainbow)
                        pixels[i] = rainbow[ind]
                    pattern_index = pattern_index + 1 if pattern_index < len(rainbow) - 1 else 0
                    update_pixels = True # update colors
                else:
                    to_white(pixels)
                    update_pixels = True
        if events['new_brightness'].is_set():
            if debug_mode:
                print("light_controller: brightness change request received to " + 
                      f"{shared_data['brightness']} from {pixels.brightness}")
            update_pixels = True # update brightness
            pixels.brightness = shared_data['brightness']
            events['new_brightness'].clear()
            
        if update_pixels: # only update when flag is set to avoid flickering
            pixels.show()
            update_pixels = False
        time.sleep(wait_time)
                