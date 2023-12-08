from datetime import datetime # Probably want this to timestamp logs
import time # Use to set logging frequency
import sys
from typing import Dict, Union
import multiprocessing as mp
import signal
import os
# May need to modify inputs as we want to log more things
def logger_loop(shared_data: Dict[str, Union[int,float,bool]], 
              events: Dict[str, mp.Event], 
              debug_mode: bool) -> None:
  
    def signal_handler(signum, frame):
      print('logger: Exiting cleanly')
      sys.exit(0)
    #Set up signal handler for Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    now = datetime.now()
    file_path = os.path.join(str(now.year),str(now.month),str(now.day))
    log_count = 0
    if not os.path.exists(file_path): # make directory if one does not exist
      os.makedirs(file_path)
    else:
      for path in os.listdir(file_path): # count existing files
        if os.path.isfile(os.path.join(file_path,path)):
          log_count = log_count + 1
    filename = os.path.join(file_path, log_count) + '.csv'# give this log new name
    
    # How to get data:
    ph = shared_data['ph'] # This is how you get ph measurement
    light_state = shared_data['state'] # State of lights
    
    with open(filename,'a') as file: #appends to data log file, or creates if it doesnt exist
      file.write("Time, pH, OD, Day/Night") #writes the headers for the file
      while True:
        ph = shared_data['ph'] # This is how you get ph measurement
        light_state = shared_data['state'] # State of lights
        od = shared_data['od']  
        now = datetime.now() #Gets current time. Time stamps
        file.write("\r\n")
        file.write(f"{now}, {ph}, {od}, {'Day' if light_state else 'Night'}") #Writes time of measurement, and
        file.flush()
        time.sleep(5) #Set to whatever frequency we need