import json

from http_api.auth import constants as cnt
from http_api.auth.base_handler import BaseHandler
from http_api.auth.common import get_response_message
from http_api.auth.config import params
from http_api.auth.database import DataBase
from http_api.auth.validators import TokenValidator, TokenType
from http_api.auth.verifiers import username_verifier


class UserHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def post(self) -> None:
        """ Add new user """
        database = DataBase()
        request_params = self._get_request_params([cnt.NAME, cnt.PASSWORD, cnt.ROLE_ID])
        if not username_verifier.verify(request_params[cnt.NAME]):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_INVALID_USERNAME])
        elif database.is_such_user_in_base(request_params[cnt.NAME]):
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_IS_IN_BASE])
        else:
            user_added = database.add_user(**request_params)
            all_done_mes = params[cnt.MSG_CODES][cnt.MSG_ALL_DONE]
            invalid_role_mes = params[cnt.MSG_CODES][cnt.MSG_INVALID_ROLE]
            response = get_response_message(all_done_mes) if user_added else get_response_message(invalid_role_mes)
        self.write(response)

    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def get(self) -> None:
        """ Get info about user """
        database = DataBase()
        request_params = self._get_request_params([cnt.ID])
        human_info = database.get_user_by_id(**request_params)
        if human_info is not None:
            response = json.dumps({
                cnt.ID: human_info[cnt.ID],
                cnt.NAME: human_info[cnt.NAME],
                cnt.ROLE: human_info[cnt.ROLE]
            })
        else:
            response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_USER_NOT_FOUND])
        self.write(response)

    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def delete(self) -> None:
        """ Delete user """
        database = DataBase()
        request_params = self._get_request_params([cnt.ID])
        database.delete_user_by_id(**request_params)
        response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_ALL_DONE])
        self.write(response)

    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def put(self) -> None:
        """ Update user """
        database = DataBase()
        post_args = self._get_request_params([cnt.ID, cnt.NAME, cnt.PASSWORD, cnt.ROLE_ID])
        database.update_user_by_id(**post_args)
        response = get_response_message(params[cnt.MSG_CODES][cnt.MSG_ALL_DONE])
        self.write(response)


class UsersListHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def get(self) -> None:
        database = DataBase()
        users = database.get_users()
        response = json.dumps({
            cnt.USERS: users
        })
        self.write(response)
