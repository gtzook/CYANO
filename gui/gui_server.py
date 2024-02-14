import signal
import sys
from typing import Dict, Union
from multiprocessing.synchronize import Event
import socket
import json
import time

def server_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    
    # Create socket to send to C++
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', 12345))
    server_socket.listen(1)
    
    # Connect to server
    print("GUI server started, waiting for connections...")
    
    conn, _ = server_socket.accept()
    
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
    
    while True:
        try:
          json_data = json.dumps(shared_data.copy())
          conn.sendall(json_data.encode('utf-8'))
          time.sleep(1)
        except ConnectionResetError:
          print('gui_server: connection reset by GUI')
          clean()
          sys.exit(0)