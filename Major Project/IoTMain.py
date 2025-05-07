from machine import Pin, UART, I2C
import utime
import ujson
from adxl345 import ADXL345

# Initialize peripherals
gps_uart = UART(2, baudrate=9600, tx=17, rx=16)  # GPS UART
gsm_uart = UART(1, baudrate=9600, tx=4, rx=5)    # GSM UART
i2c = I2C(0, scl=Pin(22), sda=Pin(21))
button = Pin(13, Pin.IN, Pin.PULL_UP)  # Emergency button
buzzer = Pin(27, Pin.OUT)
led = Pin(2, Pin.OUT)
accel = ADXL345(i2c)

# Emergency contact
contact_number = "+919876543210"

# --- Helper Functions ---
def get_gps_location():
    gps_data = gps_uart.readline()
    if gps_data and b'GPGGA' in gps_data:
        try:
            parts = gps_data.decode().split(',')
            lat = convert_to_degrees(parts[2])
            lon = convert_to_degrees(parts[4])
            return (lat, lon)
        except:
            return None
    return None

def convert_to_degrees(raw_val):
    if raw_val == '':
        return 0.0
    d = float(raw_val[:2])
    m = float(raw_val[2:])
    return d + m / 60

def send_sms(msg):
    gsm_uart.write('AT\r\n')
    utime.sleep(1)
    gsm_uart.write('AT+CMGF=1\r\n')
    utime.sleep(1)
    gsm_uart.write(f'AT+CMGS="{contact_number}"\r\n')
    utime.sleep(1)
    gsm_uart.write(msg + '\x1A')
    utime.sleep(2)

def detect_fall():
    x, y, z = accel.read()
    threshold = 250  # Experimentally determined
    if abs(x) > threshold or abs(y) > threshold or abs(z) > threshold:
        return True
    return False

# --- Main Loop ---
print("Smart Pendant System Active")
while True:
    if button.value() == 0 or detect_fall():
        buzzer.on()
        led.on()
        location = get_gps_location()
        if location:
            message = f"EMERGENCY! Location: https://maps.google.com/?q={location[0]},{location[1]}"
        else:
            message = "EMERGENCY! Location not available."
        send_sms(message)
        utime.sleep(1)
        buzzer.off()
        led.off()
        utime.sleep(5)
