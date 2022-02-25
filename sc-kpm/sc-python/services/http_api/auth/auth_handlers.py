import tornado


class PrintTestHandler(tornado.web.RequestHandler):
    def get(self):
        print("Test")


class Token(tornado.web.RequestHandler):
    def post(self):
        database = db.DataBase()
        username = self.get_argument('user', False)
        pass_hash = self.get_argument('pass', False)
        if database.is_user_valid(username, pass_hash):
            print("123!")
            access_token_data = Token._generate_access_token()
            response = json.dumps({
                "access_token": access_token_data.decode(),
                "token_type": "JWT",
                "expires_in": tcns.JWT_LIFE_SPAN
            })
            self.write(response)
        else:
            raise tornado.web.HTTPError(403, 'Access denied')

    @staticmethod
    def _generate_access_token():
        with open('private.pem', 'rb') as file:
            private_key = file.read()
        payload = {
            "iss": tcns.ISSUER,
            "exp": time.time() + tcns.JWT_LIFE_SPAN,
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
            raise tornado.web.HTTPError(400, 'Invalid request')
        else:
            database.add_user(username, pass_hash, email)
