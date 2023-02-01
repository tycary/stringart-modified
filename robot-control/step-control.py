import time as tm
import serial


# Set gear ratio
gearRatio = 1

# COM Settings
comChannel = "com6"
Baudrate = 115200
to = 5  # Timeout

# COM initializing
ser = serial.Serial(comChannel, Baudrate, timeout=to)
tm.sleep(0.5)  # Wait for com
ser.reset_input_buffer()
ser.reset_output_buffer()
tm.sleep(0.5)
print("Communication Ready")
ser.flush()
tm.sleep(2)

# Main
infile = open('stringart/output/NailOutput.txt', 'r')
commands = infile.readline()
nail = commands.split(',')
# Send gear ratio and home wheel
ser.write((str(gearRatio)+"\n").encode('utf-8'))
ser.reset_output_buffer()
print("Gear Ratio Set")
tm.sleep(1)

ser.write(("402\n").encode('utf-8'))
print("Homing")
tm.sleep(5)  # Wait time for Homing
ser.reset_output_buffer()

prevNail = 0
# Send nails to Arduino
print("Printing\n")
for i in range(30):
    print("Iteration: " + str(i) + " | current nail: " + nail[i])
    ser.write((nail[i]+"\n").encode('utf-8'))
    # tm.sleep(1.2*gearRatio)
    msg = ser.read().decode('utf-8')
    while msg.find("x") == -1:
        msg = ser.read().decode('utf-8')
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    tm.sleep(0.1)
    prevNail = int(nail[i])

print("\nPrint Complete!\n")
tm.sleep(1)

# Shutdown Sequence
ser.write(("402\n").encode('utf-8'))  # Home Steppers
tm.sleep(1)
ser.write(("401\n").encode('utf-8'))  # Turn off Steppers
tm.sleep(0.1)
ser.reset_input_buffer()
ser.reset_output_buffer()
tm.sleep(0.2)

infile.close()
