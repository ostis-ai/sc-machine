import argparse
import os
import sys
from xml.etree.ElementTree import ParseError

from support_scripts.gwf_parser import GWFParser
from support_scripts.scs_writer import SCsWriter


class Gwf2SCs:
    GWF_FORMAT = '.gwf'
    SCS_FORMAT = '.scs'

    def __init__(self):
        self.errors = []

    def run(self, params):
        file = params.input
        elements = {}
        error = Gwf2SCs.parse_gwf(file, elements)
        if error is None:
            errors = self.convert_to_scs(file + Gwf2SCs.SCS_FORMAT, elements)
            if len(errors) != 0:
                for error in errors:
                    self.log_error(file, error)
                return

        else:
            self.log_error(file, error)
            return

    @staticmethod
    def parse_gwf(input_path, elements):
        try:
            gwf_parser = GWFParser(elements)
            return gwf_parser.parse(input_path)
        except (TypeError, ParseError) as e:
            return e

    @staticmethod
    def convert_to_scs(output_path, elements):
        dir_name = os.path.dirname(output_path)
        if not os.path.isdir(dir_name):
            os.makedirs(dir_name)

        if elements is not None:
            writer = SCsWriter(output_path)
            return writer.write(elements)

        return None

    def log_error(self, file, error):
        self.errors.append((file, error))

    def check_status(self, errors_file_path: str) -> bool:
        if len(self.errors) > 0:
            with open(errors_file_path, mode='w+') as errors_file:
                for error in self.errors:
                    file, msg = error
                    errors_file.write(f'{msg}\n')
            return False

        return True


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Converts directory with GWF files into SCs files')
    parser.add_argument(
        '--input', action='store',
        help='Path to input gwf file'
    )
    parser.add_argument(
        '--errors_file', action='store',
        help='Path to log file that contains errors')
    args = parser.parse_args()

    converter = Gwf2SCs()
    converter.run(args)
    if converter.check_status(args.errors_file):
        sys.exit(0)
    else:
        sys.exit(1)
