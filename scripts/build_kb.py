import argparse
from os.path import join, abspath, relpath, commonprefix, isdir, isfile, exists, dirname
import os
import shutil
import re
import configparser

FILENAME = "repo.path"
paths = set()
exclude_paths = set()
exclude_patterns = frozenset([".git*"])
# A dir that contains the dir that contains this script - so that sc-machine/scripts/build_kb.py would become sc-machine
ostis_path = abspath(join(os.path.dirname(os.path.realpath(__file__)), "../"))

kb_scripts_path = join(ostis_path, "scripts/kb_scripts")

prepare_scripts = [
    join(kb_scripts_path, 'remove_scsi.py'),
    join(kb_scripts_path, 'gwf_to_scs.py')
]


REPO_FILE = "repo_file"
OUTPUT_PATH = "output_path"
LOGFILE_PATH = "errors_file_path"
CONFIG_PATH = "config_file_path"


def search_knowledge_bases(root_path: str):
    if isdir(root_path):
        paths.add(root_path)
    
    # if the line is a file, we presume it's a repo file and recursively read from it
    elif isfile(root_path):
        with open(join(root_path), 'r') as root_file:
            for line in root_file.readlines():
                # ignore comments and empty lines
                line = line.replace('\n', '')
                # note: with current implementation, line is considered a comment if it's the first character in the line
                if line.startswith('#') or re.match(r"^\s*$", line):
                    continue
                elif line.startswith('!'):
                    absolute_path = abspath(join(dirname(root_path), line[1:]))
                    exclude_paths.add(absolute_path)
                else:
                    absolute_path = abspath(join(dirname(root_path), line))
                    # ignore paths we've already checked
                    if absolute_path not in paths:
                        # recursively check each repo entry
                        search_knowledge_bases(absolute_path)
                    

    else:
        print("Folder", root_path, "is not found.")
        exit(1)

# return a file/dir name if it shouldn't be copied (returns .git folder and paths inside exclude_paths)
def ignore_files(directory, contents):
    ignored_files = set()
    # for each ignored path
    for path in exclude_paths:
        # check if a child of this folder is inside the excluded path
        for f in contents:
            if abspath(join(directory, f)) == path:
                ignored_files.add(f)
            # ignore glob patterns defined in exclude_patterns using shutil.ignore_patterns func factory
            ignored_files.update(shutil.ignore_patterns(*exclude_patterns)(directory, contents))
            
    # return a set of filenames that should be excluded
    return ignored_files

def copy_kb(output_path: str):
    prepared_kb = join(output_path, "prepared_kb")
    try:
        common_prefix = commonprefix(list(paths))
        for path in paths:
            # removes the common part from paths and copies what's left to prepared_kb
            # unless the file/folder is in excluded_paths 
            shutil.copytree(path, join(prepared_kb, relpath(
                path, common_prefix)), ignore=ignore_files, dirs_exist_ok=True)
    except Exception as e:
        print(e)


def parse_config(path: str) -> dict:
    config_dict = {REPO_FILE: '', OUTPUT_PATH: '', LOGFILE_PATH: ''}
    config = configparser.ConfigParser()
    if path != None:
        config.read(path)
        config_dict.update({'output_path': abspath(join(dirname(path), config['Repo']['Path']))})
        config_dict.update({'errors_file_path': abspath(join(dirname(path), config['Repo']['Logfile'], "prepare.log"))})

    return config_dict


def prepare_kb(kb_to_prepare: str, logfile: str):
    for script in prepare_scripts:
        os.system('python3 ' + script + ' ' + kb_to_prepare + ' ' + logfile)
    if exists(logfile):
        print("Error while executing prepare_kb scripts. Check " + logfile + " for logs")
        exit(1)


def build_kb(bin_folder: str, kb_to_build: str):
    bin_folder = join(bin_folder, "kb.bin")
    os.makedirs(bin_folder, exist_ok=True)
    # call sc-builder with required parameters and return the exitcode of the command
    return os.system(" ".join([join(ostis_path, "bin/sc-builder"), "-f", "-c", "-i", kb_to_build, "-o", bin_folder, "-e", join(ostis_path, "bin/extensions")]))



def main(args: dict):
    conf = parse_config(args["config_file_path"])

    # absolutize paths passed as flags

    # rewrite options which were given by flags
     
    for key in args.keys():
        if args[key] is not None:
            if key in [REPO_FILE, OUTPUT_PATH, LOGFILE_PATH]: 
                flag = abspath(join(os.getcwd(), args[key]))
            else:
                flag = args[key] 
            conf[key] = flag
    # output the final configuration for the script, just to be sure about what's going on.
    print("args:", conf)

    # prepared_kb will appear in the same folder as kb.bin
    kb_to_prepare = join(conf["output_path"], "prepared_kb")
    if isdir(kb_to_prepare):
        shutil.rmtree(kb_to_prepare)

    search_knowledge_bases(conf[REPO_FILE])

    if not paths:
        print("No KBs were found")
        exit(1)

    copy_kb(conf["output_path"])
    prepare_kb(kb_to_prepare, conf["errors_file_path"])
    exitcode = build_kb(conf["output_path"], kb_to_prepare)
    shutil.rmtree(kb_to_prepare)
    # exit with non-null code in case sc-builder encountered an error
    exit(exitcode)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(dest=REPO_FILE, type=str,
                        help="The entrypoint repo file. Folder paths in this file ")

    parser.add_argument('-o', '--output', dest="output_path",
                        help="Destination path - path where KB binaries will be stored. Taken from the config file unless overwritten by this flag.")

    parser.add_argument('-l', '--logfile', dest="errors_file_path",
                        help="Errors file path - in case of unsuccessful preparation, log will appear at this location. Taken from the config file unless overwritten by this flag.")

    parser.add_argument('-c', '--config', dest=CONFIG_PATH,
                        help="Config file path - path to the sc-machine config file (Note: config file has lower priority than flags!)")

    args = parser.parse_args()

    # convert args from namespace to a dict
    main(vars(args))
