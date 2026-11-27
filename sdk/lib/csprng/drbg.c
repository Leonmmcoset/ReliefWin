/*
 * PROJECT:     ReliefWin CSPRNG Library
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Fail-closed SP 800-90A HMAC-DRBG initialization and generation
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "drbg.h"
#include <string.h>

static void Wipe(void *Buffer, size_t Size)
{
    volatile unsigned char *Bytes = Buffer;
    while (Size--) *Bytes++ = 0;
}

int RwDrbgInitialize(RW_DRBG *State, RW_BOOT_ENTROPY *Entropy)
{
    unsigned i;
    int Varied = 0;
    Wipe(State, sizeof(*State));
    if (!Entropy) return 0;
    for (i = 1; i < sizeof(Entropy->Seed); ++i)
        Varied |= Entropy->Seed[i] ^ Entropy->Seed[0];
    if (Entropy->Magic == RW_ENTROPY_MAGIC && Entropy->Version == RW_ENTROPY_VERSION &&
        Entropy->Size == sizeof(*Entropy) && Varied &&
        (Entropy->Source == RW_ENTROPY_FIRMWARE || Entropy->Source == RW_ENTROPY_CPU))
    {
        br_hmac_drbg_init(&State->Context, &br_sha256_vtable, Entropy->Seed, sizeof(Entropy->Seed));
        State->Ready = 1;
    }
    Wipe(Entropy, sizeof(*Entropy));
    return State->Ready;
}

int RwDrbgGenerate(RW_DRBG *State, void *Buffer, size_t Size)
{
    if (!State->Ready || State->Requests == RW_DRBG_RESEED_LIMIT ||
        !Buffer || !Size || Size > RW_DRBG_BLOCK_SIZE)
        return 0;
    br_hmac_drbg_generate(&State->Context, Buffer, Size);
    ++State->Requests;
    return 1;
}
