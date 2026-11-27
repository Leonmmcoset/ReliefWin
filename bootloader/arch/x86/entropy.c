/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Intel SDM RDSEED entropy with CPUID and carry-flag validation
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bl.h>
#include <immintrin.h>
#include <bearssl_hash.h>

BOOLEAN FwArchGetEntropy(PUCHAR Buffer, ULONG Size)
{
    int Information[4];
    unsigned int Value = 0, Previous = 0;
    ULONG Attempt, Offset, Samples;
    BOOLEAN SeedInstruction = FALSE;
    br_sha512_context Hash;
    UCHAR Digest[64];
    if (Size > sizeof(Digest)) return FALSE;
    __cpuid(Information, 0);
    if (Information[0] >= 7)
    {
        __cpuidex(Information, 7, 0);
        SeedInstruction = (Information[1] & (1 << 18)) != 0;
    }
    if (!SeedInstruction)
    {
        __cpuid(Information, 1);
        if (!(Information[2] & (1 << 30))) return FALSE;
    }
    Samples = SeedInstruction ? sizeof(Digest) : 3 * 8192;
    br_sha512_init(&Hash);
    for (Offset = 0; Offset < Samples; Offset += sizeof(Value))
    {
        for (Attempt = 0; Attempt < 1024; ++Attempt)
        {
            if (SeedInstruction ? _rdseed32_step(&Value) : _rdrand32_step(&Value)) break;
            YieldProcessor();
        }
        if (Attempt == 1024 || (Offset && Value == Previous))
        {
            RtlSecureZeroMemory(Buffer, Size);
            RtlSecureZeroMemory(&Hash, sizeof(Hash));
            RtlSecureZeroMemory(&Value, sizeof(Value));
            RtlSecureZeroMemory(&Previous, sizeof(Previous));
            return FALSE;
        }
        br_sha512_update(&Hash, &Value, sizeof(Value));
        Previous = Value;
    }
    br_sha512_out(&Hash, Digest);
    RtlCopyMemory(Buffer, Digest, Size);
    RtlSecureZeroMemory(&Hash, sizeof(Hash));
    RtlSecureZeroMemory(Digest, sizeof(Digest));
    RtlSecureZeroMemory(&Value, sizeof(Value));
    RtlSecureZeroMemory(&Previous, sizeof(Previous));
    return TRUE;
}
