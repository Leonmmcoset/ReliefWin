#!/usr/bin/env python3
"""
PROJECT:     ReliefWin build tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise Mesa SDK include selection and managed feed preparation
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def run(*arguments):
    return subprocess.run(arguments, check=True, text=True, capture_output=True)


def include_probe(source):
    start = source.index('set(MESA_LIBERNT_SOURCE_DIR ')
    end = source.index('set(MESA_LLVM_ROOT ', start)
    return source[start:end] + '''
if(NOT EXISTS "${MESA_LIBERNT_INCLUDE_DIR}/libs/zlib/zlib.h")
    message(FATAL_ERROR "Mesa did not find the SDK zlib header")
endif()
file(WRITE "${OUTPUT}" "${MESA_LIBERNT_INCLUDE_DIR}")
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--mesa", type=Path, default=ROOT / "submodules/mesa")
    args = parser.parse_args()
    original = run("git", "-C", str(args.mesa), "show", "HEAD:CMakeLists.txt").stdout
    prepare = str(ROOT / "submodules/mesa-sdk.cmake")
    with tempfile.TemporaryDirectory() as directory:
        folder = Path(directory)
        source = folder / "mesa"
        source.mkdir()
        run("git", "init", "--quiet", str(source))
        cmake = source / "CMakeLists.txt"
        cmake.write_text(original)
        probe = folder / "probe.cmake"
        probe.write_text(include_probe(original))
        output = folder / "include.txt"
        select = ["cmake", f"-DMESA_LIBERNT_SOURCE_DIR={ROOT}", f"-DOUTPUT={output}", "-P", str(probe)]
        baseline = subprocess.run(select, text=True, capture_output=True)
        assert baseline.returncode != 0
        assert "Mesa did not find the SDK zlib header" in baseline.stderr
        command = ["cmake", f"-DMESA_SOURCE_DIR={source}", "-P", prepare]
        run(*command)
        prepared = cmake.read_text()
        stamp = cmake.stat().st_mtime_ns
        run(*command)
        assert cmake.read_text() == prepared
        assert cmake.stat().st_mtime_ns == stamp
        probe.write_text(include_probe(prepared))
        run(*select)
        assert output.read_text() == str(ROOT / "sdk/include/reliefwin")
        user_edit = '\nset(RELIEFWIN_TEST_LOCAL_EDIT ON)\n'
        cmake.write_text(prepared + user_edit)
        restore = ["cmake", f"-DMESA_SOURCE_DIR={source}", "-DRESTORE=ON", "-P", prepare]
        run(*restore)
        assert cmake.read_text() == original + user_edit
        run(*command)
        assert cmake.read_text() == prepared + user_edit
        run(*restore)
        assert cmake.read_text() == original + user_edit
        cmake.write_text(original)
        run("git", "-C", str(source), "symbolic-ref", "HEAD", "refs/heads/mesa-test")
        commit = ["git", "-C", str(source), "-c", "user.name=Mesa SDK Test",
                  "-c", "user.email=mesa-sdk-test@localhost", "commit", "--quiet", "-am"]
        run("git", "-C", str(source), "add", "CMakeLists.txt")
        run(*commit, "Initial feed")
        project = folder / "project"
        (project / "scripts").mkdir(parents=True)
        (project / "submodules").mkdir()
        shutil.copyfile(ROOT / "scripts/feeds", project / "scripts/feeds")
        shutil.copyfile(prepare, project / "submodules/mesa-sdk.cmake")
        (project / "feeds.conf.default").write_text(f"src-git mesa {source};mesa-test\n")
        update = ["sh", str(project / "scripts/feeds"), "update", "mesa"]
        run(*update)
        checkout = project / "submodules/mesa"
        assert (checkout / "CMakeLists.txt").read_text() == prepared
        cmake.write_text(original + user_edit)
        run(*commit, "Advance feed")
        run(*update)
        assert (checkout / "CMakeLists.txt").read_text() == prepared + user_edit
        local = checkout / "Local.cmake"
        local.write_text(user_edit)
        preserved = run(*update)
        assert "local modifications" in preserved.stderr
        assert local.read_text() == user_edit
        assert (checkout / "CMakeLists.txt").read_text() == prepared + user_edit
        cmake.write_text('cmake_minimum_required(VERSION 3.24)\n')
        unsupported = subprocess.run(command, text=True, capture_output=True)
        assert unsupported.returncode != 0
        assert "Mesa SDK include selection has changed" in unsupported.stderr
        assert cmake.read_text() == 'cmake_minimum_required(VERSION 3.24)\n'
    print("Mesa SDK header selection, idempotency, feed restoration and local edit preservation OK")


if __name__ == "__main__":
    main()
