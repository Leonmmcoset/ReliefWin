#!/usr/bin/env python3
# PROJECT:     ReliefWin setup tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify real hive Unicode strings and generated Start shortcut destinations
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
import csv
from pathlib import Path
import subprocess
import tempfile

from Registry import Registry


ROOT = Path(__file__).resolve().parents[1]


def expected_links(mode):
    source = ROOT / 'image/data' / (mode + '.inf')
    row = next(line for line in source.read_text(encoding='utf-8-sig').splitlines() if ',"RecentlyAdded",' in line)
    return source, next(csv.reader([row]))[4:]


def read_links(path):
    value = Registry.Registry(str(path)).open('Software\\ReactOS\\StartMenu2').value('RecentlyAdded')
    if value.value_type() != 7:
        raise AssertionError('RecentlyAdded must be REG_MULTI_SZ')
    return [item for item in value.value() if item], value.raw_data()


def run_tools(build, regression):
    tools = build / 'host-tools/bin'
    total = corrupt = 0
    with tempfile.TemporaryDirectory() as directory:
        work = Path(directory)
        for mode in ('preinstall', 'livecd'):
            source, expected = expected_links(mode)
            converted = work / 'overlay.inf'
            subprocess.run([str(tools / 'utf16le'), str(source), str(converted)], check=True, capture_output=True)
            assert converted.read_bytes().startswith(b'\xff\xfe'), mode
            subprocess.run([str(tools / 'mkhive'), '-h:DEFAULT', '-d:' + str(work), str(converted)], check=True, capture_output=True)
            actual, raw = read_links(work / 'default')
            assert actual == expected, (mode, actual, expected)
            assert raw == ('\0'.join(expected) + '\0\0').encode('utf-16le'), mode
            total += len(expected)
            if regression:
                subprocess.run([str(tools / 'mkhive'), '-h:DEFAULT', '-d:' + str(work), str(source)], check=True, capture_output=True)
                actual, _ = read_links(work / 'default')
                assert actual != expected, mode
                assert actual == [item.encode('utf-8').decode('latin1') for item in expected], (mode, actual)
                corrupt += sum(left != right for left, right in zip(actual, expected))
    print(f'Actual UTF16 converter/mkhive readback: {total} Recent strings preserved')
    if regression:
        print(f'Original direct UTF8 input: {corrupt} Chinese Recent strings corrupted as expected')


def check_artifacts(build):
    total = 0
    for mode in ('preinstall', 'livecd'):
        _, expected = expected_links(mode)
        hive = build / 'boot/bootdata' / ('preinstall/default' if mode == 'preinstall' else 'default')
        actual, _ = read_links(hive)
        assert actual == expected, (mode + ' actual hive', actual, expected)
        manifest = build / 'boot' / (mode + '.Debug.lst')
        entries = dict(line.split('=', 1) for line in manifest.read_text().splitlines() if '=' in line)
        if mode == 'preinstall':
            effective = dict(line.split('=', 1) for line in (build / 'boot/preinstall.effective.lst').read_text().splitlines() if '=' in line)
        else:
            effective = entries
        for suffix in actual:
            destination = 'ProgramData/Microsoft/Windows/Start Menu/Programs/' + suffix.replace('\\', '/')
            assert destination in entries, (mode, 'missing manifest target', destination)
            assert destination in effective, (mode, 'missing effective manifest target', destination)
            source = Path(entries[destination])
            assert source.is_file(), (mode, 'missing generated file', str(source))
            assert source.read_bytes()[:4] == b'L\0\0\0', (mode, 'invalid Shell Link header', str(source))
            total += 1
        print(f'{mode}: {len(actual)} actual hive suffixes match manifest and generated Shell Links')
    print(f'Generated Start shortcut destinations: {total} verified')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-root', type=Path, default=ROOT / 'output-Clang-amd64-debug')
    parser.add_argument('--verify-regression', action='store_true')
    parser.add_argument('--artifacts', action='store_true', help='also verify existing hives, manifests and generated links; invokes no build')
    args = parser.parse_args()
    run_tools(args.build_root.resolve(), args.verify_regression)
    if args.artifacts:
        check_artifacts(args.build_root.resolve())
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
