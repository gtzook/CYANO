import signal
import time
import sys
import RPIO as GPIO

GPIO.setmode(GPIO.BCM)

class gpio_dev:
    def __init__(self, pin: int, reverse_polarity: bool=False):
        self.pin = pin
        self.reverse_polarity = reverse_polarity
        GPIO.setup(self.pin,GPIO.OUT)
        self._set_handlers()
        self.off()
    
    def toggle(self):
        return self._output(not self.state)
    
    def on(self):
        return self._output(True)
    
    def off(self):
        return self._output(False)
    
    def _set_handlers(self):
        # set handlers for exit to close cleanly
        signal.signal(signal.SIGINT, self._exit)
        signal.signal(signal.SIGTERM, self._exit)
        
    def _output(self, value: bool) -> bool:
        self.state = value
        GPIO.output(self.pin, self.state != self.reverse_polarity) # write opposite state if reversed
        return self.state
    
    def _exit(self, signum, frame):
        # handle closing
        print(f"gpio_dev pin {self.pin}: Exiting cleanly")
        self.off()
        time.sleep(1)
        GPIO.cleanup()
        sys.exit(0)