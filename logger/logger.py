import datetime # Probably want this to timestamp logs
import time # Use to set logging frequency
import csv # Use for file writing operations

# May need to modify inputs as we want to log more things
def logger_loop(adc_data, # ph and od sensors
                light_data, # light state and timing
                ):
    
    # How to get data:
    ph = adc_data['ph'] # This is how you get ph measurement
    light_state = light_data['state'] # State of lights
    
    # TODO: Add logging code @Brennan
    
    time.sleep(5) # Set to whatever frequency we need
    