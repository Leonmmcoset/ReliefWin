/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Independent OpenSSL backend for portable disk encryption tests
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <reliefwin/libs/volumecrypt/volumecrypt.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <limits.h>

static void *Allocate(void *Context, size_t Size)
{
    (void)Context;
    return malloc(Size);
}

static void Free(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    VcWipe(Buffer, Size);
    free(Buffer);
}

static int Random(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    return Size <= INT_MAX && RAND_bytes(Buffer, (int)Size) == 1;
}

static int Hash(const void *Buffer, size_t Size, uint8_t Digest[32])
{
    unsigned Length = 0;
    return EVP_Digest(Buffer, Size, Digest, &Length, EVP_sha256(), NULL) && Length == 32;
}

static int Hmac(const void *Key, size_t KeySize, const void *Buffer, size_t Size, uint8_t Digest[32])
{
    unsigned Length = 0;
    return KeySize <= INT_MAX && HMAC(EVP_sha256(), Key, (int)KeySize, Buffer, Size, Digest, &Length) &&
           Length == 32;
}

static int Pbkdf2(const void *Secret, size_t SecretSize, const void *Salt, size_t SaltSize,
                  uint32_t Iterations, void *Output, size_t Size)
{
    return SecretSize <= INT_MAX && SaltSize <= INT_MAX && Iterations <= INT_MAX && Size <= INT_MAX &&
           PKCS5_PBKDF2_HMAC(Secret, (int)SecretSize, Salt, (int)SaltSize, (int)Iterations,
                             EVP_sha256(), (int)Size, Output);
}

static int Xts(const uint8_t Key[VC_KEY_SIZE], uint64_t Sector, void *Buffer, size_t Size, int Encrypt)
{
    EVP_CIPHER_CTX *Context;
    uint8_t Tweak[16] = {0};
    uint8_t *Data = Buffer;
    size_t i;
    int Length, Final, Result = 1;
    if (Size % 512)
        return 0;
    Context = EVP_CIPHER_CTX_new();
    if (!Context)
        return 0;
    while (Size)
    {
        for (i = 0; i != 8; ++i)
            Tweak[i] = (uint8_t)(Sector >> (8 * i));
        if (!EVP_CipherInit_ex(Context, EVP_aes_256_xts(), NULL, Key, Tweak, Encrypt) ||
            !EVP_CIPHER_CTX_set_padding(Context, 0) ||
            !EVP_CipherUpdate(Context, Data, &Length, Data, 512) || Length != 512 ||
            !EVP_CipherFinal_ex(Context, Data + Length, &Final) || Final)
        {
            Result = 0;
            break;
        }
        ++Sector;
        Data += 512;
        Size -= 512;
    }
    EVP_CIPHER_CTX_free(Context);
    return Result;
}

const VC_OPS VcTestOps = {NULL, Allocate, Free, Random, Hash, Hmac, Pbkdf2, Xts};

void
VcTestInitialize(void)
{
}
