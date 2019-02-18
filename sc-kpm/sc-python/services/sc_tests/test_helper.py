from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

class TestScHelper(TestCase):

  def test_setRelationLink(self):
    ctx = TestScHelper.MemoryCtx()

    addr = ctx.CreateNode(ScType.NodeConst)
    relAddr = ctx.CreateNode(ScType.NodeConstNoRole)

    templ = ScTemplate()
    templ.TripleWithRelation(
        addr,
        ScType.EdgeDCommonVar,
        ScType.Link >> '_link',
        ScType.EdgeAccessVarPosPerm,
        relAddr)

    searchRes = ctx.HelperSearchTemplate(templ)
    self.assertEqual(searchRes.Size(), 0)

    def check_value(value):
      searchRes = ctx.HelperSearchTemplate(templ)
      self.assertEqual(searchRes.Size(), 1)

      data = ctx.GetLinkContent(searchRes[0]['_link'])
      self.assertEqual(data.AsString(), value)

    helper = ScHelper(ctx)
    helper.kbSetRelationLinkValue(addr, relAddr, 'test_data')
    check_value('test_data')

    helper.kbSetRelationLinkValue(addr, relAddr, 'test_data2')
    check_value('test_data2')

  def test_replaceBinaryRelation(self):
    ctx = TestScHelper.MemoryCtx()

    addr = ctx.CreateNode(ScType.NodeConst)
    relAddr = ctx.CreateNode(ScType.NodeConstNoRole)
    node1 = ctx.CreateNode(ScType.NodeConstAbstract)
    node2 = ctx.CreateNode(ScType.NodeConstMaterial)

    templ = ScTemplate()
    templ.TripleWithRelation(
        addr,
        ScType.EdgeDCommonVar,
        ScType.Unknown >> '_trg',
        ScType.EdgeAccessVarPosPerm,
        relAddr)

    searchRes = ctx.HelperSearchTemplate(templ)
    self.assertEqual(searchRes.Size(), 0)

    def check_trg(trg):
      searchRes = ctx.HelperSearchTemplate(templ)
      self.assertEqual(searchRes[0]['_trg'], trg)

    helper = ScHelper(ctx)
    helper.kbReplaceBinaryRelation(addr, relAddr, node1)
    check_trg(node1)

    helper.kbReplaceBinaryRelation(addr, relAddr, node2)
    check_trg(node2)

  def test_getRelationLinkValue(self):
    ctx = TestScHelper.MemoryCtx()

    addr = ctx.CreateNode(ScType.NodeConst)
    relAddr = ctx.CreateNode(ScType.NodeConstNoRole)

    templ = ScTemplate()
    templ.TripleWithRelation(
        addr,
        ScType.EdgeDCommonVar,
        ScType.Link >> '_link',
        ScType.EdgeAccessVarPosPerm,
        relAddr)

    searchRes = ctx.HelperSearchTemplate(templ)
    self.assertEqual(searchRes.Size(), 0)

    helper = ScHelper(ctx)
    helper.kbSetRelationLinkValue(addr, relAddr, 'test_data')
    value = helper.kbGetRelationLinkValue(addr, relAddr).AsString()

    self.assertEqual(value, 'test_data')