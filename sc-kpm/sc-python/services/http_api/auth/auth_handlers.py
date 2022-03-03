import json
import jwt
import time
import tornado
from typing import Dict, List

from http_api.auth import constants as cnt
from http_api.auth.database import DataBase
from http_api.auth.config import params
from http_api.auth.validators import TokenValidator, TokenType
from http_api.auth.verifiers import username_verifier


def _generate_token(token_type: TokenType) -> bytes:
    with open(params[cnt.PRIVATE_KEY], 'rb') as file:
        private_key = file.read()
    access_token_life_span = params[cnt.ACCESS_TOKEN_LIFE_SPAN]
    refresh_token_life_span = params[cnt.REFRESH_TOKEN_LIFE_SPAN]
    life_span = access_token_life_span if token_type == TokenType.ACCESS else refresh_token_life_span
    payload = {
        cnt.ISS: params[cnt.ISSUER],
        cnt.EXP: time.time() + life_span,
    }
    token = jwt.encode(payload, key=private_key, algorithm='RS256')
    return token


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

    def _get_request_params(self, arg_names: List[str]) -> Dict[str, str]:
        res = dict()
        request_params = json.loads(self.request.body)
        for arg_name in arg_names:
            param = request_params[arg_name] if arg_name in request_params else None
            res[arg_name] = param
        return res


class GetTokensHandler(BaseHandler):
    def post(self) -> None:
        database = DataBase()
        username, password = self._get_user_credentials()
        if database.is_user_valid(username, password):
            access_token_data = _generate_token(TokenType.ACCESS)
            refresh_token_data = _generate_token(TokenType.REFRESH)
            response = json.dumps({
                cnt.MSG_CODE: params[cnt.MSG_CODES][cnt.MSG_ALL_DONE],
                cnt.ACCESS_TOKEN: {
                    cnt.TOKEN: access_token_data.decode(),
                    cnt.TOKEN_TYPE: cnt.JWT,
                    cnt.EXPIRES_IN: params[cnt.ACCESS_TOKEN_LIFE_SPAN],
                },
                cnt.REFRESH_TOKEN: {
                    cnt.TOKEN: refresh_token_data.decode(),
                    cnt.TOKEN_TYPE: cnt.JWT,
                    cnt.EXPIRES_IN: params[cnt.REFRESH_TOKEN_LIFE_SPAN],
                }
            })
        else:
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_NOT_FOUND])
        self.write(response)

    def _get_user_credentials(self):
        data = json.loads(self.request.body)
        username = data[cnt.USERNAME] if cnt.USERNAME in data else False
        password = data[cnt.PASSWORD] if cnt.PASSWORD in data else False
        return username, password


class GetAccessTokenHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.REFRESH)
    def post(self) -> None:
        access_token_data = _generate_token(TokenType.ACCESS)
        response = json.dumps({
            cnt.MSG_CODE: params[cnt.MSG_CODES][cnt.MSG_ALL_DONE],
            cnt.TOKEN: access_token_data.decode(),
            cnt.TOKEN_TYPE: cnt.JWT,
            cnt.EXPIRES_IN: params[cnt.ACCESS_TOKEN_LIFE_SPAN],
        })
        self.write(response)


class AddUserHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def post(self) -> None:
        database = DataBase()
        request_params = self._get_request_params([cnt.USERNAME, cnt.PASSWORD, cnt.ROLE])
        if not username_verifier.validate(request_params[cnt.USERNAME]):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_INVALID_USERNAME])
        elif database.is_such_user_in_base(request_params[cnt.USERNAME]):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_IS_IN_BASE])
        else:
            user_added = database.add_user(**request_params)
            all_done_mes = params[cnt.MSG_CODES][cnt.MSG_ALL_DONE]
            invalid_role_mes = params[cnt.MSG_CODES][cnt.MSG_INVALID_ROLE]
            response = get_response_message(all_done_mes) if user_added else get_response_message(invalid_role_mes)
        self.write(response)


class DeleteUserHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def post(self) -> None:
        database = DataBase()
        request_params = self._get_request_params([cnt.USER_ID])
        database.delete_user_by_id(request_params[cnt.USER_ID])
        response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_ALL_DONE])
        self.write(response)


class UpdateUserHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def post(self) -> None:
        database = DataBase()
        post_args = self._get_request_params([cnt.USER_ID, cnt.USERNAME, cnt.PASSWORD, cnt.ROLE])
        database.update_user_by_id(**post_args)
        response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_ALL_DONE])
        self.write(response)


def get_response_message(msg_code: int) -> str:
    response = json.dumps({
        cnt.MSG_CODE: msg_code
    })
    return response
