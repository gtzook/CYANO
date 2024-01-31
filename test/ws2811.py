import sys
sys.path.append('/home/cyano/CYANO')

from gpio_devs.gpio_dev import gpio_dev
import board
import neopixel
import random
from time import time
from time import sleep
import numpy as np
import signal

ORDER = neopixel.RGB

pixels = neopixel.NeoPixel(
    board.D18, 100, brightness=1, auto_write=False, pixel_order=ORDER
)

def cleanup(*args):
    print("light_controller: Exiting cleanly")
    ctrl.off()
    gpio_dev.cleanup()
    sys.exit(0)
    
    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)
    
#RELAY
LED_pin = 26
ctrl = gpio_dev(LED_pin, reverse_polarity=False)
ctrl.on()

def random_color():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

rainbow = np.load('gpio_devs/light_patterns/interpolated_rainbow.npy')
pattern_index = 0

while True:
    for i in range(len(pixels)): 
        ind = pattern_index + i
        while ind >= len(rainbow):
            ind -= len(rainbow)
        pixels[i] = rainbow[ind]
    #pixels.fill(rainbow[pattern_index])
    pattern_index = pattern_index + 1 if pattern_index < len(rainbow) - 1 else 0     
    pixels.show()
    sleep(0.01)  # Adjust the sleep duration to change the speed of color change
