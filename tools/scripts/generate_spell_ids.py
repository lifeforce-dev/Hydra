import generate_cpp_util as cpp_util

import logging
import os
import re
import sys
import string

HEADER_FILE_NAME = 'SpellIdEnums.h'

GENERATED_DIR_NAME = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                  '../../common/generated')
LOG_FILE_PATH = os.path.join(GENERATED_DIR_NAME, 'log.txt')
SPELL_ID_ENUMS_FILE_PATH = os.path.join(GENERATED_DIR_NAME, 'SpellIdEnums.h')

# This list will eventually come from parsing a database.
SPELL_IDS = ['NONE']


def enumerate_spell_ids_as_enum():
    enum_values_string = ''
    for idx, val in enumerate(SPELL_IDS):
        enum_values_string += cpp_util.get_indentation_spaces(1)
        enum_values_string += val
        if idx == 0:
            enum_values_string += ' = 0'
        enum_values_string += ',\n'

    return enum_values_string


def get_spell_id_enum_string():
    return (cpp_util.get_enum_signature('uint32_t', 'SpellIds') +
            cpp_util.get_new_line() +
            cpp_util.get_scope_open() +
            cpp_util.get_new_line() +
            enumerate_spell_ids_as_enum() +
            cpp_util.get_adt_scope_close())


def build_code_string():
    return (cpp_util.get_file_info_comment(HEADER_FILE_NAME) +
            cpp_util.get_include_guard() +
            cpp_util.get_generated_file_warning() +
            cpp_util.get_include_system_string('cstdint') +
            cpp_util.get_new_line() +
            cpp_util.get_namespace_opener('Common') +
            get_spell_id_enum_string() +
            cpp_util.get_new_line() +
            cpp_util.get_namespace_closer('Common'))

    return code_string


def write_spell_id_enums_header(code_string):
    try:
        with open(SPELL_ID_ENUMS_FILE_PATH,
                  'w+', encoding='utf-8') as spell_id_header:
            spell_id_header.write(code_string)
    except OSError as e:
        logging.error('Failed to create html dump. ' +
                      ' error=' + e.strerror +
                      ' file_name=' + path)


def clear_file(file_name):
    if os.path.isfile(file_name):
        file = open(file_name, 'w')
        file.close()


def clear_files(files):
    for file in files:
        clear_file(file)


def initialize_dirs():
    os.makedirs(GENERATED_DIR_NAME, exist_ok=True)
    os.makedirs(LOG_FILE_PATH, exist_ok=True)
    os.makedirs(SPELL_ID_ENUMS_FILE_PATH, exist_ok=True)


def main():
    logging.basicConfig(filename=LOG_FILE_PATH, level=logging.INFO)
    files_to_clear = [SPELL_ID_ENUMS_FILE_PATH, LOG_FILE_PATH]
    clear_files(files_to_clear)

    code_string = build_code_string()
    write_spell_id_enums_header(code_string)


if __name__ == '__main__':
    main()
