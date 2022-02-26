import http_api.auth.constants as cnt


params = {
    cnt.SQLITE_DB_PATH: 'sqlite:///' + '../sc-machine/sc-kpm/sc-python/services/http_api/auth/data.db',
    cnt.PRIVATE_KEY: '../sc-machine/sc-kpm/sc-python/services/http_api/auth/private.pem',
    cnt.USERNAME_PATTERN: '^[a-zA-Z][a-zA-Z0-9-_.]{1,20}$',
}
