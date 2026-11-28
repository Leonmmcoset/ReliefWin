#!/usr/bin/env python3
"""
PROJECT:     ReliefWin build tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise fresh-host dependencies and source-feed selection
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def functions(source):
    return '\n'.join(re.findall(r'^\w+\(\) \{\n.*?^\}', source, re.M | re.S))


def feeds(source, directory):
    (directory / 'scripts').mkdir(exist_ok=True)
    runner = directory / 'scripts/feeds'
    runner.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> "$FEED_LOG"\n')
    runner.chmod(0o755)
    script = functions(source) + '''
REACTOS_SOURCE_DIR=$1
ROSCONFIG_CACHE=$1/missing-cache
SKIP_FEEDS_UPDATE=0
KDB_ZYDIS_FEED_PATHS=External/zydis,External/zydis/dependencies/zycore
USE_CLANG=$2
ARCH=$3
ROS_CMAKEOPTS=$4
sync_feeds
'''
    cases = [
        ('default', '1', 'amd64', '', {'mesa', 'dxc', 'fex-arm64ec'}),
        ('i386', '1', 'i386', '', {'mesa', 'dxc', 'fex-arm64ec'}),
        ('arm64', '1', 'arm64', '', {'mesa', 'dxc', 'fex-arm64ec'}),
        ('source-ffmpeg', '1', 'amd64', '-DFFMPEG_FROM_SOURCE=ON', {'mesa', 'dxc', 'fex-arm64ec', 'ffmpeg'}),
        ('disabled', '1', 'amd64', '-DMESA_GALLIUM_FROM_SOURCE=OFF -DENABLE_DXILCONV=OFF -DKD_DEBUGGER=NONE', set()),
        ('last-value', '1', 'amd64', '-DENABLE_DXILCONV=OFF -DENABLE_DXILCONV:BOOL=ON', {'mesa', 'dxc', 'fex-arm64ec'}),
        ('gcc', '0', 'i386', '', {'fex-arm64ec'}),
        ('arm', '1', 'arm', '', set()),
        ('payload', '1', 'amd64', '-DENABLE_FFMPEG=ON', {'mesa', 'dxc', 'fex-arm64ec'}),
    ]
    failed = []
    log = directory / 'feeds.log'
    for name, compiler, arch, options, expected in cases:
        log.unlink(missing_ok=True)
        result = subprocess.run(['sh', '-c', script, 'probe', str(directory), compiler, arch, options],
                                env={**os.environ, 'FEED_LOG': str(log)}, text=True, capture_output=True)
        actual = set()
        if log.exists():
            for line in log.read_text().splitlines():
                args = line.split()[1:]
                if args[:1] == ['--paths']:
                    args = args[2:]
                if not args:
                    raise AssertionError('An empty selection updates every feed')
                actual.update(args)
        if result.returncode or actual != expected:
            failed.append(name)
    return failed


def packages(directory, manager, install_failure=False, still_missing=False, missing_compilers=False):
    tools = directory / manager
    tools.mkdir(exist_ok=True)
    log = tools / 'packages.log'
    marker = tools / 'installed'
    log.unlink(missing_ok=True)
    marker.unlink(missing_ok=True)
    (tools / 'flex').unlink(missing_ok=True)
    for command in ('dirname', 'id', 'git', 'cmake', 'ninja', 'make', 'bison', 'm4',
                    'ccache', 'curl', 'wget', 'xz', 'tar', 'patch', 'pkg-config', 'cc', 'c++',
                    'glslangValidator', 'msidump', 'msiextract', 'success'):
        (tools / command).unlink(missing_ok=True)
        (tools / command).write_text('#!/bin/sh\nexit 0\n')
        (tools / command).chmod(0o755)
    (tools / 'dirname').write_text('#!/bin/sh\nexec /usr/bin/dirname "$@"\n')
    (tools / 'id').write_text('#!/bin/sh\necho 0\n')
    (tools / 'python3').write_text('#!/bin/sh\n[ -f "$PACKAGE_MARKER" ] && [ "$PACKAGES_STILL_MISSING" = 0 ]\n')
    (tools / 'python3').chmod(0o755)
    if missing_compilers:
        (tools / 'cc').unlink()
        (tools / 'c++').unlink()
    executable = tools / manager
    executable.write_text('''#!/bin/sh
printf '%s\\n' "$*" >> "$PACKAGE_LOG"
[ "$PACKAGE_FAILURE" = 0 ] || exit 1
case "$*" in
  *install*|*-S*)
    /usr/bin/touch "$PACKAGE_MARKER"
    for tool in flex cc c++; do
      /bin/ln -sf "$PACKAGE_TOOLS/success" "$PACKAGE_TOOLS/$tool"
    done
    ;;
esac
''')
    executable.chmod(0o755)
    environment = {**os.environ, 'PATH': str(tools), 'ROSBE_DOCKER_ACTIVE': '0',
                   'PACKAGE_LOG': str(log), 'PACKAGE_MARKER': str(marker),
                   'PACKAGE_TOOLS': str(tools),
                   'PACKAGE_FAILURE': str(int(install_failure)),
                   'PACKAGES_STILL_MISSING': str(int(still_missing))}
    result = subprocess.run(['/bin/sh', str(ROOT / 'scripts/build-deps.sh')],
                            env=environment, text=True, capture_output=True)
    expected_failure = install_failure or still_missing
    assert bool(result.returncode) == expected_failure, result.stdout + result.stderr
    assert all(not item.is_symlink() or item.resolve().is_relative_to(tools) for item in tools.iterdir())
    calls = log.read_text().splitlines()
    if not install_failure:
        expected = ['flex', 'python-mako', 'python-yaml', 'python-packaging'] if manager == 'pacman' else [
            'flex', 'python3-mako', 'python3-yaml', 'python3-packaging']
        if missing_compilers:
            expected.insert(1, 'base-devel' if manager == 'pacman' else 'build-essential')
        arguments = calls[-1].split()[3:] if manager == 'pacman' else calls[-1].split()[2:]
        assert arguments == expected, calls
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--verify-regression', nargs='?', const='HEAD')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory() as temporary:
        directory = Path(temporary)
        source = (ROOT / 'configure.sh').read_text()
        failed = feeds(source, directory)
        assert not failed, failed
        if args.verify_regression:
            original = subprocess.check_output(['git', 'show', f'{args.verify_regression}:configure.sh'], cwd=ROOT, text=True)
            baseline = feeds(original, directory)
            assert {'default', 'source-ffmpeg', 'disabled', 'gcc', 'arm', 'payload'} <= set(baseline), baseline
            print('Original feed selection reproduces the missing DXC and source FFmpeg defects')
        packages(directory, 'pacman')
        packages(directory, 'apt-get')
        packages(directory, 'pacman', missing_compilers=True)
        packages(directory, 'apt-get', missing_compilers=True)
        packages(directory, 'pacman', install_failure=True)
        packages(directory, 'apt-get', still_missing=True)
        python = directory / 'python-version'
        python.write_text('#!/bin/sh\necho 3.8.0\nexit 1\n')
        python.chmod(0o755)
        old_python = subprocess.run(['cmake', f'-DRELIEFWIN_PYTHON_EXECUTABLE={python}',
                                     '-P', str(ROOT / 'sdk/cmake/host-dependencies.cmake')],
                                    text=True, capture_output=True)
        assert old_python.returncode and 'Python 3.9 or newer' in old_python.stderr
    print('Feed selection, Arch/Debian package names and dependency failure handling OK')


if __name__ == '__main__':
    main()
