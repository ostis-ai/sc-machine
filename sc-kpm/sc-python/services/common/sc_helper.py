from sc import *


class ScHelper:

  def __init__(self, ctx):
    self.ctx = ctx

  def kbGetBinaryRelationLinkAddr(self, _addr: ScAddr, _relAddr: ScAddr) -> ScAddr:
    """Find ScLink connected to `_addr` by relation `_relAddr`.
      SCs text:
        _addr => _relAddr: [_link];;
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

    return linkAddr

  def kbSetBinaryRelationLinkValue(self, _addr: ScAddr, _relAddr: ScAddr, _value: any):
    """Sets value of ScLink connected to `_addr` by relation `_relAddr`.
    SCs text:
    _addr => _relAddr: [_value];;
    """
    linkAddr = self.kbGetBinaryRelationLinkAddr(_addr, _relAddr)

    # generate new relation if not found
    if linkAddr:
      self.ctx.SetLinkContent(linkAddr, _value)
    else:
      linkAddr = self.ctx.CreateLink()
      self.ctx.SetLinkContent(linkAddr, _value)

      edge = self.ctx.CreateEdge(ScType.EdgeDCommonConst, _addr, linkAddr)
      self.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, _relAddr, edge)
  
  def kbGetBinaryRelationLinkValue(self, _addr: ScAddr, _relAddr: ScAddr) -> ScLinkContent:
    """Returns value of ScLink conneted to `_addr` by relation `_relAddr`.
    SCs text:
    _addr => _relAddr: [_value];;

    If value found, then returns `ScLinkContent`, otherwise returns None
    """  
    linkAddr = self.kbGetBinaryRelationLinkAddr(_addr, _relAddr)
    if linkAddr:
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

  def kbUpdateStructureValues(self, _addr: ScAddr, _values):
    """
    Update values of specified structure, where `_values` is a list of tuples. Each tuple
    contains two values: `(rel_addr, value)`. So this function tries to set/update values 
    for a such structure:
      _addr
        -> rel_addr_1: [value_1];
        -> rel_addr_2: [value_2];
        ...
    
    If value with a specified `rel_addr` doesn't exist, then it would be created. If value exists,
    then this function will change content of sc-link.
    """

