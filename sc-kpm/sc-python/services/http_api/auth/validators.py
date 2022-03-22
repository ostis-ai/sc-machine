import json
import jwt
import tornado
from enum import Enum

import http_api.auth.constants as cnt
from http_api.auth.config import params


class TokenType(Enum):
    ACCESS = 0
    REFRESH = 1


class TokenValidator:
    @staticmethod
    def _validate_token(token):
        try:
            with open(params[cnt.PUBLIC_KEY_PATH], 'rb') as file:
                public_key = file.read()
                jwt.decode(token, public_key,
                        issuer=params[cnt.ISSUER],
                        algorithm='RS256')
        except (jwt.exceptions.InvalidTokenError,
                jwt.exceptions.InvalidSignatureError,
                jwt.exceptions.InvalidIssuerError,
                jwt.exceptions.ExpiredSignatureError,
                FileNotFoundError):
            return False
        return True

    @staticmethod
    def validate_typed_token(token_type: TokenType):
        def token_type_decorator(handler):
            def wrapper(self):
                request_params = json.loads(self.request.body)
                if token_type == TokenType.ACCESS:
                    token = request_params[cnt.ACCESS_TOKEN] if cnt.ACCESS_TOKEN in request_params else None
                else:
                    token = request_params[cnt.REFRESH_TOKEN] if cnt.REFRESH_TOKEN in request_params else None
                if token is None or not TokenValidator._validate_token(token):
                    raise tornado.web.HTTPError(403, params[cnt.MSG_ACCESS_DENIED])
                handler(self)
            return wrapper
        return token_type_decorator
