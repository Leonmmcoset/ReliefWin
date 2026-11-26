#!/usr/bin/env python3
# PROJECT:     ReliefWin build tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify nested WoW64 CMake receives the selected host tool directory
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from pathlib import Path
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = 'sdk/cmake/wow64.cmake'


def argument_generation(text):
    start = text.index('get_filename_component(_wow64_toolchain ')
    end = text.index('\nif(MSVC)', start)
    return text[start:end]


def quoted(value):
    return '[==[' + str(value) + ']==]'


RECEIVER = '''
cmake_minimum_required(VERSION 3.20)
if(NOT HOST_TOOLS_DIR STREQUAL EXPECT_HOST_TOOLS_DIR)
    message(FATAL_ERROR "Host tools path changed: '${HOST_TOOLS_DIR}', expected '${EXPECT_HOST_TOOLS_DIR}'")
endif()
if(NOT CMAKE_TOOLCHAIN_FILE STREQUAL EXPECT_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Toolchain path changed: '${CMAKE_TOOLCHAIN_FILE}'")
endif()
if(NOT ARCH STREQUAL "i386" OR ENABLE_WOW64 OR NOT WOW64_I386_RUNTIME)
    message(FATAL_ERROR "Nested target arguments changed")
endif()
'''


def exercise(cmake, source, directory, case, configured, expected):
    receiver = directory / 'receive arguments.cmake'
    receiver.write_text(RECEIVER)
    script = directory / 'test.cmake'
    binary = directory / 'build directory with spaces'
    setup = ['cmake_minimum_required(VERSION 3.20)',
             'set(REACTOS_SOURCE_DIR ' + quoted(ROOT) + ')',
             'set(REACTOS_BINARY_DIR ' + quoted(binary) + ')',
             'set(CMAKE_TOOLCHAIN_FILE "sdk/../toolchain-clang.cmake")']
    setup.append('unset(HOST_TOOLS_DIR)' if configured is None else
                 'set(HOST_TOOLS_DIR ' + quoted(configured) + ')')
    setup += [source,
              'set(_expected_host_tools_dir ' + quoted(expected) + ')',
              'set(_expected_toolchain ' + quoted(ROOT / 'toolchain-clang.cmake') + ')',
              'execute_process(COMMAND "${CMAKE_COMMAND}" ${_wow64_i386_cmake_args}',
              '    "-DEXPECT_HOST_TOOLS_DIR:STRING=${_expected_host_tools_dir}"',
              '    "-DEXPECT_TOOLCHAIN_FILE:FILEPATH=${_expected_toolchain}"',
              '    -P ' + quoted(receiver),
              '    RESULT_VARIABLE _status OUTPUT_VARIABLE _output ERROR_VARIABLE _error)',
              'if(NOT _status EQUAL 0)',
              '    message(FATAL_ERROR "Nested CMake command failed: ${_output}${_error}")',
              'endif()']
    script.write_text('\n'.join(setup) + '\n')
    result = subprocess.run([cmake, '-P', str(script)], text=True, capture_output=True)
    if result.returncode and "Host tools path changed:" not in result.stderr:
        raise RuntimeError(f'{case}: Unexpected CMake failure: {result.stdout}{result.stderr}')
    return result.returncode == 0


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cmake', default='cmake')
    parser.add_argument('--verify-regression', nargs='?', const='HEAD')
    args = parser.parse_args()
    variants = [('current', (ROOT / SOURCE).read_text())]
    if args.verify_regression:
        original = subprocess.check_output(['git', 'show', f'{args.verify_regression}:{SOURCE}'], cwd=ROOT, text=True)
        variants.append(('original host tools path', original))
    with tempfile.TemporaryDirectory() as temporary:
        directory = Path(temporary)
        default = str(directory / 'build directory with spaces/host-tools/bin')
        custom = str(directory / 'shared-host-tools/bin')
        spaced = str(directory / 'shared host tools/bin directory')
        cases = [('unset', None, default), ('empty', '', default),
                 ('custom', custom, custom), ('custom with spaces', spaced, spaced)]
        for name, text in variants:
            source = argument_generation(text)
            failed = [case for case, configured, expected in cases
                      if not exercise(args.cmake, source, directory, case, configured, expected)]
            print(f'{name}: {len(cases)} nested CMake command cases, {len(failed)} failures')
            if name == 'current' and failed:
                raise RuntimeError('Nested CMake host tool paths failed: ' + ', '.join(failed))
            if name != 'current':
                if failed != ['custom', 'custom with spaces']:
                    raise RuntimeError('Original host tools path did not fail the intended custom directory cases')
                print('Original custom host tools path failed as expected')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
