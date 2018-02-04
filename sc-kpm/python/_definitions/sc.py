from typing import Tuple, List, Any, ByteString, TypeVar

# This file used by autocompletion module, don't use it in other purposes
class ScAddr:

    def IsValid(self) -> bool:
        pass

    def ToInt(self) -> int:
        pass

    def __eq__(self, other: ScAddr):
        pass

    def __ne__(self, other: ScAddr):
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

    def IsLink(self) -> bool:
        pass

    def IsEdge(self) -> bool:
        pass

    def IsNode(self) -> bool:
        pass

    def IsValid(self) -> bool:
        pass

    def IsConst(self) -> bool:
        pass

    def IsVar(self) -> bool:
        pass

    def ToInt(self) -> bool:
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

class ScLinkContent:
    String = 0
    Int = 1
    Float = 2

    def AsBinary(self) -> ByteString:
        return ''

    def AsString(self) -> str:
        return ''

    def AsInt(self) -> int:
        return 0

    def AsFloat(self) -> float:
        return 0.0

    def GetType(self) -> int:
        return ScLinkContent.String

class ScIterator3:
    def Next(self) -> bool:
        return False

    def IsValid(self) -> bool:
        return False

    def Get(self, idx: int) -> ScAddr:
        return ScAddr()

class ScIterator5:
    def Next(self) -> bool:
        return False

    def IsValid(self) -> bool:
        return False

    def Get(self, idx: int) -> ScAddr:
        return ScAddr()

class ScTemplateGenResult:
    def Size(self) -> int:
        return 0

    def __getitem__(self, name: str) -> ScAddr:
        return ScAddr()

class ScTemplateSearchResultItem:
    def Size(self) -> int:
        return 0

    def __getitem__(self, name: str) -> ScAddr:
        return ScAddr()

class ScTemplateSearchResult:
    def Size(self) -> int:
        return 0

    def __getitem__(self, idx: int) -> ScTemplateSearchResultItem:
        return ScTemplateSearchResultItem()

class ScTemplateGenParams:
    def Add(self, paramName: str, value: ScAddr):
        pass
    
    def Get(self, paramName: str) -> ScAddr:
        return ScAddr()

    def IsEmpty(self) -> bool:
        return False

class ScTemplate:

    def Triple(self, src, edge, trg):
        return None

    def TripleWithRelation(self, src, edge, trg, attrEdge, attrEl):
        return None

IterParam = TypeVar('IterParam', ScAddr, ScType)

class ScMemoryContext:
    @staticmethod
    def Create(self, name: str):
        return ScMemoryContext()

    def CreateNode(self, nodeType: ScType) -> ScAddr:
        return ScAddr()

    def CreateEdge(self, edgeType: ScType, src: ScAddr, trg: ScAddr) -> ScAddr:
        return ScAddr()

    def CreateLink(self) -> ScAddr:
        return ScAddr()

    def DeleteElement(self, elAddr: ScAddr) -> bool:
        return False

    def GetName(self) -> str:
        return ''

    def IsElement(self, addr) -> bool:
        return False

    def GetElementType(self, addr: ScAddr) -> ScType:
        return ScType()

    def GetEdgeInfo(self, addr: ScAddr) -> Tuple[ScAddr, ScAddr]:
        return ()

    def FindLinksByContent(self, content: Any) -> List[ScAddr]:
        return []

    def SetLinkContent(self, addr: ScAddr, content: Any) -> bool:
        return False

    def GetLinkContent(self, addr: ScAddr) -> ScLinkContent:
        return ScLinkContent()

    def Iterator3(self, src: IterParam, edge: IterParam, trg: IterParam) -> ScIterator3:
        return ScIterator3()

    def Iterator5(self, src: IterParam, edge: IterParam, trg: IterParam, attrEdge: IterParam, attrEl: IterParam) -> ScIterator5:
        return ScIterator5()

    def HelperResolveSystemIdtf(self, idtf: str, elType: ScType=ScType.Unknown) -> ScAddr:
        return ScAddr()

    def HelperSetSystemIdtf(self, idtf: str, addr: ScAddr) -> bool:
        return False

    def HelperGetSystemIdtf(self, addr: ScAddr) -> str:
        return ''

    def HelperCheckEdge(self, src: ScAddr, trg: ScAddr, edgeType: ScType) -> bool:
        return False

    def HelperGenTemplate(self, templ: ScTemplate, params: ScTemplateGenParams) -> ScTemplateGenResult:
        return ScTemplateGenResult()

    def HelperSearchTemplate(self, templ: ScTemplate) -> ScTemplateSearchResult:
        return ScTemplateSearchResult()

    def HelperBuildTemplate(self, addr: ScAddr) -> ScTemplate:
        return ScTemplate()

def createScMemoryContext():
    return ScMemoryContext()

def ScAddrFromHash(hash):
    return ScAddr()

