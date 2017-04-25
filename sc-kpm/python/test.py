import unittest, struct

# https://docs.python.org/2/library/unittest.html

from sc import *

def MemoryCtx(name):
    return ScMemoryContext(0, name)

class TestScAddr(unittest.TestCase):

    def test_compare(self):

        addr1 = ScAddr()
        addr2 = ScAddr()
        
        self.assertEqual(addr1, addr2)

        ctx = MemoryCtx("test")
        addr2 = ctx.CreateNode(ScType.Unknown)

        self.assertNotEqual(addr1, addr2)

    def test_is_valid(self):
        addr1 = ScAddr()

        self.assertFalse(addr1.IsValid())

        ctx = MemoryCtx("test")
        addr2 = ctx.CreateNode(ScType.Const)

        self.assertTrue(addr2.IsValid())

    def test_to_int(self):
        addr1 = ScAddr()

        self.assertEqual(addr1.ToInt(), 0)

        ctx = MemoryCtx("test")
        addr2 = ctx.CreateNode(ScType.Const)

        self.assertNotEqual(addr2.ToInt(), 0)

class TestScType(unittest.TestCase):

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

        self.assertTrue(type1.IsValid())

        type2 = ScType()

        self.assertFalse(type2.IsValid())

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

class TestScMemoryContext(unittest.TestCase):

    def test_create(self):
        ctx = MemoryCtx("test")

        addr1 = ctx.CreateNode(ScType.Const)
        self.assertTrue(addr1.IsValid())

        addr2 = ctx.CreateLink()
        self.assertTrue(addr2.IsValid())

        edge1 = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr1, addr2)
        self.assertTrue(edge1.IsValid())

        edge2 = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, addr2, ScAddr())
        self.assertFalse(edge2.IsValid())

    def test_name(self):
        ctx = MemoryCtx("test_name")
        self.assertEqual("test_name", ctx.GetName())

        ctx = MemoryCtx("test_name2")
        self.assertEqual("test_name2", ctx.GetName())

    def test_is_element(self):
        ctx = MemoryCtx("test")

        self.assertFalse(ctx.IsElement(ScAddr()))

        addr1 = ctx.CreateNode(ScType.Unknown)
        self.assertTrue(ctx.IsElement(addr1))

    def test_get_element_type(self):
        ctx = MemoryCtx("test")

        self.assertEqual(ctx.GetElementType(ScAddr()), ScType.Unknown)

        addr1 = ctx.CreateNode(ScType.NodeConst)
        self.assertEqual(ctx.GetElementType(addr1), ScType.NodeConst)

    def test_get_edge_info(self):
        ctx = MemoryCtx("test")

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

    def test_link_content(self):
        ctx = MemoryCtx("test")

        addr1 = ctx.CreateLink()
        self.assertTrue(addr1.IsValid())
        self.assertTrue(ctx.SetLinkContent(addr1, 56))
        
        value1 = ctx.GetLinkContent(addr1)
        self.assertTrue(type(value1) is str)
        value1 = struct.unpack("q", value1)[0]
        self.assertTrue(type(value1) is int)
        self.assertEqual(value1, 56)

        addr2 = ctx.CreateLink()
        self.assertTrue(addr2.IsValid())
        self.assertTrue(ctx.SetLinkContent(addr2, 56.0))

        value2 = ctx.GetLinkContent(addr2)
        self.assertTrue(type(value2) is str)
        value2 = struct.unpack("d", value2)[0]
        self.assertTrue(type(value2) is float)
        self.assertEqual(value2, 56.0)

        addr3 = ctx.CreateLink()
        self.assertTrue(addr3.IsValid())
        self.assertTrue(ctx.SetLinkContent(addr3, "any text"))

        value3 = ctx.GetLinkContent(addr3)
        self.assertTrue(type(value3) is str)
        self.assertEqual(value3, "any text")

    def test_iterator3(self):

        ctx = MemoryCtx("test")
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
        itFAA = ctx.Iterator3(addr1, ScType.EdgeAccessConstPosPerm, ScType.NodeVar)
        test_common(itFAA)
        
        # f_a_f
        itFAF = ctx.Iterator3(addr1, ScType.EdgeAccessConstPosPerm, addr2)
        test_common(itFAF)

        # a_a_f
        itAAF = ctx.Iterator3(ScType.NodeConst, ScType.EdgeAccessConstPosPerm, addr2)
        test_common(itAAF)

        # no results
        it3 = ctx.Iterator3(addr1, ScType.EdgeAccessConstPosPerm, ScType.NodeConst)
        self.assertTrue(it3.IsValid())
        self.assertFalse(it3.Next())

    def test_iterator5(self):
        ctx = MemoryCtx("test")

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

def RunTest(test):
    testItem = unittest.TestLoader().loadTestsFromTestCase(test)
    res = unittest.TextTestRunner(verbosity=2).run(testItem)

    if not res.wasSuccessful():
        raise Exception("Unit test failed")

try:

    RunTest(TestScAddr)
    RunTest(TestScType)
    RunTest(TestScMemoryContext)

except Exception as ex:
    raise ex
except:
    import sys
    print "Unexpected error:", sys.exc_info()[0]
