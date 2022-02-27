import re
import jwt

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
                       issuer=cnt.ISSUER,
                       algorithm='RS256')
        except (jwt.exceptions.InvalidTokenError,
                jwt.exceptions.InvalidSignatureError,
                jwt.exceptions.InvalidIssuerError,
                jwt.exceptions.ExpiredSignatureError):
            return False
        return True


username_validator = CredentialsValidator(params[cnt.USERNAME_PATTERN])
password_validator = CredentialsValidator(params[cnt.PASSWORD_PATTERN])
