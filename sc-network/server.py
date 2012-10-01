
"""
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
"""
from sc_memory import *
import socket
import threading
import sctp.command
import struct

HOST    =   ''
PORT    =   50987
serversocket  =   None

# map that contains information about all clients
clients = {}

class ClientThread(threading.Thread):
    
    def __init__(self, client_socket):
        
        self.socket = client_socket
        self.workable = True
    
    def run(self):
        
        data = ''
        
        while self.workable:
            data += self.socket.recv(4096)
            data = self.process_data(data) 
            
    def process_data(self, data):
        """Process recieved data, and return non-processed buffer
        """
        can_process = True
        while len(data) > 0 and can_process:
            
            data_len = len(data)
            # determine command and check if there are all data we need to process it
            cmd_id = struct.unpack('B', data[0])[0]
            
            # check if specified command can be processed
            if sctp.command.code2command.has_key(cmd_id):
                cmd = sctp.command.code2command[cmd_id]()
                cmd_size = cmd.size()
                
                if cmd_size > data_len:
                    can_process = False
                else:
                    cmd.unpack(data[:cmd_size])
                    print str(cmd)
                    data = data[cmd_size:]
            else:
                print 'Unknown command type %s' % hex(cmd_id)
                self.workable = False
                break
            
        return data

def start_server():
    """Starts sc-memory network server
    """
    sc_memory_initialize("repo")
    
    global serversocket
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    serversocket.bind((HOST, PORT))
    serversocket.listen(5)
    
    global clients
    
    print "Listen clients..."
    while 1:
        #accept connections from outside
        (clientsocket, address) = serversocket.accept()
        print "Accept client connection from %s" % str(address)
        
        assert not clients.has_key(clientsocket)
        th = ClientThread(clientsocket)
        clients[clientsocket] = th
        th.run()
    
def stop_server():
    """Stops sc-memory server
    """
    global clients
    for sock in clients.keys():
        sock.close()
    
    sc_memory_shutdown()

if __name__ == "__main__":
    start_server()