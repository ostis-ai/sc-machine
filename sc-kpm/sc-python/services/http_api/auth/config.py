import http_api.auth.constants as cnt


params = {
    # path params
    cnt.SQLITE_DB_PATH: 'sqlite:///' + '../sc-machine/sc-kpm/sc-python/services/http_api/auth/data.db',
    cnt.PRIVATE_KEY: '../sc-machine/sc-kpm/sc-python/services/http_api/auth/private.pem',
    cnt.PUBLIC_KEY: '../sc-machine/sc-kpm/sc-python/services/http_api/auth/public.pem',
    # validator params
    cnt.USERNAME_PATTERN: '^[a-zA-Z][a-zA-Z0-9-_.]{1,20}$',
    cnt.PASSWORD_PATTERN: '^(?=.*\\d)(?=.*[a-z])(?=.*[A-Z])(?!.*\\s).*$',
    # token params
    cnt.JWT_LIFE_SPAN: 1800,
    cnt.ISSUER: 'sc-auth-server',
    # messages
    cnt.MSG_ACCESS_DENIED: 'Access denied',
    cnt.MSG_INVALID_REQUEST: 'Invalid request',
    # message codes
    cnt.MSG_CODES: {
        cnt.MSG_ALL_DONE: 0,
        cnt.MSG_INVALID_USERNAME: 1,
        cnt.MSG_INVALID_PASSWORD: 2,
        cnt.MSG_USER_NOT_FOUND: 3,
    }
}
