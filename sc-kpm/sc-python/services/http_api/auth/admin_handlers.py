from http_api.auth import constants as cnt
import json
from http_api.auth.base_handler import BaseHandler
from http_api.auth.common import get_response_message
from http_api.auth.validators import TokenValidator, TokenType


class UserHandler(BaseHandler):
    @TokenValidator.validate_typed_token(TokenType.ACCESS)
    def post(self) -> None:
        """ Add new user """
        print(json.loads(self.request.body))
        response = get_response_message(cnt.MSG_ALL_DONE)
        self.write(response)
