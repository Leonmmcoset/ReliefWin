/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Bounded RFC 3629 decoding and UTF-16 boot entry conversion
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

static inline uint32_t UiUtf8Next(const char *Text, size_t Size, size_t *Used)
{
    const unsigned char *Bytes = (const unsigned char *)Text;
    uint32_t Value, Minimum;
    size_t Count, Index;
    *Used = 0;
    if (!Size || !Bytes[0]) return 0;
    *Used = 1;
    if (Bytes[0] < 0x80) return Bytes[0];
    if (Bytes[0] >= 0xc2 && Bytes[0] <= 0xdf) {Count = 2; Value = Bytes[0] & 31; Minimum = 0x80;}
    else if (Bytes[0] >= 0xe0 && Bytes[0] <= 0xef) {Count = 3; Value = Bytes[0] & 15; Minimum = 0x800;}
    else if (Bytes[0] >= 0xf0 && Bytes[0] <= 0xf4) {Count = 4; Value = Bytes[0] & 7; Minimum = 0x10000;}
    else return 0xfffd;
    if (Count > Size) return 0xfffd;
    for (Index = 1; Index < Count; ++Index)
    {
        if ((Bytes[Index] & 0xc0) != 0x80) return 0xfffd;
        Value = (Value << 6) | (Bytes[Index] & 63);
    }
    if (Value < Minimum || Value > 0x10ffff || (Value >= 0xd800 && Value <= 0xdfff)) return 0xfffd;
    *Used = Count;
    return Value;
}

static inline size_t UiUtf16ToUtf8(const uint16_t *Source, char *Target, size_t Capacity)
{
    uint32_t Value;
    size_t Length = 0, Count, Index;
    if (!Capacity) return 0;
    while (*Source)
    {
        Value = *Source++;
        if (Value >= 0xd800 && Value <= 0xdbff)
        {
            if (*Source >= 0xdc00 && *Source <= 0xdfff)
                Value = 0x10000 + ((Value - 0xd800) << 10) + (*Source++ - 0xdc00);
            else Value = 0xfffd;
        }
        else if (Value >= 0xdc00 && Value <= 0xdfff) Value = 0xfffd;
        Count = Value < 0x80 ? 1 : Value < 0x800 ? 2 : Value < 0x10000 ? 3 : 4;
        if (Count >= Capacity - Length) break;
        for (Index = Count; Index > 1; --Index)
        {
            Target[Length + Index - 1] = (char)(0x80 | (Value & 63));
            Value >>= 6;
        }
        Target[Length] = (char)(Count == 1 ? Value : Value | (Count == 2 ? 0xc0 : Count == 3 ? 0xe0 : 0xf0));
        Length += Count;
    }
    Target[Length] = 0;
    return Length;
}
