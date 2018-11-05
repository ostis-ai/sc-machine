from unittest import TestCase

from sc import *

from sc_tests.test_utils import *

class TestScTemplate(TestCase):

  def test_optional(self):
    ctx = TestScTemplate.MemoryCtx()

    # create construction that will be tried to find
    ''' scs:
       class -> instance;;
    '''
    classAddr = ctx.CreateNode(ScType.NodeConstClass)
    self.assertTrue(classAddr.IsValid())

    instAddr = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(instAddr.IsValid())

    edgeAddr = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, classAddr, instAddr)
    self.assertTrue(edgeAddr.IsValid())

    # template without optional
    templNoOpt = ScTemplate()
    
    templNoOpt.Triple(
      classAddr >> '_class',
      ScType.EdgeAccessVarPosPerm >> '_edge',
      ScType.NodeVar >> '_inst')
    
    templNoOpt.Triple(
      ScType.NodeVar,
      ScType.EdgeAccessVarPosPerm,
      '_inst'
    )

    searchResult = ctx.HelperSearchTemplate(templNoOpt)

    self.assertEqual(searchResult.Size(), 0)

    # with optional
    templOptional = ScTemplate()

    templOptional.Triple(
      classAddr >> '_class',
      ScType.EdgeAccessVarPosPerm >> '_edge',
      ScType.NodeVar >> '_inst')
    
    templOptional.Triple(
      ScType.NodeVar,
      ScType.EdgeAccessVarPosPerm,
      '_inst',
      False
    )

    searchResult = ctx.HelperSearchTemplate(templOptional)

    self.assertEqual(searchResult.Size(), 1)
    self.assertEqual(searchResult[0]['_class'], classAddr)
    self.assertEqual(searchResult[0]['_inst'], instAddr)
    self.assertEqual(searchResult[0]['_edge'], edgeAddr)
    