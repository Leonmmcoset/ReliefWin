/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Internal endian and bounded JSON contracts
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <reliefwin/libs/volumecrypt/volumecrypt.h>
#include <string.h>

#define VC_JSON_TOKENS 2048

typedef struct VC_JSON_TOKEN
{
    size_t Start;
    size_t End;
    unsigned Next;
    unsigned Count;
    char Type;
} VC_JSON_TOKEN;

typedef struct VC_JSON
{
    const char *Text;
    size_t Size;
    size_t Position;
    unsigned Count;
    VC_JSON_TOKEN Tokens[VC_JSON_TOKENS];
} VC_JSON;

uint64_t VcBe64(const uint8_t *Data);
uint32_t VcBe32(const uint8_t *Data);
void VcPut64(uint8_t *Data, uint64_t Value);
void VcPut32(uint8_t *Data, uint32_t Value);
int VcRead(const VC_IO *Io, uint64_t Offset, void *Buffer, size_t Size);
int VcWrite(const VC_IO *Io, uint64_t Offset, const void *Buffer, size_t Size);
int VcJsonParse(VC_JSON *Json, const char *Text, size_t Size);
unsigned VcJsonMember(const VC_JSON *Json, unsigned Object, const char *Name);
int VcJsonString(const VC_JSON *Json, unsigned Token, const char *Value);
int VcJsonNumber(const VC_JSON *Json, unsigned Token, uint64_t *Value, char Type);
int VcJsonBinary(const VC_JSON *Json, unsigned Token, void *Output, size_t Size);
void VcBase64(const void *Input, size_t Size, char *Output);
