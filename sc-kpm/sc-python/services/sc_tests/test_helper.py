from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

class TestScHelper(TestCase):

  def test_setBinaryRelationLink(self):
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
    helper.kbSetBinaryRelationLinkValue(addr, relAddr, 'test_data')
    check_value('test_data')

    helper.kbSetBinaryRelationLinkValue(addr, relAddr, 'test_data2')
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

  def test_getBinaryRelationLinkValue(self):
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
    helper.kbSetBinaryRelationLinkValue(addr, relAddr, 'test_data')
    value = helper.kbGetBinaryRelationLinkValue(addr, relAddr).AsString()

    self.assertEqual(value, 'test_data')

  # def test_updateStructureValues(self):
  #   ctx = TestScHelper.MemoryCtx()

  #   addr = ctx.CreateNode(ScType.NodeConstStruct)
  #   other_addr = ctx.CreateNode(ScType.NodeConstNoRole)

  #   rrel_name = ctx.CreateNode(ScType.NodeConstRole)
  #   rrel_mass = ctx.CreateNode(ScType.NodeConstRole)
  #   rrel_height = ctx.CreateNode(ScType.NodeConstRole)
  #   rrel_other = ctx.CreateNode(ScType.NodeConstRole)
  #   rrel_new = ctx.CreateNode(ScType.NodeConstRole)

  #   # empty structure setup
  #   name = 'name_1'
  #   mass = 78.9
  #   height = 178

  #   helper = ScHelper(ctx)
  #   helper.kbUpdateStructureValues(addr, [
  #     (rrel_name, name),
  #     (rrel_mass, mass),
  #     (rrel_height, height),
  #     (rrel_other, other_addr)
  #   ])

  #   templ = ScTemplate()
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_name", ScType.EdgeAccessVarPosPerm, rrel_name)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_mass", ScType.EdgeAccessVarPosPerm, rrel_mass)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_height", ScType.EdgeAccessVarPosPerm, rrel_height)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.NodeVar >> "_other", ScType.EdgeAccessVarPosPerm, rrel_other)

  #   result = ctx.HelperSearchTemplate(templ)
  #   self.assertEqual(result.Size(), 1)

  #   self.assertEqual(result[0]['_other'], other_addr)
    
  #   name_content = ctx.GetLinkContent(result[0]['_name'])
  #   self.assertEqual(name_content.GetType(), ScLinkContent.String)
  #   self.assertEqual(name_content.GetValue().AsString(), name)

  #   mass_content = ctx.GetLinkContent(result[0]['_mass'])
  #   self.assertEqual(mass_content.GetType(), ScLinkContent.Float)
  #   self.assertEqual(mass_content.GetValue().AsFloat(), mass)

  #   height_content = ctx.GetLinkContent(result[0]['_height'])
  #   self.assertEqual(height_content.GetType(), ScLinkContent.Int)
  #   self.assertEqual(height_content.GetValue().AsFloat(), height)

  #   # update existing
  #   helper.kbUpdateStructureValues(addr, [
  #     (rrel_name, mass),
  #     (rrel_other, name),
  #     (rrel_new, height),
  #     (rrel_mass, other_addr)
  #   ])

  #   templ = ScTemplate()
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.NodeVar >> "_mass", ScType.EdgeAccessVarPosPerm, rrel_mass)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_name", ScType.EdgeAccessVarPosPerm, rrel_name)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_height", ScType.EdgeAccessVarPosPerm, rrel_height)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_other", ScType.EdgeAccessVarPosPerm, rrel_other)
  #   templ.TripleWithRelation(addr, ScType.EdgeAccessVarPosPerm, ScType.Link >> "_new", ScType.EdgeAccessVarPosPerm, rrel_new)

  #   result = ctx.HelperSearchTemplate(templ)
  #   self.assertEqual(result.Size(), 1)

  #   self.assertEqual(result[0]['_mass'], other_addr)

  #   name_content = ctx.GetLinkContent(result[0]['_name'])
  #   self.assertEqual(name_content.GetType(), ScLinkContent.Float)
  #   self.assertEqual(name_content.GetValue().AsString(), mass)

  #   ohter_content = ctx.GetLinkContent(result[0]['_other'])
  #   self.assertEqual(ohter_content.GetType(), ScLinkContent.String)
  #   self.assertEqual(ohter_content.GetValue().AsString(), name)

  #   new_content = ctx.GetLinkContent(result[0]['_new'])
  #   self.assertEqual(new_content.GetType(), ScLinkContent.Int)
  #   self.assertEqual(new_content.GetValue().AsString(), height)