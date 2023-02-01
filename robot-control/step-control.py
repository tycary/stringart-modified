import time as tm
import serial

# Set gear ratio
gearRatio = 1

# Time to pause for movements
pauseRatio = 0.5

# COM Settings
comChannel = "com6"
Baudrate = 115200
to = 5  # Timeout

# COM initializing
ser = serial.Serial(comChannel, Baudrate, timeout=to)
tm.sleep(0.5)  # Wait for com
ser.reset_input_buffer()
ser.reset_output_buffer()
tm.sleep(2)
ser.flush()
tm.sleep(3)

# Main
infile = open('stringart/output/NailOutput.txt', 'r')
commands = infile.readline()
nail = commands.split(',')
# Send gear ratio and home wheel
ser.write((str(gearRatio)+"\n").encode('utf-8'))
ser.reset_output_buffer()
print("Gear Ratio Set")
tm.sleep(2)

ser.write(("402\n").encode('utf-8'))
print("Homing")
tm.sleep(5)
ser.reset_output_buffer()

prevNail = 0
# Send nails to Arduino
for i in range(10):
    print("Iteration: " + str(i) + " | current nail: " + nail[i])
    ser.write((nail[i]+"\n").encode('utf-8'))
    # tm.sleep(1.2*gearRatio)
    msg = ser.read().decode('utf-8')
    while msg.find("x") == -1:
        msg = ser.read().decode('utf-8')
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    prevNail = int(nail[i])


ser.write(("401\n").encode('utf-8'))  # Turn off Steppers
infile.close()
