from typing import Tuple, List, Any, ByteString, TypeVar

from .sc_class import *

# This file used by autocompletion module, don't use it in other purposes

IterParam = TypeVar('IterParam', ScAddr, ScType)
BuildTemplateParam = TypeVar('BuildTemplateParam', ScAddr, str)

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

  def HelperBuildTemplate(self, data: BuildTemplateParam) -> ScTemplate:
    return ScTemplate()


def createScMemoryContext() -> ScMemoryContext:
  return ScMemoryContext()


def ScAddrFromHash(hash) -> ScAddr:
  return ScAddr()
