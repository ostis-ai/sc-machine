
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
import struct

class ScAddr:
    size        =   4
    
    def __init__(self):
        self.segment = 0
        self.offset = 0
        
    def unpack(self, data):
        self.segment, self.offset = struct.unpack('!HH', data)
        
    def pack(self):
        return struct.pack('!HH', self.segment, self.offset)
    
    def __str__(self):
        return "(seg: %s, offset: %s)" % (str(self.segment), str(self.offset))
    
class CommandId:
    size        =   4

class CommandType:
    
    element_check       =   0x01    # check if sc-element exists
    element_get_type    =   0x02    # get type of spceified sc-element
    element_free        =   0x03    # free specified sc-element
    node_new            =   0x04    # create new sc-node
    link_new            =   0x05    # create new sc-link
    arc_new             =   0x06    # create new sc-arc
    arc_get_begin       =   0x07    # get begin element of specified sc-arc
    arc_get_end         =   0x08    # get end element of specified sc-arc
    
class ResultCode:
    
    Ok                  =   0x00    # there are no any error
    InvalidScAddr       =   0x01    # element with specified sc-addr doesn't exist