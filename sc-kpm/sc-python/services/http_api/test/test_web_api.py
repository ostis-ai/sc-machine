from tornado import httpserver, testing, web, websocket, gen
from unittest import TestLoader, TestCase, TextTestRunner

import json
import types
import tornado
import http_api.ws_sc_json as wsh

from common import ScModule

from sc import *

module = None


class WsJsonApiTest(testing.AsyncTestCase):

  def setUp(self):
    super(WsJsonApiTest, self).setUp()

    ioloop = tornado.ioloop.IOLoop.instance()

    app = web.Application([
        (r"/", wsh.ScJsonSocketHandler, {'evt_manager': module.events, 'ioloop': ioloop, 'auth': False}),
    ])
    server = httpserver.HTTPServer(app)
    socket, self.port = testing.bind_unused_port()
    server.add_socket(socket)

  def make_connection(self):
    return websocket.websocket_connect(
        'ws://localhost:{}/'.format(self.port)
    )

  @gen.coroutine
  def cmd_create_elements(self, client, params):

    payload = []
    for p in params:
      if 'data' in p:
        payload.append({
            'el': 'link',
            'type': p['type'].ToInt(),
            'content': p['data']
        })
      elif ('src' in p) and ('trg' in p):

        def convert(v):
          res = {}
          if isinstance(v, ScAddr):
            res = {
                'type': 'addr',
                'value': v.ToInt()
            }
          else:
            res = {
                'type': 'ref',
                'value': v
            }
          return res

        payload.append({
            'el': 'edge',
            'src': convert(p['src']),
            'trg': convert(p['trg']),
            'type': p['type'].ToInt()
        })
      else:
        payload.append({
            'el': 'node',
            'type': p['type'].ToInt()
        })

    client.write_message(self.makeRequest(1, 'create_elements', payload))

    # check response
    response = yield client.read_message()

    return json.loads(response)

  @gen.coroutine
  def cmd_check_elements(self, client, params):
    client.write_message(self.makeRequest(1, 'check_elements', params))
    response = yield client.read_message()
    return json.loads(response)

  @gen.coroutine
  def cmd_delete_elements(self, client, params):
    client.write_message(self.makeRequest(1, 'delete_elements', params))
    response = yield client.read_message()
    return json.loads(response)

  @gen.coroutine
  def cmd_search_template(self, client, params):

    def convert_value(v):
      if isinstance(v, ScAddr):
        return {
            'type': 'addr',
            'value': v.ToInt()
        }
      elif isinstance(v, ScType):
        return {
            'type': 'type',
            'value': v.ToInt()
        }

      return {
          'type': 'alias',
          'value': v
      }

    payload = []
    for p in params:
      item = []
      self.assertTrue(len(p) >= 3)
      for i in p[:3]:
        v = None
        if type(i) is list:
          self.assertEqual(len(i), 2)
          v = convert_value(i[0])
          v['alias'] = i[1]
        else:
          v = convert_value(i)
        item.append(v)
      
      # append options
      if len(p) == 4:
        item.append(p[-1])

      payload.append(item)

    client.write_message(self.makeRequest(1, 'search_template', payload))
    response = yield client.read_message()
    return json.loads(response)

  @gen.coroutine
  def cmd_generate_template(self, client, params, repl):

    def convert_value(v):
      if isinstance(v, ScAddr):
        return {
            'type': 'addr',
            'value': v.ToInt()
        }
      elif isinstance(v, ScType):
        return {
            'type': 'type',
            'value': v.ToInt()
        }

      return {
          'type': 'alias',
          'value': v
      }

    triples = []
    for p in params:
      item = []
      self.assertEqual(len(p), 3)
      for i in p:
        v = None
        if type(i) is list:
          self.assertEqual(len(i), 2)
          v = convert_value(i[0])
          v['alias'] = i[1]
        else:
          v = convert_value(i)
        item.append(v)

      triples.append(item)

    client.write_message(self.makeRequest(1, 'generate_template', {
        'templ': triples, 'params': repl
    }))
    response = yield client.read_message()
    return json.loads(response)

  @gen.coroutine
  def cmd_events(self, client, create, delete):
    client.write_message(self.makeRequest(1, 'events', {
        'create': create, 'delete': delete
    }))
    response = yield client.read_message()
    return json.loads(response)

  @gen.coroutine
  def cmd_content(self, client, commands):
    client.write_message(self.makeRequest(1, 'content', commands))
    response = yield client.read_message()
    return json.loads(response)

  @testing.gen_test
  def test_connect(self):
    c = yield self.make_connection()

    self.assertIsNotNone(c)

  def makeRequest(self, reqID, reqType, payload):
    return json.dumps({
        "id": reqID,
        "type": reqType,
        "payload": payload
    })

  @testing.gen_test
  def test_keynodes(self):
    client = yield self.make_connection()

    self.assertIsNotNone(client)

    # make request
    notFindIdtf = "Not find"
    resolveIdtf = "Resolve idtf"
    requestID = 1
    payload = [
        {
            "command": "find",
            "idtf": notFindIdtf
        },
        {
            "command": "resolve",
            "idtf": resolveIdtf,
            "elType": ScType.NodeConst.ToInt()
        }
    ]

    # send request
    client.write_message(self.makeRequest(requestID, 'keynodes', payload))
    response = yield client.read_message()

    resObj = json.loads(response)
    resPayload = resObj['payload']

    self.assertEqual(resObj['id'], requestID)
    self.assertTrue(resObj['status'])
    self.assertEqual(len(resPayload), len(payload))
    self.assertEqual(resPayload[0], 0)
    self.assertNotEqual(resPayload[1], 0)

  @testing.gen_test
  def test_keynodes_error(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    # make error request
    payload = [
        {
            "command": 'find'
        }
    ]

    client.write_message(self.makeRequest(1, 'keyndes', payload))
    response = yield client.read_message()

    resObj = json.loads(response)
    self.assertEqual(resObj['id'], 1)
    self.assertFalse(resObj['status'])

  @testing.gen_test
  def test_unsupported_command(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    client.write_message(self.makeRequest(1, 'unknown', {}))

    response = yield client.read_message()
    resObj = json.loads(response)

    self.assertEqual(resObj['id'], 1)
    self.assertFalse(resObj['status'])

  @testing.gen_test
  def test_create_elements(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    # create keynode
    client.write_message(self.makeRequest(3, 'keynodes', [
        {
            "command": "resolve",
            "idtf": "any idtf 1",
            "elType": ScType.NodeConst.ToInt()
        }]))

    response = yield client.read_message()
    resObj = json.loads(response)

    keynode = resObj['payload'][0]
    self.assertTrue(resObj['status'])
    self.assertNotEqual(keynode, 0)

    keynode = ScAddr(keynode)

    # create elements
    params = [
        {
            'type': ScType.Node
        },
        {
            'type': ScType.LinkConst,
            'data': 45.4
        },
        {
            'type': ScType.LinkConst,
            'data': 45
        },
        {
            'type': ScType.LinkConst,
            'data': "test content"
        },
        {
            "src": 0,
            'trg': keynode,
            'type': ScType.EdgeAccessConstPosPerm
        }
    ]

    resObj = yield self.cmd_create_elements(client, params)

    self.assertTrue(resObj['status'])
    results = resObj['payload']
    for addr in results:
      self.assertNotEqual(addr, 0)

  @testing.gen_test
  def test_check_elements(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    elements = yield self.cmd_create_elements(client, [
        {
            'type': ScType.Node
        },
        {
            'type': ScType.Link,
            'data': 'test data'
        }])

    elements = elements['payload']

    params = [elements[0], 0, elements[1]]
    resObj = yield self.cmd_check_elements(client, params)

    resPayload = resObj['payload']

    self.assertTrue(resObj['status'])
    self.assertEqual(len(resPayload), len(params))
    self.assertEqual(resPayload[0], ScType.Node.ToInt())
    self.assertEqual(resPayload[1], 0)
    self.assertEqual(resPayload[2], ScType.LinkConst.ToInt())

  @testing.gen_test
  def test_delete_elements(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    types = [
        ScType.Node,
        ScType.NodeConstAbstract,
        ScType.NodeConstClass
    ]
    params = []
    for t in types:
      params.append({'type': t})

    elements = yield self.cmd_create_elements(client, params)
    elements = elements['payload']

    # check elements
    types2 = yield self.cmd_check_elements(client, elements)
    types2 = types2['payload']
    self.assertEqual(len(types), len(types2))
    for idx in range(len(types)):
      self.assertEqual(types[idx].ToInt(), types2[idx])

    resObj = yield self.cmd_delete_elements(client, elements)
    self.assertTrue(resObj['status'])

    types2 = yield self.cmd_check_elements(client, elements)
    types2 = types2['payload']
    self.assertEqual(len(types), len(types2))
    for idx in range(len(types)):
      self.assertEqual(0, types2[idx])

  @testing.gen_test
  def test_template_search(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    # create construction for a test
    # ..0 ~> ..3: ..1;;
    # ..0 ~> ..3: ..2;;
    params = [
        {
            'type': ScType.NodeConst
        },
        {
            'type': ScType.NodeConst
        },
        {
            'type': ScType.NodeConst
        },
        {
            'type': ScType.NodeConstClass
        },
        {
            'type': ScType.EdgeAccessConstPosTemp,
            'src': 0,
            'trg': 1
        },
        {
            'type': ScType.EdgeAccessConstPosTemp,
            'src': 0,
            'trg': 2
        },
        {
            'type': ScType.EdgeAccessConstPosTemp,
            'src': 3,
            'trg': 4
        },
        {
            'type': ScType.EdgeAccessConstPosTemp,
            'src': 3,
            'trg': 5
        }
    ]
    elements = yield self.cmd_create_elements(client, params)
    elements = elements['payload']

    self.assertEqual(len(elements), len(params))

    # ..0 _~> ..3: _node;;
    templ = [
        [
            ScAddr(elements[0]),
            [ScType.EdgeAccessVarPosTemp, "_edge"],
            [ScType.NodeVar, "_node"]
        ],
        [
            ScAddr(elements[3]),
            ScType.EdgeAccessVarPosTemp,
            "_edge"
        ]
    ]
    result = yield self.cmd_search_template(client, templ)
    self.assertTrue(result['status'])
    result = result['payload']

    aliases = result['aliases']
    addrs = result['addrs']

    self.assertEqual(len(addrs), 2)

    self.assertTrue('_node' in aliases)
    self.assertTrue('_edge' in aliases)

    _node = aliases['_node']
    _edge = aliases['_edge']

    _nodes_list = [addrs[0][_node], addrs[1][_node]]
    _edges_list = [addrs[0][_edge], addrs[1][_edge]]

    self.assertTrue(elements[1] in _nodes_list)
    self.assertTrue(elements[2] in _nodes_list)

    self.assertTrue(elements[4] in _edges_list)
    self.assertTrue(elements[5] in _edges_list)

  @testing.gen_test
  def test_template_generate(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    # create construction for a test
    # ..0 ~> ..3: ..1;;
    # ..0 ~> ..3: ..2;;
    params = [
        {
            'type': ScType.NodeConst
        },
        {
            'type': ScType.NodeConst
        }
    ]
    elements = yield self.cmd_create_elements(client, params)
    elements = elements['payload']

    self.assertEqual(len(elements), len(params))

    # ..0 _~> ..3: _node;;
    templ = [
        [
            ScAddr(elements[0]),
            [ScType.EdgeAccessVarPosPerm, '_edge'],
            [ScType.NodeVar, '_node']
        ],
        [
            '_node',
            ScType.EdgeAccessVarPosTemp,
            '_edge'
        ]
    ]

    result = yield self.cmd_search_template(client, templ)
    self.assertTrue(result['status'])
    result = result['payload']

    addrs = result['addrs']

    # have no any construction
    self.assertEqual(len(addrs), 0)

    # generate new construction
    result = yield self.cmd_generate_template(client, templ, {
        '_node': elements[1]
    })
    self.assertTrue(result['status'])
    result = result['payload']
    aliases = result['aliases']
    addrs = result['addrs']

    def get_by_alias(alias):
      return addrs[aliases[alias]]

    self.assertNotEqual(get_by_alias('_edge'), 0)

    # try to search generated construction
    result = yield self.cmd_search_template(client, templ)
    self.assertTrue(result['status'])
    result = result['payload']

    aliases = result['aliases']
    addrs = result['addrs']

    self.assertEqual(len(addrs), 1)
    self.assertEqual(addrs[0][aliases['_node']], elements[1])

    # invalid params
    result = yield self.cmd_generate_template(client, templ, {
        '_edge': elements[1]
    })
    self.assertFalse(result['status'])

  # @testing.gen_test
  # def test_events(self):
  #     client = yield self.make_connection()
  #     self.assertIsNotNone(client)

  #     elements = yield self.cmd_create_elements(client, [
  #         {
  #             'type': ScType.Node
  #         },
  #         {
  #             'type': ScType.Link,
  #             'data': 'test data'
  #         }])

  #     elements = elements['payload']
  #     self.assertNotEqual(elements[0], 0)
  #     self.assertNotEqual(elements[1], 0)

  #     # TODO: implement events testing

  @testing.gen_test
  def test_content(self):
    client = yield self.make_connection()
    self.assertIsNotNone(client)

    elements = yield self.cmd_create_elements(client, [
        {
            'type': ScType.Node
        },
        {
            'type': ScType.Link,
            'data': 45
        }])

    elements = elements['payload']
    self.assertNotEqual(elements[0], 0)
    self.assertNotEqual(elements[1], 0)

    # TODO: implement events testing
    commands = [
        {'command': 'get', 'addr': elements[0]},
        {'command': 'get', 'addr': elements[1]}
    ]

    result = yield self.cmd_content(client, commands)
    self.assertTrue(result['status'])
    result = result['payload']

    self.assertFalse(result[0]['value'])
    self.assertEqual(result[1]['value'], 45)
    self.assertEqual(result[1]['type'], 'int')


def RunTest(test):
  global TestLoader, TextTestRunner
  testItem = TestLoader().loadTestsFromTestCase(test)
  res = TextTestRunner(verbosity=2).run(testItem)

  if not res.wasSuccessful():
    raise Exception("Unit test failed")


class TestModule(ScModule):
  def __init__(self):
    ScModule.__init__(self,
                      ctx=__ctx__,
                      cpp_bridge=__cpp_bridge__,
                      keynodes=[
                      ])

  def DoTests(self):
    try:
      RunTest(WsJsonApiTest)
    except Exception as ex:
      raise ex
    except:
      import sys
      print("Unexpected error:", sys.exc_info()[0])
    finally:
      self.Stop()

  def OnInitialize(self, params):
    self.DoTests()

  def OnShutdown(self):
    pass


module = TestModule()
module.Run()
