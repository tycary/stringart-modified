import time as tm
import serial

# Set gear ratio
gearRatio = 1

# Time to pause for movements
pauseRatio = 1

# COM Settings
comChannel = "com6"
Baudrate = 115200
to = 5  # Timeout

# COM initializing
ser = serial.Serial(comChannel, Baudrate, timeout=to)
tm.sleep(2)  # Wait for com
print("Ready for printing to serial\n")
ser.flush()

# Main
infile = open('stringart/output/NailOutput.txt', 'r')
commands = infile.readline()
nail = commands.split(',')
# Send gear ratio and home wheel
ser.write((gearRatio+"\n402\n").encode('utf-8'))
tm.sleep(15)
ser.reset_output_buffer()

prevNail = 0
# Send nails to Arduino
for i in range(10):
    print(i)
    ser.write((nail[i]+"\n").encode('utf-8'))
    tm.sleep(abs(prevNail-nail[i])*gearRatio +
             pauseRatio)  # Pauses for motor movement
    ser.reset_output_buffer()
    prevNail = nail[i]


ser.write(("404\n").encode('utf-8'))  # Turn off Steppers
infile.close()
