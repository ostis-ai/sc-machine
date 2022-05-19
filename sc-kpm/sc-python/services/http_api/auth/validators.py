import json
import jwt
import tornado
from enum import Enum

import http_api.auth.constants as cnt
from http_api.auth.config import params


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
                FileNotFoundError) as e:
            print(e)
            return False
        return True
