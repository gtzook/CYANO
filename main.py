#!/usr/bin//env python
import gui.gui
import gpio_devs.light_controller
import gpio_devs.laser_controller
import gpio_devs
import logger.logger
import usb.adc
import multiprocessing as mp
import sys

if __name__ == "__main__":  
    # Shared memory manager
    manager = mp.Manager()
    
    # TODO: If we are having speed issues, consider moving to Queue-only design
    # https://www.geeksforgeeks.org/python-multiprocessing-queue-vs-multiprocessing-manager-queue/
    
    # Shared memory items
    shared_data = manager.dict({'ph':-1, # ph value from adc
                                'od':-1, # processed OD value
                                'period':10, # period for light cycle
                                'state':False, # state of lights
                                'elapsed':-1, # time elapsed in this light state
                                'remaining':-1}) # time remaining in this light state
    
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
    
    usb_proc.start()
    if not '-nolight' in sys.argv:
        light_proc.start()
    laser_proc.start()
    gui_proc.start()
    log_proc.start()
    
    try:
        while True:
            pass
    except KeyboardInterrupt:
        gpio_devs.gpio_dev.pi.off()
