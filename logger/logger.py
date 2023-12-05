from datetime import datetime # Probably want this to timestamp logs
import time # Use to set logging frequency
import sys
from typing import Dict, Union
import multiprocessing as mp

# May need to modify inputs as we want to log more things
def logger_loop(shared_data: Dict[str, Union[int,float,bool]], 
              events: Dict[str, mp.Event], 
              debug_mode: bool) -> None:
    
    # How to get data:
    ph = shared_data['ph'] # This is how you get ph measurement
    light_state = shared_data['state'] # State of lights
    
    try:
      with  open('Data_Logging.csv','a') as file: #appends to data log file, or creates if it doesnt exist
        file.write("Time, pH, OD600, Light") #writes the headers for the file
        while True:
          ph = shared_data['ph'] # This is how you get ph measurement
          light_state = shared_data['state'] # State of lights
          od = shared_data['od']
          now = datetime.now() #Gets current time. Time stamps
          file.write("\r\n")
          file.write(str(now) + "," + str(ph) + "," + str(od) + ',' + str(light_state)) #Writes time of measurement, and
          file.flush()
          time.sleep(5) #Set to whatever frequency we need
    except KeyboardInterrupt:
      print("logger: Exiting cleanly")
      sys.exit(0)