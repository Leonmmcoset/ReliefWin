#!/usr/bin/env python3
import ast
import json
from pathlib import Path
import sys


def read_po(path):
    entries = {}
    entry = {}
    key = None
    fuzzy = False
    def flush():
        if not fuzzy and entry.get('msgid') and entry.get('msgstr'):
            entries[entry['msgid']] = entry['msgstr']
    for line in path.read_text(encoding='utf-8').splitlines() + ['']:
        line = line.strip()
        if not line:
            flush()
            entry, key, fuzzy = {}, None, False
        elif line.startswith('#,') and 'fuzzy' in line:
            fuzzy = True
        elif line.startswith('#'):
            continue
        elif line.startswith(('msgid ', 'msgstr ')):
            key, value = line.split(' ', 1)
            entry[key] = ast.literal_eval(value)
        elif line.startswith('"') and key:
            entry[key] += ast.literal_eval(line)
        else:
            raise ValueError(f'Unsupported PO entry in {path}: {line}')
    return entries


if __name__ == '__main__':
    root, output = map(Path, sys.argv[1:])
    text = ['struct OfficeTranslation { const char *id; const char *text; };']
    apps = sorted(root.iterdir())
    for app in apps:
        items = sorted(read_po(app / 'zh_CN.po').items())
        text += [f'static const OfficeTranslation {app.name}[] = {{']
        text += ['{' + json.dumps(k, ensure_ascii=False) + ',' + json.dumps(v, ensure_ascii=False) + '},' for k, v in items]
        text += ['};']
    text += ['struct OfficeCatalog { const char *name; const OfficeTranslation *entries; unsigned count; };',
             'static const OfficeCatalog office_catalogs[] = {']
    text += [f'{{"{a.name}",{a.name},sizeof({a.name})/sizeof(*{a.name})}},' for a in apps]
    text += ['};']
    output.write_text('\n'.join(text) + '\n', encoding='utf-8')
