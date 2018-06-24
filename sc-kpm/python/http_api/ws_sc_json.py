
import tornado

from tornado import websocket
from sc import *

import json
import sys
import traceback
import threading

clients = []


class EventHandler:

  def __init__(self):
    self.evt_native = None
    self.send_func = None

  def Set(self, evt, func):
    self.evt_native = evt
    self.send_func = func

  def OnEmit(self, evt):
    self.send_func(evt)


class ScJsonSocketHandler(websocket.WebSocketHandler):

  def initialize(self, evt_manager):
    self.events = {}
    self.event_manager = evt_manager
    self.alive = False

  def check_origin(self, origin):
    return True

  def open(self):
    if self not in clients:
      clients.append(self)
    self.alive = True
    print('connected')

  def on_close(self):
    if self in clients:
      clients.remove(self)
    print('disconnected', len(clients))
    self.alive = False

    # remove all events
    for eid, evt in self.events.items():
      self.event_manager.DestroyEvent(evt.evt_native)
    self.events.clear()

  def on_message(self, msg):
    params = json.loads(msg)
    status = False

    ctx = ScMemoryContext.Create(str(self))
    try:
      request_type = params['type']
      request_payload = params['payload']

      response_payload = None
      if request_type == 'keynodes':
        response_payload = self.handleKeynodes(ctx, request_payload)
      elif request_type == 'create_elements':
        response_payload = self.handleCreateElements(ctx, request_payload)
      elif request_type == 'check_elements':
        response_payload = self.handleCheckElements(ctx, request_payload)
      elif request_type == 'delete_elements':
        response_payload = self.handleDeleteElements(ctx, request_payload)
      elif request_type == 'search_template':
        response_payload = self.handleTemplateSearch(ctx, request_payload)
      elif request_type == 'generate_template':
        response_payload = self.handleTemplateGenerate(ctx, request_payload)
      elif request_type == 'content':
        response_payload = self.handleContent(ctx, request_payload)
      elif request_type == 'events':
        response_payload = self.handleEvents(ctx, request_payload)

      if response_payload:
        status = True
      else:
        response_payload = "Unsupported request type: {}".format(request_type)
    except:
      response_payload = sys.exc_info()[0]
      print("Unexpected error:", response_payload)
      traceback.print_exc(file=sys.stdout)
    finally:
      pass

    # make and send response
    response = {
        'id': params['id'],
        'event': False,
        'status': status,
        'payload': response_payload
    }

    self.sendMessage(json.dumps(response))

  def sendMessage(self, msg):
    self.write_message(msg)

  def handleKeynodes(self, ctx, payload):
    result = [0] * len(payload)

    idx = 0
    for cmd in payload:
      cmdType = cmd['command']
      idtf = cmd['idtf']

      if cmdType == 'find':
        result[idx] = ctx.HelperResolveSystemIdtf(idtf, ScType.Unknown).ToInt()
      elif cmdType == 'resolve':
        elType = ScType(cmd['elType'])
        result[idx] = ctx.HelperResolveSystemIdtf(idtf, elType).ToInt()

      idx += 1

    return result

  def handleCreateElements(self, ctx, payload):

    result = [0] * len(payload)

    idx = 0
    for cmd in payload:
      el = cmd['el']
      elType = ScType(cmd['type'])

      if el == 'node':
        result[idx] = ctx.CreateNode(elType).ToInt()

      elif el == 'edge':

        def resolveAdjAddr(obj):
          value = obj['value']
          if obj['type'] == 'ref':
            return ScAddr(result[value])

          return ScAddr(value)

        src = resolveAdjAddr(cmd['src'])
        trg = resolveAdjAddr(cmd['trg'])

        result[idx] = ctx.CreateEdge(elType, src, trg).ToInt()

      elif el == 'link':
        # TODO: support link type
        addr = ctx.CreateLink()
        if addr.IsValid():
          ctx.SetLinkContent(addr, cmd['content'])
        result[idx] = addr.ToInt()

      idx += 1

    return result

  def handleCheckElements(self, ctx, payload):
    result = [0] * len(payload)

    idx = 0
    for value in payload:
      addr = ScAddr(value)
      if addr.IsValid():
        result[idx] = ctx.GetElementType(addr).ToInt()

      idx += 1

    return result

  def handleDeleteElements(self, ctx, payload):

    for value in payload:
      ctx.DeleteElement(ScAddr(value))

    return True

  def makeTemplate(self, triples):
    used_aliases = {}

    def convert_value(value):
      t = value['type']
      v = value['value']
      result = v

      if t == 'type':
        result = ScType(v)
      elif t == 'addr':
        result = ScAddr(v)

      if 'alias' in value:
        alias = value['alias']
        result = result >> alias
        used_aliases[alias] = True

      return result

    templ = ScTemplate()
    for triple in triples:
      src = convert_value(triple[0])
      edg = convert_value(triple[1])
      trg = convert_value(triple[2])
      templ.Triple(src, edg, trg)

    return (templ, used_aliases)

  def handleTemplateSearch(self, ctx, payload):
    templ, used_aliases = self.makeTemplate(payload)

    # run search
    search_result = ctx.HelperSearchTemplate(templ)
    alias_num = 0
    aliases = {}
    for k in used_aliases.keys():
      aliases[k] = alias_num
      alias_num += 1

    addrs = []
    for idx in range(search_result.Size()):
      items = [0] * alias_num
      for alias_name, i in aliases.items():
        items[i] = search_result[idx][alias_name].ToInt()
      addrs.append(items)

    return {
        'aliases': aliases,
        'addrs': addrs
    }

  def handleTemplateGenerate(self, ctx, payload):
    templ, used_aliases = self.makeTemplate(payload["templ"])

    params = payload['params']
    templ_params = ScTemplateGenParams()
    for alias, value in params.items():
      templ_params.Add(alias, ScAddr(value))

    # run search
    gen_result = ctx.HelperGenTemplate(templ, templ_params)
    if not gen_result:
      return None

    result = {}
    for alias in used_aliases:
      result[alias] = gen_result[alias].ToInt()

    return result

  def handleContent(self, ctx, payload):
    result = []
    for cmd in payload:
      t = cmd['command']
      a = ScAddr(cmd['addr'])

      if t == 'set':
        # TODO: support type of content
        result.append(ctx.SetLinkContent(a, cmd['data']))
      elif t == 'get':
        content = ctx.GetLinkContent(a)
        value = None
        ctype = None
        if content:
          ctype = content.GetType()

          if ctype == ScLinkContent.Int:
            value = content.AsInt()
            ctype = 'int'
          elif ctype == ScLinkContent.Float:
            value = content.AsFloat()
            ctype = 'float'
          elif ctype == ScLinkContent.String:
            value = content.AsString()
            ctype = 'string'

        result.append({
            'value': value,
            'type': ctype
        })

    return result

  def onEmitEvent(self, evt):
    response = {
        'id': evt.id,
        'event': True,
        'status': True,
        'payload': [evt.addr.ToInt(), evt.edge_addr.ToInt(), evt.other_addr.ToInt()]
    }
    data = json.dumps(response)
    tornado.ioloop.IOLoop.instance().add_callback(self.sendMessage, data)

  def handleEvents(self, ctx, payload):
    result = []
    createEvents = None
    try:
      createEvents = payload['create']
    except KeyError:
      pass

    if createEvents:
      for evt in createEvents:
        # determine function that will be create events
        evtType = evt['type']
        createFunc = None
        if evtType == 'add_outgoing_edge':
          createFunc = self.event_manager.CreateEventAddOutputEdge
        elif evtType == 'add_ingoing_edge':
          createFunc = self.event_manager.CreateEventAddInputEdge
        elif evtType == 'remove_outgoing_edge':
          createFunc = self.event_manager.CreateEventRemoveOutputEdge
        elif evtType == 'remove_ingoing_edge':
          createFunc = self.event_manager.CreateEventRemoveInputEdge
        elif evtType == 'content_change':
          createFunc = self.event_manager.CreateEventContentChanged
        elif evtType == 'delete_element':
          createFunc == self.event_manager.CreateEventEraseElement

        assert createFunc != None

        addr = ScAddr(evt['addr'])

        evt_handler = EventHandler()
        evt = createFunc(addr, evt_handler.OnEmit)
        evt_handler.Set(evt, self.onEmitEvent)

        self.events[evt.GetID()] = evt_handler
        result.append(evt.GetID())

    return result
