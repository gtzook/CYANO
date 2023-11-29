import time
import RPi.GPIO as GPIO
import util.time_formatting
import signal
import sys

class led_controller:
    # LED pin number
    LED = 26
    def __init__(self):
        led_controller._gpio_setup()
        self._set_handlers()
        self.off()
    
    def toggle(self):
        return self._output(not self.ledState)
    
    def on(self):
        return self._output(True)
    
    def off(self):
        return self._output(False)
    
    def _set_handlers(self):
        # set handlers for exit to close cleanly
        signal.signal(signal.SIGINT, self._exit)
        signal.signal(signal.SIGTERM, self._exit)
        
    def _output(self, value):
        self.ledState = value
        GPIO.output(led_controller.LED, not self.ledState)
        return self.ledState
    
    def _exit(self, signum, frame):
        # handle closing
        print("led_controller: Exiting cleanly")
        self.off()
        time.sleep(3)
        GPIO.cleanup()
        sys.exit(0)
        
    def _gpio_setup():
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(led_controller.LED,GPIO.OUT)

def led_loop(light_data, debug_mode):
    """
    Main loop for led process
    """
    ctrl = led_controller()
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
            