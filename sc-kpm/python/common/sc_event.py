from sc import *
from scb import *

import threading
import json


class ScEventParams:

  def __init__(self, eid, addr, edge_addr, other_addr):
    self.id = eid
    self.addr = addr
    self.edge_addr = edge_addr
    self.other_addr = other_addr


class ScEvent:

  def __init__(self, evt, callback):
    self.evt = evt  # pointer to ScPythonEvent
    self.callback = callback

  def Emit(self, evt):
    if self.callback:
      self.callback(evt)

  def GetID(self):
    return self.evt.GetID()


'''This class is thread safe
'''


class ScEventManager:

  def __init__(self, cpp):
    self.cpp = cpp
    self.events = {}

  def CreateEventInternal(self, addr, evtType, callback):
    result = None
    try:
      evt = self.cpp.SubscribeEvent(addr, evtType)
      result = ScEvent(evt, callback)
      self.events[evt.GetID()] = result
    except:
      pass

    return result

  def DestroyEvent(self, evt):
    try:
      del self.events[evt.GetID()]
      evt.evt.Destroy()
    except KeyError:
      pass

  def CreateEventAddOutputEdge(self, addr: ScAddr, callback) -> ScEvent:
    return self.CreateEventInternal(addr, ScPythonEventType.AddOutputEdge, callback)

  def CreateEventAddInputEdge(self, addr: ScAddr, callback) -> ScEvent:
    return self.CreateEventInternal(addr, ScPythonEventType.AddInputEdge, callback)

  def CreateEventRemoveOutputEdge(self, addr: ScAddr, callback) -> ScEvent:
    return self.CreateEventInternal(addr, ScPythonEventType.RemoveOutputEdge, callback)

  def CreateEventRemoveInputEdge(self, addr: ScAddr, callback) -> ScEvent:
    return self.CreateEventInternal(addr, ScPythonEventType.RemoveInputEdge, callback)

  def CreateEventContentChanged(self, addr: ScAddr, callback) -> ScEvent:
    return self.CreateEventInternal(addr, ScPythonEventType.ContentChanged, callback)

  def CreateEventEraseElement(self, addr, callback):
    return self.CreateEventInternal(addr, ScPythonEventType.EraseElement, callback)

  def EmitEvent(self, evt_params):
    try:
      evt = self.events[evt_params.id]
      evt.Emit(evt_params)
    except KeyError:
      print("Can't find event: {}".format(evt_params.id))
