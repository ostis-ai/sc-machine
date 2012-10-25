import socket
import struct
import sc_memory as scm

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 55770))

def check_elements():
	print "Test element check"
	addr = scm._sc_addr()
	addr.seg = 0

	alldata = ''

	for i in xrange(0, 100):
		addr.offset = i

		print "check element: %d, %d" % (addr.seg, addr.offset)
		params = struct.pack('=HH', addr.seg, addr.offset)

		data = struct.pack('=BBII', 0x01, 0, i, len(params))

		alldata += data + params

	s.send(alldata)

	for i in xrange(0, 100):
		data = s.recv(10)
		cmdCode, cmdId, resCode, resSize = struct.unpack('=BIBI', data)
		print "code: %s, id: %d, resCode: %s, resSize: %d" % (hex(cmdCode), cmdId, hex(resCode), resSize)


def get_element_type():
	print "Test element type getting"

	addr = scm._sc_addr()
	addr.seg = 0

	alldata = ''
	for i in xrange(100):
		addr.offset = i
		print "get type for element: %d, %d" % (addr.seg, addr.offset)
		params = struct.pack('=HH', addr.seg, addr.offset)

		data = struct.pack('=BBII', 0x02, 0, i, len(params))
		alldata += data + params

	s.send(alldata)

	for i in xrange(0, 100):
		data = s.recv(12)
		cmdCode, cmdId, resCode, resSize, elType = struct.unpack('=BIBIH', data)
		print "code: %s, id: %d, resCode: %s, resSize: %d, elType: %s" % (hex(cmdCode), cmdId, hex(resCode), resSize, hex(elType))


check_elements()
get_element_type()


