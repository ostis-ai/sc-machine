import json
import jwt
import time
import tornado

from http_api.auth import constants as cnt
from http_api.auth.database import DataBase
from http_api.auth.config import params
from http_api.auth.validators import password_validator, username_validator


class TokenHandler(tornado.web.RequestHandler):
    def post(self) -> None:
        database = DataBase()
        username = self.get_argument(cnt.USER, False)
        pass_hash = self.get_argument(cnt.PASS, False)
        if database.is_user_valid(username, pass_hash):
            access_token_data = TokenHandler._generate_access_token()
            response = json.dumps({
                cnt.ACCESS_TOKEN: access_token_data.decode(),
                cnt.TOKEN_TYPE: cnt.JWT,
                cnt.EXPIRES_IN: cnt.JWT_LIFE_SPAN
            })
            self.write(response)
        else:
            raise tornado.web.HTTPError(403, params[cnt.MSG_ACCESS_DENIED])

    @staticmethod
    def _generate_access_token() -> bytes:
        with open(params[cnt.PRIVATE_KEY], 'rb') as file:
            private_key = file.read()
        payload = {
            "iss": cnt.ISSUER,
            "exp": time.time() + cnt.JWT_LIFE_SPAN,
        }
        access_token = jwt.encode(payload, key=private_key, algorithm='HS256')
        return access_token


# TODO: add user info validation
class AddUserHandler(tornado.web.RequestHandler):
    def post(self) -> None:
        database = DataBase()
        username = self.get_argument(cnt.USER, None)
        pass_hash = self.get_argument(cnt.PASS, None)
        if not (username_validator.validate(username) and password_validator.validate(pass_hash)):
            raise tornado.web.HTTPError(400, params[cnt.MSG_INVALID_REQUEST])
        else:
            database.add_user(username, pass_hash)
            response = json.dumps({
                cnt.USER: username
            })
            self.write(response)
