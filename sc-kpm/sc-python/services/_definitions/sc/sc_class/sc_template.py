from sc_addr import ScAddr

class ScTemplateGenResult:
  def Size(self) -> int:
    return 0

  def __getitem__(self, name: str) -> ScAddr:
    return ScAddr()

  def Aliases(self) -> [str]:
    return []


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

  def Aliases(self) -> [str]:
    return []

class ScTemplateParams:
  def Add(self, paramName: str, value: ScAddr):
    pass

  def Get(self, paramName: str) -> ScAddr:
    return ScAddr()

  def IsEmpty(self) -> bool:
    return False


class ScTemplate:

  def __init__():
    pass

  def Triple(self, src, edge, trg, is_required = True):
    return None

  def TripleWithRelation(self, src, edge, trg, attrEdge, attrEl, is_required = True):
    return None