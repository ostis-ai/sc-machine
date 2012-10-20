import socket
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 50987))

data = struct.pack('!BBII', 0x01, 0, 1, 1)
s.send(data)

print s.recv(1024)
