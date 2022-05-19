from unittest import defaultTestLoader, TestCase, TextTestRunner, TestSuite

from sc_tests.test_common import TestScAddr, TestScType
from sc_tests.test_events import TestEvents
from sc_tests.test_helper import TestScHelper
from sc_tests.test_memory_ctx import TestScMemoryContext
from sc_tests.test_set import TestScSet
from sc_tests.test_agent import TestScAgent

from sc_tests.test_utils import CreateNodeWithIdtf

from common import *
from sc import *

import sys

sys.stdout = sys.__stdout__
sys.stderr = sys.__stderr__

module = None

def MemoryCtx() -> ScMemoryContext:
  return __ctx__


def RunTests():
  global TestLoader, TextTestRunner, MemoryCtx
  
  tests = [
    TestScAddr,
    TestScAgent,
    TestScType,
    TestScMemoryContext,
    TestScSet,
    TestEvents,
    TestScHelper,
    ]

  for testItem in tests:
    testItem.MemoryCtx = MemoryCtx
    testItem.module = module
    suite = defaultTestLoader.loadTestsFromTestCase(testItem)
    res = TextTestRunner(verbosity=2).run(suite)
    if not res.wasSuccessful():
      raise Exception("Unit test failed")

class TestModule(ScModule):
  def __init__(self):
    ScModule.__init__(self,
                      ctx=__ctx__,
                      cpp_bridge=__cpp_bridge__,
                      keynodes=[
                      ])

  def DoTests(self):
    try:
      RunTests()
    except Exception as ex:
      raise ex
    except:
      print("Unexpected error:", sys.exc_info()[0])
    finally:
      module.Stop()

  def OnInitialize(self, params):
    self.DoTests()

  def OnShutdown(self):
    pass


module = TestModule()
module.Run()
