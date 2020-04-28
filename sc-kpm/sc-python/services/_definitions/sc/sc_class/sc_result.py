from enum import Enum

class ScResult(Enum):
  Ok = 0
  Error = 1
  ErrorInvalidParams = 2
  ErrorInvalidType = 3
  ErrorIO = 4
  ErrorInvalidState = 5
  ErrorNotFound = 6
  ErrorNoWriteRights = 7
  ErrorNoReadRights = 8
  ErrorNoRights = 9
  No = 10
  Unknown = 11