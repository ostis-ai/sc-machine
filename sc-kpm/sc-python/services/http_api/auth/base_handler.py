import json
import tornado

from typing import Dict, List


class BaseHandler(tornado.web.RequestHandler):
    def set_default_headers(self):
        self.set_header("Access-Control-Allow-Origin", "*")
        self.set_header("Access-Control-Allow-Headers", "Content-Type")
        self.set_header('Access-Control-Allow-Methods', 'POST, GET, OPTIONS')

    def options(self, *args):
        # no body
        # `*args` is for route with `path arguments` supports
        self.set_status(204)
        self.finish()

    def _get_request_params(self, arg_names: List[str]) -> Dict[str, str]:
        res = dict()
        request_params = json.loads(self.request.body)
        for arg_name in arg_names:
            param = request_params[arg_name] if arg_name in request_params else None
            res[arg_name] = param
        return res
