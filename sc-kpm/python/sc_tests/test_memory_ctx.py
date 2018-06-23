from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

class TestScMemoryContext(TestCase):

  def test_create(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.Const)
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateLink()
    self.assertTrue(addr2.IsValid())

    edge1 = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)
    self.assertTrue(edge1.IsValid())

    edge2 = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr2, ScAddr())
    self.assertFalse(edge2.IsValid())

  def test_is_element(self):
    ctx = TestScMemoryContext.MemoryCtx()

    self.assertFalse(ctx.IsElement(ScAddr()))

    addr1 = ctx.CreateNode(ScType.Unknown)
    self.assertTrue(ctx.IsElement(addr1))

  def test_get_element_type(self):
    ctx = TestScMemoryContext.MemoryCtx()

    self.assertEqual(ctx.GetElementType(ScAddr()), ScType.Unknown)

    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertEqual(ctx.GetElementType(addr1), ScType.NodeConst)

  def test_get_edge_info(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType())
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateNode(ScType())
    self.assertTrue(addr2.IsValid())

    edge = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)
    self.assertTrue(edge.IsValid())

    src, trg = ctx.GetEdgeInfo(edge)
    self.assertEqual(addr1, src)
    self.assertEqual(addr2, trg)

    src, trg = ctx.GetEdgeInfo(addr1)
    self.assertEqual(src, None)
    self.assertEqual(trg, None)

  def test_find_links_by_content(self):
    ctx = TestScMemoryContext.MemoryCtx()

    def createLink(content):
      addr = ctx.CreateLink()
      self.assertTrue(addr.IsValid())
      self.assertTrue(ctx.SetLinkContent(addr, content))
      return addr

    c1 = 'test_link_1'
    link1 = createLink(c1)
    link1copy = createLink(c1)

    link1List = [link1, link1copy]

    c2 = 'test_link_2'
    link2 = createLink(c2)

    res1 = ctx.FindLinksByContent(c1)
    self.assertEqual(len(res1), len(link1List))
    for el in res1:
      self.assertTrue(el in link1List)

    res2 = ctx.FindLinksByContent(c2)
    self.assertEqual(len(res2), 1)
    self.assertEqual(res2[0], link2)

    res3 = ctx.FindLinksByContent('test_any_not_found')
    self.assertEqual(len(res3), 0)

  def test_link_content(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateLink()
    self.assertTrue(addr1.IsValid())
    self.assertTrue(ctx.SetLinkContent(addr1, 56))

    v1 = ctx.GetLinkContent(addr1)
    self.assertEqual(v1.GetType(), ScLinkContent.Int)

    value1 = v1.AsInt()
    self.assertTrue(type(value1) is int)
    self.assertEqual(value1, 56)

    addr2 = ctx.CreateLink()
    self.assertTrue(addr2.IsValid())
    self.assertTrue(ctx.SetLinkContent(addr2, 56.0))

    v2 = ctx.GetLinkContent(addr2)
    value2 = v2.AsFloat()
    self.assertTrue(type(value2) is float)
    self.assertEqual(value2, 56.0)
    self.assertEqual(v2.GetType(), ScLinkContent.Float)

    addr3 = ctx.CreateLink()
    self.assertTrue(addr3.IsValid())
    self.assertTrue(ctx.SetLinkContent(addr3, "any text"))

    v3 = ctx.GetLinkContent(addr3)
    value3 = v3.AsString()
    self.assertTrue(type(value3) is str)
    self.assertEqual(value3, "any text")
    self.assertEqual(v3.GetType(), ScLinkContent.String)

  def test_iterator3(self):

    ctx = TestScMemoryContext.MemoryCtx()
    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateNode(ScType.NodeVar)
    self.assertTrue(addr2.IsValid())

    edge = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)
    self.assertTrue(edge.IsValid())

    def test_common(it):
      self.assertTrue(it.IsValid())
      self.assertTrue(it.Next())
      self.assertEqual(it.Get(0), addr1)
      self.assertEqual(it.Get(1), edge)
      self.assertEqual(it.Get(2), addr2)
      self.assertFalse(it.Next())

    # f_a_a
    itFAA = ctx.Iterator3(
        addr1, ScType.EdgeAccessConstPosPerm, ScType.NodeVar)
    test_common(itFAA)

    # f_a_f
    itFAF = ctx.Iterator3(addr1, ScType.EdgeAccessConstPosPerm, addr2)
    test_common(itFAF)

    # a_a_f
    itAAF = ctx.Iterator3(
        ScType.NodeConst, ScType.EdgeAccessConstPosPerm, addr2)
    test_common(itAAF)

    # no results
    it3 = ctx.Iterator3(
        addr1, ScType.EdgeAccessConstPosPerm, ScType.NodeConst)
    self.assertTrue(it3.IsValid())
    self.assertFalse(it3.Next())

  def test_iterator5(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateNode(ScType.NodeVar)
    self.assertTrue(addr2.IsValid())

    attr = ctx.CreateNode(ScType.Unknown)
    self.assertTrue(attr.IsValid())

    edge = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)
    self.assertTrue(edge.IsValid())

    edgeAttr = ctx.CreateEdge(ScType.EdgeAccessVarPosTemp, attr, edge)
    self.assertTrue(edgeAttr.IsValid())

    def test_common(it):
      self.assertTrue(it.IsValid())
      self.assertTrue(it.Next())
      self.assertEqual(it.Get(0), addr1)
      self.assertEqual(it.Get(1), edge)
      self.assertEqual(it.Get(2), addr2)
      self.assertEqual(it.Get(3), edgeAttr)
      self.assertEqual(it.Get(4), attr)
      self.assertFalse(it.Next())

    # f_a_f_a_f
    itFAFAF = ctx.Iterator5(
        addr1,
        ScType.EdgeAccessConstPosPerm,
        addr2,
        ScType.EdgeAccessVarPosTemp,
        attr)
    test_common(itFAFAF)

    # f_a_f_a_a
    itFAFAA = ctx.Iterator5(
        addr1,
        ScType.EdgeAccessConstPosPerm,
        addr2,
        ScType.EdgeAccessVarPosTemp,
        ScType.Node)
    test_common(itFAFAA)

    # f_a_a_a_f
    itFAAAF = ctx.Iterator5(
        addr1,
        ScType.EdgeAccessConstPosPerm,
        ScType.NodeVar,
        ScType.EdgeAccessVarPosTemp,
        attr)
    test_common(itFAAAF)

    # f_a_a_a_a
    itFAAAA = ctx.Iterator5(
        addr1,
        ScType.EdgeAccessConstPosPerm,
        ScType.NodeVar,
        ScType.EdgeAccessVarPosTemp,
        ScType.Node)
    test_common(itFAAAA)

    # a_a_f_a_f
    itAAFAF = ctx.Iterator5(
        ScType.NodeConst,
        ScType.EdgeAccessConstPosPerm,
        addr2,
        ScType.EdgeAccessVarPosTemp,
        attr)
    test_common(itAAFAF)

    # a_a_f_a_a
    itAAFAA = ctx.Iterator5(
        ScType.NodeConst,
        ScType.EdgeAccessConstPosPerm,
        addr2,
        ScType.EdgeAccessVarPosTemp,
        ScType.Node)
    test_common(itAAFAA)

  def test_helper_sys_idtf(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.HelperResolveSystemIdtf("sc_result", None)
    self.assertTrue(addr1.IsValid())

    # non exist
    addr2 = ctx.HelperResolveSystemIdtf("test_example_value_idtf", None)
    self.assertFalse(addr2.IsValid())

    # create new
    addr3 = ctx.HelperResolveSystemIdtf(
        "test_example_idtf1", ScType.NodeConst)
    self.assertTrue(addr3.IsValid())
    self.assertEqual(ScType.NodeConst, ctx.GetElementType(addr3))

    # Set
    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr1.IsValid())
    self.assertTrue(ctx.HelperSetSystemIdtf("idtf_1_2_test", addr1))

    addr2 = ctx.HelperResolveSystemIdtf("idtf_1_2_test", None)
    self.assertEqual(addr1, addr2)

    # get
    self.assertEqual(ctx.HelperGetSystemIdtf(addr2), "idtf_1_2_test")

  def test_helper_has_edge(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr2.IsValid())

    edge = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)
    self.assertTrue(edge.IsValid())
    self.assertTrue(ctx.HelperCheckEdge(
        addr1, addr2, ScType.EdgeAccessConstPosPerm))

  def test_helper_template(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr2.IsValid())

    attrAddr = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(attrAddr.IsValid())

    templ = ScTemplate()
    templ.Triple(
      addr1,
      ScType.EdgeAccessVarPosPerm >> "_edge",
      ScType.NodeVar >> "_target")

    templ.Triple(
      attrAddr,
      ScType.EdgeAccessVarPosPerm,
      "_edge")

    # generate by template
    params = ScTemplateGenParams()
    params.Add("_target", addr2)

    self.assertEqual(params.Get("_target_none"), None)

    genResult = ctx.HelperGenTemplate(templ, params)
    self.assertTrue(type(genResult) is ScTemplateGenResult)
    self.assertEqual(genResult["not_exist"], None)

    # check aliases
    aliases = genResult.Aliases().keys()
    self.assertEqual(len(aliases), 2)
    self.assertTrue("_edge" in aliases)
    self.assertTrue("_target" in aliases)

    # search by this template and compare results
    searchResult = ctx.HelperSearchTemplate(templ)
    self.assertTrue(type(searchResult) is ScTemplateSearchResult)
    self.assertEqual(searchResult.Size(), 1)

    # check aliases
    aliases = searchResult.Aliases().keys()
    self.assertEqual(len(aliases), 2)
    self.assertTrue("_edge" in aliases)
    self.assertTrue("_target" in aliases)

    searchItem = searchResult[0]
    self.assertEqual(searchItem["_edge"], genResult["_edge"])
    self.assertEqual(searchItem["_target"], genResult["_target"])

  def test_helper_build_template_addr(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr1.IsValid())

    addr2 = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(addr2.IsValid())

    edge = ctx.CreateEdge(ScType.EdgeAccessVarPosPerm, addr1, addr2)
    self.assertTrue(edge.IsValid())

    templAddr = ctx.CreateNode(ScType.NodeConstStruct)
    self.assertTrue(templAddr.IsValid())

    for addr in [addr1, addr2, edge]:
      e = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, templAddr, addr)
      self.assertTrue(e.IsValid())

    templ = ctx.HelperBuildTemplate(templAddr)
    self.assertTrue(type(templ) is ScTemplate)

  def test_helper_build_template_scs(self):
    ctx = TestScMemoryContext.MemoryCtx()

    # generate test content for template search
    personAddr = CreateNodeWithIdtf(ctx, ScType.NodeConstClass, "person")
    self.assertTrue(personAddr.IsValid())

    nrelRelAddr = CreateNodeWithIdtf(ctx, ScType.NodeConstNoRole, "nrel_rel")
    self.assertTrue(nrelRelAddr.IsValid())

    nodeValidAddr = ctx.CreateNode(ScType.NodeConstMaterial)
    self.assertTrue(nodeValidAddr.IsValid())

    nodeErrorAddr = ctx.CreateNode(ScType.NodeConst)
    self.assertTrue(nodeErrorAddr.IsValid())

    relEdgeValidAddr = ctx.CreateEdge(ScType.EdgeDCommonConst, personAddr, nodeValidAddr)
    self.assertTrue(relEdgeValidAddr.IsValid())

    relEdgeAttrValidAddr = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, nrelRelAddr, relEdgeValidAddr)
    self.assertTrue(relEdgeAttrValidAddr.IsValid())

    relEdgeErrorAddr = ctx.CreateEdge(ScType.EdgeDCommonConst, personAddr, nodeErrorAddr)
    self.assertTrue(relEdgeErrorAddr.IsValid())

    relEdgeAttrErrorAddr = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, nrelRelAddr, relEdgeErrorAddr)
    self.assertTrue(relEdgeAttrErrorAddr.IsValid())

    # make template
    scs = "person _=> nrel_rel:: .._x;;"\
      "person <- sc_node_class;;"\
      ".._x <- sc_node_material;;"


    templ = ctx.HelperBuildTemplate(scs)
    self.assertTrue(type(templ) is ScTemplate)

    # try to find data with that template
    searchResult = ctx.HelperSearchTemplate(templ)
    self.assertTrue(type(searchResult) is ScTemplateSearchResult)
    self.assertEqual(searchResult.Size(), 1)

    searchItem = searchResult[0]
    self.assertEqual(searchItem["person"], personAddr)
    self.assertEqual(searchItem[".._x"], nodeValidAddr)


  def test_rshift(self):
    ctx = TestScMemoryContext.MemoryCtx()

    addr1 = ctx.CreateNode(ScType.NodeConst)

    value1 = addr1 >> "test_repl"
    self.assertTrue(type(value1) is ScTemplateItemValue)

    value2 = ScType.NodeConst >> "test_repl2"
    self.assertTrue(type(value2) is ScTemplateItemValue)

