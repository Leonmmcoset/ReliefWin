# Tab page frame layout validation

Validated on 2026-11-26.

## Cause and scope

The page dialogs used fixed positions instead of the tab control's display
rectangle. Their opaque backgrounds could paint over the frame below the tabs.
The unfixed Task Manager pages began at client position `(15,30)` while the
display rectangle began at `(9,32)`, covering two rows of the frame. Remote
Desktop Connection placed its pages at y=128 while its display area began at
y=132. System Configuration and ReactX Diagnostic also overlapped the frame
when the tab height was increased to 48 pixels.

The source audit searched `base` for `SysTabControl32`, `WC_TABCONTROL`,
`TabCtrl_AdjustRect`, and `TCM_ADJUSTRECT`. The four applications below were the
custom tab page hosts found by that search. This is a scoped source audit and
runtime check of these applications, not an assertion that every program or
property sheet in the system has been tested.

| Application | Change |
| --- | --- |
| Task Manager (`taskmgr`) | Lay out all three pages inside the actual display rectangle on initial creation and resize. |
| System Configuration (`msconfig`) | Move the fixed-size pages to the display origin; enlarge the tab and outer dialog when needed and move the buttons below them. |
| Remote Desktop Connection (`mstsc`) | Preserve the fixed page dimensions and enlarge the enclosing dialog when necessary, keeping the bottom controls visible. |
| ReactX Diagnostic (`dxdiag`) | Fit the selected page to the display rectangle, mapping coordinates to that page's parent; static and adapter pages have different parents. |

The layout callbacks run on each application's UI thread. Tab notifications,
page visibility, and window resizing use that thread's existing message flow;
the changes introduce no worker, shared ownership, or deferred callback. Page
creation precedes layout, and existing dialog destruction owns page teardown.

The selected tab normally joins the page frame. That intentional opening is
preserved; the fix restores the frame alongside it.

## Contract evidence

The rectangle conversion and coordinate handling follow Microsoft's documented
control and window APIs:

- [TCM_ADJUSTRECT](https://learn.microsoft.com/en-us/windows/win32/controls/tcm-adjustrect)
  converts the tab's window rectangle to its page display rectangle.
- [MapWindowPoints](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mapwindowpoints)
  maps that rectangle into the page parent's coordinate space.
- [TCM_SETITEMSIZE](https://learn.microsoft.com/en-us/windows/win32/controls/tcm-setitemsize)
  supplies the taller-tab regression fixture.

These are documented layout corrections. No Windows binary implementation was
used. A Windows 11 runtime baseline is still **INFO_NEEDED**: no runnable
reference environment was available, so these results do not establish Windows
11 parity or completion of that repository validation requirement.

## Regression tests and results

`modules/rostests/apitests/appsmoke/tablayout.c` launches the actual application
specified on the command line. The task manager test checks all page bounds and
visibility through tab switches, enlargement, restoration, maximization, and
tab height changes. The generic test checks every selected page against its
display rectangle, checks visible direct child controls for clipping, and
restores the tab metrics. `TCM_ADJUSTRECT` uses a rectangle allocated in the
application process because this message is not automatically marshalled
between processes. Cleanup closes the process created by the test.

Run in the guest with the four application binaries and test executable on F:

```bat
F:\appsmoke_apitest.exe taskmgr_layout F:\taskmgr.exe
F:\appsmoke_apitest.exe tab_layout F:\msconfig.exe
F:\appsmoke_apitest.exe tab_layout F:\dxdiag.exe
F:\appsmoke_apitest.exe tab_layout F:\mstsc.exe
```

Final amd64 guest results, using the executable hashes below:

| Test | Checks | Failures | Skipped |
| --- | ---: | ---: | ---: |
| `taskmgr_layout` | 339 | 0 | 0 |
| `tab_layout` / msconfig | 223 | 0 | 0 |
| `tab_layout` / dxdiag | 829 | 0 | 0 |
| `tab_layout` / mstsc | 223 | 0 | 0 |
| Total | 1614 | 0 | 0 |

The unfixed task manager failed 63 of the same 339 checks. The first generic
test revision caught frame overlap in the unfixed msconfig (4/115 checks),
dxdiag (6/163), and mstsc (6/67). Those smaller totals predate the child-control
clipping checks and must not be compared directly with final totals.
The strengthened test also caught five clipped controls in msconfig and two
in mstsc in an intermediate shrink-to-fit attempt. That attempt was removed;
the final implementation preserves those page sizes and passes the checks.

All five targets built successfully in each existing Clang debug build:

```sh
CCACHE_DISABLE=1 ninja -C output-Clang-amd64-debug -j1 taskmgr msconfig dxdiag mstsc appsmoke_apitest
CCACHE_DISABLE=1 ninja -C output-Clang-i386-debug -j1 taskmgr msconfig dxdiag mstsc appsmoke_apitest
CCACHE_DISABLE=1 ninja -C output-Clang-arm64-debug -j1 taskmgr msconfig dxdiag mstsc appsmoke_apitest
```

Only amd64 was run. i386 and arm64 runtime behavior, physical hardware, other
languages, and other DPI settings were not validated.

## Runtime environment and artifacts

The existing `output-Clang-amd64-debug/ReliefWin.img` was booted with QEMU TCG,
two vCPUs, 2048 MiB RAM, standard VGA at 1280x900, OVMF, and no network adapter.
The tests ran interactively as `ReliefUser`, not as SYSTEM. The fixed payload was
`/tmp/reliefwin-branding-fat.img`, attached as F:. The single working test log
was `/tmp/reliefwin-taskmgr.log`, overwritten between phases. Detailed captured
results and hashes are retained in `/tmp/reliefwin-taskmgr-validation.json` for
this workspace session.

```sh
ROS_VM_MONITOR=/tmp/reliefwin-vm-monitor.py ROS_QEMU_DISPLAY=none \
python3 scripts/vm_monitor.py \
  --img output-Clang-amd64-debug/ReliefWin.img \
  --apps /tmp/reliefwin-branding-fat.img \
  --timeout 30 --stall 6 --smp 2 --memory 2048 \
  --keep --continue-after-observation
```

The temporary monitor was needed because the external monitor normally used
by the repository wrapper was absent. The VM was shut down before any image
write. The final four application executables were also installed offline in
the image's `Windows\system32` and verified against these SHA-256 hashes. A
subsequent boot and Ctrl+Shift+Esc launch verified the installed Task Manager
and supplied its final dark-theme screenshot. The VM was then shut down.

| Final amd64 binary | SHA-256 |
| --- | --- |
| taskmgr.exe | `2c654b06ca31b7af039f12805f316b512a56be969994f52a972c2bad7be9f6f3` |
| msconfig.exe | `a54511fbe9e1b5eca99f0fc77e5e448952df845a31a191c3520596806a78075d` |
| dxdiag.exe | `f7b451a2de0faed013305585c3cf118f71823291f31b89a4c7091a91d9c78fb0` |
| mstsc.exe | `60006e8be5f56246acf7b25fa287071465875bcf2e5aae899c478bf8e67e5804` |
| appsmoke_apitest.exe | `d67b8ed9b74e4f9f50244360fbf82d463843adbabadf86725f3fef2197c622a8` |

`git diff --check` passed for the affected files. cppcheck 2.7 was attempted
using the build compilation database, includes, and target defines, with a
separate file filter for each changed C file. Existing generated `wtypes.h`,
`shellapi.h`, and CRT macro parsing errors prevented complete application
analysis. Preprocessing with the actual Clang command still left enum parsing
errors in the four applications; the final test source parsed without findings.
No header changes or suppressions were introduced. This is an incomplete
cppcheck result, not a clean static-analysis claim.

## Screenshots

These are real guest captures. Each full screenshot is 1280x900. The comparison
only crops and places the images side by side; it does not retouch their pixels.
Task Manager uses the same dark theme in both captures. msconfig and dxdiag
explicitly use the 48-pixel tab-height fixture to expose the overlap; those
images do not represent an unmodified default tab height or a DPI test.

[Combined comparison: before on the left, after on the right](../graphics/screenshots/tab-layout-comparison.png)

| Program / fixture | Before | After |
| --- | --- | --- |
| Task Manager / default tabs | [Screenshot](../graphics/screenshots/taskmgr-tabs-before.png) | [Screenshot](../graphics/screenshots/taskmgr-tabs-after.png) |
| Remote Desktop Connection / default tabs | [Screenshot](../graphics/screenshots/mstsc-tabs-before.png) | [Screenshot](../graphics/screenshots/mstsc-tabs-after.png) |
| System Configuration / 48 px tabs | [Screenshot](../graphics/screenshots/msconfig-tabs-before.png) | [Screenshot](../graphics/screenshots/msconfig-tabs-after.png) |
| ReactX Diagnostic / 48 px tabs | [Screenshot](../graphics/screenshots/dxdiag-tabs-before.png) | [Screenshot](../graphics/screenshots/dxdiag-tabs-after.png) |
