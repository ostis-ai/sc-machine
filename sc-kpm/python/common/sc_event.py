from sc import *
from scb import *

import threading
import json

class ScEvent:

    def __init__(self, evt, callback):
        self.evt = evt  # pointer to ScPythonEvent
        self.callback = callback

    def Emit(self, addr, edgeAddr, otherAddr):
        if self.callback:
            self.callback(addr, edgeAddr, otherAddr)

    def GetID(self):
        return self.evt.GetID()


'''This class is thread safe
'''
class ScEventManager:

    def __init__(self, cpp):
        self.cpp = cpp
        self.events = {}
        self.lock = threading.Lock()

    def CreateEventInternal(self, addr, evtType, callback):
        self.lock.acquire()
        result = None
        try:
            evt = self.cpp.SubscribeEvent(addr, evtType)
            result = ScEvent(evt, callback)
            self.events[evt.GetID()] = result
        except:
            pass
        finally:
            self.lock.release()

        return result

    def DestroyEvent(self, evt):
        self.lock.acquire()

        try:
            del self.events[evt.GetID()]
            evt.evt.Destroy()
        except KeyError:
            pass
        finally:
            self.lock.release()

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
        self.lock.acquire()

        try:
            data = json.loads(data)

            evtID = int(data['id'])
            addr = ScAddrFromHash(int(data['addr']))
            edgeAddr = ScAddrFromHash(int(data['edgeAddr']))
            otherAddr = ScAddrFromHash(int(data['otherAddr']))

            # find and emit event
            if evtID in self.events:
                evt = self.events[evtID]
                evt.Emit(addr, edgeAddr, otherAddr)
        except:
            pass
        finally:
            self.lock.release()
