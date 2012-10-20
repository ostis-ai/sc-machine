
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
from sctp.types import CommandType


class SctpCommand:
    """Base class for Sctp-command
    """    
    def __init__(self, flags = 0, cmd_id = 0):
        """Construct command with header data
        """
        self.flags = flags
        self.id = cmd_id
                
    def unpack(self, data):
        """Unpack command from binary data
        @param data: String that contains packed data
        @attention: Do not override this function
        """
        h_size = self.header_size()
        code, self.flags, self.id = struct.unpack('!BBI', data[:h_size])        
        assert code == self.code()
        
        self.unpack_arguments(data[h_size:])
    
    def pack(self):
        """Pack command into binary data
        @return: Returns string that contains packed data
        @attention: Do not override this function
        """
        args = self.pack_arguments()
        res = struct.pack('!BBI', self.code(), self.flags, self.id)
        if args is not None:
            res += args
        return res
    
    def size(self):
        """Returns size of the command in bytes
        """
        return self.header_size() + self.size_arguments()
    
    def pack_arguments(self):
        """Pack arguments of specified command into binary data
        @return: Returns string that contains packed arguments. If there
        are no arguments for specified command, then this function must return None
        @note: This function must be override for each type of command
        """
        return None
    
    def unpack_arguments(self, data):
        """Unpack command arguments from binary data
        @param data: String that contains packed arguments
        @note: This function must be override for each type of command
        """
        pass
    
    def size_arguments(self):
        """Returns size of arguments (in bytes)
        @note: This function must be override for each type of command
        """
        return 0
    
    @staticmethod
    def header_size():
        """Returns offset to arguments
        """
        return 2 + CommandId.size
    
    def code(self):
        """Returns command code
        """
        return 0
    
    def _str_name_(self):
        """Returns string name of command
        """
        return "Base"
    
    def _str_args_(self):
        """Returns string representation of arguments
        """
        return "None"
    
    def __str__(self):
        return self._str_name_() + " command (" + SctpCommand.__str__(self) + " Arguments: %s" % self._str_args_() + ')'
    

class SctpCommandBaseOneScAddr(SctpCommand):
    """Base class for commands that work with one sc_addr as argument
    """
    def __init__(self, flags = 0, cmd_id = 0, addr = None):
        SctpCommand.__init__(self, flags, cmd_id)
        self.addr = addr
        
    def unpack_arguments(self, data):       
        # unpack arguments
        if self.addr is None:
            self.addr = ScAddr()
        self.addr.unpack(data)
    
    def pack_arguments(self):
        assert self.addr is not None
        return self.addr.pack()
    
    def size_arguments(self):
        return ScAddr.size
    
    def _str_args_(self):
        return str(self.addr)
    
    
class SctpCommandElementCheck(SctpCommandBaseOneScAddr):
    """Command to check if specified sc-element exist
    """
    def code(self):
        return CommandType.element_check
    
    def _str_name_(self):
        return "Check sc-element"
    
class SctpCommandElementGetType(SctpCommandBaseOneScAddr):
    """Command to get type of specified sc-element
    """
    def code(self):
        return CommandType.element_get_type
    
    def _str_name_(self):
        return "Get type of sc-element"

class SctpCommandElementFree(SctpCommandBaseOneScAddr):
    """Command to delete specified sc-element
    """
    def code(self):
        return CommandType.element_free
    
    def _str_name_(self):
        return "Delete sc-element"

# map to convert command type into class that implements specified command
code2command = {CommandType.element_check: SctpCommandElementCheck,
                CommandType.element_get_type: SctpCommandElementGetType,
                CommandType.element_free: SctpCommandElementFree,
                
                }