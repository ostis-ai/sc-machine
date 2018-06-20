from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

def CreateNodeWithIdtf(ctx, _type, _idtf):
  addr = ctx.CreateNode(_type)
  linkAddr = ctx.CreateLink()

  ctx.SetLinkContent(linkAddr, _idtf)
