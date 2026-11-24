#!/usr/bin/env python3
"""
PROJECT:     ReliefWin API tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Check PCI synchronous IRP completion and device reference ownership
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def function(source, name):
    start = source.index(name + "(")
    end = source.index("{", start) + 1
    depth = 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return "static NTSTATUS " + source[start:end] + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", metavar="REF")
    args = parser.parse_args()
    path = "drivers/bus/pcix/utils.c"
    source = (subprocess.check_output(["git", "show", args.baseline + ":" + path], cwd=ROOT).decode()
              if args.baseline else (ROOT / path).read_text())
    harness = r'''
/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Exercise PCI helpers against synchronous I/O ownership mocks
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int32_t NTSTATUS;
#include "ntstatus.h"
#define IN
#define OUT
#define NTAPI
#define PAGED_CODE() ((void)0)
#define RtlZeroMemory(p, n) memset((p), 0, (n))
#define FALSE 0
#define TRUE 1
#define SynchronizationEvent 1
#define Executive 0
#define KernelMode 0
#define IRP_MJ_PNP 0x1b
#define IRP_MN_QUERY_CAPABILITIES 0x09
typedef uint32_t ULONG;
typedef unsigned char BOOLEAN;
typedef void *PVOID;
typedef struct { unsigned References; } DEVICE_OBJECT, *PDEVICE_OBJECT;
typedef struct { int Signaled; } KEVENT;
typedef struct { NTSTATUS Status; uintptr_t Information; } IO_STATUS_BLOCK;
typedef struct {
    uint16_t Size, Version;
    ULONG Address, UINumber;
    unsigned char Extra[16];
} DEVICE_CAPABILITIES, *PDEVICE_CAPABILITIES;
typedef struct {
    unsigned char MajorFunction, MinorFunction;
    struct { struct { PDEVICE_CAPABILITIES Capabilities; } DeviceCapabilities; } Parameters;
} IO_STACK_LOCATION, *PIO_STACK_LOCATION;
typedef struct { IO_STATUS_BLOCK IoStatus; IO_STACK_LOCATION Stack; } IRP, *PIRP;
static DEVICE_OBJECT AttachedDevice;
static IRP Request;
static KEVENT *CompletionEvent;
static IO_STATUS_BLOCK *CompletionStatus;
static const char *CaseName;
static NTSTATUS DispatchStatus, FinalStatus;
static unsigned Checks, Failures, Calls, Waits, Completions, Dereferences;
static int FailBuild, CompleteEarly, CapabilityRequest, Released;
static unsigned char Input[4], Output[8];
#define EXPECT(x) do { ++Checks; if (!(x)) { ++Failures; fprintf(stderr, "%s: %s\n", CaseName, #x); } } while (0)
#define ASSERT(x) EXPECT(x)

static void KeInitializeEvent(KEVENT *event, int type, int state)
{
    EXPECT(type == SynchronizationEvent);
    EXPECT(state == FALSE);
    event->Signaled = state;
}

static PDEVICE_OBJECT IoGetAttachedDeviceReference(PDEVICE_OBJECT device)
{
    EXPECT(device == &AttachedDevice);
    ++device->References;
    return device;
}

static void ObDereferenceObject(PDEVICE_OBJECT device)
{
    EXPECT(device == &AttachedDevice);
    EXPECT(device->References == 1);
    --device->References;
    ++Dereferences;
}

static PIRP Build(PDEVICE_OBJECT device, KEVENT *event, IO_STATUS_BLOCK *status)
{
    EXPECT(device == &AttachedDevice);
    EXPECT(device->References == 1);
    EXPECT(event->Signaled == FALSE);
    if (FailBuild)
        return NULL;
    CompletionEvent = event;
    CompletionStatus = status;
    memset(&Request, 0, sizeof(Request));
    return &Request;
}

static PIRP IoBuildDeviceIoControlRequest(ULONG code, PDEVICE_OBJECT device,
                                         PVOID input, ULONG input_length,
                                         PVOID output, ULONG output_length,
                                         BOOLEAN internal, KEVENT *event,
                                         IO_STATUS_BLOCK *status)
{
    EXPECT(!CapabilityRequest);
    EXPECT(code == 0x1234);
    EXPECT(input == Input && input_length == sizeof(Input));
    EXPECT(output == Output && output_length == sizeof(Output));
    EXPECT(internal == FALSE);
    return Build(device, event, status);
}

static PIRP IoBuildSynchronousFsdRequest(ULONG major, PDEVICE_OBJECT device,
                                        PVOID buffer, ULONG length, PVOID offset,
                                        KEVENT *event, IO_STATUS_BLOCK *status)
{
    EXPECT(CapabilityRequest);
    EXPECT(major == IRP_MJ_PNP);
    EXPECT(buffer == NULL && length == 0 && offset == NULL);
    return Build(device, event, status);
}

static PIO_STACK_LOCATION IoGetNextIrpStackLocation(PIRP irp)
{
    EXPECT(irp == &Request && !Released);
    return &irp->Stack;
}

static void IoSetCompletionRoutine(PIRP irp, PVOID routine, PVOID context,
                                    BOOLEAN success, BOOLEAN error, BOOLEAN cancel)
{
    EXPECT(irp == &Request && !Released);
    EXPECT(routine == NULL && context == NULL);
    EXPECT(!success && !error && !cancel);
}

static void Complete(void)
{
    EXPECT(!Released && !CompletionEvent->Signaled);
    CompletionStatus->Status = FinalStatus;
    CompletionStatus->Information = sizeof(Output);
    if (CapabilityRequest)
    {
        PDEVICE_CAPABILITIES caps = Request.Stack.Parameters.DeviceCapabilities.Capabilities;
        caps->Address = 0x15;
        caps->UINumber = 3;
    }
    else
    {
        memset(Output, 0x5a, sizeof(Output));
    }
    CompletionEvent->Signaled = TRUE;
    Request.IoStatus.Status = STATUS_NOT_SUPPORTED;
    Released = TRUE;
    ++Completions;
}

static NTSTATUS IoCallDriver(PDEVICE_OBJECT device, PIRP irp)
{
    EXPECT(device == &AttachedDevice && device->References == 1);
    EXPECT(irp == &Request && !Released);
    if (CapabilityRequest)
    {
        PDEVICE_CAPABILITIES caps = irp->Stack.Parameters.DeviceCapabilities.Capabilities;
        unsigned i;
        EXPECT(irp->IoStatus.Status == STATUS_NOT_SUPPORTED && irp->IoStatus.Information == 0);
        EXPECT(irp->Stack.MajorFunction == IRP_MJ_PNP);
        EXPECT(irp->Stack.MinorFunction == IRP_MN_QUERY_CAPABILITIES);
        EXPECT(caps->Size == sizeof(*caps) && caps->Version == 1);
        EXPECT(caps->Address == UINT32_MAX && caps->UINumber == UINT32_MAX);
        for (i = 0; i < sizeof(caps->Extra); ++i)
            EXPECT(caps->Extra[i] == 0);
    }
    ++Calls;
    if (DispatchStatus != STATUS_PENDING || CompleteEarly)
        Complete();
    return DispatchStatus;
}

static NTSTATUS KeWaitForSingleObject(KEVENT *event, int reason, int mode,
                                      BOOLEAN alertable, PVOID timeout)
{
    EXPECT(event == CompletionEvent);
    EXPECT(reason == Executive && mode == KernelMode && !alertable && timeout == NULL);
    EXPECT(DispatchStatus == STATUS_PENDING && Calls == 1);
    ++Waits;
    if (!event->Signaled)
        Complete();
    EXPECT(event->Signaled && Released);
    return STATUS_SUCCESS;
}
'''
    harness += function(source, "PciSendIoctl") + function(source, "PciGetDeviceCapabilities")
    harness += r'''
static void Run(int capabilities, int fail, int pending, int early, NTSTATUS status)
{
    DEVICE_CAPABILITIES caps;
    NTSTATUS result;
    char name[80];
    unsigned i;
    snprintf(name, sizeof(name), "%s/%s/%s", capabilities ? "caps" : "ioctl",
             fail ? "build-failure" : pending ? (early ? "pending-early" : "pending-late") : "synchronous",
             status == STATUS_SUCCESS ? "success" : "failure");
    CaseName = name;
    AttachedDevice.References = 0;
    Calls = Waits = Completions = Dereferences = 0;
    Released = FALSE;
    FailBuild = fail;
    CompleteEarly = early;
    CapabilityRequest = capabilities;
    DispatchStatus = pending ? STATUS_PENDING : status;
    FinalStatus = status;
    CompletionEvent = NULL;
    CompletionStatus = NULL;
    memset(Output, 0, sizeof(Output));
    memset(&caps, 0xa5, sizeof(caps));
    result = capabilities ? PciGetDeviceCapabilities(&AttachedDevice, &caps)
                          : PciSendIoctl(&AttachedDevice, 0x1234, Input, sizeof(Input), Output, sizeof(Output));
    EXPECT(result == (fail ? STATUS_INSUFFICIENT_RESOURCES : status));
    EXPECT(AttachedDevice.References == 0);
    EXPECT(Dereferences == 1);
    EXPECT(Calls == (unsigned)!fail);
    EXPECT(Waits == (unsigned)(!fail && pending));
    EXPECT(Completions == (unsigned)!fail);
    if (!fail)
    {
        EXPECT(Released);
        if (capabilities)
        {
            EXPECT(caps.Size == sizeof(caps) && caps.Version == 1);
            EXPECT(caps.Address == 0x15 && caps.UINumber == 3);
        }
        else
        {
            for (i = 0; i < sizeof(Output); ++i)
                EXPECT(Output[i] == 0x5a);
        }
    }
}

int main(void)
{
    int capabilities, pending, early, failure;
    for (capabilities = 0; capabilities < 2; ++capabilities)
    {
        Run(capabilities, 1, 0, 0, STATUS_SUCCESS);
        for (pending = 0; pending < 2; ++pending)
            for (early = 0; early <= pending; ++early)
                for (failure = 0; failure < 2; ++failure)
                    Run(capabilities, 0, pending, early,
                        failure ? STATUS_ACCESS_DENIED : STATUS_SUCCESS);
    }
    printf("PCI IRP ownership: %u checks, %u failures\n", Checks, Failures);
    return Failures ? 1 : 0;
}
'''
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    c_file, binary = folder / "pci-irp.c", folder / "pci-irp"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    "-I" + str(ROOT / "sdk/include/psdk"), str(c_file), "-o", str(binary)], check=True)
    result = subprocess.run([str(binary)], capture_output=True, text=True)
    print(result.stdout, end="")
    if args.baseline:
        expected = ("ioctl/build-failure/success: AttachedDevice.References == 0",
                    "ioctl/pending-late/success: result ==",
                    "caps/pending-late/success: result ==")
        if result.returncode != 1 or not all(text in result.stderr for text in expected):
            raise SystemExit("The original PCI helpers did not fail the intended ownership checks: " + result.stderr)
        print("Original PCI helpers failed pending-completion status and allocation-failure reference checks as expected")
    else:
        print(result.stderr, end="")
        result.check_returncode()


if __name__ == "__main__":
    main()
