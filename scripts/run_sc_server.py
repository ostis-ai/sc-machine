from os.path import join, abspath
import os
import argparse


ostis_path = abspath(join(os.path.dirname(os.path.realpath(__file__)), "../"))


def main(args: dict):
    run_command = " ".join([join(ostis_path, 'bin/sc-server')])
    if args['ext_path']:
        path = abspath(join(os.getcwd(), args['ext_path']))
        run_command = " ".join([run_command, '-e', path])
    if args['bin_path']:
        path = abspath(join(os.getcwd(), args['kb']))
        run_command = " ".join([run_command, '--kb', path])
    if args['config']:
        path = abspath(join(os.getcwd(), args['config']))
        run_command = " ".join([run_command, '--config', path])
    if args['verbose']:
        run_command = " ".join([run_command, '-v'])
    if args['clear']:
        run_command = " ".join([run_command, '--clear'])

    os.system(run_command)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-e', '--extensions', dest='ext_path', help='Path to directory with sc-memory extensions')
    parser.add_argument('--kb', help='Path to kb.bin', dest='bin_path')
    parser.add_argument('-v', '--verbose', help="Flag to don't save sc-memory state on exit", action='store_true')
    parser.add_argument('--clear', help='Flag to clear sc-memory on start', action='store_true')
    parser.add_argument('-c', '--config', help='Path to configuration file (Note: config file has lower priority than flags!)', required=True)

    args = parser.parse_args()

    main(vars(args))
