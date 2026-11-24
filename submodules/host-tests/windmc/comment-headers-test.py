#!/usr/bin/env python3
# PROJECT:     ReliefWin host-native tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Compile real message headers and preserve their binary resources across comment fixes
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


SOURCES = (
    'sdk/include/reliefwin/mc/ntiologc.mc',
    'sdk/include/reliefwin/mc/neteventmsg.mc',
    'sdk/include/reliefwin/mc/netmsgmsg.mc',
    'sdk/include/reliefwin/mc/errcodes.mc',
    'dll/win32/mferror/mferror.mc',
    'dll/win32/msxml3/msxml.mc',
    'dll/win32/wininet/winineterror.mc',
    'dll/win32/kernelbase/wine/winerror.mc',
)


def generated(args, tool, directory, text, encoding):
    directory.mkdir()
    source = directory / 'messages.mc'
    if encoding == 'UTF16':
        source.write_bytes(text.replace('\n', '\r\n').encode('utf-16'))
        options = ['-u']
    else:
        source.write_bytes(text.replace('\n', '\r\n' if encoding == 'CRLF' else '\n').encode('utf-8'))
        options = ['-C', '65001']
    result = subprocess.run([str(tool), *options, '-h', str(directory), '-r', str(directory), str(source)],
                            capture_output=True, text=True, errors='replace')
    if result.returncode:
        raise RuntimeError(str(source) + ': ' + result.stderr)
    header = directory / 'messages.h'
    unit = directory / 'check.c'
    unit.write_text('#include <stdint.h>\ntypedef int32_t NTSTATUS;\n'
                    'typedef uint32_t DWORD;\ntypedef uint32_t ULONG;\n'
                    '#include "messages.h"\n'
                    '#ifdef FACILITY_DXGI\n'
                    '_Static_assert(FACILITY_NULL == 0, "FACILITY_NULL");\n'
                    '_Static_assert(FACILITY_DXGI == 2170, "FACILITY_DXGI");\n'
                    '_Static_assert((uint32_t)E_NOTIMPL == 0x80004001u, "E_NOTIMPL");\n'
                    '_Static_assert((uint32_t)E_FAIL == 0x80004005u, "E_FAIL");\n'
                    '_Static_assert((uint32_t)DXGI_ERROR_INVALID_CALL == 0x887a0001u, "DXGI_ERROR_INVALID_CALL");\n'
                    '_Static_assert((uint32_t)DXGI_ERROR_NOT_CURRENTLY_AVAILABLE == 0x887a0022u, "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE");\n'
                    '#endif\n')
    syntax = subprocess.run([args.cc, '-std=c11', '-fsyntax-only', str(unit)],
                            capture_output=True, text=True, errors='replace')
    definitions = re.findall(rb'^#define .+$', header.read_bytes(), re.MULTILINE)
    resources = {p.name: p.read_bytes() for p in directory.glob('*.bin')}
    if not resources:
        raise RuntimeError('No message-table resources generated for ' + str(source))
    return syntax, definitions, resources, header


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument('--cc', default='cc')
    parser.add_argument('--windmc', type=Path)
    parser.add_argument('--verify-regression', action='store_true')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory() as temporary:
        directory = Path(temporary)
        tool = args.windmc or args.root / 'output-Clang-amd64-debug/host-tools/bin/windmc'
        if not tool.is_file():
            tool = directory / 'windmc'
            subprocess.run([args.cc, '-std=c11', str(args.root / 'sdk/tools/windmc/windmc.c'),
                            '-o', str(tool)], check=True)
        passed = 0
        for index, relative in enumerate(SOURCES):
            text = (args.root / relative).read_text()
            old = re.sub(r'^(\s*;)(//)(?=\s)', r'\1', text, flags=re.MULTILINE)
            if old == text:
                raise RuntimeError('No corrected prose comments in ' + relative)
            prose = re.search(r'^\s*;//\s+([a-zA-Z_]+)', text, flags=re.MULTILINE).group(1)
            for encoding in ('LF', 'CRLF', 'UTF16'):
                case = directory / (str(index) + encoding)
                case.mkdir()
                fixed = generated(args, tool, case / 'fixed', text, encoding)
                if fixed[0].returncode:
                    raise RuntimeError(relative + ' ' + encoding + ': ' + fixed[0].stderr[:1500])
                if args.verify_regression:
                    baseline = generated(args, tool, case / 'old', old, encoding)
                    if (baseline[0].returncode == 0 or str(baseline[3]) not in baseline[0].stderr or
                            prose not in baseline[0].stderr):
                        raise RuntimeError('The original bare-comment header did not fail for ' + relative + ' ' + encoding)
                    if fixed[1:3] != baseline[1:3]:
                        raise RuntimeError('Message IDs or binary resources changed for ' + relative + ' ' + encoding)
                    if relative.endswith('/wine/winerror.mc'):
                        for facility, value in (('Null', '0x0'), ('Dxgi', '0x87a')):
                            missing = text.replace(f'{facility}={value}:FACILITY_{facility.upper()}', '')
                            try:
                                generated(args, tool, case / facility, missing, encoding)
                            except RuntimeError as error:
                                if 'Unknown facility: ' + facility not in str(error):
                                    raise
                            else:
                                raise RuntimeError('Missing facility did not fail: ' + facility)
                passed += 1
            print(relative + ': LF, CRLF, UTF16 headers passed')
        print(str(passed) + ' real-source header syntax checks passed')
        if args.verify_regression:
            print(str(passed) + ' original-comment failures confirmed; message IDs and binary resources unchanged')
            print('Missing Null/Dxgi declarations: 6 expected compiler failures confirmed')


if __name__ == '__main__':
    main()
