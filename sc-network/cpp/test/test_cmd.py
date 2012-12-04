import socket
import struct
import random

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 55770))

def check_elements():
	print "Test element check"
	seg = 0

	alldata = ''

	for i in xrange(0, 100):
		offset = i

		print "check element: %d, %d" % (seg, offset)
		params = struct.pack('=HH', seg, offset)

		data = struct.pack('=BBII', 0x01, 0, i, len(params))

		alldata += data + params

	s.send(alldata)

	for i in xrange(0, 100):
		data = s.recv(10)
		cmdCode, cmdId, resCode, resSize = struct.unpack('=BIBI', data)
		print "code: %s, id: %d, resCode: %s, resSize: %d" % (hex(cmdCode), cmdId, hex(resCode), resSize)


def get_element_type():
	print "Test element type getting"

	seg = 0

	alldata = ''
	for i in xrange(100):
		offset = i
		print "get type for element: %d, %d" % (seg, offset)
		params = struct.pack('=HH', seg, offset)

		data = struct.pack('=BBII', 0x02, 0, i, len(params))
		alldata += data + params

	s.send(alldata)

	for i in xrange(0, 100):
		data = s.recv(12)
		cmdCode, cmdId, resCode, resSize, elType = struct.unpack('=BIBIH', data)
		print "code: %s, id: %d, resCode: %s, resSize: %d, elType: %s" % (hex(cmdCode), cmdId, hex(resCode), resSize, hex(elType))

def erase_elements():
	print "Test element erase"

	seg = 0

	alldata = ''

	for i in xrange(0, 10):
		seg = random.randrange(0, 100)
		offset = random.randrange(0, 65535)

		print "erase element: %d, %d" % (seg, offset)
		params = struct.pack('=HH', seg, offset)

		data = struct.pack('=BBII', 0x03, 0, i, len(params))

		alldata += data + params

	s.send(alldata)

	for i in xrange(0, 10):
		data = s.recv(10)
		cmdCode, cmdId, resCode, resSize = struct.unpack('=BIBI', data)
		print "code: %s, id: %d, resCode: %s, resSize: %d" % (hex(cmdCode), cmdId, hex(resCode), resSize)


def get_contents():
	print "Test sc-link get content"

	seg = 0

	alldata = ''

	for i in xrange(0, 100):
		seg = 0
		offset = i + 1

		print "get data for: %d, %d" % (seg, offset)
		params = struct.pack('=HH', seg, offset)

		data = struct.pack('=BBII', 0x09, 0, i, len(params))

		alldata += data + params

	s.send(alldata)

	for i in xrange(0, 100):
		data = s.recv(10)
		cmdCode, cmdId, resCode, resSize = struct.unpack('=BIBI', data)
		print "code: %s, id: %d, resCode: %s, resSize: %d\n" % (hex(cmdCode), cmdId, hex(resCode), resSize)
		content_data = None
		if resSize > 0:
			content_data = s.recv(resSize)
			print "Data: %s\n" % str(content_data)

check_elements()
get_element_type()
erase_elements()
get_contents()


# sutdown server
#s.send(struct.pack('=BBII', 0xfe, 0, 0, 0))
