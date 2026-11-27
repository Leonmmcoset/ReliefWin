# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise dump preparation after the storage adapter enters D3
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--source", type=pathlib.Path)
args = parser.parse_args()
source = args.source or pathlib.Path(__file__).resolve().parents[1] / "pdo.c"
match = re.search(r"static NTSTATUS NTAPI PortDumpPrepare\([^\n]+\)\n\{.*?\n\}", source.read_text(), re.S)
if not match:
    raise RuntimeError("PortDumpPrepare not found")

fixture = r"""
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int32_t NTSTATUS;
typedef void *PVOID;
#define NTAPI
#define _In_
#define TRUE 1
#define STATUS_SUCCESS ((NTSTATUS)0)
#define STATUS_IO_DEVICE_ERROR ((NTSTATUS)0xc0000185)
#define KeMemoryBarrier() ((void)0)
enum {PowerDeviceUnspecified, PowerDeviceD0, PowerDeviceD1, PowerDeviceD2, PowerDeviceD3};
enum {ScsiQuerySupportedControlTypes, ScsiStopAdapter, ScsiRestartAdapter};
enum {ScsiAdapterControlSuccess, ScsiAdapterControlUnsuccessful};
typedef struct {unsigned char HwDeviceExtension;} MINI_EXTENSION;
typedef struct {int (*HwResetBus)(PVOID, unsigned);} INIT;
typedef struct {INIT *InitData; MINI_EXTENSION *MiniportExtension;} MINI;
typedef struct {int DumpMode, DevicePowerState; MINI Miniport;} FDO, *PFDO_DEVICE_EXTENSION;
typedef struct {FDO *FdoExtension; unsigned Bus;} PDO;
typedef struct {PDO *PdoExtension;} CONTEXT, *PPORT_DUMP_CONTEXT;
static unsigned RestartCalls, ResetCalls, Failures, Checks;
static int EngineReady, RestartFails, ResetFails;
static int MiniportAdapterControl(MINI *Mini, int Operation, PVOID Parameters)
{
    assert(Mini && Operation == ScsiRestartAdapter && !Parameters);
    ++RestartCalls;
    if (RestartFails) return ScsiAdapterControlUnsuccessful;
    EngineReady = 1;
    return ScsiAdapterControlSuccess;
}
static int Reset(PVOID Hardware, unsigned Bus)
{
    assert(Hardware && Bus == 2);
    ++ResetCalls;
    return EngineReady && !ResetFails;
}
static void Check(int Condition, const char *Name)
{
    ++Checks;
    if (!Condition) {++Failures; printf("FAIL: %s\n", Name);}
}
"""

harness = r"""
int main(void)
{
    MINI_EXTENSION Hardware = {0};
    INIT Init = {Reset};
    FDO Fdo = {0, PowerDeviceD0, {&Init, &Hardware}};
    PDO Pdo = {&Fdo, 2};
    CONTEXT Context = {&Pdo};
    EngineReady = 1;
    Check(PortDumpPrepare(&Context) == STATUS_SUCCESS, "running adapter prepares");
    Check(RestartCalls == 0 && ResetCalls == 1 && Fdo.DumpMode, "running adapter keeps its engine");
    Fdo.DevicePowerState = PowerDeviceD3;
    EngineReady = 0;
    Check(PortDumpPrepare(&Context) == STATUS_SUCCESS, "stopped adapter prepares");
    Check(RestartCalls == 1 && ResetCalls == 2 && EngineReady, "restart precedes reset");
    RestartFails = 1;
    EngineReady = 0;
    Check(PortDumpPrepare(&Context) == STATUS_IO_DEVICE_ERROR, "restart failure propagates");
    Check(RestartCalls == 2 && ResetCalls == 2 && !EngineReady, "failed restart skips reset");
    RestartFails = 0;
    ResetFails = 1;
    Check(PortDumpPrepare(&Context) == STATUS_IO_DEVICE_ERROR, "reset failure propagates");
    Check(RestartCalls == 3 && ResetCalls == 3, "reset failure follows successful restart");
    Init.HwResetBus = NULL;
    Check(PortDumpPrepare(&Context) == STATUS_IO_DEVICE_ERROR, "missing reset callback rejected");
    printf("%u dump preparation checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
"""

program = pathlib.Path("/tmp/reliefwin-dump-prepare-test.c")
binary = pathlib.Path("/tmp/reliefwin-dump-prepare-test")
program.write_text(fixture + match.group(0) + harness)
subprocess.run(["clang", "-std=c11", "-Wall", "-Wextra", "-Werror", "-Wno-unused-function", str(program), "-o", str(binary)], check=True)
raise SystemExit(subprocess.run([str(binary)]).returncode)
