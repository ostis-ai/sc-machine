import http_api.auth.constants as cnt


params = {
    # path params
    cnt.PUBLIC_KEY_PATH: '../sc-machine/sc-kpm/sc-python/services/http_api/auth/public.pem',
    cnt.AUTH_FLAG: False,
    # validator patters
    # token params
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
        cnt.MSG_USER_IS_IN_BASE: 4,
    },
    # messages text
    cnt.MSG_TEXT: {
        cnt.MSG_ALL_DONE: "All done",
        cnt.MSG_INVALID_USERNAME: "User name is incorrect",
        cnt.MSG_INVALID_PASSWORD: "User password is incorrect",
        cnt.MSG_USER_NOT_FOUND: "User not found in database",
        cnt.MSG_USER_IS_IN_BASE: "User already exists in database",
    }
}
