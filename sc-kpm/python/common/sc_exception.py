class ScKeynodeException(Exception):
  def __init__(self, sys_idtf):
    Exception.__init__(self, "Can't find keynode: {}".format(sys_idtf))


class ScKnowledgeBaseException(Exception):
  def __init__(self, msg):
    Exception.__init__(self, "Invalid state of knowledge base: {}".format(msg))


class ScCriticalException(Exception):
  def __init__(self, msg):
    Exception.__init__(self, "Critical error: {}".format(msg))
