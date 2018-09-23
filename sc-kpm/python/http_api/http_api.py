"""This module implements http API for SmartHome
"""
import asyncio
import os
import threading
import tornado

from ws_sc_json import ScJsonSocketHandler
from common import ScModule
from keynodes import Keynodes

from sc import *

self_path = os.path.dirname(__file__)


class DebugStaticFileHandler(tornado.web.StaticFileHandler):
  def set_extra_headers(self, path):
    # Disable cache
    self.set_header(
        'Cache-Control', 'no-store, no-cache, must-revalidate, max-age=0')


class MainHandler(tornado.web.RequestHandler):

  #@tornado.web.asynchronous
  def get(self, path):
    self.render(os.path.join(getScConfigValue('web', 'path'),
                             "client/assets/templates/index.html"))


class ContentHandler(tornado.web.RequestHandler):

  @tornado.web.asynchronous
  def get(self, addr):
    ctx = ScMemoryContext.Create('ContentHandler_{}'.format(addr))
    link_addr = ScAddr(int(addr))
    # try to find mime and get content
    templ = ScTemplate()

    templ.TripleWithRelation(
        link_addr,
        ScType.EdgeDCommonVar,
        ScType.NodeVar >> '_format',
        ScType.EdgeAccessVarPosPerm,
        Keynodes.Get(Keynodes.NrelFormat))

    templ.TripleWithRelation(
        '_format',
        ScType.EdgeDCommonVar,
        ScType.Link >> '_mime',
        ScType.EdgeAccessVarPosPerm,
        Keynodes.Get(Keynodes.NrelMimeType))

    searchRes = ctx.HelperSearchTemplate(templ)
    if searchRes.Size() > 0:
      mime = ctx.GetLinkContent(searchRes[0]['_mime']).AsString()

    data = ctx.GetLinkContent(link_addr)
    bdata = data.AsBinary().tobytes()

    self.set_header('Content-Type', mime)
    self.write(bdata)
    self.finish()


class ServerThread(threading.Thread):

  def __init__(self, module, address='', port=8090):
    threading.Thread.__init__(self)

    assets_path = os.path.join(
        getScConfigValue('web', 'path'), 'client/assets')
    isDebug = bool(getScConfigValue('debug', 'is_debug'))
    staticHandler = DebugStaticFileHandler
    if not isDebug:
      staticHandler = tornado.web.StaticFileHandler

    print(staticHandler)

    self.port = port
    self.app = tornado.web.Application([
        (r"/ws_json", ScJsonSocketHandler, {'evt_manager': module.events}),
        (r"/content/([0-9]+)", ContentHandler),
        (r'/assets/(.*)', staticHandler, {'path': assets_path}),

        # should be a last
        (r"/(.*)", MainHandler),
    ])

  def run(self):
    asyncio.set_event_loop(asyncio.new_event_loop())

    self.running = True
    self.app.listen(self.port)

    tornado.ioloop.IOLoop.instance().start()

  def stop(self):
    tornado.ioloop.IOLoop.instance().stop()


class HttpModule(ScModule):

  def __init__(self):
    ScModule.__init__(
        self,
        ctx=__ctx__,
        cpp_bridge=__cpp_bridge__,
        keynodes=[
        ])

    self.server = None

  def OnInitialize(self, params):
    print('Initialize HTTP module')

    print('Initialize keynodes')
    Keynodes.Init(__ctx__)
    # TODO: parse port
    port = 8090

    self.server = ServerThread(self)
    self.server.start()

  def OnShutdown(self):
    print('Shutting down HTTP module')
    self.server.stop()
    self.server.join()


service = HttpModule()
service.Run()
