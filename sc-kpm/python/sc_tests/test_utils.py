from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

def CreateNodeWithIdtf(ctx, _type, _idtf):
  addr = ctx.CreateNode(_type)
  ctx.HelperSetSystemIdtf(_idtf, addr)
  return addr
