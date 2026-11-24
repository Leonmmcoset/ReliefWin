# Printer ANSI conversion host regression

Run `python3 scripts/test_printer_ansi.py` from the repository root. The test extracts the actual shared string converter, DEVMODE converter/wrapper, and public ANSI printer, job, form and driver entry points. It uses the public SDK structures, Windows scalar widths and `offsetof`, with deterministic heap and Unicode spooler mocks. Linux `iconv` supplies CP936/UTF-8 conversion for the explicitly tested vectors; the expected Chinese CP936 bytes are also independently specified.

The cases cover ASCII, empty and null strings, single and repeated Chinese characters, varying lengths, multiple printer records, all 37 printer string call sites, allocation/query/conversion failures, unchanged buffer canaries, and UTF-8 results that fit or exceed the original UTF-16 slot. DEVMODE cases cover invalid existing size validation, allocation and both fixed-name conversion failures, unchanged output on failure, metadata, driver-private bytes, separate and identical input/output buffers, and error propagation through every affected public caller.

FORM_INFO_2 cases extract the actual local provider packing helper, its offset table and PackStrings. They verify unchanged ANSI keyword bytes, null/empty and odd/even byte lengths, WCHAR alignment, multiple records, all three wide fields and client Level 2 error propagation. The host fixture aliases FORM_INFO_LIST to its embedded public FORM_INFO_2W fields consumed by that helper; this is not a claim about the private provider list layout. Actual AddFormA/SetFormA wrappers also verify that the original keyword pointer reaches W unchanged, with the returned provider packet exercised through EnumFormsA/GetFormA. The submission mock owns copies of input strings for the readback; provider input ownership is outside this test.

`--cc clang --sanitize` runs AddressSanitizer/UndefinedBehaviorSanitizer. `--baseline REF` selects historical public entry-point source while retaining the current shared-converter fixtures to isolate the old inline conversion and error-propagation defects. The `--mutation` options remove individual safety checks or restore the original provider/client keyword code from baseline commit a676708adf7c492ecd32e35c2b35ec1d3edb0a28; those runs must fail. `unsafe_devmode_null` intentionally demonstrates the original null dereference with sanitizers; core dumps are disabled.

Public contracts used:

- [WideCharToMultiByte](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte): byte capacity, sizing query, terminating NUL and conversion errors.
- [EnumPrinters](https://learn.microsoft.com/en-us/windows/win32/printdocs/enumprinters) and [GetPrinter](https://learn.microsoft.com/en-us/windows/win32/printdocs/getprinter): pointer-containing output buffers and byte counts.
- Microsoft public SDK [winspool.h](https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/um/winspool.h) and [wingdi.h](https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/um/wingdi.h): public printer and DEVMODE fields.
- [FORM_INFO_2](https://learn.microsoft.com/en-us/windows/win32/printdocs/form-info-2): pKeyword remains LPCSTR in both ANSI and Unicode structures.

`INFO_NEEDED`: a complete ANSI packing path when a conversion exceeds the existing UTF-16 slot, including Windows 11 `pcbNeeded` behavior. This change preserves the current W layout/size path and returns an error before overwriting such a slot. The test does not establish full support for every system ANSI code page.

`INFO_NEEDED`: Windows 11 behavior for non-terminated or multibyte-truncated fixed DEVMODE names. The change checks conversion failure and preserves the original error; it introduces no truncation policy.

These are Linux host tests. They do not run the actual RPC spooler, establish Windows 11 parity, or replace a target OS boot/API test. Existing `winspool_apitest` still needs a Windows 11 baseline and a run on the target OS with a configured printer.
