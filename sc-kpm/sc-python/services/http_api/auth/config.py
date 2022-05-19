import http_api.auth.constants as cnt
from pathlib import Path

params = {
    # auth params
    cnt.PUBLIC_KEY_PATH: Path(__file__).parent / 'public.pem' ,
    cnt.AUTH_FLAG: False,
    cnt.ISSUER: 'sc-auth-server',
}
