#!/usr/bin/python

import sys
from time import sleep
import socket

def help():
	print("{}: <host>\n".format(sys.argv[0]), file = sys.stderr)
	exit(1)

if len(sys.argv) < 2:
	help()

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = (sys.argv[1], 23)  # 23 is the default telnet port
print('connecting to {} port {}'.format(server_address[0], server_address[1]), file = sys.stderr)
sock.connect(server_address)

for i in range(10):
	print('On', file = sys.stderr)
	sock.sendall(b"led on\n")
	sleep(1)
	print('Off', file = sys.stderr)
	sock.sendall(b"led off\n")
	sleep(1)

sock.close()
print('Done', file = sys.stderr)
print('\nFor interactive command line interface use telnet or PuTTY from https://putty.org/' , file = sys.stderr)
print('For bash scripts use nc' , file = sys.stderr)
