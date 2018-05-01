from sc import *


class ScHelper:

  def __init__(self, ctx):
    self.ctx = ctx

  def kbSetRelationLinkValue(self, _addr: ScAddr, _relAddr: ScAddr, _value: any):
    """Sets value of ScLink connected to `_addr` by relation `_relAddr`.
    SCs text:
    _addr => _relAddr: [_value];;
    """
    templ = ScTemplate()

    templ.TripleWithRelation(
        _addr,
        ScType.EdgeDCommonVar,
        ScType.Link >> '_link',
        ScType.EdgeAccessVarPosPerm,
        _relAddr)

    linkAddr = None
    searchRes = self.ctx.HelperSearchTemplate(templ)
    if searchRes.Size() > 0:
      linkAddr = searchRes[0]['_link']

    # generate new relation if not found
    if linkAddr:
      self.ctx.SetLinkContent(linkAddr, _value)
    else:
      linkAddr = self.ctx.CreateLink()
      self.ctx.SetLinkContent(linkAddr, _value)

      edge = self.ctx.CreateEdge(ScType.EdgeDCommonConst, _addr, linkAddr)
      self.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, _relAddr, edge)
  
  def kbGetRelationLinkValue(self, _addr: ScAddr, _relAddr: ScAddr) -> ScLinkContent:
    """Returns value of ScLink conneted to `_addr` by relation `_relAddr`.
    SCs text:
    _addr => _relAddr: [_value];;

    If value found, then returns `ScLinkContent`, otherwise returns None
    """
    templ = ScTemplate()
    templ.TripleWithRelation(
        _addr,
        ScType.EdgeDCommonVar,
        ScType.Link >> '_link',
        ScType.EdgeAccessVarPosPerm,
        _relAddr)

    searchRes = self.ctx.HelperSearchTemplate(templ)
    if searchRes.Size() > 0:
      linkAddr = searchRes[0]['_link']
      return self.ctx.GetLinkContent(linkAddr)

    return None


  def kbReplaceBinaryRelation(self, _addr: ScAddr, _relAddr: ScAddr, _newTarget: ScAddr) -> bool:
    """
    Any construction that corresponds template `_addr => _relAddr: _X;;` will be deleted (where _X - any object).
    After that new one will be created by template `_addr => _relAddr: _newTarget;;`

    Returns `True`, when new relation created
    """
    templ = ScTemplate()
    templ.TripleWithRelation(
        _addr,
        ScType.EdgeDCommonVar,
        ScType.Unknown >> '_x',
        ScType.EdgeAccessVarPosPerm,
        _relAddr)

    searchRes = self.ctx.HelperSearchTemplate(templ)
    if searchRes.Size() > 0:
      self.ctx.DeleteElement(searchRes[0]['_x'])

    # create new relation
    params = ScTemplateGenParams()
    params.Add('_x', _newTarget)

    return self.ctx.HelperGenTemplate(templ, params)
