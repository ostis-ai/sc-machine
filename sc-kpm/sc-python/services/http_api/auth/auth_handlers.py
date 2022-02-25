import json
import jwt
import time
import tornado

import constants as cnt


class PrintTestHandler(tornado.web.RequestHandler):
    def get(self):
        print("Test")


class Token(tornado.web.RequestHandler):
    def post(self):
        database = db.DataBase()
        username = self.get_argument(cnt.USER, False)
        pass_hash = self.get_argument(cnt.PASS, False)
        if database.is_user_valid(username, pass_hash):
            access_token_data = Token._generate_access_token()
            response = json.dumps({
                cnt.ACCESS_TOKEN: access_token_data.decode(),
                cnt.TOKEN_TYPE: cnt.JWT,
                cnt.EXPIRES_IN: cnt.JWT_LIFE_SPAN
            })
            self.write(response)
        else:
            raise tornado.web.HTTPError(403, cnt.MSG_ACCESS_DENIED)

    @staticmethod
    def _generate_access_token():
        with open('private.pem', 'rb') as file:
            private_key = file.read()
        payload = {
            "iss": cnt.ISSUER,
            "exp": time.time() + cnt.JWT_LIFE_SPAN,
        }
        access_token = jwt.encode(payload, key=private_key, algorithm='HS256')
        return access_token


# TODO: add user info validation
class AddUser(tornado.web.RequestHandler):
    def post(self):
        database = db.DataBase()
        username = self.get_argument('user', False)
        pass_hash = self.get_argument('pass', False)
        email = self.get_argument('email', False)
        if not (username and pass_hash and email):
            raise tornado.web.HTTPError(400, cnt.MSG_INVALID_REQUEST)
        else:
            database.add_user(username, pass_hash, email)
