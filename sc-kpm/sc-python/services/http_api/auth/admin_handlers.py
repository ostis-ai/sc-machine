import json

from http_api.auth import constants as cnt
from http_api.auth.base_handler import BaseHandler
from http_api.auth.common import get_response_message
from http_api.auth.database import DataBase
from http_api.auth.validators import TokenValidator, TokenType
from http_api.auth.verifiers import username_verifier


def _verify_user_info_in_database(database: DataBase, name: str, password: str) -> str:
    if not username_verifier.verify(name):
        message_desc = cnt.MSG_INVALID_USERNAME
    elif len(password.strip()) == 0:
        message_desc = cnt.MSG_INVALID_PASSWORD
    elif database.is_such_user_in_base(name):
        message_desc = cnt.MSG_USER_IS_IN_BASE
    else:
        message_desc = cnt.MSG_ALL_DONE
    return message_desc


class UserHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def post(self) -> None:
        """ Add new user """
        database = DataBase()
        request_params = self._get_request_params([cnt.NAME, cnt.PASSWORD])
        msg_desc = _verify_user_info_in_database(
            database,
            name=request_params[cnt.NAME],
            password=request_params[cnt.PASSWORD],
        )
        response = get_response_message(msg_desc)
        if msg_desc == cnt.MSG_ALL_DONE:
            database.add_user(**request_params)
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
                cnt.NAME: human_info[cnt.NAME]
            })
        else:
            response = get_response_message(cnt.MSG_USER_NOT_FOUND)
        self.write(response)

    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def delete(self) -> None:
        """ Delete user """
        database = DataBase()
        request_params = self._get_request_params([cnt.ID])
        delete_users_count = database.delete_user_by_id(**request_params)
        if delete_users_count == 0:
            response = get_response_message(cnt.MSG_USER_NOT_FOUND)
        else:
            response = get_response_message(cnt.MSG_ALL_DONE)
        self.write(response)

    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def put(self) -> None:
        """ Update user """
        database = DataBase()
        request_params = self._get_request_params([cnt.ID, cnt.NAME, cnt.PASSWORD])
        msg_desc = _verify_user_info_in_database(
            database,
            name=request_params[cnt.NAME],
            password=request_params[cnt.PASSWORD]
        )
        response = get_response_message(msg_desc)
        if msg_desc == cnt.MSG_ALL_DONE:
            updates_users_count = database.update_user_by_id(**request_params)
            if updates_users_count == 0:
                response = get_response_message(cnt.MSG_USER_NOT_FOUND)
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
