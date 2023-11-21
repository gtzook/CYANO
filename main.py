import agitator
import gui
import lights.light_controller
import usb.adc
import multiprocessing

if __name__ == "__main__":
    # ADC serial monitor
    usb_proc = multiprocessing.Process(name='usb', 
                                       target=usb.adc.ADC_loop) 
    light_proc =  multiprocessing.Process(name='lights', 
                                          target=lights.light_controller.led_loop,
                                          args=[12,])
    usb_proc.start()
    light_proc.start()
    try:
        usb_proc.join()
    except KeyboardInterrupt:
        pass