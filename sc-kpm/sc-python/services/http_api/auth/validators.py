import re

import http_api.auth.constants as cnt
from http_api.auth.config import params


class Validator:
    def __init__(self, pattern: str) -> None:
        self.pattern = re.compile(pattern)

    def validate(self, string: str) -> bool:
        return bool(self.pattern.match(string))


username_validator = Validator(params[cnt.USERNAME_PATTERN])
