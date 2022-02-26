import re

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


username_validator = CredentialsValidator(params[cnt.USERNAME_PATTERN])
password_validator = CredentialsValidator(params[cnt.PASSWORD_PATTERN])
