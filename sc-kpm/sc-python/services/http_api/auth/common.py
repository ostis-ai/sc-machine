import json

from http_api.auth import constants as cnt
from http_api.auth.config import params


def get_response_message(message_desc: str) -> str:
    response = json.dumps({
        cnt.MSG_CODE: params[cnt.MSG_CODES][message_desc],
        cnt.MSG_TEXT: params[cnt.MSG_TEXT][message_desc],
    })
    return response
