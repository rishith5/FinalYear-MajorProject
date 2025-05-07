# adxl345.py
from machine import I2C
import utime

class ADXL345:
    def __init__(self, i2c, addr=0x53):
        self.i2c = i2c
        self.addr = addr
        self.i2c.writeto_mem(self.addr, 0x2D, b'\x08')  # Power on

    def read(self):
        data = self.i2c.readfrom_mem(self.addr, 0x32, 6)
        x = int.from_bytes(data[0:2], 'little', signed=True)
        y = int.from_bytes(data[2:4], 'little', signed=True)
        z = int.from_bytes(data[4:6], 'little', signed=True)
        return (x, y, z)
