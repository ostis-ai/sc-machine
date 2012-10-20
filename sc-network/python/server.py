
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
import SocketServer
import sctp.command
import sctp.result
import struct
from sctp.types import ResultCode
import sc_memory
from sctp.command import SctpCommand

HOST = ''
PORT = 50987
server = None



def make_sc_addr(addr):
    _addr = sc_memory._sc_addr()
    
    _addr.seg = addr.seg
    _addr.offset = addr.offset
    
    return _addr            

def start_server():
    """Starts sc-memory network server
    """
    sc_memory_initialize("repo")
    
    
def stop_server():
    """Stops sc-memory server
    """
    global clients
    for sock in clients.keys():
        sock.close()
    
    sc_memory_shutdown()
    
class ForkableTCPRequestHandler(SocketServer.BaseRequestHandler):
    
    def setup(self):
        print self.client_address, ' connected'
    
    def finish(self):
        print self.client_address, ' disconnected'
    
    def handle(self):
        data = ''
        
        self.workable = True
        self.cmd_funcs = {sctp.types.CommandType.element_check: self.do_element_check,
                          }
        
        while self.workable:
            data += self.request.recv(4096)
            data = self.process_data(data)
        
    def process_data(self, data):
        """Process recieved command, and return response
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
                    self.run_command(cmd)
                    data = data[cmd_size:]
            else:
                print 'Unknown command type %s' % hex(cmd_id)
                self.workable = False
                break
            
        return data
    
    def run_command(self, cmd):
        """Run specified command
        """
        cmd_type = cmd.code()
        result = None
        
        if self.cmd_funcs.has_key(cmd_type):
            result = self.cmd_funcs[cmd_type](cmd)
        else:
            print "Command isn't implemented:\n%s" % str(cmd)
            
        # send result
        res_data = result.pack()
        res_data_len = len(res_data)
        total_send = 0
        self.request.sendall(res_data[total_send:])
    
    def do_element_check(self, cmd):
        
        if sc_memory_is_element(make_sc_addr(cmd.addr)) == SC_TRUE:
            return sctp.result.SctpResultElementCheck(cmd.id, ResultCode.Yes)
        
        return sctp.result.SctpResultElementCheck(cmd.id, ResultCode.No)
    
class ForkableTCPServer(SocketServer.ForkingMixIn, SocketServer.TCPServer):
    
    def __init__(self, host, RequestHandler):
        SocketServer.TCPServer.allow_reuse_address = True
        SocketServer.TCPServer.request_queue_size = 10
        SocketServer.TCPServer.socket_type = socket.SOCK_STREAM
    
        SocketServer.TCPServer.__init__(self, host, RequestHandler)
    
    def server_activate(self):
        sc_memory_initialize("repo")
        
        return SocketServer.TCPServer.server_activate(self)
    
    def shutdown(self):
        
        res = SocketServer.TCPServer.shutdown(self)
        
        sc_memory_shutdown()
        return res
    
if __name__ == "__main__":
    
    global server
        
    server = ForkableTCPServer((HOST, PORT), ForkableTCPRequestHandler)
    ip, port = server.server_address
    
    # Start a thread with the server -- that thread will then start one
    # more thread for each request
    #server_thread = threading.Thread(target = server.serve_forever)
    # Exit the server thread when the main thread terminates
    #server_thread.daemon = True
    #server_thread.start()
    
    print "Sctp-server start on %s:%s" % (str(ip), str(port))
    server.serve_forever()
    server.shutdown()
    
