#!/usr/bin/env python3
# PROJECT:     ReliefWin artifact branding checks
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Audit version identities and resource languages in existing PE build artifacts
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

"""Read existing PE files with the third-party pefile package (https://pypi.org/project/pefile/).

Install the optional audit dependency with: python3 -m pip install pefile
Rerun this script to include newly built modules. It does not invoke a build or edit artifacts.
"""

import argparse
from collections import Counter
import json
from pathlib import Path
import re
import sys
import time

try:
    import pefile
except ImportError:
    raise SystemExit('This artifact audit requires pefile: python3 -m pip install pefile')


ROOT = Path(__file__).resolve().parents[2]
EXTENSIONS = {'.exe', '.dll', '.sys', '.drv', '.ocx', '.cpl', '.scr', '.efi', '.acm', '.ax', '.tsp', '.ime', '.mui'}
EXCLUDED = {'symbols', 'host-tools', 'CMakeFiles', 'sdk'}
FIELDS = {'ProductName', 'FileDescription', 'CompanyName'}
OBSOLETE = re.compile(r'libernt|liberos|liber[ -](?:nt|os)|lbrn(?:t)?|reactos|реактос', re.IGNORECASE)
UPSTREAM_CATALOG_IDENTITIES = {
    (f'output-Clang-{architecture}-debug/{prefix}submodules/icu-payload/72.1-1/system32/icu.dll',
     '040904B0', 'FileDescription', 'ICU for LiberNT')
    for architecture, prefix in (('amd64', ''), ('amd64', '_wow64_i386/'), ('i386', ''), ('arm64', ''))
}


def display(path):
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


def text(value):
    if isinstance(value, bytes):
        return value.decode('utf-8', errors='replace').rstrip('\0')
    return str(value).rstrip('\0')


def snapshot(path):
    stat = path.stat()
    return stat.st_ino, stat.st_size, stat.st_mtime_ns


def candidates(root, errors):
    pending = [root]
    while pending:
        directory = pending.pop()
        try:
            children = sorted(directory.iterdir())
        except OSError as error:
            errors.append({'path': display(directory), 'reason': str(error)})
            continue
        for path in children:
            if path.is_dir():
                if path.name not in EXCLUDED and not path.is_symlink():
                    pending.append(path)
            elif path.suffix.lower() in EXTENSIONS:
                yield path


def read_module(path):
    image = pefile.PE(str(path), fast_load=True)
    try:
        image.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_RESOURCE']])
        languages = set()
        for resource_type in getattr(getattr(image, 'DIRECTORY_ENTRY_RESOURCE', None), 'entries', []):
            for resource_name in getattr(getattr(resource_type, 'directory', None), 'entries', []):
                for language in getattr(getattr(resource_name, 'directory', None), 'entries', []):
                    if isinstance(language.id, int):
                        languages.add(language.id)
        tables = []
        for group in getattr(image, 'FileInfo', []):
            for info in group:
                for table in getattr(info, 'StringTable', []):
                    fields = {text(key): text(value) for key, value in table.entries.items() if text(key) in FIELDS}
                    tables.append({'translation': text(table.LangID), 'fields': fields})
        return {'machine': f'0x{image.FILE_HEADER.Machine:04x}',
                'resource_languages': [f'0x{language:04x}' for language in sorted(languages)],
                'version_tables': tables}
    finally:
        image.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('build_roots', nargs='*', type=Path,
                        help='existing build directories; default: all output-Clang-*-debug directories')
    parser.add_argument('--report', type=Path, default=Path('/tmp/reliefwin-pe-brand-audit.json'))
    parser.add_argument('--settle-seconds', type=float, default=2,
                        help='defer files written more recently than this interval')
    args = parser.parse_args()
    requested_roots = args.build_roots or sorted(ROOT.glob('output-Clang-*-debug'))
    invalid_roots = [{'path': str(path), 'reason': 'not an existing directory'}
                     for path in requested_roots if not path.is_dir()]
    roots = [path.resolve() for path in requested_roots if path.is_dir()]
    modules, findings, retained, upstream, deferred, unreadable = [], [], [], [], [], []
    traversal_errors = []
    seen = set()
    scanned = 0
    for root in roots:
        for path in candidates(root, traversal_errors):
            path = path.absolute()
            if path in seen:
                continue
            seen.add(path)
            scanned += 1
            before = None
            try:
                before = snapshot(path)
                if time.time() - before[2] / 1e9 < args.settle_seconds:
                    deferred.append({'path': display(path), 'reason': 'recent write'})
                    continue
                with path.open('rb') as stream:
                    if stream.read(2) != b'MZ':
                        continue
                module = read_module(path)
                if before != snapshot(path):
                    deferred.append({'path': display(path), 'reason': 'changed during read'})
                    continue
            except (OSError, pefile.PEFormatError, ValueError, TypeError) as error:
                try:
                    after = snapshot(path)
                except OSError:
                    after = None
                if after is None or (before is not None and before != after):
                    deferred.append({'path': display(path), 'reason': 'changed or disappeared during failed read'})
                else:
                    unreadable.append({'path': display(path), 'reason': str(error)})
                continue
            module.update({'path': display(path), 'build_root': display(root)})
            modules.append(module)
            for table in module['version_tables']:
                for field, value in table['fields'].items():
                    if not OBSOLETE.search(value):
                        continue
                    issue = {'path': display(path), 'translation': table['translation'], 'field': field, 'value': value}
                    if field == 'CompanyName' and value == 'ReliefWin, Legacy Wine/ReactOS':
                        issue['reason'] = 'explicit original Wine/ReactOS author credit alongside the current ReliefWin identity'
                        retained.append(issue)
                    elif (display(path), table['translation'], field, value) in UPSTREAM_CATALOG_IDENTITIES:
                        issue['reason'] = 'verified original ICU 72.1-1 package used only as catalog input; the image installs the locally built ICU stub'
                        upstream.append(issue)
                    else:
                        findings.append(issue)
    fields = Counter(field for module in modules for table in module['version_tables'] for field in table['fields'])
    report = {'dependency': {'name': 'pefile', 'version': pefile.__version__, 'url': 'https://pypi.org/project/pefile/'},
              'scope': 'existing build artifacts; presence in a build directory does not establish that a module is installed in an image',
              'status': 'findings' if findings or unreadable or invalid_roots or traversal_errors else 'partial' if deferred or not modules else 'pass',
              'build_roots': [display(root) for root in roots], 'candidate_files': scanned,
              'parsed_pe_files': len(modules), 'checked_fields': dict(sorted(fields.items())),
              'modules_with_zh_cn_resources': sum('0x0804' in module['resource_languages'] for module in modules),
              'findings': findings, 'upstream_package_metadata': upstream, 'retained_author_credits': retained,
              'deferred_files': deferred, 'unreadable_files': unreadable,
              'invalid_roots': invalid_roots, 'traversal_errors': traversal_errors,
              'modules': sorted(modules, key=lambda module: module['path'])}
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2, ensure_ascii=False) + '\n')
    print(f'Parsed {len(modules)} existing PE files; checked version fields: {dict(fields)}')
    print(f'{report["modules_with_zh_cn_resources"]} modules contain Simplified Chinese (0x0804) resources')
    print(f'{len(findings)} OS identity findings; {len(upstream)} upstream package metadata observations; '
          f'{len(retained)} retained author credits; {len(deferred)} deferred; {len(unreadable)} unreadable')
    print(f'Status: {report["status"]}; {len(invalid_roots)} invalid roots; {len(traversal_errors)} traversal errors')
    print('Report: ' + str(args.report))
    return report['status'] != 'pass'


if __name__ == '__main__':
    sys.exit(main())
