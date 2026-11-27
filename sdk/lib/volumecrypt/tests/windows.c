/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Production SymCrypt backend for Windows and ReliefWin tests
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <windows.h>
#include <bcrypt.h>
#include <reliefwin/libs/volumecrypt/crypto.h>
#include <stdio.h>

static void *Allocate(void *Context, size_t Size)
{
    (void)Context;
    return HeapAlloc(GetProcessHeap(), 0, Size);
}

static void Free(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    VcWipe(Buffer, Size);
    HeapFree(GetProcessHeap(), 0, Buffer);
}

static int Random(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    return Size <= UINT32_MAX && BCryptGenRandom(NULL, Buffer, (ULONG)Size, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0;
}

void
VcCryptoFatal(uint32_t Code)
{
    fprintf(stderr, "Cryptographic self-test failed: %08lx\n", (unsigned long)Code);
    TerminateProcess(GetCurrentProcess(), 2);
}

void
VcTestInitialize(void)
{
    VcCryptoInitialize();
}

const VC_OPS VcTestOps = {NULL, Allocate, Free, Random, VcCryptoHash, VcCryptoHmac, VcCryptoPbkdf2, VcCryptoXts};
