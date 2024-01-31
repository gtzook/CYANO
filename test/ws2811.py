from gpio_devs.gpio_dev import gpio_dev
import board
import neopixel
pixels = neopixel.NeoPixel(board.D18, 30)

LED_pin = 26

# Relay
ctrl = gpio_dev(LED_pin, reverse_polarity=False)
ctrl.on()

ctrl.off()