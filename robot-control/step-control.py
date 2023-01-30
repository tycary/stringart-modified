import time as tm
import serial

Baudrate = 115200
ser = serial.Serial("com6", Baudrate, timeout=5)
tm.sleep(2)  # Wait for com
print("Ready for printing to serial\n")
ser.flush()
while (msg == 0):
    ser.write((input()+"\n").encode('utf-8'))
    tm.sleep(1)
    msg = 1
