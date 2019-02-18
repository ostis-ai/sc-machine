from enum import Enum

class ScPythonEventType(Enum):
    AddInputEdge = 0
    AddOutputEdge = 1
    ContentChanged = 2
    EraseElement = 3
    RemoveInputEdge = 4
    RemoveOutputEdge = 5