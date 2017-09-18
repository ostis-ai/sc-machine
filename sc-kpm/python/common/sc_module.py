from common.sc_keynodes import ScKeynodes
from common.sc_exception import ScKeynodeException
from common.sc_event import ScEventManager, ScEventParams
import urllib.request
import asyncio
import time, sys, traceback

from scb import *

class ScModule:

    def __init__(self, context,  cpp_bridge, keynodes=[]):
        self.sc_context = context
        self.keynodes = ScKeynodes(self.sc_context)
        
        self.cpp = cpp_bridge
        assert self.cpp

        self.events = ScEventManager(self.cpp)
        self.KeynodesCheck(keynodes)

        self.loop = asyncio.SelectorEventLoop()
        asyncio.set_event_loop(self.loop)

    def KeynodesCheck(self, keynodes_list):
        for idtf in keynodes_list:
            addr = self.keynodes[idtf]
            if not addr:
                raise ScKeynodeException(idtf)

    # --- handlers calls in main thread ---
    def HandleOnClose(self):
        self.loop.call_soon_threadsafe(self.DoLoopStop)

    def HandleOnEvent(self, eid, addr, edge_addr, other_addr):
        params = ScEventParams(eid, addr, edge_addr, other_addr)
        self.loop.call_soon_threadsafe(self.DoEmitEvent, params)

    # --- tasks that ---
    def DoLoopStop(self):
        self.loop.stop()

    def DoEmitEvent(self, evt_params):
        self.events.EmitEvent(evt_params)

    # --- overloads ---
    def OnInitialize(self, params):
        """This function calls when module initialized.
        You should overload it to run code of your module on initialize
        """
        pass

    def OnShutdown(self):
        pass

    # --- common state functions ---
    def Initialize(self):
        self.cpp.onClose = self.HandleOnClose
        self.cpp.onEvent = self.HandleOnEvent
        # notify c++ code that bridge is ready for work
        self.cpp.Ready()

        self.OnInitialize(self.cpp.InitParams())

    def Shutdown(self):
        self.OnShutdown()
        self.cpp.onClose = None
        self.cpp.onEvent = None
        self.cpp.Finish()

    def Run(self):
        if self.cpp:
            # wait until cpp bridge will be initialized
            self.Initialize()
            self.loop.run_forever()
            self.Shutdown()

    # Set of usefull functions
    @staticmethod
    def GetDataByUrl(url):
        try:
            with urllib.request.urlopen(url) as response:
                data = response.read()
        except:
            return None

        return data