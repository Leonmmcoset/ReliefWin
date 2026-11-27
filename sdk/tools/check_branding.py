#!/usr/bin/env python3
# PROJECT:     ReliefWin branding checks
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Check product names, defaults and intentional upstream attribution
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
import collections
import io
import json
from pathlib import Path
import re
import subprocess
import sys
import tarfile

ROOT = Path(__file__).resolve().parents[2]
OLD = re.compile(r"libernt|liberos|liber[ -](?:nt|os)|lbrn(?:t)?|реактос", re.IGNORECASE)
ATTRIBUTION = re.compile(r"copyright|\(c\)|版权所有|حقوق النشر|©|\\u00a9|autoriõigus|hak cipta|telif hakkı|कॉपीराइट|זכויות יוצרים|drept de autor", re.IGNORECASE)
DEPENDENCY = re.compile(
    r"https://(?:github\.com/LiberNT/(?:libernt-build-toolchain|libernt-mesa|FEX)(?:[^\s\"<>)]*)|"
    r"raw\.githubusercontent\.com/LiberNT/libernt-build-toolchain/[^\s\"<>)]*|"
    r"github\.com/LiberNT/LiberNT/releases/download/[^\s\"<>)]*)|"
    r"(?<=reliefwin_download_payload\()LiberNT/LiberNT|"
    r"libernt-(?:ffmpeg|icu|mesa-llvmpipe)[\w.<>${}\-]*",
    re.IGNORECASE,
)
PROVENANCE = {
    "CREDITS.md", "TRADEMARK.md", "README.md", "image/data/readme.txt",
    "media/doc/UPSTREAM_TRADEMARK.md", "media/doc/RELIEFWIN_VALIDATION.md",
}
EVIDENCE = {"sdk/tools/check_branding.py", "sdk/tools/check_built_branding.py", "scripts/check_brand_assets.py", "media/graphics/branding/obsolete-assets.json", "media/doc/UPSTREAM_IDENTIFIERS.json", "submodules/host-tests/branding/audit-test.py"}
BINARY_SUFFIXES = {".png", ".jpg", ".jpeg", ".ico", ".bmp", ".rgba", ".wav", ".ttf", ".otf", ".exe", ".dll", ".sys", ".pdf", ".zip", ".bin", ".img"}
LITERAL_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".c++", ".h", ".hh", ".hpp", ".hxx", ".h++", ".inc", ".inl", ".ipp", ".tcc", ".tpp", ".rc", ".rc2"}
LITERAL_TOKENS = re.compile(
    r'//[^\n]*|/\*.*?\*/|(?P<raw>(?:u8|u|U|L)?R"(?P<delimiter>[^\s()\\]{0,16})\(.*?\)(?P=delimiter)")|'
    r'(?P<quoted>"(?:\\.|[^"\\])*")|\'(?:\\.|[^\'\\])*\'', re.DOTALL,
)


def source_files(ref):
    if ref:
        process = subprocess.Popen(["git", "archive", "--format=tar", ref], cwd=ROOT, stdout=subprocess.PIPE)
        with tarfile.open(fileobj=process.stdout, mode="r|") as archive:
            for member in archive:
                if member.isfile():
                    stream = archive.extractfile(member)
                    yield member.name, stream.read()
        if process.wait():
            raise RuntimeError("git archive failed")
    else:
        names = subprocess.check_output(["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"], cwd=ROOT)
        for name in sorted(set(names.decode().split("\0")) - {""}):
            path = ROOT / name
            if path.is_file():
                yield name, path.read_bytes()


def decode(data):
    if data.startswith((b"\xff\xfe", b"\xfe\xff")):
        return data.decode("utf-16")
    if b"\0" in data:
        return None
    return data.decode("utf-8-sig", errors="replace")


def classify(name, line):
    if name == "drivers/base/kdnet/kdsession.c" and re.search(r'"LiberNT KDNET (?:1|v1 (?:key|hello|rendezvous))"', line):
        return "KDNET wire protocol compatibility"
    if name == "modules/rostests/kmtests/ksecdd/KsecBcryptKeyPair.c" and 'SHA-256("LiberNT ECDH A"/"... B")' in line:
        return "original cryptographic fixture seed"
    if name in EVIDENCE:
        return "audit pattern"
    if re.search(r'copyright assignment|contributor licen[cs]e agreement', line, re.IGNORECASE):
        return None
    if name.startswith("translations/") and name.endswith("/README.md") and "[CREDITS.md]" in line:
        return "upstream attribution and policy"
    if name in PROVENANCE:
        return "upstream attribution and policy"
    if ATTRIBUTION.search(line):
        return "original copyright"
    if not OLD.search(DEPENDENCY.sub("", line)):
        return "upstream build dependency"
    return None


def check_defaults(files, errors):
    def require(name, expression, description):
        if name not in files or not re.search(expression, files[name], re.MULTILINE | re.IGNORECASE):
            errors.append({"path": name, "check": description})

    require("sdk/include/reliefwin/version.rc", r'REACTOS_DEFAULT_STR_PRODUCT_NAME\s+"ReliefWin Operating System"', "PE product name")
    require("sdk/include/reliefwin/version.rc", r'REACTOS_DEFAULT_STR_FILE_DESCRIPTION\s+"ReliefWin Core Component"', "default PE file description")
    require("image/data/hivesft.inf", r'"ProductName",2,"ReliefWin"', "installed product name")
    require("image/data/hivesys.inf", r'ComputerName\\ComputerName","ComputerName",0x00000002,"RELIEFWIN"', "default computer name")
    for value in ("Hostname", "NV Hostname"):
        require("media/inf/nettcpip.inf", r'"Parameters","' + value + r'",0x00000000,"RELIEFWIN"', value + " network default")
    for target in ("Header", "Rsdp"):
        require("bootloader/uefi/fdtacpi.c", r'RtlCopyMemory\(' + target + r'->OEMID,\s*"RWIN  ",\s*sizeof\(' + target + r'->OEMID\)\)', target + " fixed-width ACPI OEM ID")
    require("bootloader/uefi/fdtacpi.c", r'RtlCopyMemory\(Header->CreatorID,\s*"RWIN",\s*sizeof\(Header->CreatorID\)\)', "fixed-width ACPI creator ID")
    require("hal/arch/common/acpi/tables.c", r'HaliGetCachedAcpiTable\(FADT_SIGNATURE,\s*"RWIN",\s*"DTPLATFM"\)', "synthetic ACPI OEM consumer")
    require("image/bcd/bcd.inf.in", r'"Element",,"ReliefWin Boot Manager"', "boot manager name")
    for mode in ("bootcd", "livecd", "preinstall"):
        require("image/data/" + mode + "/unattend.inf", r'^LocaleID\s*=\s*804\s*$', mode + " default language")
    require("image/data/txtsetup.sif", r'^DefaultLanguage\s*=\s*00000804\s*$', "loader NLS language")
    for value in ("ACP", "OEMCP"):
        require("image/data/hivesys.inf", r'"' + value + r'",0x00000000,"936"', value + " code page")
    for value in ("Default", "InstallLanguage"):
        require("image/data/hivesys.inf", r'Nls\\Language","' + value + r'",0x00000000,"0804"', value + " language")
    require("image/data/preinstall.inf", r'International","Locale",0x00000000,"00000804"', "preinstalled user locale override")
    require("base/setup/reliefwin/lntupdate.inf", r'"Enabled",0x00010003,0', "automatic updates disabled")
    require("base/setup/reliefwin/lntupdate.inf", r'"Server",0x00000002,""', "no inherited update server")
    require("base/applications/systemsettings/page_misc.cpp", r'ReadDword\(HKEY_LOCAL_MACHINE, UPDATE_KEY, L"Enabled", 0\)', "settings update toggle disabled fallback")
    require("sdk/include/psdk/ntverp.h", r'VER_COMPANYNAME_STR\s+\\\s*"ReliefWin Project"', "SDK default PE company name")
    agent = files.get("base/setup/reliefwin/updagent.c", "")
    if 'ReadSetting(L"Enabled", 0)' not in agent or "DEFAULT_SERVER" in agent:
        errors.append({"path": "base/setup/reliefwin/updagent.c", "check": "disabled and unconfigured update fallback"})
    require("base/shell/explorer/explorer.rc", r'IDS_HELP_COMMAND "notepad.exe>', "local help command")
    require("image/data/CMakeLists.txt", r'add_livecd_shortcut\("ReliefWin Package Manager"', "package manager shortcut title")
    require("image/data/CMakeLists.txt", r'add_livecd_shortcut\("安装 ReliefWin"', "Simplified Chinese installer shortcut")
    require("image/data/CMakeLists.txt", r'add_livecd_shortcut\("使用说明"', "Simplified Chinese readme shortcut")
    require("image/data/CMakeLists.txt", r'add_livecd_shortcut\("完成设置"', "Simplified Chinese finish setup shortcut")
    require("image/data/CMakeLists.txt", r'add_livecd_shortcut\("命令提示符"', "Simplified Chinese command prompt shortcut")
    english_start = set(re.findall(r'^\s*(IDS_SM2_\w+)\s+"', files.get("base/shell/explorer/lang/en-US.rc", ""), re.MULTILINE))
    chinese_start = set(re.findall(r'^\s*(IDS_SM2_\w+)\s+"', files.get("base/shell/explorer/lang/zh-CN.rc", ""), re.MULTILINE))
    if not english_start or english_start - chinese_start:
        errors.append({"path": "base/shell/explorer/lang/zh-CN.rc", "check": "complete Simplified Chinese modern Start menu strings"})
    require("base/shell/explorer/lang/zh-CN.rc", r'IDS_SM2_SEARCH\s+"搜索程序和文件"', "Simplified Chinese Start menu search")
    for mode in ("preinstall", "livecd"):
        require("sdk/cmake/CMakeMacros.cmake", r'utf16le_convert\(\$\{CMAKE_SOURCE_DIR\}/image/data/' + mode + r'\.inf \$\{CMAKE_BINARY_DIR\}/image/data/' + mode + r'_utf16\.inf\)', mode + " Unicode hive overlay conversion")
        require("sdk/cmake/CMakeMacros.cmake", r'list\(APPEND _' + mode + r'_inf_files\s+\$\{_registry_inf\}\s+\$\{CMAKE_BINARY_DIR\}/image/data/' + mode + r'_utf16\.inf', mode + " converted Unicode hive overlay input")
        require("image/data/" + mode + ".inf", r'"RecentlyAdded"[^\n]*\\ReliefWin Package Manager\.lnk"', mode + " package manager recent shortcut")
        require("image/data/" + mode + ".inf", r'"RecentlyAdded"[^\n]*\\命令提示符\.lnk"', mode + " Simplified Chinese command prompt recent shortcut")
    for field in ("FILE_DESCRIPTION", "PRODUCT_NAME"):
        require("base/applications/cmdutils/lntget/lntget.rc", r'REACTOS_STR_' + field + r'\s+"ReliefWin Package Manager(?: \(GPLv3\+\))?"', "package manager " + field.lower())
    require("base/applications/cmdutils/lntget/lntget.exe.manifest", r'<description>ReliefWin Package Manager</description>', "package manager manifest description")
    package_gui = "base/applications/cmdutils/lntget/gui.cpp"
    for expression, description in (
        (r'const std::wstring text = L"ReliefWin Package Manager "', "about text"),
        (r'params\.lpszCaption = L"About ReliefWin Package Manager"', "about title"),
        (r'state\.appTitle = CreateWindowExW\(0, L"STATIC", L"ReliefWin Package Manager"', "page title"),
        (r'std::wstring caption = L"ReliefWin Package Manager — winget catalog \("', "dynamic window title"),
        (r'HWND window = CreateWindowExW\(0, L"LntGetCatalog", L"ReliefWin Package Manager — winget catalog"', "initial window title"),
        (r'L"ReliefWin Package Manager compatibility notice"', "compatibility notice title"),
    ):
        require(package_gui, expression, "package manager " + description)


def check_upstream_identifiers(files, errors):
    name = "media/doc/UPSTREAM_IDENTIFIERS.json"
    if name not in files:
        errors.append({"path": name, "check": "missing upstream identifier audit"})
        return
    report = json.loads(files[name])
    expected = collections.Counter((item["path"], item["literal"]) for item in report["literals"])
    for item in report["literals"]:
        if not item.get("category") or not item.get("reason"):
            errors.append({"path": name, "check": "unexplained upstream identifier"})
    actual = collections.Counter()
    for path, content in files.items():
        if Path(path).suffix.lower() not in LITERAL_SUFFIXES:
            continue
        for match in LITERAL_TOKENS.finditer(content):
            literal = match.group()
            if (match.group('quoted') or match.group('raw')) and "reactos" in literal.lower():
                actual[path, literal] += 1
    for path, literal in actual - expected:
        errors.append({"path": path, "check": "unaudited upstream product literal: " + literal})
    for path, literal in expected - actual:
        errors.append({"path": path, "check": "stale upstream identifier audit: " + literal})


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--ref", help="check an existing Git revision without altering the checkout")
    parser.add_argument("--report", type=Path, help="write the complete findings as JSON")
    args = parser.parse_args()
    findings, errors, files = [], [], {}
    scanned = 0
    for name, data in source_files(args.ref):
        scanned += 1
        if OLD.search(name):
            errors.append({"path": name, "check": "obsolete project filename"})
        if Path(name).suffix.lower() in BINARY_SUFFIXES:
            continue
        try:
            text = decode(data)
        except UnicodeError:
            continue
        if text is None:
            continue
        files[name] = text
        for number, line in enumerate(text.splitlines(), 1):
            if OLD.search(line):
                reason = classify(name, line)
                finding = {"path": name, "line": number, "text": line, "reason": reason}
                findings.append(finding)
                if reason is None:
                    errors.append(finding)
    check_defaults(files, errors)
    check_upstream_identifiers(files, errors)
    counts = collections.Counter(item["reason"] or "unapproved" for item in findings)
    report = {"revision": args.ref or "working tree", "scanned_files": scanned, "preserved": dict(counts), "errors": errors, "findings": findings}
    if args.report:
        args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n")
    print("Scanned", scanned, "files; preserved:", dict(counts))
    for error in errors[:50]:
        print(error["path"] + (":" + str(error["line"]) if "line" in error else "") + ": " + error.get("check", error.get("text", "")))
    print("FAIL" if errors else "PASS", str(len(errors)) + " unexpected remnants or failed defaults")
    return bool(errors)


if __name__ == "__main__":
    sys.exit(main())
