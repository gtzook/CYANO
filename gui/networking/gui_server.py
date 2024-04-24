import signal
import sys
from typing import Dict, Union
from multiprocessing.synchronize import Event
import socket
import json
import time
import subprocess

def server_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
  
    process = subprocess.Popen("/home/cyano/CYANO/gui/gui")
    
    # Create socket to send to C++
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('localhost', 12345))
    server_socket.listen(1)
    
    # Connect to server
    print("gui_server: waiting for connections...")
    
    conn, addr = server_socket.accept()
    conn.settimeout(100)
    
    print(f"gui_server: connection at {addr}")  
    
    def clean():
      conn.close()
      server_socket.close()
      
    def signal_handler(signum, frame):
      print('gui_server: Exiting cleanly')
      clean()
      sys.exit(0)
      
    #Set up signal handler for Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    print("gui_server: waiting for user settings")
    # Receive settings
    conn.settimeout(None) # Wait until user is done inputting settings
    received_data = conn.recv(1024)
    if received_data:
        # Decode the received bytes to a string (if necessary)
        received_string = received_data.decode('utf-8')
        # Parse the JSON string
        settings = json.loads(received_string)
        
        # set time strings
        shared_data['to_night'] = settings["night"]
        shared_data['to_day'] = settings["day"]
        
        # set agitation period
        shared_data['agi_freq'] = settings['agi_freq']
        
        #set ph warning thresholds
        shared_data['ph_upper'] = settings['ph_upper']
        shared_data['ph_lower'] = settings['ph_lower']
        
        # Print the parsed JSON data
        print("gui_server: Received JSON data:", settings)
    else:
        print("gui_server: Failed to retrieve settings.")
    # notify services that settings are received
    events['new_settings'].set()
    conn.settimeout(0.1)
    # Normal loop
    while True:
        try:
          json_data = json.dumps(shared_data.copy())
          conn.sendall(json_data.encode('utf-8'))
          time.sleep(.5)
          try:
            rec = conn.recv(1024).decode('utf-8').strip()
            num = 0
            end = rec.rfind('B') # last occurence of B
            rec = rec[0:end]
            start = rec.rfind('A') # last occurence of A
            rec = rec[start + 1:]
            if len(rec) > 1:
              num = int(rec[1:])
            # brightness command
            if 'b' in rec:
              shared_data['brightness'] = num / 100.0
              events['new_brightness'].set()
            # agitation command
            elif 'a' in rec:
              shared_data['agi_duty'] = num
              events['new_agi'].set()
            # demo state change command
            elif 'p' in rec:
              shared_data['demo'] = not shared_data['demo']
            # blank requested
            elif 'x' in rec:
              events['blank_request'].set()
            elif 's' in rec:
              if '1' in rec:
                events['solenoid_on'].set()
              else:
                events['solenoid_off'].set()
          except TimeoutError:
            pass
        except (ConnectionResetError, BrokenPipeError):
          print('gui_server: connection to GUI lost')
          clean()
          sys.exit(0)