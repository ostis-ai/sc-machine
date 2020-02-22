from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

class TestScSet(TestCase):

  def test_sc_set(self):
    ctx = TestScSet.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.NodeConst)
    addr2 = ctx.CreateNode(ScType.Node)
    addr3 = ctx.CreateNode(ScType.Node)

    edge = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)

    _set = ScSet(ctx, addr1)

    # check has element
    self.assertTrue(_set.Has(addr2))
    self.assertFalse(_set.Has(addr3))

    # check add element
    self.assertTrue(_set.Add(addr3))
    self.assertTrue(_set.Has(addr3))
    self.assertFalse(_set.Add(addr3))

    # check remove element
    self.assertTrue(_set.Remove(addr3))
    self.assertFalse(_set.Has(addr3))
    self.assertFalse(_set.Remove(addr3))
    self.assertTrue(_set.Has(addr2))

  def test_sc_set_clear(self):
    ctx = TestScSet.MemoryCtx()

    addrSet = ctx.CreateNode(ScType.Node)
    addr1 = ctx.CreateNode(ScType.NodeConst)
    addr2 = ctx.CreateNode(ScType.Node)
    addr3 = ctx.CreateNode(ScType.Node)

    elements = [addr1, addr2, addr3]

    _set = ScSet(ctx, addrSet)
    for el in elements:
      self.assertTrue(_set.Add(el))

    _set.Clear()

    for el in elements:
      self.assertFalse(_set.Has(el))

  def test_sc_set_iter(self):
    ctx = TestScSet.MemoryCtx()

    addrSet = ctx.CreateNode(ScType.Node)
    addr1 = ctx.CreateNode(ScType.NodeConst)
    addr2 = ctx.CreateNode(ScType.Node)
    addr3 = ctx.CreateNode(ScType.Node)

    elements = [addr1, addr2, addr3]

    _set = ScSet(ctx, addrSet)
    for a in elements:
      self.assertTrue(_set.Add(a))

    # iterate elements in set
    count = 0
    for el in _set:
      self.assertTrue(el in elements)
      count += 1
    self.assertEqual(count, len(elements))

  def test_sc_set_relation(self):
    ctx = TestScSet.MemoryCtx()

    addrSet = ctx.CreateNode(ScType.Node)
    relAddr = ctx.CreateNode(ScType.NodeConstNoRole)
    addr1 = ctx.CreateNode(ScType.NodeConst)
    addr2 = ctx.CreateNode(ScType.NodeConstClass)
    addr3 = ctx.CreateNode(ScType.NodeConstAbstract)

    elements = [addr1, addr2, addr3]

    _set = ScRelationSet(ctx, addrSet, relAddr)
    for a in elements:
      self.assertFalse(_set.Has(a))

    for a in elements:
      self.assertTrue(_set.Add(a))

    count = 0
    for el in _set:
      self.assertTrue(el in elements)
      count += 1
    self.assertEqual(count, len(elements))

    _set.Clear()
    for a in elements:
      self.assertFalse(_set.Has(a))
