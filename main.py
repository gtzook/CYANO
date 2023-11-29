#!/usr/bin//env python
import agitator
import gui.gui
import lights.light_controller
import logger.logger
import usb.adc
import multiprocessing as mp
import time
import sys

if __name__ == "__main__":
    # Check cmd line args
    debug_mode = False
    if len(sys.argv) > 1:
        # pass in 'd' to enable debugging
        if sys.argv[1] == '-d':
            debug_mode = True
    
    # Shared memory manager
    manager = mp.Manager()
    
    # Shared memory items
    adc_data = manager.dict({'ph':-1})
    light_data = manager.dict({'period':10,
                               'state':False,
                               'elapsed':-1,
                               'remaining':-1})
    
    # Events
    new_ph_event = mp.Event()
    
    # ADC serial monitor
    usb_proc = mp.Process(name='usb', 
                        target=usb.adc.ADC_loop,
                        args=[adc_data, new_ph_event, debug_mode]) 
    
    # Light controller
    light_proc =  mp.Process(name='lights', 
                            target=lights.light_controller.led_loop,
                            args=[light_data, debug_mode])
    # GUI
    gui_proc = mp.Process(name = 'gui',
                          target=gui.gui.gui_loop,
                          args=[adc_data, new_ph_event,
                                light_data])
    
    #Logging
    log_proc = mp.Process(name = 'log',
                          target=logger.logger.logger_loop,
                          args=[adc_data,light_data])
    
    usb_proc.start()
    light_proc.start()
    gui_proc.start()
    log_proc.start()
    try:
        while True:
            pass
    except KeyboardInterrupt:
        pass
