from sc import *
from scb import *

import json

class ScEvent:

    def __init__(self, evt, callback):
        self.evt = evt  # pointer to ScPythonEvent
        self.callback = callback

    def Emit(self, addr, edgeAddr, otherAddr):
        if self.callback:
            self.callback(addr, edgeAddr, otherAddr)

class ScEventManager:

    def __init__(self, cpp):
        self.cpp = cpp
        self.events = {}

    def CreateEventInternal(self, addr, evtType, callback):
        evt = self.cpp.SubscribeEvent(addr, evtType)
        result = ScEvent(evt, callback)
        self.events[evt.GetID()] = result

        return result

    def CreateEventAddOutputEdge(self, addr, callback):
        return self.CreateEventInternal(addr, ScPythonEventType.AddOutputEdge, callback)

    def CreateEventAddInputEdge(self, addr, callback):
        return self.CreateEventInternal(addr, ScPythonEventType.AddInputEdge, callback)

    def CreateEventRemoveOutputEdge(self, addr, callback):
        return self.CreateEventInternal(addr, ScPythonEventType.RemoveOutputEdge, callback)

    def CreateEventRemoveInputEdge(self, addr, callback):
        return self.CreateEventInternal(addr, ScPythonEventType.RemoveInputEdge, callback)

    def CreateEventContentChanged(self, addr, callback):
        return self.CreateEventInternal(addr, ScPythonEventType.ContentChanged, callback)

    def CreateEventEraseElement(self, addr, callback):
        return self.CreateEventInternal(addr, ScPythonEventType.EraseElement, callback)

    def ProcessEvent(self, data):
        data = json.loads(data)

        evtID = int(data['id'])
        addr = ScAddrFromHash(int(data['addr']))
        edgeAddr = ScAddrFromHash(int(data['edgeAddr']))
        otherAddr = ScAddrFromHash(int(data['otherAddr']))

        # find and emit event
        if evtID in self.events:
            evt = self.events[evtID]
            evt.Emit(addr, edgeAddr, otherAddr)

