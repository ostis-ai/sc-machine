from unittest import TestCase
from datetime import datetime

from common import *
from sc import *

from sc_tests.test_utils import *

import time

class TestEvents(TestCase):

  def test_events(self):
    ctx = TestEvents.MemoryCtx()
    events = TestEvents.module.events

    def waitTimeout(seconds, checkFunc):
      start = datetime.now()
      delta = 0

      while not checkFunc() and delta < seconds:
        TestEvents.module.EmitEvents()
        delta = (datetime.now() - start).seconds
        time.sleep(0.1)

    addr1 = ctx.CreateNode(ScType.NodeConst)
    addr2 = ctx.CreateNode(ScType.NodeConst)

    class EventCheck:
      def __init__(self):
        self.passed = False

      def onEvent(self, evt_params):
        print('onEvent')
        self.passed = True

      def isPassed(self):
        return self.passed

    check = EventCheck()

    evtAddOutputEdge = events.CreateEventAddOutputEdge(
        addr1, check.onEvent)

    # emit event and wait
    edge1 = ctx.CreateEdge(ScType.EdgeAccess, addr1, addr2)
    waitTimeout(3, check.isPassed)

    self.assertTrue(check.isPassed())