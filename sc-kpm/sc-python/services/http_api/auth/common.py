import json

from http_api.auth import constants as cnt
from http_api.auth.config import params


def get_response_message(msg_code: int) -> str:
    response = json.dumps({
        cnt.MSG_CODE: msg_code,
        cnt.MSG_TEXT: params[cnt.MSG_TEXT][msg_code],
    })
    return response
