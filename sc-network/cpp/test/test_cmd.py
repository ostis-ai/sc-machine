import socket
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 55770))

for cmd in xrange(10):
	data = struct.pack('!BBII', cmd, 0, 1, 0)
	s.send(data)

#print s.recv(1024)
