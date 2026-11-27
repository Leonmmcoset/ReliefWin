/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     XTS-AES-256 with the constant-time BearSSL bitslice primitive
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <reliefwin/libs/volumecrypt/crypto.h>
#include <inner.h>

static uint32_t Load32(const uint8_t *Data)
{
    return (uint32_t)Data[0] | ((uint32_t)Data[1] << 8) |
           ((uint32_t)Data[2] << 16) | ((uint32_t)Data[3] << 24);
}

static void Store32(uint8_t *Data, uint32_t Value)
{
    unsigned i;
    for (i = 0; i != 4; ++i)
        Data[i] = (uint8_t)(Value >> (8 * i));
}

static void AesBlocks(const uint64_t *Schedule, uint8_t *Data, unsigned Blocks, int Encrypt)
{
    uint64_t Q[8] = {0};
    uint32_t Words[16] = {0};
    unsigned i;
    for (i = 0; i < Blocks * 4; ++i)
        Words[i] = Load32(Data + i * 4);
    for (i = 0; i != 4; ++i)
        br_aes_ct64_interleave_in(Q + i, Q + i + 4, Words + i * 4);
    br_aes_ct64_ortho(Q);
    if (Encrypt)
        br_aes_ct64_bitslice_encrypt(14, Schedule, Q);
    else
        br_aes_ct64_bitslice_decrypt(14, Schedule, Q);
    br_aes_ct64_ortho(Q);
    for (i = 0; i < Blocks; ++i)
        br_aes_ct64_interleave_out(Words + i * 4, Q[i], Q[i + 4]);
    for (i = 0; i < Blocks * 4; ++i)
        Store32(Data + i * 4, Words[i]);
    VcWipe(Q, sizeof(Q));
    VcWipe(Words, sizeof(Words));
}

static void Multiply(uint8_t Tweak[16])
{
    unsigned i;
    uint8_t Carry = Tweak[15] >> 7;
    for (i = 15; i != 0; --i)
        Tweak[i] = (uint8_t)((Tweak[i] << 1) | (Tweak[i - 1] >> 7));
    Tweak[0] = (uint8_t)((Tweak[0] << 1) ^ (0x87 & (0 - Carry)));
}

int
VcCryptoXts(const uint8_t Key[VC_KEY_SIZE], uint64_t Sector, void *Buffer, size_t Size, int Encrypt)
{
    uint64_t Compact[30], DataKey[120], TweakKey[120];
    uint8_t Tweak[16], Tweaks[64], *Data = Buffer;
    unsigned i, Block;
    size_t Offset;
    if (Size % 512 || (Size && Size / 512 - 1 > UINT64_MAX - Sector) ||
        VcEqual(Key, Key + 32, 32))
        return 0;
    br_aes_ct64_keysched(Compact, Key, 32);
    br_aes_ct64_skey_expand(DataKey, 14, Compact);
    br_aes_ct64_keysched(Compact, Key + 32, 32);
    br_aes_ct64_skey_expand(TweakKey, 14, Compact);
    while (Size)
    {
        memset(Tweak, 0, sizeof(Tweak));
        for (i = 0; i != 8; ++i)
            Tweak[i] = (uint8_t)(Sector >> (8 * i));
        AesBlocks(TweakKey, Tweak, 1, 1);
        for (Offset = 0; Offset != 512; Offset += 64)
        {
            for (Block = 0; Block != 4; ++Block)
            {
                memcpy(Tweaks + Block * 16, Tweak, 16);
                Multiply(Tweak);
            }
            for (i = 0; i != 64; ++i)
                Data[Offset + i] ^= Tweaks[i];
            AesBlocks(DataKey, Data + Offset, 4, Encrypt);
            for (i = 0; i != 64; ++i)
                Data[Offset + i] ^= Tweaks[i];
        }
        Data += 512;
        Size -= 512;
        ++Sector;
    }
    VcWipe(Compact, sizeof(Compact));
    VcWipe(DataKey, sizeof(DataKey));
    VcWipe(TweakKey, sizeof(TweakKey));
    VcWipe(Tweak, sizeof(Tweak));
    VcWipe(Tweaks, sizeof(Tweaks));
    return 1;
}
