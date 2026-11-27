/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Freestanding SymCrypt encryption backend
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <reliefwin/libs/volumecrypt/volumecrypt.h>

void VcCryptoInitialize(void);
void VcCryptoFatal(uint32_t Code);
int VcCryptoHash(const void *Buffer, size_t Size, uint8_t Digest[32]);
int VcCryptoHmac(const void *Key, size_t KeySize, const void *Buffer, size_t Size, uint8_t Digest[32]);
int VcCryptoPbkdf2(const void *Secret, size_t SecretSize, const void *Salt, size_t SaltSize,
                   uint32_t Iterations, void *Output, size_t Size);
int VcCryptoXts(const uint8_t Key[VC_KEY_SIZE], uint64_t Sector, void *Buffer, size_t Size, int Encrypt);
