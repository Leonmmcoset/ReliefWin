#!/usr/bin/env python3
"""Build original ReliefOffice regression tests against the port's POSIX branch."""
import argparse
import os
from pathlib import Path
import subprocess
import shutil

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--work", type=Path, default=Path("/tmp/reliefoffice-host"))
parser.add_argument("--cxx", default=os.environ.get("HOSTCXX", "clang++"))
parser.add_argument("--only", action="append", default=[])
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
work = args.work.resolve()
work.mkdir(parents=True, exist_ok=True)
data = work / "data"
env = dict(os.environ, TMPDIR=str(data), ASAN_OPTIONS="detect_leaks=1", UBSAN_OPTIONS="halt_on_error=1")
groups = {
    "reliefword": (["model", "document", "layout"], ["model", "document", "layout"]),
    "reliefexcel": (["storage", "model", "formula", "document"], ["model", "edit", "formula", "document"]),
    "reliefppt": (["model", "document", "layout"], ["model", "document", "geometry"]),
    "reliefdiagram": ([""], ["model", "document", "scene"]),
    "reliefmathpp": ([""], ["model", "document", "expression", "analysis", "mesh", "scene"]),
}
for app, (suites, units) in groups.items():
    for suite in suites:
        name = app + ("-" + suite if suite else "")
        if args.only and name not in args.only:
            continue
        test = root / "tests" / ("upstream-" + name + ".cpp")
        command = [args.cxx, "-std=c++17", "-D_POSIX_C_SOURCE=200809L", "-g", "-Wall", "-Wextra",
                   "-fno-exceptions", "-fno-rtti", "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
                   "-I" + str(root / app), "-I" + str(root), str(test)]
        command += [str(root / app / (unit + ".cpp")) for unit in units]
        command += [str(root / "platform/posix.cpp"), "-o", str(work / "regression"), "-lm", "-pthread"]
        subprocess.run(command, check=True)
        if data.is_symlink():
            data.unlink()
        elif data.exists():
            shutil.rmtree(data)
        data.mkdir()
        run = [str(work / "regression")]
        if app == "reliefexcel" or (app == "reliefword" and suite == "model"):
            run.append(str(data))
        subprocess.run(run, env=env, check=True)
        print("PASS", name, flush=True)
