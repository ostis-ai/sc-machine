from common.sc_keynodes import ScKeynodes
from common.sc_exception import ScKeynodeException
from common.sc_event import ScEventManager, ScEventParams
from common.sc_agent import ScAgent

import urllib.request
import time, sys, traceback
import queue

from scb import *

class Task:
    def __init__(self, func, *args):
        self.func = func
        self.args = args

    def do(self):
        self.func(*self.args)

class ScModule:

    def __init__(self, context,  cpp_bridge, keynodes=[]):
        self.sc_context = context
        self.keynodes = ScKeynodes(self.sc_context)
        
        self.cpp = cpp_bridge
        assert self.cpp

        self.events = ScEventManager(self.cpp)
        self.KeynodesCheck(keynodes)

        self.is_running = True
        self.task_queue = queue.Queue()

        self.agents = [] # list of registered agents

    def KeynodesCheck(self, keynodes_list):
        for idtf in keynodes_list:
            addr = self.keynodes[idtf]
            if not addr:
                raise ScKeynodeException(idtf)

    # --- handlers calls in main thread ---
    def HandleOnClose(self):
        self.task_queue.put(Task(self.Stop))

    def HandleOnEvent(self, eid, addr, edge_addr, other_addr):
        params = ScEventParams(eid, addr, edge_addr, other_addr)
        self.task_queue.put(Task(self.DoEmitEvent, params))

    # --- tasks ---
    def DoEmitEvent(self, evt_params):
        self.events.EmitEvent(evt_params)

    # --- overloads ---
    def OnInitialize(self, params):
        """This function calls when module initialized.
        You should overload it to run code of your module on initialize
        """
        pass

    def OnUpdate(self):
        pass

    def OnShutdown(self):
        pass

    def Stop(self):
        self.is_running = False

    # --- common state functions ---
    def Initialize(self):
        self.cpp.onClose = self.HandleOnClose
        self.cpp.onEvent = self.HandleOnEvent
        # notify c++ code that bridge is ready for work
        self.cpp.Ready()

        ScAgent.InitGlobal(self)
        self.OnInitialize(self.cpp.InitParams())

    def Shutdown(self):
        self.OnShutdown()
        self.cpp.onClose = None
        self.cpp.onEvent = None
        self.cpp.Finish()

    def EmitEvents(self):
        try:
            task = self.task_queue.get(block=True, timeout=0.01)
            task.do()
        except queue.Empty:
            pass

    def Run(self):
        if self.cpp:
            # wait until cpp bridge will be initialized
            self.Initialize()
            while self.is_running:
                self.EmitEvents()
                self.OnUpdate()

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