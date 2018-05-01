class ScType:
    
  def __eq__(self, other) -> bool:
    return False

  def __ne__(self, other) -> bool:
    return False

  def __or__(self, other):
    return None

  def __and__(self, other):
    return None

  def __rshift__(self, other):
    return None

  def rshift(self, other):
    return None

  def IsLink(self) -> bool:
    return False

  def IsEdge(self) -> bool:
    return False

  def IsNode(self) -> bool:
    return False

  def IsValid(self) -> bool:
    return False

  def IsConst(self) -> bool:
    return False

  def IsVar(self) -> bool:
    return False

  def ToInt(self) -> bool:
    return False

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