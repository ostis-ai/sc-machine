from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

class TestScAddr(TestCase):

  def test_compare(self):

    addr1 = ScAddr()
    addr2 = ScAddr()

    self.assertEqual(addr1, addr2)

    ctx = TestScAddr.MemoryCtx()
    addr2 = ctx.CreateNode(ScType.Unknown)

    self.assertNotEqual(addr1, addr2)

  def test_is_valid(self):
    addr1 = ScAddr()

    self.assertFalse(addr1.IsValid())

    ctx = TestScAddr.MemoryCtx()
    addr2 = ctx.CreateNode(ScType.Const)

    self.assertTrue(addr2.IsValid())

  def test_to_int(self):
    addr1 = ScAddr()

    self.assertEqual(addr1.ToInt(), 0)

    ctx = TestScAddr.MemoryCtx()
    addr2 = ctx.CreateNode(ScType.Const)

    self.assertNotEqual(addr2.ToInt(), 0)


class TestScType(TestCase):

  def test_compare(self):
    type1 = ScType.Node
    type2 = ScType.Link

    self.assertNotEqual(type1, type2)

    type2 = ScType.NodeConst
    type3 = type1 | ScType.Const

    self.assertEqual(type2, type3)

    type4 = type3 & type1

    self.assertEqual(type1, type4)

  def test_to_int(self):
    v = ScType.Node.ToInt()
    self.assertGreater(v, 0)
    self.assertEqual(v, 1)

  def test_is_valid(self):
    type1 = ScType.Node

    self.assertFalse(type1.IsUnknown())

    type2 = ScType()

    self.assertTrue(type2.IsUnknown())

  def test_is_func(self):

    typeNode = ScType.NodeConst

    self.assertTrue(typeNode.IsNode())
    self.assertTrue(typeNode.IsConst())
    self.assertFalse(typeNode.IsEdge())
    self.assertFalse(typeNode.IsLink())

    typeEdge = ScType.EdgeAccessVarPosTemp

    self.assertTrue(typeEdge.IsEdge())
    self.assertTrue(typeEdge.IsVar())
    self.assertFalse(typeEdge.IsConst())
    self.assertFalse(typeEdge.IsNode())
    self.assertFalse(typeEdge.IsLink())

    typeLink = ScType.Link

    self.assertTrue(typeLink.IsLink())
    self.assertFalse(typeLink.IsConst())
    self.assertFalse(typeLink.IsVar())
    self.assertFalse(typeLink.IsEdge())
    self.assertFalse(typeLink.IsNode())
