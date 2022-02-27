import json
import jwt
import time
import tornado

from http_api.auth import constants as cnt
from http_api.auth.database import DataBase
from http_api.auth.config import params
from http_api.auth.validators import password_validator, username_validator, TokenValidator


class TokenHandler(tornado.web.RequestHandler):
    def post(self) -> None:
        database = DataBase()
        username = self.get_argument(cnt.USER, False)
        pass_hash = self.get_argument(cnt.PASS, False)
        if database.is_user_valid(username, pass_hash):
            access_token_data = TokenHandler._generate_access_token()
            response = json.dumps({
                cnt.MSG_CODE: params[cnt.MSG_CODES][cnt.MSG_ALL_DONE],
                cnt.ACCESS_TOKEN: access_token_data.decode(),
                cnt.TOKEN_TYPE: cnt.JWT,
                cnt.EXPIRES_IN: params[cnt.JWT_LIFE_SPAN]
            })
        else:
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_NOT_FOUND])
        self.write(response)

    @staticmethod
    def _generate_access_token() -> bytes:
        with open(params[cnt.PRIVATE_KEY], 'rb') as file:
            private_key = file.read()
        payload = {
            cnt.ISS: params[cnt.ISSUER],
            cnt.EXP: time.time() + params[cnt.JWT_LIFE_SPAN],
        }
        access_token = jwt.encode(payload, key=private_key, algorithm='RS256')
        return access_token


# TODO: add user info validation
class AddUserHandler(tornado.web.RequestHandler):
    def post(self) -> None:
        access_token = self.get_argument(cnt.ACCESS_TOKEN, None)
        if access_token is None or not TokenValidator.verify_access_token(access_token):
            raise tornado.web.HTTPError(403, params[cnt.MSG_ACCESS_DENIED])
        database = DataBase()
        username, pass_hash = self.get_argument(cnt.USER, None), self.get_argument(cnt.PASS, None)
        if not (username_validator.validate(username)):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_INVALID_USERNAME])
        elif not (password_validator.validate(pass_hash)):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_INVALID_PASSWORD])
        elif database.is_such_user_in_base(username):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_IS_IN_BASE])
        else:
            database.add_user(username, pass_hash)
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_ALL_DONE])
        self.write(response)


def get_response_message(msg_code: int) -> str:
    response = json.dumps({
        cnt.MSG_CODE: msg_code
    })
    return response
