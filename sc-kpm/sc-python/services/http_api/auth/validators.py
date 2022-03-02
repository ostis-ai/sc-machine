import re
import json
import jwt
import tornado

import http_api.auth.constants as cnt
from http_api.auth.config import params


class BaseValidator:
    def __init__(self, pattern: str) -> None:
        self.pattern = re.compile(pattern)

    def validate(self, string: str) -> bool:
        return bool(self.pattern.match(string))


class CredentialsValidator(BaseValidator):
    def validate(self, string: str) -> bool:
        return string is not None and super().validate(string)


class TokenValidator:
    @staticmethod
    def verify_access_token(access_token):
        with open(params[cnt.PUBLIC_KEY], 'rb') as file:
            public_key = file.read()
        try:
            jwt.decode(access_token, public_key,
                       issuer=params[cnt.ISSUER],
                       algorithm='RS256')
        except (jwt.exceptions.InvalidTokenError,
                jwt.exceptions.InvalidSignatureError,
                jwt.exceptions.InvalidIssuerError,
                jwt.exceptions.ExpiredSignatureError):
            return False
        return True

    @staticmethod
    def validate_post(post):
        def wrapper(self):
            access_token = self.get_argument(cnt.ACCESS_TOKEN, None)
            if access_token is None or not TokenValidator.verify_access_token(access_token):
                raise tornado.web.HTTPError(403, params[cnt.MSG_ACCESS_DENIED])
            post(self)
        return wrapper

    @staticmethod
    def validate_json(on_message):
        def wrapper(self, msg):
            socket_params = json.loads(msg)
            access_token = params[cnt.ACCESS_TOKEN] if cnt.ACCESS_TOKEN in params else None
            if access_token is None or not TokenValidator.verify_access_token(access_token):
                raise tornado.web.HTTPError(403, socket_params[cnt.MSG_ACCESS_DENIED])
            on_message(self, msg)
        return wrapper


username_validator = CredentialsValidator(params[cnt.USERNAME_PATTERN])
password_validator = CredentialsValidator(params[cnt.PASSWORD_PATTERN])
