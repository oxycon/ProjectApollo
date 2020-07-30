#!/usr/bin/python

# pip install pyserial


import sys
from time import sleep
import serial

def help():
	print("{}: <COM-port>\n".format(sys.argv[0]), file = sys.stderr)
	exit(1)

if len(sys.argv) < 2:
	help()

ser = serial.Serial(
    port=sys.argv[1],
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

print('connecting to {}'.format(sys.argv[1]), file = sys.stderr)
ser.isOpen()

for i in range(10):
	print('On', file = sys.stderr)
	ser.write(b"led on\n")
	sleep(1)
	print('Off', file = sys.stderr)
	ser.write(b"led off\n")
	sleep(1)

ser.close()
print('Done', file = sys.stderr)
