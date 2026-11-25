#!/usr/bin/env python3
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--output', type=Path, default=Path('/tmp/reliefoffice-math-host'))
parser.add_argument('--compiler', default='clang++')
args = parser.parse_args()
base = Path(__file__).resolve().parents[2]
core = base / 'reliefmath'
output = args.output.resolve()
output.mkdir(parents=True, exist_ok=True)
(output / 'out/tmp').mkdir(parents=True, exist_ok=True)
(output / 'jobs-files').mkdir(exist_ok=True)
sets = {'model': ['model', 'history'], 'protocol': ['protocol'],
        'document': ['model', 'document', 'file_io', 'recovery'],
        'jobs': ['model', 'document', 'file_io', 'recovery', 'jobs'],
        'render': ['model', 'render_types', 'protocol']}
for name, sources in sets.items():
    command = [args.compiler, '-std=c++17', '-g', '-O1', '-fno-exceptions', '-fno-rtti',
               '-fsanitize=address,undefined', '-fno-sanitize-recover=all', '-DRM_TESTING',
               '-iquote', str(core), str(Path(__file__).parent / (name + '.cpp'))]
    command += [str(core / (source + '.cpp')) for source in sources]
    if name in ('document', 'jobs', 'render'):
        command += [str(base / 'platform/posix.cpp')]
    if name == 'document':
        command += ['-Wl,--wrap=fsync']
    if name == 'jobs':
        command += [str(base / 'platform/worker.cpp'), '-pthread']
    if name == 'render':
        command += [str(source) for source in (core / 'worker').glob('*.cpp') if source.name != 'main.cpp']
        command += subprocess.check_output(['pkg-config', '--cflags', '--libs', 'cairo', 'freetype2'], text=True).split()
    command += ['-o', str(output / name)]
    subprocess.run(command, check=True)
    run = [str(output / name)]
    if name == 'jobs':
        run.append(str(output / 'jobs-files'))
    if name == 'render':
        fonts = output / 'fonts'
        fonts.mkdir(exist_ok=True)
        root = base.parents[2]
        for destination, source in [('STIXTwoMath-Regular.otf', base / 'ReliefMath/STIXTwoMath-Regular.otf'),
                                    ('simsun.ttc', root / 'media/fonts/NotoSansSC-Regular.otf')]:
            link = fonts / destination
            link.unlink(missing_ok=True)
            link.symlink_to(source)
        run.append(str(fonts))
    subprocess.run(run, cwd=output, check=True)
    print(name + ': passed', flush=True)
