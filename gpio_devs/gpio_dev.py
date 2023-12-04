import signal
import time
import sys
import pigpio

pi = pigpio.pi()

class gpio_dev:
    def __init__(self, pin: int, reverse_polarity: bool=False):
        self.pin = pin
        self.reverse_polarity = reverse_polarity
        print(f"gpio_dev initialized pin {self.pin}, dev_count: {gpio_dev.dev_count}")
        pi.set_mode(self.pin,pigpio.OUTPUT)
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
        pi.write(self.pin, self.state != self.reverse_polarity) # write opposite state if reversed
        return self.state
    
    def _exit(self, signum, frame):
        # handle closing
        print(f"gpio_dev pin {self.pin}: Exiting cleanly")
        self.off()
        time.sleep(.1)
        sys.exit(0)

class pwm_dev(gpio_dev):
    def __init__(self, pin: int, pwmFreq: int = 25000, pwmDuty: int = 50):
        super().__init__(pin)
        pi.set_PWM_frequency(self.pin,pwmFreq)
        self.duty = pwm_dev._percent_duty_convert(pwmDuty)
        
    def on(self):
        pi.set_PWM_dutycycle(self.pin, self.duty)
    
    def off(self):
        pi.set_PWM_dutycycle(self.pin, 0)
    
    def _percent_duty_convert(val: int):
        return int((50/100.0) * 255)