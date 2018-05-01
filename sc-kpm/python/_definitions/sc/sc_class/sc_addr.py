class ScAddr:

  def IsValid(self) -> bool:
    return False

  def ToInt(self) -> int:
    return 0

  def __eq__(self, other: ScAddr) -> bool:
    return False

  def __ne__(self, other: ScAddr) -> bool:
    return False

  def __rshift__(self, other):
    return None

  def rshift(self, other):
    return None
