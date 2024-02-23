import sys
sys.path.append('/home/cyano/CYANO')

from gpio_devs.gpio_dev import gpio_dev
import board
import neopixel

LED_pin = 26
ctrl = gpio_dev(LED_pin, reverse_polarity=False)

ctrl.off()