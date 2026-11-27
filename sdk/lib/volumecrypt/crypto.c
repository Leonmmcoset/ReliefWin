/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     SymCrypt SHA-256, HMAC, PBKDF2 and AES-XTS adapters
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <windef.h>
#include <winbase.h>
#include <symcrypt.h>
#include <sc_lib.h>
#include <reliefwin/libs/volumecrypt/crypto.h>

SYMCRYPT_ENVIRONMENT_DEFS(ReliefVolume);

void SYMCRYPT_CALL
SymCryptInitEnvReliefVolume(UINT32 Version)
{
    SymCryptInitEnvCommon(Version);
}

SYMCRYPT_CPU_FEATURES SYMCRYPT_CALL
SymCryptCpuFeaturesNeverPresentEnvReliefVolume(void)
{
    return (SYMCRYPT_CPU_FEATURES)~0;
}

_Analysis_noreturn_ void SYMCRYPT_CALL
SymCryptFatalEnvReliefVolume(UINT32 Code)
{
    VcCryptoFatal(Code);
    SymCryptFatalHang(Code);
}

void SYMCRYPT_CALL
SymCryptTestInjectErrorEnvReliefVolume(PBYTE Buffer, SIZE_T Size)
{
    (void)Buffer;
    (void)Size;
}

void
VcCryptoInitialize(void)
{
    static const uint8_t Expected[32] =
    {
        0x1e,0x2d,0xd4,0xe2,0xfc,0x82,0x7e,0x8a,0xd1,0xf2,0x88,0x6d,0x8d,0x9f,0xc3,0xf3,
        0x01,0xc1,0x21,0x89,0xb1,0xbf,0xe3,0x45,0xba,0x92,0x2a,0x7a,0x2f,0x57,0xa1,0xda
    };
    uint8_t Key[64], Data[512], Digest[32];
    unsigned i;
    int Valid;
    SymCryptInit();
    SymCryptSha256Selftest();
    SymCryptHmacSha256Selftest();
    SymCryptPbkdf2_HmacSha256SelfTest();
    for (i = 0; i != sizeof(Key); ++i)
        Key[i] = (uint8_t)i;
    for (i = 0; i != sizeof(Data); ++i)
        Data[i] = (uint8_t)i;
    Valid = VcCryptoXts(Key, UINT64_C(0x123456789abcdef0), Data, sizeof(Data), 1) &&
            VcCryptoHash(Data, sizeof(Data), Digest) && VcEqual(Digest, Expected, sizeof(Digest)) &&
            VcCryptoXts(Key, UINT64_C(0x123456789abcdef0), Data, sizeof(Data), 0);
    for (i = 0; i != sizeof(Data); ++i)
        Valid &= Data[i] == (uint8_t)i;
    VcWipe(Key, sizeof(Key));
    VcWipe(Data, sizeof(Data));
    VcWipe(Digest, sizeof(Digest));
    if (!Valid)
        VcCryptoFatal(0x58545331);
}

int
VcCryptoHash(const void *Buffer, size_t Size, uint8_t Digest[32])
{
    SymCryptSha256(Buffer, Size, Digest);
    return 1;
}

int
VcCryptoHmac(const void *Key, size_t KeySize, const void *Buffer, size_t Size, uint8_t Digest[32])
{
    SYMCRYPT_HMAC_SHA256_EXPANDED_KEY Expanded;
    SYMCRYPT_ERROR Error = SymCryptHmacSha256ExpandKey(&Expanded, Key, KeySize);
    if (Error == SYMCRYPT_NO_ERROR)
        SymCryptHmacSha256(&Expanded, Buffer, Size, Digest);
    VcWipe(&Expanded, sizeof(Expanded));
    return Error == SYMCRYPT_NO_ERROR;
}

int
VcCryptoPbkdf2(const void *Secret, size_t SecretSize, const void *Salt, size_t SaltSize,
               uint32_t Iterations, void *Output, size_t Size)
{
    return SymCryptPbkdf2(SymCryptHmacSha256Algorithm, Secret, SecretSize, Salt, SaltSize,
                          Iterations, Output, Size) == SYMCRYPT_NO_ERROR;
}
