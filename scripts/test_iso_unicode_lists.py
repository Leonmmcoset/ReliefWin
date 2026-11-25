#!/usr/bin/env python3
"""
PROJECT:     ReliefWin image tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Preserve UTF-8 image manifests through CMake and actual mkisofs
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
import json
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def source(path, revision):
    if revision:
        return subprocess.check_output(["git", "show", f"{revision}:{path}"], cwd=ROOT).decode()
    return (ROOT / path).read_text()


def cmake_path(path):
    return str(path).replace("\\", "/").replace('"', '\\"')


def iso_files(data):
    trees = []
    for sector in range(16, len(data) // 2048):
        descriptor = data[sector * 2048:(sector + 1) * 2048]
        assert descriptor[1:6] == b"CD001", "ISO volume descriptor signature"
        if descriptor[0] == 255:
            break
        if descriptor[0] not in (1, 2):
            continue
        files = {}

        def directory(record, prefix):
            extent = int.from_bytes(record[2:6], "little") * 2048
            size = int.from_bytes(record[10:14], "little")
            end = extent + size
            while extent < end:
                length = data[extent]
                if not length:
                    extent = (extent // 2048 + 1) * 2048
                    continue
                child = data[extent:extent + length]
                name = child[33:33 + child[32]]
                extent += length
                if name in (b"\0", b"\1"):
                    continue
                path = prefix + name
                if child[25] & 2:
                    directory(child, path + b"/")
                else:
                    start = int.from_bytes(child[2:6], "little") * 2048
                    count = int.from_bytes(child[10:14], "little")
                    files[path.split(b";", 1)[0]] = data[start:start + count]

        directory(descriptor[156:190], b"")
        trees.append(files)
    assert trees, "ISO directory tree exists"
    return trees


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build-root", type=Path, default=ROOT / "output-Clang-amd64-debug")
    parser.add_argument("--baseline", help="exercise existing sources at this Git revision")
    args = parser.parse_args()
    mkisofs = args.build_root.resolve() / "host-tools/bin/mkisofs"
    assert mkisofs.is_file(), str(mkisofs)
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    work = folder / "iso-unicode"
    work.mkdir(exist_ok=True)
    host = work / "input-data"
    host.mkdir(exist_ok=True)
    empty = host / "empty"
    empty.mkdir(exist_ok=True)
    payloads = {
        "安装 ReliefWin.lnk": b"setup-shortcut",
        "使用说明.lnk": b"readme-shortcut",
        "ReliefWin 资源管理器.lnk": b"explorer-shortcut",
        "文件-α-📝.txt": b"optional-unicode-file" * 110000,
        "boot.txt": b"boot-wins-over-live",
        "live.txt": b"live-original",
    }
    for name, content in payloads.items():
        (host / name).write_bytes(content)
    macros = source("sdk/cmake/CMakeMacros.cmake", args.baseline)
    match = re.search(r"^function\(create_iso_lists\).*?^endfunction\(\)", macros, re.M | re.S)
    assert match, "actual create_iso_lists"
    implementation = work / "create_iso_lists.cmake"
    implementation.write_text(match.group() + "\n")
    optional = work / "optional_files.cmake"
    optional.write_text(source("sdk/cmake/optional_files.cmake", args.baseline))
    image_source = source("image/images.cmake", args.baseline)
    readers = []
    for expression, variable in (
        (r'file\(STRINGS "\$\{_livecd_extra_file_list\}" _livecd_extra_entries[^)]*\)', "_livecd_extra_entries"),
        (r'file\(STRINGS "\$\{_preinstall_extra_file_list\}" _preinstall_extra_entries[^)]*\)', "_preinstall_extra_entries"),
        (r'file\(STRINGS "\$\{_wine_mono_support_dir\}/monosupport.lst" _wine_mono_support_entries[^)]*\)', "_wine_mono_support_entries"),
    ):
        reader = re.search(expression, image_source)
        assert reader, "actual overlay or support manifest reader"
        readers.append(reader.group() + f'\nstring(REPLACE ";" "\\n" _overlay "${{{variable}}}")\n'
                       f'file(WRITE "${{CMAKE_BINARY_DIR}}/{variable}.lst" "${{_overlay}}\\n")\n')
    project = work / "CMakeLists.txt"
    project.write_text(f'''cmake_minimum_required(VERSION 3.17)
project(ReliefWinUnicodeIsoLists NONE)
set(REACTOS_BINARY_DIR "${{CMAKE_BINARY_DIR}}")
function(create_txtsetup_sif)
endfunction()
include("{cmake_path(implementation)}")
file(MAKE_DIRECTORY "${{CMAKE_BINARY_DIR}}/boot")
foreach(image bootcd livecd preinstall)
    file(WRITE "${{CMAKE_BINARY_DIR}}/boot/${{image}}.cmake.lst" "{cmake_path(empty)}\\n")
endforeach()
set_property(GLOBAL PROPERTY BOOTCD_FILE_LIST "重复项.txt={cmake_path(host)}/boot.txt")
set_property(GLOBAL PROPERTY LIVECD_FILE_LIST
    "Users/Public/Desktop/安装 ReliefWin.lnk={cmake_path(host)}/安装 ReliefWin.lnk"
    "Users/Public/Desktop/使用说明.lnk={cmake_path(host)}/使用说明.lnk"
    "重复项.txt={cmake_path(host)}/live.txt")
set_property(GLOBAL PROPERTY LIVECD_OVERLAY_FILE_LIST
    "ProgramData/Microsoft/Windows/Start Menu/Programs/ReliefWin 资源管理器.lnk={cmake_path(host)}/ReliefWin 资源管理器.lnk")
set_property(GLOBAL PROPERTY LIVECD_OPTIONAL_FILE_LIST
    "Program Files/中文/文件-α-📝.txt={cmake_path(host)}/文件-α-📝.txt"
    "中文目的地.txt={cmake_path(host)}/boot.txt"
    "source-only.txt={cmake_path(host)}/文件-α-📝.txt"
    "missing/不存在.txt={cmake_path(host)}/不存在.txt")
set_property(GLOBAL PROPERTY PREINSTALL_FILE_LIST
    "reactos/中文.txt={cmake_path(host)}/使用说明.lnk")
create_iso_lists()
set(_livecd_extra_file_list "${{CMAKE_BINARY_DIR}}/boot/livecd.cmake.lst")
set(_preinstall_extra_file_list "${{_livecd_extra_file_list}}")
set(_wine_mono_support_dir "${{CMAKE_BINARY_DIR}}")
configure_file("${{_livecd_extra_file_list}}" "${{CMAKE_BINARY_DIR}}/monosupport.lst" COPYONLY)
{''.join(readers)}
''')
    subprocess.run(["cmake", "-S", str(work), "-B", str(work / "generated"),
                    "-DCMAKE_BUILD_TYPE=Debug"], check=True, capture_output=True)
    build = work / "generated"
    effective = build / "boot/bootcd.effective.lst"
    optional_command = ["cmake", "-DMODE=list", f"-DBASE={build}/boot/bootcd.Debug.lst",
                        f"-DOPTIONAL={build}/boot/bootcd.optional.Debug.lst", f"-DOUTPUT={effective}",
                        "-DSYSTEM_DIR=Windows", f"-DINVENTORY={build}/boot/bootcd.files",
                        "-P", str(optional)]
    subprocess.run(optional_command, check=True, capture_output=True)
    result = subprocess.run([str(mkisofs), "-quiet", "-o", "-", "-iso-level", "4",
                             "-relaxed-filenames", "-duplicates-once", "-no-cache-inodes",
                             "-graft-points", "-path-list", str(effective)],
                            cwd=work, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode:
        raise AssertionError("actual mkisofs rejected generated manifest:\n" + result.stderr.decode(errors="replace"))
    expected = {
        "Users/Public/Desktop/安装 ReliefWin.lnk": payloads["安装 ReliefWin.lnk"],
        "Users/Public/Desktop/使用说明.lnk": payloads["使用说明.lnk"],
        "ProgramData/Microsoft/Windows/Start Menu/Programs/ReliefWin 资源管理器.lnk": payloads["ReliefWin 资源管理器.lnk"],
        "Program Files/中文/文件-α-📝.txt": payloads["文件-α-📝.txt"],
        "重复项.txt": payloads["boot.txt"],
        "中文目的地.txt": payloads["boot.txt"],
        "source-only.txt": payloads["文件-α-📝.txt"],
    }
    entries = dict(line.split("=", 1) for line in effective.read_text().splitlines() if "=" in line)
    for name in expected:
        assert name in entries, ("effective destination preserved", name)
    assert not any("missing/" in name for name in entries), "missing optional source remains excluded"
    for variable in ("_livecd_extra_entries", "_preinstall_extra_entries", "_wine_mono_support_entries"):
        overlay = (build / (variable + ".lst")).read_text()
        assert overlay == (build / "boot/livecd.cmake.lst").read_text(), variable + " reader preserves every UTF-8 entry"
    trees = iso_files(result.stdout)
    assert any(all(tree.get(name.encode()) == content for name, content in expected.items()) for tree in trees), "actual ISO paths and payload bytes preserved"
    inventory = (build / "boot/bootcd.files").read_text()
    for name in expected:
        assert name.replace("/", "\\") in inventory, ("servicing inventory preserves destination", name)
    stable_files = [effective, build / "boot/bootcd.files"]
    stable = [(path.read_bytes(), path.stat().st_mtime_ns) for path in stable_files]
    subprocess.run(optional_command, check=True, capture_output=True)
    assert stable == [(path.read_bytes(), path.stat().st_mtime_ns) for path in stable_files], "unchanged manifest and inventory retain timestamps"

    for script in ("image/bcd/medialist.cmake", "sdk/cmake/pack_rosprofiler_symbols.cmake",
                   "sdk/cmake/format_ram_partition.cmake"):
        (work / Path(script).name).write_text(source(script, args.baseline))
    media_list = work / "media.lst"
    media_list.write_bytes(effective.read_bytes())
    subprocess.run(["cmake", f"-DLIST={media_list}", "-DEFISYS=fixture-efi.bin", "-P",
                    str(work / "medialist.cmake")], check=True, capture_output=True)
    media_entries = dict(line.split("=", 1) for line in media_list.read_text().splitlines() if "=" in line)
    assert all(media_entries.get(name) == entries[name] for name in expected), "media repack retains UTF-8 paths"
    subprocess.run(["cmake", f"-DOUTPUT_DIR={work}/symbols", f"-DPREINSTALL_LIST={effective}",
                    "-DIMAGE_SIZE_MB=64", "-DRESERVE_MB=4", "-DMAX_SYMBOL_MB=4", "-DPACKAGE_PDBS=OFF",
                    "-P", str(work / "pack_rosprofiler_symbols.cmake")], check=True, capture_output=True)
    symbol_manifest = work / "symbols/rosprofiler-symbols.txt"
    actual_size = re.search(r"^estimated_base_payload_bytes=(\d+)$", symbol_manifest.read_text(), re.M)
    expected_size = sum(((Path(path).stat().st_size + 4095) // 4096) * 4096
                        for path in entries.values() if Path(path).is_file())
    assert actual_size and int(actual_size.group(1)) == expected_size, "symbol budget counts UTF-8 source payloads"

    formatter = work / "format-request.py"
    formatter.write_text('''#!/usr/bin/env python3
import json
from pathlib import Path
import sys
Path(__file__).with_name("format-request.json").write_text(json.dumps(sys.argv[1:]))
''')
    formatter.chmod(0o755)
    subprocess.run(["cmake", f"-DNTFSIMG={formatter}", f"-DLIST={effective}", "-DPARTITION=not-created.img",
                    "-DHIDDEN_SECTORS=2048", "-DSECTORS_PER_CLUSTER=8", "-DFREE_MB=0", "-DLABEL=ReliefWin",
                    "-P", str(work / "format_ram_partition.cmake")], check=True, capture_output=True)
    request = json.loads((work / "format-request.json").read_text())
    payload_size = expected_size + len(entries) * 1024
    directories = {str(Path(name).parent) if "/" in name else "" for name in entries}
    expected_sectors = ((payload_size + len(directories) * 8192) // 1048576 + 64) * 2048
    assert int(request[2]) == expected_sectors, "RAM partition sizing includes UTF-8 sources and destinations"
    print("ISO UTF-8 manifests: actual create_iso_lists, optional filtering, overlay reader, destination override,")
    print("servicing inventory and in-memory mkisofs ISO readback passed; 7 exact paths and payloads preserved")
    print("Media repack, symbol budget, RAM partition size request and unchanged-write stability passed")


if __name__ == "__main__":
    main()
