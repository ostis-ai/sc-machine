# This file used by autocompletion module, don't use it in other purposes
class ScAddr:

    def IsValid(self, other):
        pass

    def ToInt(self, other):
        pass

    def __eq__(self, other):
        pass

    def __ne__(self, other):
        pass

    def __rshift__(self, other):
        pass

    def rshift(self, other):
        pass

class ScType:
    def __eq__(self, other):
        pass

    def __ne__(self, other):
        pass

    def __or__(self, other):
        pass

    def __and__(self, other):
        pass

    def __rshift__(self, other):
        pass

    def rshift(self, other):
        pass

    def IsLink(self):
        pass

    def IsEdge(self):
        pass

    def IsNode(self):
        pass

    def IsValid(self):
        pass

    def IsConst(self):
        pass

    def IsVar(self):
        pass

    def ToInt(self):
        pass

    Unknown = 0
    Const = 1
    Var = 2
    Node = 3
    Link = 4

    EdgeUCommon = 5
    EdgeDCommon = 6
    EdgeUCommonConst = 7
    EdgeDCommonConst = 8
    EdgeAccess = 9
    EdgeAccessConstPosPerm = 10
    EdgeAccessConstNegPerm = 11
    EdgeAccessConstFuzPerm = 12
    EdgeAccessConstPosTemp = 13
    EdgeAccessConstNegTemp = 14
    EdgeAccessConstFuzTemp = 15
    EdgeUCommonVar = 16
    EdgeDCommonVar = 17
    EdgeAccessVarPosPerm = 18
    EdgeAccessVarNegPerm = 19
    EdgeAccessVarFuzPerm = 20
    EdgeAccessVarPosTemp = 21
    EdgeAccessVarNegTemp = 22
    EdgeAccessVarFuzTemp = 23

    NodeConst = 24
    NodeVar = 25
    LinkConst = 26
    LinkVar = 27

    NodeConstStruct = 28
    NodeConstTuple = 29
    NodeConstRole = 30
    NodeConstNoRole = 31
    NodeConstClass = 32
    NodeConstAbstract = 33
    NodeConstMaterial = 34

    NodeVarStruct = 35
    NodeVarTuple = 36
    NodeVarRole = 37
    NodeVarNoRole = 38
    NodeVarClass = 39
    NodeVarAbstract = 40
    NodeVarMaterial = 41

class ScMemoryContext:
    def CreateNode(self, nodeType):
        return ScAddr()

    def CreateEdge(self, edgeType, src, trg):
        return ScAddr()

    def CreateLink(self):
        return ScAddr()

    def DeleteElement(self, elAddr):
        return False

    def GetName(self):
        return ''

    def IsElement(self, addr):
        return False

    def GetElementType(self, addr):
        return ScType()

    def GetEdgeInfo(self, addr):
        return ()

    def FindLinksByContent(self, content):
        return []

    def SetLinkContent(self, addr, content):
        return False

    def GetLinkContent(self, addr):
        return ScLinkContent()

    def Iterator3(self, src, edge, trg):
        return ScIterator3()

    def Iterator5(self, src, edge, trg, attrEdge, attrEl):
        return ScIterator5()

    def HelperResolveSystemIdtf(self, idtf, elType=ScType.Unknown):
        return ScAddr()

    def HelperSetSystemIdtf(self, idtf, addr):
        return False

    def HelperGetSystemIdtf(self, addr):
        return ''

    def HelperCheckEdge(self, src, trg, edgeType):
        return False

    def HelperGenTemplate(self, templ, params):
        return ScTemplateGenResult()

    def HelperSearchTemplate(self, templ):
        return ScTemplateSearchResult()

    def HelperBuildTemplate(self, addr):
        return ScTemplate()

class ScIterator3:
    def Next(self):
        return False

    def IsValid(self):
        return False

    def Get(self, idx):
        return ScAddr()

class ScIterator5:
    def Next(self):
        return False

    def IsValid(self):
        return False

    def Get(self, idx):
        return ScAddr()

class ScLinkContent:
    String = 0
    Int = 1
    Float = 2

    def AsString(self):
        return ''

    def AsInt(self):
        return 0

    def AsFloat(self):
        return 0.0

    def GetType(self):
        return ScLinkContent.String

class ScTemplateGenResult:
    def Size(self):
        return 0

    def __getitem__(self, name):
        return ScAddr()

class ScTemplateSearchResultItem:
    def Size(self):
        return 0

    def __getitem__(self, name):
        return ScAddr()

class ScTemplateSearchResult:
    def Size(self):
        return 0

    def __getitem__(self, idx):
        return ScTemplateSearchResultItem()

class ScTemplateGenParams:
    def Add(self, paramName, value):
        pass
    
    def Get(self, paramName):
        return ScAddr()

    def IsEmpty(self):
        return False

class ScTemplate:

    def Triple(self, src, edge, trg):
        return None

    def TripleWithRelation(self, src, edge, trg, attrEdge, attrEl):
        return None

def createScMemoryContext():
    return ScMemoryContext()

def ScAddrFromHash(hash):
    return ScAddr()

