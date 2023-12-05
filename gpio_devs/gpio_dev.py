import signal
import time
import sys
import pigpio

pi = pigpio.pi()

class gpio_dev:
    def __init__(self, pin: int, reverse_polarity: bool=False):
        self.pin = pin
        self.reverse_polarity = reverse_polarity
        print(f"gpio_dev initialized pin {self.pin}")
        pi.set_mode(self.pin,pigpio.OUTPUT)
        self.off()
    
    def toggle(self):
        return self._output(not self.state)
    
    def on(self):
        return self._output(True)
    
    def off(self):
        return self._output(False)   
        
    def _output(self, value: bool) -> bool:
        self.state = value
        pi.write(self.pin, self.state != self.reverse_polarity) # write opposite state if reversed
        return self.state

class pwm_dev(gpio_dev):
    def __init__(self, pin: int, pwmFreq: int = 10000, pwmDuty: int = 50):
        super().__init__(pin)
        pi.set_PWM_frequency(self.pin,pwmFreq)
        
    def set_duty_percent(self, duty_percent):
        pi.set_PWM_dutycycle(self.pin, pwm_dev._percent_duty_convert(duty_percent))
    
    def set_duty(self, duty):
        pi.set_PWM_dutycycle(self.pin, duty)
    
    def off(self):
        pi.set_PWM_dutycycle(self.pin, 0)
    
    def _percent_duty_convert(val: int):
        return int((val/100.0) * 255)