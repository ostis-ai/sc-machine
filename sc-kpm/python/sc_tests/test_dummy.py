from scb import *
from sc import *
from common import *

import time
import random


class CleanModule(ScModule):

  def __init__(self):
    ScModule.__init__(self, __ctx__, keynodes=[
    ], cpp_bridge=__cpp_bridge__)

  def OnInitialize(self, params):
    time.sleep(random.random() + 0.01)
    self.CallLater(self.Stop)

  def OnShutdown(self):
    pass


service = CleanModule()
service.Run()
