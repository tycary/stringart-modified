import time as tm
import serial


# Set gear ratio
gearRatio = 1.0

# COM Settings
comChannel = "com3"
Baudrate = 115200
to = 10  # Timeout

# COM initializing
ser = serial.Serial(comChannel, Baudrate, timeout=to)
tm.sleep(0.5)  # Wait for com
ser.reset_input_buffer()
ser.reset_output_buffer()
tm.sleep(0.2)
print("Communication Ready")
ser.flush()
tm.sleep(2)

# Main
infile = open('stringart/output/NailOutput.txt', 'r')
commands = infile.readline()
nails = commands.split(',')
# Send gear ratio and home wheel
ser.write((str(gearRatio)+"\n").encode('utf-8'))
ser.reset_output_buffer()
print("Gear Ratio Set")
tm.sleep(0.5)

# Home Wheel
ser.write(("402\n").encode('utf-8'))
print("Homing")
# Wait for Homing
tm.sleep(0.5*gearRatio + 0.5)
ser.reset_output_buffer()
print("Homed")
tm.sleep(0.1)

prevNail = 0
# Send nails to Arduino
curIt = 0
print("Printing\n")
for n in nails:
    prevTime = tm.time()
    print("Iteration: " + str(curIt) + " | Current nail: " + n)
    ser.write((n+"\n").encode('utf-8'))
    msg = ser.read().decode('utf-8')
    while msg.find("x") == -1:
        msg = ser.read().decode('utf-8')
        # Resend Code if arduino takes too long
        if (tm.time() > (prevTime + abs(int(n) - prevNail)*gearRatio/400.0 + 1)):
            print("Nail failed...Execution Restarting")
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            ser.flush()
            tm.sleep(3)
            ser.write((n+"\n").encode('utf-8'))
            prevTime = tm.time()
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    tm.sleep(0.6)
    prevNail = int(n)
    curIt += 1

print("\nPrint Complete!\n")
tm.sleep(1)

# Shutdown Sequence
ser.write(("401\n").encode('utf-8'))  # Turn off Steppers
tm.sleep(0.1)
ser.reset_input_buffer()
ser.reset_output_buffer()
tm.sleep(0.2)

ser.close()
infile.close()
