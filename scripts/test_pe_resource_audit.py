#!/usr/bin/env python3
# PROJECT:     ReliefWin resource tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Pin PE resource-directory and embedded version-language audit failures
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
import hashlib
import inspect
from pathlib import Path
import struct

import test_ui_languages as audit


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--binary', type=Path, default=ROOT / 'output-Clang-amd64-debug/base/applications/notepad/notepad.exe')
    parser.add_argument('--verify-regression', action='store_true')
    args = parser.parse_args()
    data = args.binary.read_bytes()
    checks = 0

    def expect(condition, message):
        nonlocal checks
        checks += 1
        if not condition:
            raise AssertionError(message)

    entries = audit.pe_resource_entries(audit.BinaryData(data))
    expect(bool(entries), 'Actual test binary must contain resources')
    expect(audit.pe_resources(audit.BinaryData(data)) == [item for item, payload in entries],
           'Resource tuple API changed')
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    optional = pe + 24
    magic = struct.unpack_from('<H', data, optional)[0]
    directory = optional + (96 if magic == 0x10b else 112) + 16
    original_rva, original_size = struct.unpack_from('<II', data, directory)
    expect(bool(original_rva and original_size), 'Actual binary has no resource directory')
    invalid = []
    for field in (0, 4):
        changed = bytearray(data)
        struct.pack_into('<I', changed, directory + field, 0)
        invalid.append(bytes(changed))
        try:
            audit.pe_resources(audit.BinaryData(changed))
        except ValueError as error:
            expect(str(error) == 'Incomplete PE resource data directory', 'Wrong directory failure reason')
        else:
            raise AssertionError('One-sided zero resource data directory was silently accepted')
    changed = bytearray(data)
    struct.pack_into('<II', changed, directory, 0, 0)
    expect(audit.pe_resources(audit.BinaryData(changed)) == [], 'Absent resource directory was rejected')
    if args.verify_regression:
        fixed = '''    if not resource_rva and not resource_size:
        return []
    if not resource_rva or not resource_size:
        raise ValueError('Incomplete PE resource data directory')
'''
        source = inspect.getsource(audit.pe_resource_entries)
        if source.count(fixed) != 1:
            raise RuntimeError('Cannot isolate original incomplete-directory acceptance')
        original = source.replace(fixed, '    if not resource_rva or not resource_size:\n        return []\n', 1)
        namespace = {'struct': struct}
        exec(compile(original, '<original PE directory check>', 'exec'), namespace)
        prior = namespace['pe_resource_entries']
        expect(prior(audit.BinaryData(data)) == entries, 'Original parser disagrees on unchanged actual binary')
        for changed in invalid:
            expect(prior(audit.BinaryData(changed)) == [], 'Original parser did not reproduce silent acceptance')
        print('Original PE directory check silently accepts both malformed real-binary variants as expected')

    source = audit.Source()
    try:
        classify = audit.make_resource_classifier(source)
    finally:
        source.close()
    clean = audit.audit_version_resources(args.binary.name, data, classify)
    expect(clean['resources'] > 0 and not clean['violations'], 'Actual binary fails version audit before mutation')
    versions = [(key, payload) for key, payload in entries if key[0] == 16]
    expect(len(versions) == 1, 'Expected one actual version resource for focused mutations')
    key, payload = versions[0]
    expect(data.count(payload) == 1, 'Actual version resource payload is not unique')
    payload_begin = data.index(payload)
    table_keys = [(section, language, codepage) for section, language, codepage in audit.version_languages(payload)
                  if section == 'StringFileInfo' and language == 0x0409]
    expect(len(table_keys) == 1, 'Expected one English StringTable in actual binary')
    codepage = table_keys[0][2]
    table = f'0409{codepage:04x}'.encode('utf-16-le')
    translation = struct.pack('<HH', 0x0409, codepage)
    for section, old, new in (
            ('StringFileInfo', table, f'0419{codepage:04x}'.encode('utf-16-le')),
            ('VarFileInfo', translation, struct.pack('<HH', 0x0419, codepage))):
        expect(payload.count(old) == 1, f'Actual {section} language value is not unique')
        changed = bytearray(data)
        begin = payload_begin + payload.index(old)
        changed[begin:begin + len(old)] = new
        expect(audit.pe_resources(audit.BinaryData(changed)) == audit.pe_resources(audit.BinaryData(data)),
               f'{section} mutation changed public resource directory language tags')
        result = audit.audit_version_resources(args.binary.name, changed, classify)
        expect(len(result['violations']) == 1 and section in result['violations'][0]['reason'] and
               'language 0x0419' in result['violations'][0]['reason'],
               f'Embedded {section} foreign language was not detected')
    changed_payload = bytearray(payload)
    struct.pack_into('<H', changed_payload, 0, len(payload) + 1)
    changed = bytearray(data)
    changed[payload_begin:payload_begin + len(payload)] = changed_payload
    result = audit.audit_version_resources(args.binary.name, changed, classify)
    expect(len(result['violations']) == 1 and 'version resource block' in result['violations'][0]['reason'],
           'Malformed version resource was silently accepted')
    vendor_path = ROOT / 'drivers/network/dd/netr28ux/RaCoInstx.dll'
    vendor = vendor_path.read_bytes()
    vendor_digest = hashlib.sha256(vendor).hexdigest()
    changed_digest = hashlib.sha256(vendor + b'\0').hexdigest()
    vendor_resources = audit.pe_resources(audit.BinaryData(vendor))
    foreign = [entry for entry in vendor_resources if entry[2] == 0x0404]
    if len(foreign) != 49 or set(foreign) != audit.VENDOR_RESOURCES:
        raise RuntimeError('Approved vendor binary resource tuples changed')
    for entry in foreign:
        expect(classify(vendor_path.name, *entry, vendor_digest) == 'vendor-exception',
               'Approved original vendor resource was rejected')
        expect(classify(vendor_path.name, *entry) is None,
               'Vendor resource without a whole-binary hash was allowed')
        expect(classify(vendor_path.name, *entry, changed_digest) is None,
               'Modified vendor binary was allowed')
        expect(classify('different.dll', *entry, vendor_digest) is None,
               'Different binary filename was allowed by the vendor exception')
    expect(classify(vendor_path.name, 6, 999, 0x0404, vendor_digest) is None,
           'Extra vendor resource tuple was allowed')
    expect(classify(vendor_path.name, 6, 3841, 0x0407, vendor_digest) is None,
           'Different vendor resource language was allowed')
    print(f'Actual PE resource and version-language mutations: {checks} checks, 0 failures')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
