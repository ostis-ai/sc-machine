import threading
import functools

from sc import *


def synchronized(lock=None):
  def _decorator(wrapped):
    @functools.wraps(wrapped)
    def _wrapper(*args, **kwargs):
      with lock:
        return wrapped(*args, **kwargs)
    return _wrapper
  return _decorator


keynodesLock = threading.Lock()


class Keynodes:

  resolved = {}

  NrelMimeType = 'nrel_mimetype'
  NrelFormat = 'nrel_format'

  @staticmethod
  @synchronized(keynodesLock)
  def Get(key):
    try:
      return Keynodes.resolved[key]
    except:
      return ScAddr()

  @staticmethod
  @synchronized(keynodesLock)
  def Init(ctx):
    keynodesList = [
        Keynodes.NrelMimeType,
        Keynodes.NrelFormat
    ]

    for k in keynodesList:
      addr = ctx.HelperResolveSystemIdtf(k, ScType.Unknown)
      if addr.IsValid():
        Keynodes.resolved[k] = addr

      print('Keynode: {} - {}'.format(k, addr.ToInt()))
