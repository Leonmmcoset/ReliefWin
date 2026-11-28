#!/usr/bin/env python3
"""
PROJECT:     ReliefWin build tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Verify that Ninja can generate the EFI image required by recovery
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import subprocess


ROOT = Path(__file__).resolve().parents[1]


def ninja(build, *arguments):
    result = subprocess.run(
        ["ninja", "-C", str(build), *arguments],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    )
    if result.returncode:
        raise RuntimeError(result.stdout)
    return result.stdout


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build", default="output-Clang-amd64-debug")
    args = parser.parse_args()
    build = ROOT / args.build
    ninja(build, "-n", "bootcd")
    graph = ninja(build, "-t", "query", "boot/efisys.bin")
    if "input: CUSTOM_COMMAND" not in graph:
        raise AssertionError("EFI image has no producing command:\n" + graph)
    if "boot/recovery/Winre.wim" not in graph:
        raise AssertionError("Recovery image no longer depends on the EFI image:\n" + graph)
    print(f"{build.name}: EFI image producer and bootcd dependency graph OK")


if __name__ == "__main__":
    main()
