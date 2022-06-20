from sc_client import client
from sc_client.models import ScIdtfResolveParams
from sc_client.constants import sc_types

try:
    client.connect("ws://localhost:8090/ws_json")
    params = ScIdtfResolveParams(idtf='technology_OSTIS', type=sc_types.NODE_CONST)
    addr = client.resolve_keynodes(params)
    if not addr:
        client.disconnect()
        exit(1)
    client.disconnect()
    exit(0)
except ConnectionRefusedError:
    exit(1)
