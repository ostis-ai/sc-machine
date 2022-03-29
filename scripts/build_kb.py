import argparse
from os.path import join, abspath, relpath, commonprefix, isdir, exists
import os
import shutil

paths = set()

#A dir that contains the dir that contains this script - so that sc-machine/scripts/build_kb.py would become sc-machine
ostis_path = abspath(join(os.path.dirname(os.path.realpath(__file__)), "../"))

kb_scripts_path = join(ostis_path, "scripts/kb_scripts")

prepare_scripts = [
    join(kb_scripts_path, 'remove_scsi.py'),
    join(kb_scripts_path, 'gwf_to_scs.py')
]


def search_knowledge_bases(root_path: str, output_path: str, filename: str):
    try:
        with open(join(root_path, filename), 'r') as root_file:
            for line in root_file.readlines():
                line = line.replace('\n', '')
                # note: with current implementation, line is considered a comment if it's the first character in the line
                if line in paths or line.startswith('#'):
                    continue
                else:
                    absolute_path = abspath(join(root_path, line))
                    paths.add(absolute_path)
                    search_knowledge_bases(
                        absolute_path, output_path, filename)
    except FileNotFoundError:
        #this branch will be used when a subsequent KB doesn't have any dependencies
        pass


def copy_kb(output_path: str):
    prepared_kb = join(output_path, "prepared_kb")
    try:
        for path in paths:
            common_prefix = commonprefix(list(paths))
            # removes the common part from paths and copies what's left to prepared_kb
            shutil.copytree(path, join(prepared_kb, relpath(
                path, common_prefix)), dirs_exist_ok=True)
    except Exception as e:
        print(e)


def prepare_kb(kb_to_prepare: str, logfile: str):
    for script in prepare_scripts:
        os.system('python3 ' + script + ' ' + kb_to_prepare + ' ' + logfile)
    if exists(logfile):
        print("Error while executing prepare_kb scripts. Check " + logfile + " for logs")
        exit(1)


def build_kb(kb_to_build: str):
    bin_folder = join(os.getcwd(), "kb.bin")
    os.makedirs(bin_folder, exist_ok=True)
    os.environ['LD_LIBRARY_PATH'] = join(ostis_path, "/bin")
    #call sc-builder with required parameters
    os.system(" ".join([join(ostis_path, "bin/sc-builder"), "-f", "-c", "-i", kb_to_build, "-o", bin_folder, "-s", join(ostis_path, "config/sc-web.ini"), "-e", join(ostis_path, "bin/extensions")]))


def main(root_repo_path: str, output_path: str, logfile: str, repo_filename: str):
    root_repo_path = abspath(root_repo_path)
    output_path = abspath(output_path)
    logfile = abspath(logfile)

    kb_to_prepare = join(output_path, "prepared_kb")
    if isdir(kb_to_prepare):
        shutil.rmtree(kb_to_prepare)

    search_knowledge_bases(root_repo_path, output_path, repo_filename)

    if not paths:
        print("No KBs were found")
        exit(1)

    copy_kb(output_path)
    prepare_kb(kb_to_prepare, logfile)
    build_kb(kb_to_prepare)
    shutil.rmtree(kb_to_prepare)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(dest="repo_folder", type=str,
                        help="The entrypoint folder, should contain a repo file (repo.path by default)")

    parser.add_argument('-o', '--output', dest="output_path",
                        help="Destination path - path where prepared KB and built KB (kb.bin) will be stored. Default: <cwd>", default=os.getcwd())

    parser.add_argument('-l', '--logfile', dest="errors_file_path",
                        help="Errors file path - in case of unsuccessful preparation, log will appear at this location. Default: <cwd>/prepare.log", default=join(os.getcwd(), "prepare.log"))

    parser.add_argument('-f', '--filename', dest="repo_path_name",
                        help="Repo file name - a filename for repo file that will be used in all subsequent KBs. Default: repo.path", default="repo.path")

    args = parser.parse_args()

    main(args.repo_folder, args.output_path,
         args.errors_file_path, args.repo_path_name)
