import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BCM)
LED = 26
ledState = False
GPIO.setup(LED,GPIO.OUT)

while True:
	ledState = not ledState
	GPIO.output(LED, ledState)
	print(ledState)
	sleep(.1)
