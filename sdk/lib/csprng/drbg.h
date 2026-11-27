/*
 * PROJECT:     ReliefWin CSPRNG Library
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Seeded HMAC-DRBG with a bounded reseed interval
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <stdint.h>
#include <bearssl_rand.h>
#include <reliefwin/bootentropy.h>

#define RW_DRBG_BLOCK_SIZE 256
#define RW_DRBG_RESEED_LIMIT (UINT64_C(1) << 48)

typedef struct RW_DRBG
{
    br_hmac_drbg_context Context;
    uint64_t Requests;
    int Ready;
} RW_DRBG;

int RwDrbgInitialize(RW_DRBG *State, RW_BOOT_ENTROPY *Entropy);
int RwDrbgGenerate(RW_DRBG *State, void *Buffer, size_t Size);
