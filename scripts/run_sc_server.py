from os.path import join, abspath, relpath
import os
import argparse
import re


ostis_path = abspath(join(os.path.dirname(os.path.realpath(__file__)), "../"))


def parse_config(path: str) -> dict:
    config_dict = {'path': '', 'ext': ''}
    with open(path, mode='r') as config:
        reading_state = False
        for line in config.readlines():
            line = line.replace('\n', '')
            if line == '[Repo]':
                reading_state = True
            elif line == '[Extensions]':
                reading_state = True
            elif re.search(r'\[.+\]', line):
                reading_state = False
            if line.startswith('#'): 
                continue
            if line.find("Path = ") != -1 and reading_state:
                line = line.replace("Path = ", "")
                config_dict.update({'path': line})
            if line.find("Directory = ") != -1 and reading_state:
                line = line.replace('Directory = ', '')
                config_dict.update({'ext': line})
    return config_dict


def main(ext_path: str, bin_path: str, conf_path: str, verbose: bool, clear: bool):
    conf = parse_config(conf_path)
    if conf['path'] == '':
        bin_path = abspath(bin_path)
    elif bin_path is None:
        bin_path = relpath(conf['path'], ostis_path)

    if conf['ext'] == '':
        ext_path = abspath(ext_path)
    elif ext_path is None:
        ext_path = relpath(conf['ext'], ostis_path)


    run_command = " ".join([join(ostis_path, 'bin/sc-server'), '-r', bin_path, '-e', ext_path, '-i', conf_path])
    if verbose:
        run_command = " ".join([run_command, '-v'])
    if clear:
        run_command = " ".join([run_command, '--clear'])

    os.environ['LD_LIBRARY_PATH'] = join(ostis_path, "/bin")
    os.system(run_command)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-e', '--extensions', dest='ext_path', help='Path to directory with sc-memory extensions')
    parser.add_argument('--kb', help='Path to kb.bin', dest='bin_path')
    parser.add_argument('-v', '--verbose', help="Flag to don't save sc-memory state on exit", action='store_true')
    parser.add_argument('--clear', help='Flag to clear sc-memory on start', action='store_true')
    parser.add_argument('-c', '--config', help='Path to configuration file (Note: config file has lower priority than flags!)', required=True)

    args = parser.parse_args()

    main(args.ext_path, args.bin_path, args.config, args.verbose, args.clear)
