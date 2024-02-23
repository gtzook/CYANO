import spidev
import time
spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 1
spi.max_speed_hz = 1000000  
try:
   while True:
      spi.writebytes([0xC3])
      time.sleep(0.1)
      resp = spi.readbytes(1)
      print(f"{hex(resp[0])}")
      time.sleep(1)
except KeyboardInterrupt:
   spi.close()
