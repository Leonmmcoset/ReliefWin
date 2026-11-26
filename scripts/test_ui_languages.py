#!/usr/bin/env python3
# PROJECT:     ReliefWin resource tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Audit retained UI translations and compiled PE resource languages
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import struct
import subprocess


ROOT = Path(__file__).resolve().parents[1]
LOCALES = {'en-US', 'ja-JP', 'zh-CN'}
UI_LANGIDS = {0x0409, 0x0411, 0x0804}
NEUTRAL_LANGIDS = {0, 0x0400}
FLAGS = {'LANGUAGE_EN_US', 'LANGUAGE_JA_JP', 'LANGUAGE_ZH_CN'}
RC_LANGUAGE = re.compile(r'^\s*LANGUAGE\s+([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)', re.M)
LITERAL_OR_COMMENT = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*[\s\S]*?\*/')
LOCALE_NAME = re.compile(r'^[a-z]{2,3}[-_][A-Z]{2}$')
IMM32 = 'subsystems/win/win32k/user/imm32/imm32.rc'
PE_EXTENSIONS = {'.exe', '.dll', '.cpl', '.scr', '.ocx', '.sys', '.acm', '.drv', '.mun', '.mui',
                 '.msstyles', '.efi', '.ax', '.pyd', '.tsp', '.ime'}
VENDOR_SHA256 = '6b342af3086911b07e60aecc10700d7d10203d7ff5674b70b664a74bf2ef1ca6'
VENDOR_RESOURCES = {(kind, name, 0x0404) for kind, names in (
    (1, range(1, 17)),
    (2, (30994, 30996)),
    (5, (30721, 30734)),
    (6, (3841, 3842, 3843, 3857, 3858, 3859, 3860, 3865, 3866, 3867, 3868, 3869, 3887)),
    (12, (30977, *range(30998, 31012))),
    (16, (1,)),
) for name in names}


def decode(data):
    if data.startswith((b'\xff\xfe', b'\xfe\xff')):
        return data.decode('utf-16')
    try:
        return data.decode('utf-8-sig')
    except UnicodeDecodeError:
        return data.decode('latin-1')


def uncomment(text):
    return LITERAL_OR_COMMENT.sub(lambda match: match[0] if match[0][0] in ('"', "'")
                                  else re.sub(r'[^\n]', ' ', match[0]), text)


def normalized_locale(value):
    value = value.replace('_', '-').lower()
    return {'en': 'en-US', 'en-us': 'en-US', 'ja': 'ja-JP', 'ja-jp': 'ja-JP',
            'zh-cn': 'zh-CN'}.get(value, value)


def fixture(path):
    for prefix in ('modules/rostests/', 'submodules/host-tests/'):
        position = path.find(prefix)
        if position >= 0:
            path = path[position:]
            break
    return ((path.startswith(('modules/rostests/apitests/', 'submodules/host-tests/')) or
             (path.startswith('modules/rostests/winetests/') and
              not path.startswith('modules/rostests/winetests/GUI/')) or
             path.startswith('modules/rostests/win32/user32/kbdlayout/')) and '/lang/' not in path)


class Source:
    def __init__(self, revision=None):
        self.revision = revision
        command = ['git', 'ls-tree', '-rz', '--name-only', revision] if revision else ['git', 'ls-files', '--cached', '--others', '--exclude-standard', '-z']
        self.paths = set(subprocess.check_output(command, cwd=ROOT).decode().split('\0')) - {''}
        if not revision:
            self.paths = {path for path in self.paths if (ROOT / path).is_file()}
        self.process = None
        if revision:
            self.process = subprocess.Popen(['git', 'cat-file', '--batch'], cwd=ROOT,
                                            stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def read(self, path):
        if not self.process:
            return (ROOT / path).read_bytes()
        self.process.stdin.write(f'{self.revision}:{path}\n'.encode())
        self.process.stdin.flush()
        header = self.process.stdout.readline().split()
        if len(header) != 3 or header[1] != b'blob':
            raise ValueError(f'Cannot read {self.revision}:{path}')
        data = self.process.stdout.read(int(header[2]))
        if self.process.stdout.read(1) != b'\n':
            raise ValueError('Malformed git cat-file response')
        return data

    def close(self):
        if self.process:
            self.process.stdin.close()
            self.process.stdout.close()
            if self.process.wait():
                raise ValueError('git cat-file failed')


def constants(source):
    values = {}
    for path in ('sdk/include/host/nls.h', 'sdk/include/xdk/ntbasedef.h'):
        text = decode(source.read(path))
        values.update(re.findall(r'^#define\s+((?:LANG|SUBLANG)_[A-Z_]+)\s+((?:LANG|SUBLANG)_[A-Z_]+|0[xX][0-9a-fA-F]+|[0-9]+)\b', text, re.M))
    result = {}
    for name, value in values.items():
        chain = {name}
        while value in values:
            if value in chain:
                raise ValueError(f'Cyclic language definition {name}')
            chain.add(value)
            value = values[value]
        result[name] = int(value, 0)
    return result


def langid(match, definitions):
    def value(token):
        return definitions[token] if token in definitions else int(token, 0)
    return (value(match[2]) << 10) | value(match[1])


def blocks(text, definitions):
    text = uncomment(text)
    matches = list(RC_LANGUAGE.finditer(text))
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        yield langid(match, definitions), match.start(), text[match.end():end]


def retained_text(text, definitions):
    result = []
    for language, start, body in blocks(text, definitions):
        if language not in UI_LANGIDS and language not in {9, 17}:
            continue
        body = re.sub(r'^\s*#.*$', '', body, flags=re.M)
        tokens = re.findall(r'"(?:\\.|[^"\\])*"|[^\s]+', body)
        if tokens:
            result.append(tokens)
    return result


def bitmap_ids(source):
    text = decode(source.read(str(PurePosixPath(IMM32).with_name('resource.h'))))
    return {int(value, 0) for value in re.findall(r'^#define\s+IDB_T1_[A-Z_]+\s+(\d+)\b', text, re.M)}


def nls_resources(source):
    path = PurePosixPath('dll/win32/kernel32/winnls/locale_rc.rc')
    resource = decode(source.read(str(path)))
    definitions = constants(source)
    values = {}
    for text in (decode(source.read('sdk/include/psdk/winnls.h')), resource):
        values.update(re.findall(r'^#define\s+((?:LOCALE|LGRPID)_[A-Z0-9_]+)\s+((?:LOCALE|LGRPID)_[A-Z0-9_]+|0[xX][0-9a-fA-F]+|[0-9]+)\b', text, re.M))
    result = set()
    for included in re.findall(r'^\s*#\s*include\s+"(nls/[^"\n]+\.nls)"', resource, re.M):
        text = uncomment(decode(source.read(str(path.parent / included))))
        statement = re.search(r'\bSTRINGTABLE\s+LANGUAGE\s+(\w+)\s*,\s*(\w+)', text)
        if not statement:
            raise ValueError(f'NLS resource has no language: {included}')
        language = langid(statement, definitions)
        for expression in re.findall(r'^\s*((?:LOCALE|LGRPID)_[A-Z0-9_+]+)\s+L?"', text, re.M):
            number = 0
            for name in expression.split('+'):
                value = name
                visited = set()
                while value in values:
                    if value in visited:
                        raise ValueError(f'Cyclic NLS resource definition: {name}')
                    visited.add(value)
                    value = values[value]
                number += int(value, 0)
            result.add((6, (number >> 4) + 1, language))
    return result


def make_resource_classifier(source):
    keyboard = bitmap_ids(source)
    regional = nls_resources(source)

    def classify(filename, kind, name, language, sha256=None):
        filename = PurePosixPath(str(filename).replace('\\', '/')).name.lower()
        if (filename == 'racoinstx.dll' and sha256 == VENDOR_SHA256 and
                (kind, name, language) in VENDOR_RESOURCES):
            return 'vendor-exception'
        if filename == 'kernel32.dll' and (kind, name, language) in regional:
            return 'regional-data'
        if filename == 'imm32.dll' and kind == 2 and language == 0x0404 and name in keyboard:
            return 'keyboard-data'
        if language in UI_LANGIDS:
            return 'ui'
        if language in NEUTRAL_LANGIDS:
            return 'neutral'
        return None

    return classify


def mc_languages(text):
    declaration = re.search(r'\bLanguageNames\s*=\s*\(([^)]*)\)', text, re.S | re.I)
    names = {name: int(value, 0) for name, value in
             re.findall(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(0x[0-9a-fA-F]+|[0-9]+)\s*:', declaration[1] if declaration else '')}
    if not declaration:
        names['English'] = 0x0409
    return names


def retained_mc(text):
    text = text.replace('\r\n', '\n')
    names = mc_languages(text)
    message = symbol = ''
    result = []
    tokens = re.compile(r'^\s*MessageId\s*=\s*([^\n]*)|^\s*SymbolicName\s*=\s*([^\n]*)|^\s*Language\s*=\s*(\w+)[^\n]*\n(.*?)^\.[ \t]*$', re.M | re.S)
    for match in tokens.finditer(text):
        if match[1] is not None:
            message = match[1].strip()
        elif match[2] is not None:
            symbol = match[2].strip()
        elif names.get(match[3]) in UI_LANGIDS | NEUTRAL_LANGIDS:
            result.append((message, symbol, names[match[3]], match[4]))
    return result


def retained_inf(text):
    result = []
    text = text.replace('\r\n', '\n')
    sections = list(re.finditer(r'^\s*\[([^]\n]+)\][^\n]*$', text, re.M))
    for index, section in enumerate(sections):
        name = section[1].lower()
        match = re.fullmatch(r'strings\.([0-9a-f]+)', name)
        if name != 'strings' and (not match or int(match[1], 16) not in UI_LANGIDS | {0, 0x0011}):
            continue
        end = sections[index + 1].start() if index + 1 < len(sections) else len(text)
        lines = [line.strip() for line in text[section.end():end].splitlines()
                 if line.strip() and not line.lstrip().startswith(';')]
        result.append((name, lines))
    return result


def audit_source(source):
    violations = []
    counts = Counter()
    definitions = constants(source)
    seen = set()

    def fail(path, reason):
        violations.append({'path': path, 'reason': reason})

    for path in sorted(source.paths):
        p = PurePosixPath(path)
        if p.parts[0] == 'translations' and len(p.parts) > 1 and p.parts[1] not in LOCALES:
            fail(path, 'Removed documentation locale')
        if fixture(path):
            continue
        if p.suffix.lower() not in {'.rc', '.mc', '.inf', '.po', '.pot', '.desktop', '.html', '.htm', '.xml'}:
            continue
        text = decode(source.read(path))
        counts[p.suffix.lower()] += 1
        if ('lang' in p.parts or 'locale' in p.parts) and LOCALE_NAME.fullmatch(p.stem):
            if normalized_locale(p.stem) not in LOCALES:
                fail(path, 'Removed translation locale')
        if p.suffix.lower() == '.rc':
            clean = uncomment(text)
            for language, start, body in blocks(text, definitions):
                if language not in UI_LANGIDS | NEUTRAL_LANGIDS:
                    bitmap_only = (path == IMM32 and language == 0x0404 and
                                   re.fullmatch(r'(?:\s*IDB_T1_[A-Z_]+\s+BITMAP\s+"[^"\n]+"\s*)+', body))
                    if not bitmap_only:
                        fail(path, f'RC language 0x{language:04x} at line {clean.count(chr(10), 0, start) + 1}')
                elif language in UI_LANGIDS and re.search(r'"[^"\n]+"', body):
                    seen.add(language)
            for flag in set(re.findall(r'\bLANGUAGE_[A-Z]{2,3}_[A-Z]{2}\b', clean)) - FLAGS:
                fail(path, f'Removed language build guard {flag}')
            for included in re.findall(r'^\s*#\s*include\s+"([^"\n]+)"', clean, re.M):
                if included.startswith('lang/') and str(p.parent / included) not in source.paths:
                    fail(path, f'Missing translation include {included}')
        elif p.suffix.lower() == '.inf':
            for value in re.findall(r'^\s*\[Strings\.([0-9a-fA-F]+)\]', text, re.M | re.I):
                language = int(value, 16)
                if language not in UI_LANGIDS | {0, 0x0011}:
                    fail(path, f'INF strings language 0x{language:04x}')
        elif p.suffix.lower() == '.mc':
            names = mc_languages(text)
            for name in re.findall(r'^\s*Language\s*=\s*(\w+)', text, re.M | re.I):
                if name not in names or names[name] not in UI_LANGIDS | NEUTRAL_LANGIDS:
                    fail(path, f'MC language {name}')
            for name, value in names.items():
                if value not in UI_LANGIDS | NEUTRAL_LANGIDS:
                    fail(path, f'MC language declaration {name}=0x{value:04x}')
        elif p.suffix.lower() == '.po':
            for value in re.findall(r'^"Language:\s*([^\\"\s]+)', text, re.M):
                if normalized_locale(value) not in LOCALES:
                    fail(path, f'PO language {value}')
        elif p.suffix.lower() == '.desktop':
            for value in re.findall(r'^\w+\[([^\]]+)\]\s*=', text, re.M):
                if normalized_locale(value) not in LOCALES:
                    fail(path, f'Desktop translation {value}')
        elif p.suffix.lower() in {'.html', '.htm', '.xml'}:
            for value in re.findall(r'\b(?:xml:)?lang\s*=\s*[\'"]([^\'\"]+)[\'"]', text):
                if normalized_locale(value) not in LOCALES:
                    fail(path, f'Document language {value}')
    config = uncomment(decode(source.read('sdk/cmake/localization.cmake')))
    for flag in set(re.findall(r'\bLANGUAGE_[A-Z]{2,3}_[A-Z]{2}\b', config)) - FLAGS:
        fail('sdk/cmake/localization.cmake', f'Removed language build definition {flag}')
    if not UI_LANGIDS <= seen:
        fail('source', f'Missing retained UI resource languages: {sorted(UI_LANGIDS - seen)}')
    return {'files': dict(counts), 'violations': violations, 'retained_langids': sorted(seen)}


def compare_retained(source, reference):
    current_defs = constants(source)
    reference_defs = constants(reference)
    violations = []
    checked = Counter()
    for path in sorted(reference.paths):
        p = PurePosixPath(path)
        extension = p.suffix.lower()
        if extension not in {'.rc', '.mc', '.inf'} or fixture(path):
            continue
        if LOCALE_NAME.fullmatch(p.stem) and normalized_locale(p.stem) not in LOCALES:
            continue
        if extension == '.rc':
            extract = lambda text: retained_text(text, reference_defs)
        else:
            extract = retained_mc if extension == '.mc' else retained_inf
        before = extract(decode(reference.read(path)))
        if not before:
            continue
        checked[extension] += 1
        if extension == '.rc':
            extract = lambda text: retained_text(text, current_defs)
        after = extract(decode(source.read(path))) if path in source.paths else []
        if before != after:
            violations.append({'path': path, 'reason': 'Retained-language resource text changed or removed'})
    return {'files': dict(checked), 'violations': violations}


def pe_resource_entries(path):
    data = path.read_bytes()
    if data[:2] != b'MZ':
        return None

    def unpack(fmt, offset):
        if offset < 0 or offset + struct.calcsize(fmt) > len(data):
            raise ValueError('Truncated PE structure')
        return struct.unpack_from(fmt, data, offset)

    pe = unpack('<I', 0x3c)[0]
    if data[pe:pe + 4] != b'PE\0\0':
        raise ValueError('Invalid PE signature')
    sections_count = unpack('<H', pe + 6)[0]
    optional_size = unpack('<H', pe + 20)[0]
    optional = pe + 24
    magic = unpack('<H', optional)[0]
    if magic not in (0x10b, 0x20b):
        raise ValueError('Unknown PE optional header')
    directory_start = 96 if magic == 0x10b else 112
    if optional_size < directory_start:
        raise ValueError('Truncated PE optional header')
    if unpack('<I', optional + directory_start - 4)[0] <= 2:
        return []
    if optional_size < directory_start + 24:
        raise ValueError('Truncated PE resource directory entry')
    resource_rva, resource_size = unpack('<II', optional + directory_start + 16)
    if not resource_rva and not resource_size:
        return []
    if not resource_rva or not resource_size:
        raise ValueError('Incomplete PE resource data directory')
    sections = [unpack('<IIII', optional + optional_size + index * 40 + 8)
                for index in range(sections_count)]

    def offset(rva, size):
        for virtual_size, address, raw_size, raw in sections:
            if address <= rva < address + max(virtual_size, raw_size):
                delta = rva - address
                if delta + size <= raw_size and raw + delta + size <= len(data):
                    return raw + delta
                break
        raise ValueError('Resource RVA is not backed by file data')

    base = offset(resource_rva, resource_size)
    found = []
    visited = set()

    def within(relative, size):
        if relative < 0 or relative + size > resource_size:
            raise ValueError('Resource directory exceeds its data directory')
        return base + relative

    def walk(relative, identifiers):
        if relative in visited or len(identifiers) > 2:
            raise ValueError('Cyclic or over-deep resource tree')
        visited.add(relative)
        header = within(relative, 16)
        named, numbered = unpack('<HH', header + 12)
        for index in range(named + numbered):
            name, child = unpack('<II', within(relative + 16 + index * 8, 8))
            if name & 0x80000000:
                location = name & 0x7fffffff
                length = unpack('<H', within(location, 2))[0]
                begin = within(location + 2, length * 2)
                identifier = data[begin:begin + length * 2].decode('utf-16-le')
            else:
                identifier = name
            entries = identifiers + (identifier,)
            if child & 0x80000000:
                walk(child & 0x7fffffff, entries)
            else:
                if len(entries) != 3 or not isinstance(entries[2], int):
                    raise ValueError('Resource leaf has no numeric LANGID')
                rva, size, codepage, reserved = unpack('<IIII', within(child, 16))
                begin = offset(rva, size)
                found.append((entries, data[begin:begin + size]))
        visited.remove(relative)

    walk(0, ())
    return found


def pe_resources(path):
    entries = pe_resource_entries(path)
    return [resource for resource, payload in entries] if entries is not None else None


def version_languages(data):
    def node(position, limit, depth=0):
        if depth > 4 or position + 6 > limit:
            raise ValueError('Truncated or over-deep version resource')
        length, value_length, value_type = struct.unpack_from('<HHH', data, position)
        end = position + length
        if length < 6 or end > limit or value_type not in {0, 1}:
            raise ValueError('Invalid version resource block')
        cursor = position + 6
        while cursor + 2 <= end and data[cursor:cursor + 2] != b'\0\0':
            cursor += 2
        if cursor + 2 > end:
            raise ValueError('Unterminated version resource key')
        key = data[position + 6:cursor].decode('utf-16-le')
        value_begin = (cursor + 5) & ~3
        value_end = value_begin + value_length * (2 if value_type else 1)
        if value_end > end and (value_length or value_begin - end > 3):
            raise ValueError('Version resource value exceeds its block')
        children = []
        cursor = (value_end + 3) & ~3
        while cursor < end:
            if end - cursor < 6:
                if any(data[cursor:end]):
                    raise ValueError('Invalid version resource trailing data')
                break
            child = node(cursor, end, depth + 1)
            children.append(child)
            cursor = (cursor + child['length'] + 3) & ~3
        return {'key': key, 'value': data[value_begin:value_end], 'type': value_type,
                'children': children, 'length': length}

    root = node(0, len(data))
    if root['key'] != 'VS_VERSION_INFO' or root['type'] != 0:
        raise ValueError('Missing VS_VERSION_INFO root')
    if any(data[root['length']:]):
        raise ValueError('Unexpected data after version resource')
    result = []
    for section in root['children']:
        if section['key'] == 'StringFileInfo':
            for table in section['children']:
                if not re.fullmatch(r'[0-9A-Fa-f]{8}', table['key']):
                    raise ValueError('Invalid version StringTable language/codepage key')
                result.append(('StringFileInfo', int(table['key'][:4], 16), int(table['key'][4:], 16)))
        elif section['key'] == 'VarFileInfo':
            for translation in section['children']:
                if translation['key'] != 'Translation':
                    continue
                if translation['type'] != 0 or len(translation['value']) % 4:
                    raise ValueError('Invalid version Translation value')
                result.extend(('VarFileInfo', language, codepage)
                              for language, codepage in struct.iter_unpack('<HH', translation['value']))
    return result


def audit_version_resources(filename, data, classify):
    entries = pe_resource_entries(BinaryData(data))
    counts = Counter()
    classes = Counter()
    violations = []
    resources = 0
    digest = hashlib.sha256(data).hexdigest()
    for (kind, name, language), payload in entries or []:
        if kind != 16:
            continue
        resources += 1
        try:
            for section, inner_language, codepage in version_languages(payload):
                counts[f'{inner_language:04x}'] += 1
                category = classify(filename, kind, name, inner_language, digest)
                if category:
                    classes[category] += 1
                else:
                    violations.append({'path': str(filename), 'reason':
                                       f'PE version {name}/{section} language 0x{inner_language:04x}, codepage {codepage}'})
        except (ValueError, struct.error, UnicodeError) as error:
            violations.append({'path': str(filename), 'reason': f'PE version {name}: {error}'})
    return {'resources': resources, 'version_langids': dict(counts),
            'version_classes': dict(classes), 'violations': violations}


class BinaryData:
    def __init__(self, data):
        self.data = data

    def read_bytes(self):
        return self.data


def audit_source_binaries(source):
    classify = make_resource_classifier(source)
    violations = []
    counts = Counter()
    classes = Counter()
    version_counts = Counter()
    version_classes = Counter()
    versions = 0
    binaries = 0
    for path in sorted(source.paths):
        p = PurePosixPath(path)
        if p.suffix.lower() not in PE_EXTENSIONS or fixture(path):
            continue
        try:
            data = source.read(path)
            resources = pe_resources(BinaryData(data))
            if resources is None:
                continue
            binaries += 1
            version = audit_version_resources(p.name, data, classify)
            versions += version['resources']
            version_counts.update(version['version_langids'])
            version_classes.update(version['version_classes'])
            violations.extend({'path': path, 'reason': item['reason']} for item in version['violations'])
            digest = hashlib.sha256(data).hexdigest()
            for kind, name, language in resources:
                counts[f'{language:04x}'] += 1
                category = classify(p.name, kind, name, language, digest)
                if category:
                    classes[category] += 1
                else:
                    violations.append({'path': path, 'reason': f'PE resource {kind}/{name} language 0x{language:04x}'})
        except (OSError, ValueError, struct.error, UnicodeError) as error:
            violations.append({'path': path, 'reason': str(error)})
    return {'binaries': binaries, 'resource_langids': dict(counts),
            'resource_classes': dict(classes), 'version_resources': versions,
            'version_langids': dict(version_counts), 'version_classes': dict(version_classes),
            'violations': violations}


def audit_pe(inputs, classify):
    files = set()
    for item in inputs:
        if item.is_dir():
            files.update(p for p in item.rglob('*') if p.is_file() and p.suffix.lower() in PE_EXTENSIONS)
        else:
            files.add(item)
    violations = []
    counts = Counter()
    classes = Counter()
    version_counts = Counter()
    version_classes = Counter()
    versions = 0
    binaries = 0
    for path in sorted(files):
        if fixture(path.as_posix()) or 'symbols' in path.parts:
            continue
        try:
            data = path.read_bytes()
            resources = pe_resources(BinaryData(data))
            if resources is None:
                continue
            binaries += 1
            version = audit_version_resources(path.name, data, classify)
            versions += version['resources']
            version_counts.update(version['version_langids'])
            version_classes.update(version['version_classes'])
            violations.extend({'path': str(path), 'reason': item['reason']} for item in version['violations'])
            digest = hashlib.sha256(data).hexdigest()
            for kind, name, language in resources:
                counts[f'{language:04x}'] += 1
                category = classify(path.name, kind, name, language, digest)
                if category:
                    classes[category] += 1
                else:
                    violations.append({'path': str(path), 'reason': f'PE resource {kind}/{name} language 0x{language:04x}'})
        except (OSError, ValueError, struct.error, UnicodeError) as error:
            violations.append({'path': str(path), 'reason': str(error)})
    if not binaries:
        violations.append({'path': 'artifacts', 'reason': 'No PE binaries were checked'})
    return {'binaries': binaries, 'resource_langids': dict(counts),
            'resource_classes': dict(classes), 'version_resources': versions,
            'version_langids': dict(version_counts), 'version_classes': dict(version_classes),
            'violations': violations}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--revision')
    parser.add_argument('--baseline')
    parser.add_argument('--verify-regression')
    parser.add_argument('--no-source', action='store_true')
    parser.add_argument('--pe', type=Path, action='append', default=[])
    parser.add_argument('--json', type=Path)
    args = parser.parse_args()
    result = {}
    source = Source(args.revision)
    try:
        if not args.no_source:
            result['source'] = audit_source(source)
            result['source_prebuilt'] = audit_source_binaries(source)
        if args.baseline:
            reference = Source(args.baseline)
            try:
                result['retained'] = compare_retained(source, reference)
            finally:
                reference.close()
        if args.verify_regression:
            reference = Source(args.verify_regression)
            try:
                prior = audit_source(reference)
                prior_binaries = audit_source_binaries(reference)
            finally:
                reference.close()
            if not prior['violations'] and not prior_binaries['violations']:
                raise ValueError('Original resources did not fail the language restriction')
            result['regression'] = {'revision': args.verify_regression,
                                    'expected_violations': len(prior['violations']) + len(prior_binaries['violations']),
                                    'prebuilt_binaries': prior_binaries['binaries'],
                                    'expected_prebuilt_violations': len(prior_binaries['violations'])}
        if args.pe:
            result['artifacts'] = audit_pe(args.pe, make_resource_classifier(source))
    finally:
        source.close()
    if args.json:
        args.json.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n')
    failures = 0
    for name, section in result.items():
        violations = section.get('violations', [])
        failures += len(violations)
        print(f'{name}: {json.dumps({k: v for k, v in section.items() if k != "violations"}, ensure_ascii=False)}; {len(violations)} failures')
        for violation in violations[:40]:
            print(f'  {violation["path"]}: {violation["reason"]}')
        if len(violations) > 40:
            print(f'  {len(violations) - 40} more failures')
    return 1 if failures else 0


if __name__ == '__main__':
    raise SystemExit(main())
