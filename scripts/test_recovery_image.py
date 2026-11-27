#!/usr/bin/env python3
"""
PROJECT:     ReliefWin Recovery tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Check generated recovery devices and disk identity updates
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import re
import struct
import subprocess
import tempfile

from Registry import Registry
from test_boot_brand import function


ROOT = Path(__file__).resolve().parents[1]
MANAGER = "{9dea862c-5cdd-4e70-acc1-f32b344d4795}"
OS = "{354fa6e5-5e45-49af-91bf-1fd2754e470a}"
RECOVERY = "{e24552c5-f0e2-5ec0-9f7d-52a05f389a30}"


def element(hive, identifier, name):
    return hive.open(f"Objects\\{identifier}\\Elements\\{name}").value("Element").value()


def http_tools():
    source = (ROOT / "image/bcd/CMakeLists.txt").read_text()
    script = source[:source.index("function(bootloader_bcd_store")]
    start = source.index("function(bootloader_http_boot_entry")
    script += source[start:source.index("endfunction()", start) + len("endfunction()")]
    script += '\nset(BOOTLOADER_HTTP_BOOT ON)\nset(BOOTLOADER_HTTP_BOOT_URL "http://127.0.0.1/boot.wim")\n'
    script += 'bootloader_http_boot_entry("")\nfile(WRITE "${CMAKE_CURRENT_LIST_DIR}/http.inf" "${BCD_HTTP_ENTRY}")\n'
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        (path / "tools.cmake").write_text(script)
        subprocess.run(["cmake", "-P", str(path / "tools.cmake")], check=True)
        tools = next(line for line in (path / "http.inf").read_text().splitlines() if "24000010" in line)
        assert RECOVERY in tools, tools
    print("Recovery remains available with HTTP boot enabled")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build", default="output-Clang-amd64-debug")
    args = parser.parse_args()
    build = ROOT / args.build
    http_tools()
    fixtures = []
    for name in ("standard", "standardpcat", "media", "media/pcat", "template", "template/pcat"):
        path = build / "bootloader/image" / name / "BCD"
        if not path.exists():
            continue
        hive = Registry.Registry(str(path))
        assert RECOVERY in element(hive, MANAGER, "24000010"), name
        assert element(hive, MANAGER, "23000003") != RECOVERY, name
        device = element(hive, RECOVERY, "21000001")
        loader = element(hive, RECOVERY, "11000001")
        expected = ("\\deployment" if name.startswith("media") else "") + "\\Recovery\\ReliefWin\\Winre.wim"
        if name.startswith("template"):
            expected = "\\Recovery\\ReliefWin\\Winre.wim"
        assert device.endswith(expected.encode("utf-16le") + b"\0\0"), name
        assert device[0x44:0x44 + 0x48] == loader[0x10:], name
        assert element(hive, RECOVERY, "26000022") == b"\x01", name
        fixtures.extend((loader, device))
        print(name, "recovery device OK")

    source = (ROOT / "base/applications/cmdutils/lntexpandstorage/lntexpandstorage.c").read_text()
    defines = "\n".join(re.findall(r"^#define BCD_.*$", source, re.M))
    harness = """
#include <stdint.h>
#include <string.h>
#include <assert.h>
typedef uint8_t BYTE;
typedef uint32_t DWORD, ULONG;
#define MAXDWORD UINT32_MAX
""" + defines + "\n" + function(source, "BcdSignatureOffset", "DWORD")
    harness += "int main(void) {\n"
    for index, data in enumerate(fixtures):
        values = ",".join(str(byte) for byte in data)
        harness += f"BYTE fixture{index}[] = {{{values}}};\n"
        harness += f"BYTE *data{index} = fixture{index};\n"
        harness += f"DWORD offset{index} = BcdSignatureOffset(data{index}, sizeof(fixture{index}));\n"
        base = 0x10 if index % 2 == 0 else 0x44
        expected = base + 0x28 if struct.unpack_from("<I", data, base)[0] == 6 and struct.unpack_from("<I", data, base + 0x24)[0] == 1 else 0xffffffff
        harness += f"assert(offset{index} == {expected}u);\n"
        harness += f"if (offset{index} != MAXDWORD) {{\n"
        harness += f"assert(offset{index} == {0x38 if index % 2 == 0 else 0x6c});\n"
        harness += f"memset(data{index} + offset{index}, 0x5a, sizeof(DWORD));\n"
        harness += f"assert(BcdSignatureOffset(data{index}, sizeof(fixture{index})) == offset{index});\n"
        harness += "}\n"
        harness += f"for (DWORD size = 0; size < {0x58 if index % 2 == 0 else 0x8c}; ++size) "
        harness += f"assert(BcdSignatureOffset(data{index}, size) == MAXDWORD);\n"
        if index % 2 == 0:
            harness += f"memset(data{index} + 0x34, 0, sizeof(DWORD));\n"
        else:
            harness += f"memset(data{index} + 0x3c, 0xff, sizeof(DWORD));\n"
        harness += f"assert(BcdSignatureOffset(data{index}, sizeof(fixture{index})) == MAXDWORD);\n"
    harness += "return 0; }\n"
    with tempfile.TemporaryDirectory() as directory:
        test = Path(directory) / "signature.c"
        binary = Path(directory) / "signature"
        test.write_text(harness)
        subprocess.run(["cc", "-std=c99", "-Wall", "-Werror", "-fsanitize=address,undefined",
                        str(test), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
    hive = Registry.Registry(str(build / "boot/bootdata/recovery/system"))
    assert hive.open("Setup").value("CmdLine").value() == "%SystemRoot%\\system32\\recoverycenter.exe /environment"
    print("Disk signature parser and recovery startup OK")


if __name__ == "__main__":
    main()
