from datetime import datetime # Probably want this to timestamp logs
import time # Use to set logging frequency
import csv # Use for file writing operations
import os

# May need to modify inputs as we want to log more things
def logger_loop(adc_data, # ph and od sensors
                light_data, # light state and timing
                ):
    
    # How to get data:
    ph = adc_data['ph'] # This is how you get ph measurement
    light_state = light_data['state'] # State of lights
    
    # TODO: Add logging code @Brennan
    with  open('Data_Logging.csv','a') as file: #appends to data log file, or creates if it doesnt exist
      file.write("Time, pH, OD600, Light") #writes the headers for the file
      while True:
        now = datetime.now() #Gets current time. Time stamps
        file.write("\r\n")
        file.write(str(now) + "," + str(ph) + "," 'OD Placeholder' + ',' + str(light_state)) #Writes time of measurement, and
        file.flush()
        time.sleep(5) #Set to whatever frequency we need
      
    
