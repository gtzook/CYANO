import sys
sys.path.append('/home/cyano/CYANO')

from gpio_devs.gpio_dev import gpio_dev
import board
import neopixel
import random
from time import time
from time import sleep

ORDER = neopixel.RGB

pixels = neopixel.NeoPixel(
    board.D18, 100, brightness=1, auto_write=True, pixel_order=ORDER
)

#pixels = neopixel.NeoPixel(board.D18, 3)

#RELAY
LED_pin = 26
ctrl = gpio_dev(LED_pin, reverse_polarity=False)
ctrl.on()

def random_color():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

start_time = time()
while True:
    for i in range(len(pixels)):
        pixels[i] = random_color()
    pixels.show()
    sleep(0.1)  # Adjust the sleep duration to change the speed of color change



pixels.show()
sleep(5)

ctrl.off()