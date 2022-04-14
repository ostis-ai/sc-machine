from os.path import join, abspath
import os
import argparse


ostis_path = abspath(join(os.path.dirname(os.path.realpath(__file__)), "../"))


def main(config_path: str):
    config_path = abspath(config_path)

    run_command = " ".join([join(ostis_path, 'bin/sctp-server'), config_path])

    os.system(run_command)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    
    parser.add_argument('-c', '--config', help='Path to configuration file', required=True)

    args = parser.parse_args()

    main(args.config)
