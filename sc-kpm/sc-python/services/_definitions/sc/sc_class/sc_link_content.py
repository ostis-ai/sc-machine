from typing import ByteString

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
