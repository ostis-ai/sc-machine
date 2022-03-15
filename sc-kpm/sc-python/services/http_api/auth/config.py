import http_api.auth.constants as cnt


params = {
    # path params
    cnt.SQLITE_DB_PATH: 'sqlite:///' + '../sc-machine/sc-kpm/sc-python/services/http_api/auth/database.db',
    cnt.PRIVATE_KEY_PATH: '../sc-machine/sc-kpm/sc-python/services/http_api/auth/private.pem',
    cnt.PUBLIC_KEY_PATH: '../sc-machine/sc-kpm/sc-python/services/http_api/auth/public.pem',
    # validator patters
    cnt.USERNAME_PATTERN: '^[a-zA-Z][a-zA-Z0-9-_.]{1,20}$',
    # token params
    cnt.ACCESS_TOKEN_LIFE_SPAN: 1800,
    cnt.REFRESH_TOKEN_LIFE_SPAN: 2592000,
    cnt.ISSUER: 'sc-auth-server',
    # messages
    cnt.MSG_ACCESS_DENIED: 'Access denied',
    cnt.MSG_INVALID_REQUEST: 'Invalid request',
    # message codes
    cnt.MSG_CODES: {
        cnt.MSG_ALL_DONE: 0,
        cnt.MSG_INVALID_USERNAME: 1,
        cnt.MSG_INVALID_PASSWORD: 2,
        cnt.MSG_INVALID_ROLE: 3,
        cnt.MSG_USER_NOT_FOUND: 4,
        cnt.MSG_USER_IS_IN_BASE: 5,
    },
    # messages text
    cnt.MSG_TEXT: {
        cnt.MSG_ALL_DONE: "All done",
        cnt.MSG_INVALID_USERNAME: "User name is incorrect",
        cnt.MSG_INVALID_PASSWORD: "User password is incorrect",
        cnt.MSG_INVALID_ROLE: "User role is invalid",
        cnt.MSG_USER_NOT_FOUND: "User not found in database",
        cnt.MSG_USER_IS_IN_BASE: "User already exists in database",
    },
    # roles
    cnt.ROLES: {
        cnt.ASSISTANT_ROLE: 0,
        cnt.PATIENT_ROLE: 1,
        cnt.ADMIN_ROLE: 255,
    }
}
