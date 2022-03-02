import json
import jwt
import time
import tornado

from http_api.auth import constants as cnt
from http_api.auth.database import DataBase
from http_api.auth.config import params
from http_api.auth.validators import password_validator, username_validator, TokenValidator


class BaseHandler(tornado.web.RequestHandler):
    def set_default_headers(self):
        self.set_header("Access-Control-Allow-Origin", "*")
        self.set_header("Access-Control-Allow-Headers", "x-requested-with")
        self.set_header("Access-Control-Allow-Headers", "Content-Type")
        self.set_header('Access-Control-Allow-Methods', 'POST, GET, OPTIONS')

    def options(self, *args):
        # no body
        # `*args` is for route with `path arguments` supports
        self.set_status(204)
        self.finish()


class TokenHandler(BaseHandler):
    def post(self) -> None:
        database = DataBase()
        username, password = self._get_user_credentials()
        if database.is_user_valid(username, password):
            access_token_data = TokenHandler._generate_access_token()
            response = json.dumps({
                cnt.MSG_CODE: params[cnt.MSG_CODES][cnt.MSG_ALL_DONE],
                cnt.ACCESS_TOKEN: access_token_data.decode(),
                cnt.TOKEN_TYPE: cnt.JWT,
                cnt.EXPIRES_IN: params[cnt.ACCESS_TOKEN_LIFE_SPAN]
            })
        else:
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_NOT_FOUND])
        self.write(response)

    def _get_user_credentials(self):
        data = json.loads(self.request.body)
        username = data[cnt.USERNAME] if cnt.USERNAME in data else False
        password = data[cnt.PASSWORD] if cnt.PASSWORD in data else False
        return username, password

    @staticmethod
    def _generate_access_token() -> bytes:
        with open(params[cnt.PRIVATE_KEY], 'rb') as file:
            private_key = file.read()
        payload = {
            cnt.ISS: params[cnt.ISSUER],
            cnt.EXP: time.time() + params[cnt.ACCESS_TOKEN_LIFE_SPAN],
        }
        access_token = jwt.encode(payload, key=private_key, algorithm='RS256')
        return access_token


class AddUserHandler(BaseHandler):
    @TokenValidator.validate_post
    def post(self) -> None:
        database = DataBase()
        username, password = self.get_argument(cnt.USERNAME, None), self.get_argument(cnt.PASSWORD, None)
        role = self.get_argument(cnt.ROLE, None)
        if not username_validator.validate(username):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_INVALID_USERNAME])
        elif not password_validator.validate(password):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_INVALID_PASSWORD])
        elif database.is_such_user_in_base(username):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_IS_IN_BASE])
        else:
            user_added = database.add_user(username, password, role)
            all_done_mes = params[cnt.MSG_CODES][cnt.MSG_ALL_DONE]
            invalid_role_mes = params[cnt.MSG_CODES][cnt.MSG_INVALID_ROLE]
            response = get_response_message(all_done_mes) if user_added else get_response_message(invalid_role_mes)
        self.write(response)


def get_response_message(msg_code: int) -> str:
    response = json.dumps({
        cnt.MSG_CODE: msg_code
    })
    return response
