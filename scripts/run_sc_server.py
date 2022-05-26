from os.path import join, abspath
import os
import argparse


ostis_path = abspath(join(os.path.dirname(os.path.realpath(__file__)), "../"))

def path_handler(run_command, flag) -> str:
    path = abspath(join(os.getcwd(), args[flag]))
    return " ".join([run_command, '--' + flag, path])

def bool_handler(run_command, flag) -> str:
    run_command = " ".join([run_command, '--' + flag])

def main(args: dict):
    run_command = " ".join([join(ostis_path, 'bin/sc-server')])
    handlers = {"extensions": path_handler, "kb": path_handler, "config": path_handler, "verbose": bool_handler, "clear": bool_handler}
    for flag, handler in handlers.items():
        if args[flag]:
            run_command = handler(run_command, flag)
    os.system(run_command)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-e', '--extensions', dest='extensions', help='Path to directory with sc-memory extensions')
    parser.add_argument('--kb', help='Path to kb.bin', dest='kb')
    parser.add_argument('-v', '--verbose', help="Flag to don't save sc-memory state on exit", action='store_true')
    parser.add_argument('--clear', help='Flag to clear sc-memory on start', action='store_true')
    parser.add_argument('-c', '--config', help='Path to configuration file (Note: config file has lower priority than flags!)', required=True)

    args = vars(parser.parse_args())

    main(args)
