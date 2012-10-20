
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
from sctp.types import ResultCode

class SctpResult:
    
    def __init__(self, cmd_id = 0, result_code = ResultCode.No):    
        self.cmd_id = cmd_id
        self.result_code = result_code
        
     
    def code(self):
        """Returns code of command that returned this result
        @note: This function must to be override in a subclass
        """
        return None
    
    def __str__(self):
        return "Result for command (Type: %s, Id: %s, Result: %s)" % (str(self.code()), 
                                                                      str(self.cmd_id), 
                                                                      str(self.result_code))

    @staticmethod
    def header_size():
        """Returns offset to results
        """
        return 2 + CommandId.size    

    def unpack(self, data):
        """Unpack result from binary data
        @param data: String that contains packed data
        @attention: Do not override this function
        """
        h_size = self.header_size()
        print struct.calcsize('!BIB')
        code, self.cmd_id, self.result_code = struct.unpack('!BIB', data[:h_size])        
        assert code == self.code()
        
        self.unpack_values(data[h_size:])
    
    def pack(self):
        """Pack result into binary data
        @return: Returns string that contains packed data
        @attention: Do not override this function
        """
        args = self.pack_values()
        res = struct.pack('!BIB', self.code(), self.cmd_id, self.result_code)
        if args is not None:
            res += args
        return res
    
    def size(self):
        """Returns size of the command in bytes
        """
        return self.header_size() + self.size_values()
    
    def pack_values(self):
        """Pack result values into binary data
        @return: Returns string that contains packed result values. If there
        are no values for specified result, then this function must return None
        @note: This function must be override for each type of result
        """
        return None
    
    def unpack_values(self, data):
        """Unpack result values from binary data
        @param data: String that contains packed values
        @note: This function must be override for each type of result
        """
        pass
    
    def size_values(self):
        """Returns size of arguments (in bytes)
        @note: This function must be override for each type of result
        """
        return 0
    
class SctpResultElementCheck(SctpResult):
    
    def __init__(self, cmd_id = 0, result_code = ResultCode.No):
        SctpResult.__init__(self, cmd_id, result_code)
        
    def code(self):
        return CommandType.element_check
        
    def __str__(self):
        return "Check sc-element result (Command type: %s, Command id: %s, Result: %s)" % (str(self.code()),
                                                                                           str(self.cmd_id),
                                                                                           self.result_code)
        