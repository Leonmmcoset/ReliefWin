#!/usr/bin/env python3
# PROJECT:     ReliefWin branding audit tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise source-literal and existing-PE audit rejection paths
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from contextlib import redirect_stdout
from copy import deepcopy
import importlib.util
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[3]


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source-only', action='store_true', help='run source/default audit guards without a built PE fixture')
    parser.add_argument('--icu-module', type=Path, default=ROOT / 'output-Clang-amd64-debug/submodules/icu-payload/72.1-1/system32/icu.dll',
                        help='existing original ICU catalog module; read only, never copied or modified')
    args = parser.parse_args()
    if not args.source_only and not args.icu_module.is_file():
        parser.error('the existing ICU module is required for the actual PE audit regression')
    source = load('source_branding_audit', ROOT / 'sdk/tools/check_branding.py')
    manifest = 'media/doc/UPSTREAM_IDENTIFIERS.json'
    empty = json.dumps({'literals': []})
    cases = {
        'title.rc': 'CAPTION "rEaCtOs"',
        'title.rc2': 'CAPTION "REACTOS"',
        'title.cxx': 'const char* title = "reactos";',
        'title.hpp': 'const char* title = "rEaCtOs";',
        'title.inl': 'const char* title = "REACTOS";',
        'title.cpp': 'const char* title = R"tag("rEaCtOs" with internal quotes)tag";',
    }
    for path, text in cases.items():
        errors = []
        source.check_upstream_identifiers({manifest: empty, path: text}, errors)
        assert len(errors) == 1 and errors[0]['path'] == path, (path, errors)
    errors = []
    source.check_upstream_identifiers({manifest: empty, 'history.rc': '/* ReactOS attribution */\n// REACTOS history'}, errors)
    assert not errors, errors
    retained = {'path': 'class.rc', 'literal': '"REACTOS_CLASS"', 'category': 'compatibility interface',
                'reason': 'Exact test window class used to verify occurrence-scoped approval.'}
    errors = []
    source.check_upstream_identifiers({manifest: json.dumps({'literals': [retained]}),
                                      'class.rc': 'CLASS "REACTOS_CLASS"\nCAPTION "ReactOS"'}, errors)
    assert len(errors) == 1 and '"ReactOS"' in errors[0]['check'], errors
    checked = len(cases) + 2
    for alias in ('LBRNT', 'lbrnt', 'LbRnT', 'LBRN', 'lbrn'):
        line = 'const char *title = "' + alias + '";'
        assert source.OLD.search(line) and source.classify('product.c', line) is None, alias
        checked += 1
    for path in ('CONTRIBUTING.md', 'README.md'):
        line = 'LiberNT has no copyright assignment and no contributor licence agreement.'
        assert source.OLD.search(line) and source.classify(path, line) is None, path
        checked += 1
    defaults_paths = (
        'sdk/include/reliefwin/version.rc', 'image/data/hivesft.inf', 'image/bcd/bcd.inf.in',
        'image/data/bootcd/unattend.inf', 'image/data/livecd/unattend.inf', 'image/data/preinstall/unattend.inf',
        'image/data/txtsetup.sif', 'image/data/hivesys.inf', 'image/data/preinstall.inf',
        'base/setup/reliefwin/lntupdate.inf', 'base/applications/systemsettings/page_misc.cpp',
        'sdk/include/psdk/ntverp.h', 'base/setup/reliefwin/updagent.c', 'base/shell/explorer/explorer.rc',
        'image/data/CMakeLists.txt', 'image/data/livecd.inf', 'base/applications/cmdutils/lntget/lntget.rc',
        'base/applications/cmdutils/lntget/lntget.exe.manifest', 'base/applications/cmdutils/lntget/gui.cpp',
        'media/inf/nettcpip.inf', 'bootloader/uefi/fdtacpi.c', 'hal/arch/common/acpi/tables.c',
        'base/shell/explorer/lang/en-US.rc', 'base/shell/explorer/lang/zh-CN.rc', 'media/inf/shortcuts.inf',
        'sdk/cmake/CMakeMacros.cmake',
    )
    defaults = {path: source.decode((ROOT / path).read_bytes()) for path in defaults_paths}
    errors = []
    source.check_defaults(defaults, errors)
    assert not errors, errors
    checked += 1
    for path in ('image/data/CMakeLists.txt', 'base/applications/cmdutils/lntget/gui.cpp'):
        changed = dict(defaults)
        changed[path] = changed[path].replace('ReliefWin Package Manager', 'LntGet')
        errors = []
        source.check_defaults(changed, errors)
        assert errors and all(error['path'] == path for error in errors), (path, errors)
        checked += 1
    for title in ('安装 ReliefWin', '使用说明'):
        changed = dict(defaults)
        changed['image/data/CMakeLists.txt'] = changed['image/data/CMakeLists.txt'].replace(title, 'English shortcut')
        errors = []
        source.check_defaults(changed, errors)
        assert len(errors) == 1 and errors[0]['path'] == 'image/data/CMakeLists.txt', errors
        checked += 1
    for path, original, replacement in (
        ('image/data/hivesys.inf', '"ComputerName",0x00000002,"RELIEFWIN"', '"ComputerName",0x00000002,"OldHost"'),
        ('media/inf/nettcpip.inf', '"Hostname",0x00000000,"RELIEFWIN"', '"Hostname",0x00000000,"OldHost"'),
        ('media/inf/nettcpip.inf', '"NV Hostname",0x00000000,"RELIEFWIN"', '"NV Hostname",0x00000000,"OldHost"'),
        ('bootloader/uefi/fdtacpi.c', 'Header->OEMID, "RWIN  "', 'Header->OEMID, "RWIN "'),
        ('bootloader/uefi/fdtacpi.c', 'Rsdp->OEMID, "RWIN  "', 'Rsdp->OEMID, "RWIN "'),
        ('bootloader/uefi/fdtacpi.c', 'Header->CreatorID, "RWIN"', 'Header->CreatorID, "RWI"'),
        ('hal/arch/common/acpi/tables.c', 'FADT_SIGNATURE, "RWIN", "DTPLATFM"', 'FADT_SIGNATURE, "Other", "DTPLATFM"'),
    ):
        changed = dict(defaults)
        assert original in changed[path], (path, original)
        changed[path] = changed[path].replace(original, replacement)
        errors = []
        source.check_defaults(changed, errors)
        assert len(errors) == 1 and errors[0]['path'] == path, (path, errors)
        checked += 1
    if args.source_only:
        print(f'PASS: {checked} source/default branding audit checks; PE checks not requested')
        return 0
    with tempfile.TemporaryDirectory() as directory:
        fixture = Path(directory)
        empty_root = fixture / 'empty'
        empty_root.mkdir()
        package_root = fixture / 'package'
        (package_root / 'submodules').mkdir(parents=True)
        (package_root / 'submodules/unknown.dll').symlink_to(args.icu_module.resolve())

        def artifact(root, settle=0):
            report = fixture / 'report.json'
            run = subprocess.run([sys.executable, str(ROOT / 'sdk/tools/check_built_branding.py'), str(root),
                                  '--settle-seconds', str(settle), '--report', str(report)],
                                 text=True, capture_output=True)
            return run.returncode, json.loads(report.read_text()), run.stdout + run.stderr

        code, report, output = artifact(fixture / 'missing')
        assert code and report['invalid_roots'] and report['status'] != 'pass', output
        checked += 1
        code, report, output = artifact(empty_root)
        assert code and report['parsed_pe_files'] == 0 and report['status'] != 'pass', output
        checked += 1
        code, report, output = artifact(package_root)
        assert code and report['parsed_pe_files'] == 1 and report['findings'] and not report['upstream_package_metadata'], output
        checked += 1
        code, report, output = artifact(args.icu_module.resolve().parent)
        assert not code and report['status'] == 'pass' and report['upstream_package_metadata'] and not report['findings'], output
        checked += 1
        code, report, output = artifact(args.icu_module.resolve().parent, 1e12)
        assert code and report['status'] == 'partial' and report['deferred_files'], output
        checked += 1
        built = load('built_branding_audit', ROOT / 'sdk/tools/check_built_branding.py')
        actual = built.read_module(args.icu_module.resolve())
        for alias in ('LBRNT', 'LBRN'):
            metadata = deepcopy(actual)
            metadata['version_tables'][0]['fields']['ProductName'] = alias
            report_path = fixture / 'alias-report.json'
            arguments = ['check_built_branding.py', str(args.icu_module.resolve().parent),
                         '--settle-seconds', '0', '--report', str(report_path)]
            with patch.object(sys, 'argv', arguments), patch.object(built, 'read_module', return_value=metadata), redirect_stdout(io.StringIO()):
                code = built.main()
            report = json.loads(report_path.read_text())
            assert code and report['status'] == 'findings' and any(item['value'] == alias for item in report['findings']), report
            checked += 1
    print(f'PASS: {checked} source-literal and actual PE audit checks')
    return 0


if __name__ == '__main__':
    sys.exit(main())
