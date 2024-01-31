import sys
sys.path.append('/home/cyano/CYANO')

from gpio_devs.gpio_dev import gpio_dev
import board
import neopixel
from time import sleep
pixels = neopixel.NeoPixel(board.D19, 30)

LED_pin = 26

# Relay
ctrl = gpio_dev(LED_pin, reverse_polarity=False)
ctrl.on()
pixels[0] = (255,0,0)
sleep(1)
ctrl.off()