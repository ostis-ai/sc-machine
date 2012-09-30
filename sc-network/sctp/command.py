
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
from types import CommandType, ScAddr, CommandId
import struct

class SctpCommand:
    """Base class for Sctp-command
    """
    
    def __init__(self, code, flags, id):
        """Construct command with header data
        """
        self.code = 0
        self.flags = 0
        self.id = 0
        
    def unpack(self, data):
        """Unpack command from binary data
        @param data: String that contains packed data
        """
        self.code, self.flags, self.id = struct.unpack('BBI')
    
    def pack(self):
        """Pack command into binary data
        @return: Returns string that contains packed data 
        """
        return struct.pack('BBI', self.code, self.flags, self.id)
    
    def size(self):
        """Returns size of the command in bytes
        """
        return ScAddr.size + 1 + CommandId.size
    
    @staticmethod
    def header_size(self):
        """Returns offset to arguments
        """
        return ScAddr.size + 1 + CommandId.size
    
class SctpCommandElementCheck(SctpCommand):
    
    def __init__(self, code, flags, id, addr):
        """Construct command to check if specified sc-element exist
        @param addr: sc-addr of sc-element to check 
        """
        SctpCommand.__init__(self, code, flags, id)
        self.addr = addr
        
    def unpack(self, data):
        h_size = self.header_size()       
        
        # unpack header
        SctpCommand.unpack(self, data[:h_size])
        
        # unpack arguments
        if self.addr is None:
            self.addr = ScAddr()
        self.addr.unpack(data[h_size:])
    
    def pack(self):
        return SctpCommand.pack(self) + self.addr.pack()
    
    def size(self):
        return SctpCommand.size(self) + ScAddr.size