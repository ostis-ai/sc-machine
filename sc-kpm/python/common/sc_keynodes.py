from sc import *

class ScKeynodes:

    def __init__(self, context):
        self.context = context
        self.resolved = {}

    def __getitem__(self, sys_idtf):
        if sys_idtf in self.resolved:
            return self.resolved[sys_idtf]

        addr = self.context.HelperResolveSystemIdtf(sys_idtf, ScType.Unknown)
        if addr:
            self.resolved[sys_idtf] = addr

        return addr
