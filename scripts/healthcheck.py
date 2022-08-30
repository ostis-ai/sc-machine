import argparse

import json
from websocket import create_connection

SC_SERVER_HOST = "host"
SC_SERVER_PORT = "port"

SC_SERVER_HOST_DEFAULT = "localhost"
SC_SERVER_PORT_DEFAULT = "8090"


def main(args: dict):
    try:
        ws = create_connection(f"ws://{args[SC_SERVER_HOST]}:{args[SC_SERVER_PORT]}")
    except Exception as e:
        print(e)
        exit(1)
    ws.send(json.dumps({"type": "healthcheck"}))
    result = ws.recv()
    ws.close()
    if result == "OK":
        exit(0)
    else:
        exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--host', type=str, dest=SC_SERVER_HOST, default=SC_SERVER_HOST_DEFAULT, help="Sc-server host")
    parser.add_argument(
        '--port', type=int, dest=SC_SERVER_PORT, default=SC_SERVER_PORT_DEFAULT, help="Sc-server port")
    args = parser.parse_args()

    main(vars(args))
