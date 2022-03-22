import json
import tornado

from http_api.auth import constants as cnt
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

    def _get_user_credentials(self):
        data = json.loads(self.request.body)
        name = data[cnt.NAME] if cnt.NAME in data else False
        password = data[cnt.PASSWORD] if cnt.PASSWORD in data else False
        return name, password


