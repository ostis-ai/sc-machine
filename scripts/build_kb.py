import argparse
from os.path import join, abspath, exists, isdir
import os
from shutil import rmtree
import configparser

# kb_scripts folder near this script by default
kb_scripts_path = join(os.path.dirname(os.path.realpath(__file__)), "kb_scripts")

prepare_scripts = [
    join(kb_scripts_path, 'remove_scsi.py'),
    join(kb_scripts_path, 'gwf_to_scs.py')
]

REPO_FILE = "repo_file"
OUTPUT_PATH = "output_path"
LOGFILE_PATH = "errors_file_path"
CONFIG_PATH = "config_file_path"
OSTIS_PATH = "ostis_path"


def parse_config(path: str) -> dict:
    config_dict = {REPO_FILE: '', OUTPUT_PATH: '', LOGFILE_PATH: '', OSTIS_PATH: abspath(join(os.path.dirname(os.path.realpath(path)), "bin"))}
    config = configparser.ConfigParser()
    if path is not None:
        config.read(path)

        path_to_file = os.path.abspath(path).rsplit('/', 1)[0] + '/'

        output_path = config['sc-memory']['repo_path']
        config_dict.update(
            {'output_path': output_path if output_path[0] == '/' else path_to_file + output_path})

        log_file = config['sc-builder']['log_file']
        config_dict.update(
            {'errors_file_path': log_file if log_file[0] == '/' else path_to_file + log_file})

    return config_dict


def prepare_kb(kb_to_prepare: str, logfile: str):
    for script in prepare_scripts:
        os.system('python3 ' + script + ' ' + kb_to_prepare + ' ' + logfile)
    if exists(logfile):
        print("Error while executing prepare_kb scripts. Check " + logfile + " for logs")
        exit(1)


def build_kb(kb_bin_folder: str, kb_to_build: str, ostis_path: str, config_path: str):
    os.makedirs(kb_bin_folder, exist_ok=True)
    # call sc-builder with required parameters and return the exitcode of the command
    return os.system(" ".join([join(ostis_path, "sc-builder"), "-f", "--clear", "-i", kb_to_build, "-o", kb_bin_folder, "-c", config_path]))


def main(args: dict):
    conf = parse_config(args["config_file_path"])
    # absolutize paths passed as flags

    # rewrite options which were given by flags
    for key in args.keys():
        if args[key] is not None:
            if key in [REPO_FILE, OUTPUT_PATH, LOGFILE_PATH, OSTIS_PATH]:
                flag = abspath(join(os.getcwd(), args[key]))
            else:
                flag = args[key]
            conf[key] = flag
    # output the final configuration for the script, just to be sure about what's going on.
    print("args:", conf)

    if not exists(join(conf[OSTIS_PATH], "sc-builder")):
        print("OSTIS binaries are not found. Check if the binary path exists and contains necessary files. You may"
              "have compiled the project in a non-default location (pass -b flag to override binary location) or "
              "didn't build the project successfully.")
        exit(1)

    if isdir(conf["output_path"]):
        for entry in os.scandir(conf["output_path"]):
            if entry.is_file(): os.remove(entry.path)
            if entry.is_dir(): shutil.rmtree(entry.path)

    prepare_kb(conf[REPO_FILE], conf[LOGFILE_PATH])
    exitcode = build_kb(conf[OUTPUT_PATH], conf[REPO_FILE], conf[OSTIS_PATH], abspath(args["config_file_path"]))
    # exit with non-null code in case sc-builder encountered an error
    exit(exitcode)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(dest=REPO_FILE, type=str,
                        help="The entrypoint repo file. Folder paths in this file ")

    parser.add_argument('-o', '--output_path', dest=OUTPUT_PATH,
                        help="Destination path - path where KB binaries will be stored. Taken from the config file "
                             "unless overwritten by this flag.")

    parser.add_argument('-l', '--log_file', dest=LOGFILE_PATH,
                        help="Errors file path - in case of unsuccessful preparation, log will appear at this "
                             "location. Taken from the config file unless overwritten by this flag.")

    parser.add_argument('-c', '--config', dest=CONFIG_PATH,
                        help="Config file path - path to the sc-machine config file (Note: config file has lower "
                             "priority than flags!)")

    parser.add_argument('-b', '--ostis_path', dest=OSTIS_PATH,
                        help="Path to the compiled binaries of OSTIS system (<config_dir>/bin used by default)")

    args = parser.parse_args()

    # convert args from namespace to a dict
    main(vars(args))
