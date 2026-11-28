#!/usr/bin/env python3
"""
PROJECT:     ReliefWin build tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Require generated SDK headers before compiling ReliefOffice libraries
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
import importlib.util
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
sys.dont_write_bytecode = True
spec = importlib.util.spec_from_file_location('depcheck', ROOT / 'sdk/tools/depcheck.py')
depcheck = importlib.util.module_from_spec(spec)
spec.loader.exec_module(depcheck)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'output-Clang-amd64-debug')
    parser.add_argument('--expect-regression', action='store_true')
    args = parser.parse_args()
    graph = depcheck.Graph(str(args.build.resolve()))
    header = graph.path('sdk/include/psdk/msxml.h')
    assert header in graph.producer
    reachability = depcheck.Reachability(graph, {header})
    targets = {'reliefoffice_cairo', 'reliefoffice_png', 'reliefoffice_locale', 'reliefoffice_platform'}
    checked = set()
    failed = set()
    for edge in graph.edges:
        if not edge.outs:
            continue
        target = depcheck.target_of(edge.outs[0], graph.build_dir)[0]
        if target in targets and edge.rule.startswith(('C_COMPILER', 'CXX_COMPILER')):
            checked.add(target)
            if not reachability.of_edge(edge):
                failed.add(target)
    assert checked == targets, checked
    if args.expect_regression:
        assert failed == targets, failed
        print('Original graph reproduces the missing SDK-header ordering in all four libraries')
    else:
        assert not failed, failed
        print(f'{args.build.name}: all four ReliefOffice libraries wait for generated SDK headers')


if __name__ == '__main__':
    main()
