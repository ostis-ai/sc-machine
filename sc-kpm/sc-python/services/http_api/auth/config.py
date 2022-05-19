import http_api.auth.constants as cnt
from pathlib import Path

params = {
    # auth params
    cnt.PUBLIC_KEY_PATH: Path(__file__).parent / 'public.pem',
    cnt.ISSUER: 'sc-auth-server',
    cnt.AUTH_FLAG_STRING: 'option(AUTH_ENABLED ON)'
}
