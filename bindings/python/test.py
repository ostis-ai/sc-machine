from sc_memory import *

sc_memory_initialize("repo")

# create some nodes
nodes = []
print "Test nodes..."
for i in xrange(100):
	addr = sc_memory_node_new(0)
	nodes.append(addr)

print "Test arcs..."
for i in xrange(100):
	for j in xrange(100):
		addr = sc_memory_arc_new(sc_type_arc_common, nodes[i], nodes[j])

print "Test contents..."
for i in xrange(5):
	addr = sc_memory_link_new()
	stream = sc_stream_file_new("test.py", SC_STREAM_READ)
	sc_memory_set_link_content(addr, stream)
	sc_stream_free(stream)



sc_memory_shutdown()
