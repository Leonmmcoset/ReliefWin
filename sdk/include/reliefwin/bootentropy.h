/*
 * PROJECT:     ReliefWin Boot Entropy
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Private boot entropy handoff outside the Windows loader ABI
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <stdint.h>

#define RW_ENTROPY_MAGIC UINT32_C(0x52454e54)
#define RW_ENTROPY_VERSION 1
#define RW_ENTROPY_SEED_SIZE 64
#define RW_ENTROPY_FIRMWARE 1
#define RW_ENTROPY_CPU 2

typedef struct RW_BOOT_ENTROPY
{
    uint32_t Magic;
    uint32_t Version;
    uint32_t Size;
    uint32_t Source;
    uint8_t Seed[RW_ENTROPY_SEED_SIZE];
} RW_BOOT_ENTROPY;
