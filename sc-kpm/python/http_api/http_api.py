"""This module implements http API for SmartHome
"""
import threading
import tornado

from ws_sc_json import ScJsonSocketHandler
from common import ScModule
from keynodes import Keynodes

from sc import *

class MainHandler(tornado.web.RequestHandler):

    #@tornado.web.asynchronous
    def get(self):
        self.write("<h2>It works!</h2>")

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

        self.port = port
        self.app = tornado.web.Application([
            (r"/", MainHandler),
            (r"/ws_json", ScJsonSocketHandler, { 'evt_manager': module.events }),
            (r"/content/([0-9]+)", ContentHandler),
        ])

    def run(self):
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
            keynodes = [
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

