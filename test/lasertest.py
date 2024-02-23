import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BCM)
LED = 13
ledState = False
GPIO.setup(LED,GPIO.OUT)

try:
	while True:
		ledState = not ledState
		GPIO.output(LED, ledState)
		print(ledState)
		sleep(1)
except KeyboardInterrupt:
	GPIO.output(LED, True)
	#GPIO.cleanup()