/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     HMAC-DRBG known answers, seed clearing and fail-closed boundaries
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "drbg.h"
#include <stdio.h>
#include <string.h>

static unsigned Checks, Failures;
#define CHECK(C) do { ++Checks; if (!(C)) { ++Failures; printf("FAIL %u: %s\n", (unsigned)__LINE__, #C); } } while (0)

static void Decode(const char *Text, unsigned char *Bytes)
{
    unsigned Value;
    while (*Text)
    {
        sscanf(Text, "%2x", &Value);
        *Bytes++ = (unsigned char)Value;
        Text += 2;
    }
}

int main(void)
{
    RW_BOOT_ENTROPY Entropy, Empty;
    RW_DRBG State;
    br_hmac_drbg_context Known;
    unsigned char Seed[42], Output[RW_DRBG_BLOCK_SIZE + 1], Previous[RW_DRBG_BLOCK_SIZE + 1], Reference[30];
    unsigned i;
    const char *Answers[] = {
        "9305a46de7ff8eb107194debd3fd48aa20d5e7656cbe0ea69d2a8d4e7c67",
        "c70c78608a3b5be9289be90ef6e81a9e2c1516d5751d2f75f50033e45f73",
        "475e80e992140567fcc3a50dab90fe84bcd7bb03638e9c4656a06f37f650"
    };
    Decode("009a4d6792295a7f730fc3f2b49cbc0f62e862272f01795edf0d54db760f156d0dac04c0322b3a204224", Seed);
    br_hmac_drbg_init(&Known, &br_sha256_vtable, Seed, sizeof(Seed));
    for (i = 0; i < 3; ++i)
    {
        Decode(Answers[i], Reference);
        br_hmac_drbg_generate(&Known, Output, sizeof(Reference));
        CHECK(!memcmp(Output, Reference, sizeof(Reference)));
    }
    memset(&Empty, 0, sizeof(Empty));
    memset(Output, 0xa5, sizeof(Output));
    memcpy(Previous, Output, sizeof(Output));
    CHECK(!RwDrbgInitialize(&State, NULL));
    CHECK(!RwDrbgGenerate(&State, Output, 32));
    CHECK(!memcmp(Output, Previous, sizeof(Output)));
    for (i = 0; i < 7; ++i)
    {
        memset(&Entropy, 0, sizeof(Entropy));
        Entropy.Magic = RW_ENTROPY_MAGIC;
        Entropy.Version = RW_ENTROPY_VERSION;
        Entropy.Size = sizeof(Entropy);
        Entropy.Source = RW_ENTROPY_FIRMWARE;
        if (i == 0) Entropy.Magic ^= 1;
        if (i == 1) ++Entropy.Version;
        if (i == 2) --Entropy.Size;
        if (i == 3) Entropy.Source = 0;
        if (i == 4) Entropy.Source = 3;
        if (i != 5) memset(Entropy.Seed, 0xff, sizeof(Entropy.Seed));
        if (i < 5) Entropy.Seed[0] = 1;
        CHECK(!RwDrbgInitialize(&State, &Entropy));
        CHECK(!memcmp(&Entropy, &Empty, sizeof(Entropy)));
        CHECK(!RwDrbgGenerate(&State, Output, 32));
        CHECK(!memcmp(Output, Previous, sizeof(Output)));
    }
    for (i = 0; i < 2; ++i)
    {
        unsigned j;
        Entropy.Magic = RW_ENTROPY_MAGIC;
        Entropy.Version = RW_ENTROPY_VERSION;
        Entropy.Size = sizeof(Entropy);
        Entropy.Source = i ? RW_ENTROPY_CPU : RW_ENTROPY_FIRMWARE;
        for (j = 0; j < sizeof(Entropy.Seed); ++j) Entropy.Seed[j] = (unsigned char)j;
        CHECK(RwDrbgInitialize(&State, &Entropy));
        CHECK(!memcmp(&Entropy, &Empty, sizeof(Entropy)));
        CHECK(!RwDrbgGenerate(&State, NULL, 32));
        CHECK(!RwDrbgGenerate(&State, Output, 0));
        CHECK(!RwDrbgGenerate(&State, Output, sizeof(Output)));
        CHECK(RwDrbgGenerate(&State, Output, RW_DRBG_BLOCK_SIZE));
        memcpy(Previous, Output, sizeof(Output));
        CHECK(RwDrbgGenerate(&State, Output, RW_DRBG_BLOCK_SIZE));
        CHECK(memcmp(Output, Previous, RW_DRBG_BLOCK_SIZE));
        State.Requests = RW_DRBG_RESEED_LIMIT - 1;
        CHECK(RwDrbgGenerate(&State, Output, 1));
        memcpy(Previous, Output, sizeof(Output));
        CHECK(!RwDrbgGenerate(&State, Output, 1));
        CHECK(!memcmp(Output, Previous, sizeof(Output)));
    }
    printf("%u entropy checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
