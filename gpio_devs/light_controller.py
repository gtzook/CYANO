import time
import util.time_formatting
import gpio_dev

LED_pin = 26

def led_loop(light_data, debug_mode):
    """
    Main loop for led process
    """
    ctrl = gpio_dev(LED_pin)
    light_data['state'] = ctrl.on()
    
    # time to toggle between day and night
    toggle_time = light_data['period']/2.0
    start_t = time.time()
    while True:
        # Time elapsed since start of this state
        light_data['elapsed'] = time.time() - start_t

        # If time elapsed, toggle LED
        if light_data['elapsed'] > toggle_time:
            print("led_controller: Toggling...")
        
            # Update time measure
            start_t = time.time()
            
            # Toggle
            light_data['state'] = ctrl.toggle()
            
        else:       
            # Calculate time remaining
            rm_t = toggle_time - light_data['elapsed']
            light_data['remaining'] = rm_t
            if debug_mode:
                # Get formatted time string
                time_str = util.time_formatting.time_string_from_sec(rm_t)
                
                state_str = "night" if light_data['state'] else "day"
                print("led_controller: Time to " + state_str + " is " + time_str)  
            
            time.sleep(1)
            