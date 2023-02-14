import sys
import os
import re


def process_scsi(scsi_file_path , structure_name) -> None:
    # Try to optimize 
    # Append structure name at the beginning of file and close structure at the end
        with open(scsi_file_path, 'r', encoding='utf-8') as scsi_file:
            content = scsi_file.read()
        with open(scsi_file_path, "w", encoding='utf-8') as scsi_file:
            scsi_file.seek(0, 0)
            scsi_file.write(structure_name + " [*" + content + "*];;")

	# Rename scsi file to content.scs
        pre, _ = os.path.splitext(scsi_file.name)
        os.rename(scsi_file.name, pre + "_content.scs")


def process_file(scsi_file_paths, dir_path, buffer_file_list) -> list:
    for scsi_file_path in scsi_file_paths:
        edit_line_list = scsi_file_path[0].split('[*^"file://')
        # Get scsi file path
        scsi_path = edit_line_list[1].split('"*];;')[0]

        relative_path = ""
        if scsi_path.rfind('/') >= 0:
            relative_path = scsi_path[:scsi_path.rfind('/') + 1]

        process_scsi(os.path.join(dir_path, scsi_path), edit_line_list[0])

    return buffer_file_list


def main(work_directory: str) -> None:
    list_of_files = list()
    # Recursively walk through working directory and list all files
    for (dir_path, _, file_names) in os.walk(work_directory):
        list_of_files += [os.path.join(dir_path, file) for file in file_names]

    # Process every file in list
    for file in list_of_files:
        if file.endswith(".scs"):
            is_file_processed = False
            # List of strings for cleared file
            buffer_file_list = []
            with open(file, 'r', encoding='utf-8') as scs_file:
                dir_path = os.path.split(file)[0]
                for scs_file_line in scs_file:
                    # Get the list of files with scsi
                    scsi_file_paths = re.findall(r"((\w*\s*=\s*)?\[\*\^\"file:\/\/.*\.scsi\"\*];;)", scs_file_line)
                    if len(scsi_file_paths) > 0:
                        buffer_file_list = process_file(scsi_file_paths, dir_path, buffer_file_list)
                        is_file_processed = True
                    else:
                        buffer_file_list.append(scs_file_line)
            if is_file_processed:
                # If were changes write them to file
                with open(file, "w", encoding="utf_8") as scs_file:
                    for scs_file_line in buffer_file_list:
                        scs_file.write(scs_file_line)


if __name__ == '__main__':
    if len(sys.argv) == 3:
        main(sys.argv[1])
    else:
        print("invalid number of arguments, Please specify only the work directory")
