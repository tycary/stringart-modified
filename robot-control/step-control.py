import time as tm
import serial


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
for i in range(50):
    print(i)
    ser.write((nail[i]+"\n").encode('utf-8'))
    tm.sleep(1.1)
    ser.reset_output_buffer()

infile.close()
