# Authentication

To activate authentication you should set `option(AUTH_ENABLED ON)` in CMakeLists.txt

To use it you should activate clone and activate [auth-server](https://github.com/ostis-ai/sc-server-auth)

Also make sure you have public key from auth-server `public.pem` in `sc-kpm/sc-python/http_api/auth`