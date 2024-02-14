#!/usr/bin//env python
import gui.gui
import gpio_devs.light_controller
import gpio_devs.laser_controller
import gpio_devs.co2_controller
import gpio_devs
import logger.logger
import usb.adc
import multiprocessing as mp
import sys
from datetime import datetime as dt
from util.time_formatting import getTimeFromUser, isTimeFormat
import time
import socket
import json

if __name__ == "__main__":   
    print(f"CYANO starting at {dt.now()}")
    # Shared memory manager
    manager = mp.Manager()
    
    # TODO: If we are having speed issues, consider moving to Queue-only design
    # https://www.geeksforgeeks.org/python-multiprocessing-queue-vs-multiprocessing-manager-queue/
    
    # Shared memory items
    shared_data = manager.dict({'ph':-1, # ph value from adc
                                'od':-1, # processed OD value
                                'to_day': '09:00:00', # time to change to day
                                'to_night': '18:00:00', # time to change to night
                                'state':False, # state of lights
                                'co2':False, # co2 running
                                'elapsed':-1, # time elapsed in this light state
                                'remaining':-1, # time remaining in this light state
                                'demo': '-demo' in sys.argv,
                                'brightness': .8}) 
    time_str = "%H:%M:%S"
    shared_data['to_day'] = isTimeFormat('09:00:00')
    shared_data['to_night'] = isTimeFormat('18:00:00')

    # Events
    events = {'new_adc': mp.Event(),
              'new_light': mp.Event()}

    # ADC serial monitor
    usb_proc = mp.Process(name='usb', 
                        target=usb.adc.ADC_loop,
                        args=[shared_data, events, '-adcdebug' in sys.argv]) 
    # Light controller
    light_proc =  mp.Process(name='lights', 
                            target=gpio_devs.light_controller.led_loop,
                            args=[shared_data, events, '-lightdebug' in sys.argv])
    # Laser controller
    laser_proc = mp.Process(name='od',
                        target=gpio_devs.laser_controller.laser_loop,
                        args=[shared_data, events, '-oddebug' in sys.argv])
    
    # GUI
    gui_proc = mp.Process(name = 'gui',
                          target=gui.gui.gui_loop,
                          args=[shared_data, events, '-guidebug' in sys.argv])
    
    #Logging
    log_proc = mp.Process(name = 'log',
                          target=logger.logger.logger_loop,
                          args=[shared_data, events, '-loggerdebug' in sys.argv])
    
    # CO2
    co2_proc = mp.Process(name = 'co2',
                          target=gpio_devs.co2_controller.co2_loop,
                          args=[shared_data, events, '-co2debug' in sys.argv])
    
    usb_proc.start()
    if not '-nolight' in sys.argv:
        light_proc.start()
    laser_proc.start()
    if not '-nogui' in sys.argv:
        gui_proc.start()
    log_proc.start()
    co2_proc.start()
    print('usb: ', usb_proc.pid)
    print('lights: ', light_proc.pid)
    print('laser: ', laser_proc.pid)
    print('gui: ', gui_proc.pid)
    print('log: ', log_proc.pid)
    print('co2: ', co2_proc.pid)
        
    # Create socket to send to C++
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # Connect to server
    server_address = ('localhost',12345)
    client_socket.connect(server_address)
    
    try:
        while True:
            json_data = json.dumps(shared_data)
            client_socket.sendall(json_data.encode('utf-8'))
            time.sleep(1)
    except KeyboardInterrupt:
        client_socket.close()
