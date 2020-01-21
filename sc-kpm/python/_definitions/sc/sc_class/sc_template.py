from sc_addr import ScAddr

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


class ScTemplateParams:
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