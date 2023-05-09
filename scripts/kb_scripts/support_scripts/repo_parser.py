import os
import re
from os.path import exists, splitext, join, abspath, dirname, isfile, isdir
from typing import Set

REPO_PATH_EXT = ".path"

def normalize_path(path: str) -> str:
    return path.replace('\n', '').strip()


def is_source_file(path: str) -> bool:
    if not isfile(path):
        return False

    ext = splitext(path)[1]
    return ext == ".scs" or ext == ".gwf"


def is_skip_text(string: str) -> bool:
    return re.match(r"^\s*$", string) or string.startswith('#')


def is_excluded_path(path: str) -> bool:
    return path.startswith('!')


def is_repo_path(path: str) -> bool:
    return splitext(path)[1] == REPO_PATH_EXT


def parse_exclude_paths(path: str) -> Set[str]:
    exclude_paths = set()

    if not is_repo_path(path):
        return exclude_paths

    with open(join(path), 'r') as root_file:
        for line in root_file.readlines():
            source_path = normalize_path(line)

            if source_path.startswith('!'):
                source_path = source_path.replace('!', '')
                exclude_paths.add(abspath(join(dirname(path), source_path)))

    return exclude_paths


def collect_files(path: str, exclude_paths: Set[str], sources: Set[str]):
    if path in exclude_paths:
        return

    if is_source_file(path):
        sources.add(path)
        return

    if not isdir(path):
        return

    for source_path in os.listdir(path):
        source_path = join(path, source_path)

        if is_source_file(source_path) and source_path not in exclude_paths:
            sources.add(source_path)
        elif isdir(source_path):
            collect_files(source_path, exclude_paths, sources)


def parse_knowledge_bases_sources(path: str, exclude_paths: Set[str], sources: Set[str]):
    if not exists(path):
        print(path, "does not exist.")

    if path in exclude_paths:
        return
    elif is_repo_path(path):
        with open(join(path), 'r') as root_file:
            for line in root_file.readlines():
                source_path = normalize_path(line)

                # note: with current implementation, line is considered a comment if it's the first character
                # in the line
                if is_skip_text(source_path) or is_excluded_path(source_path):
                    continue

                source_path = abspath(join(dirname(path), source_path))
                # recursively check each repo entry
                collect_files(source_path, exclude_paths, sources)
    else:
        collect_files(path, exclude_paths, sources)


def parse_repo_path(path: str) -> Set[str]:
    sources = set()
    exclude_paths = parse_exclude_paths(path)
    parse_knowledge_bases_sources(path, exclude_paths, sources)
    return sources
