/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Bounded strict JSON and Base64 parsing for LUKS2 metadata
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "internal.h"

static void
VcJsonSpace(VC_JSON *Json)
{
    while (Json->Position < Json->Size &&
           (Json->Text[Json->Position] == ' ' || Json->Text[Json->Position] == '\t' ||
            Json->Text[Json->Position] == '\r' || Json->Text[Json->Position] == '\n'))
        ++Json->Position;
}

static int
VcJsonValue(VC_JSON *Json, unsigned Depth)
{
    unsigned Index, Previous, Child;
    size_t Start, Length;
    char Type, End, Ch;

    VcJsonSpace(Json);
    if (Depth > 24 || Json->Position == Json->Size || Json->Count == VC_JSON_TOKENS)
        return 0;
    Index = ++Json->Count;
    Start = Json->Position;
    Type = Json->Text[Json->Position++];
    Json->Tokens[Index - 1].Type = Type;
    Json->Tokens[Index - 1].Start = Start;
    Json->Tokens[Index - 1].Count = 0;
    if (Type == '"')
    {
        while (Json->Position < Json->Size)
        {
            Ch = Json->Text[Json->Position++];
            if (Ch == '"')
                goto Done;
            if ((unsigned char)Ch < 0x20 || (unsigned char)Ch >= 0x80)
                return 0;
            if (Ch == '\\')
            {
                if (Json->Position == Json->Size)
                    return 0;
                Ch = Json->Text[Json->Position++];
                if (Ch == 'u')
                {
                    unsigned i;
                    for (i = 0; i != 4; ++i)
                    {
                        if (Json->Position == Json->Size)
                            return 0;
                        Ch = Json->Text[Json->Position++];
                        if (!((Ch >= '0' && Ch <= '9') || (Ch >= 'a' && Ch <= 'f') ||
                              (Ch >= 'A' && Ch <= 'F')))
                            return 0;
                    }
                }
                else if (!strchr("\"\\/bfnrt", Ch))
                    return 0;
            }
        }
        return 0;
    }
    if (Type == '{' || Type == '[')
    {
        End = Type == '{' ? '}' : ']';
        VcJsonSpace(Json);
        if (Json->Position < Json->Size && Json->Text[Json->Position] == End)
        {
            ++Json->Position;
            goto Done;
        }
        for (;;)
        {
            if (Type == '{')
            {
                VcJsonSpace(Json);
                Child = Json->Count + 1;
                if (Json->Position == Json->Size || Json->Text[Json->Position] != '"' ||
                    !VcJsonValue(Json, Depth + 1))
                    return 0;
                if (memchr(Json->Text + Json->Tokens[Child - 1].Start, '\\',
                           Json->Tokens[Child - 1].End - Json->Tokens[Child - 1].Start))
                    return 0;
                for (Previous = Index + 1; Previous < Child;
                     Previous = Json->Tokens[Json->Tokens[Previous - 1].Next - 1].Next)
                {
                    Length = Json->Tokens[Child - 1].End - Json->Tokens[Child - 1].Start;
                    if (Length == Json->Tokens[Previous - 1].End - Json->Tokens[Previous - 1].Start &&
                        !memcmp(Json->Text + Json->Tokens[Child - 1].Start,
                                Json->Text + Json->Tokens[Previous - 1].Start, Length))
                        return 0;
                }
                VcJsonSpace(Json);
                if (Json->Position == Json->Size || Json->Text[Json->Position++] != ':')
                    return 0;
            }
            if (!VcJsonValue(Json, Depth + 1))
                return 0;
            ++Json->Tokens[Index - 1].Count;
            VcJsonSpace(Json);
            if (Json->Position == Json->Size)
                return 0;
            Ch = Json->Text[Json->Position++];
            if (Ch == End)
                break;
            if (Ch != ',')
                return 0;
        }
        goto Done;
    }
    Json->Position = Start;
    if (Type == '-' || (Type >= '0' && Type <= '9'))
    {
        if (Type == '-')
            ++Json->Position;
        if (Json->Position == Json->Size)
            return 0;
        if (Json->Text[Json->Position] == '0')
            ++Json->Position;
        else
        {
            if (Json->Text[Json->Position] < '1' || Json->Text[Json->Position] > '9')
                return 0;
            while (Json->Position < Json->Size && Json->Text[Json->Position] >= '0' &&
                   Json->Text[Json->Position] <= '9')
                ++Json->Position;
        }
        if (Json->Position < Json->Size && Json->Text[Json->Position] == '.')
        {
            ++Json->Position;
            Start = Json->Position;
            while (Json->Position < Json->Size && Json->Text[Json->Position] >= '0' &&
                   Json->Text[Json->Position] <= '9')
                ++Json->Position;
            if (Start == Json->Position)
                return 0;
        }
        if (Json->Position < Json->Size &&
            (Json->Text[Json->Position] == 'e' || Json->Text[Json->Position] == 'E'))
        {
            ++Json->Position;
            if (Json->Position < Json->Size &&
                (Json->Text[Json->Position] == '+' || Json->Text[Json->Position] == '-'))
                ++Json->Position;
            Start = Json->Position;
            while (Json->Position < Json->Size && Json->Text[Json->Position] >= '0' &&
                   Json->Text[Json->Position] <= '9')
                ++Json->Position;
            if (Start == Json->Position)
                return 0;
        }
        Json->Tokens[Index - 1].Type = 'n';
    }
    else
    {
        const char *Literal = Type == 't' ? "true" : Type == 'f' ? "false" : Type == 'n' ? "null" : "";
        Length = strlen(Literal);
        if (!Length || Length > Json->Size - Json->Position ||
            memcmp(Json->Text + Json->Position, Literal, Length))
            return 0;
        Json->Position += Length;
    }
Done:
    Json->Tokens[Index - 1].End = Json->Position;
    Json->Tokens[Index - 1].Next = Json->Count + 1;
    return 1;
}

int
VcJsonParse(VC_JSON *Json, const char *Text, size_t Size)
{
    Json->Text = Text;
    Json->Size = Size;
    Json->Count = 0;
    Json->Position = 0;
    if (!VcJsonValue(Json, 0))
        return 0;
    VcJsonSpace(Json);
    return Json->Position == Size && Json->Tokens[0].Type == '{';
}

int
VcJsonString(const VC_JSON *Json, unsigned Token, const char *Value)
{
    const VC_JSON_TOKEN *T;
    size_t Length = strlen(Value);
    if (!Token || Token > Json->Count)
        return 0;
    T = &Json->Tokens[Token - 1];
    return T->Type == '"' && T->End - T->Start == Length + 2 &&
           !memcmp(Json->Text + T->Start + 1, Value, Length);
}

unsigned
VcJsonMember(const VC_JSON *Json, unsigned Object, const char *Name)
{
    unsigned Token;
    if (!Object || Object > Json->Count || Json->Tokens[Object - 1].Type != '{')
        return 0;
    for (Token = Object + 1; Token < Json->Tokens[Object - 1].Next;)
    {
        if (VcJsonString(Json, Token, Name))
            return Token + 1;
        Token = Json->Tokens[Json->Tokens[Token - 1].Next - 1].Next;
    }
    return 0;
}

int
VcJsonNumber(const VC_JSON *Json, unsigned Token, uint64_t *Value, char Type)
{
    size_t Start, End;
    uint64_t Number = 0;
    if (!Token || Token > Json->Count || Json->Tokens[Token - 1].Type != Type)
        return 0;
    Start = Json->Tokens[Token - 1].Start + (Type == '"');
    End = Json->Tokens[Token - 1].End - (Type == '"');
    if (Start == End || (End - Start > 1 && Json->Text[Start] == '0'))
        return 0;
    while (Start < End)
    {
        unsigned Digit = (unsigned char)Json->Text[Start++] - '0';
        if (Digit > 9 || Number > (UINT64_MAX - Digit) / 10)
            return 0;
        Number = Number * 10 + Digit;
    }
    *Value = Number;
    return 1;
}

static const char VcBase64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void
VcBase64(const void *Input, size_t Size, char *Output)
{
    const uint8_t *Bytes = Input;
    size_t i, j = 0;
    for (i = 0; i < Size; i += 3)
    {
        unsigned N = (unsigned)Bytes[i] << 16;
        if (i + 1 < Size)
            N |= (unsigned)Bytes[i + 1] << 8;
        if (i + 2 < Size)
            N |= Bytes[i + 2];
        Output[j++] = VcBase64Alphabet[N >> 18];
        Output[j++] = VcBase64Alphabet[(N >> 12) & 63];
        Output[j++] = i + 1 < Size ? VcBase64Alphabet[(N >> 6) & 63] : '=';
        Output[j++] = i + 2 < Size ? VcBase64Alphabet[N & 63] : '=';
    }
    Output[j] = 0;
}

int
VcJsonBinary(const VC_JSON *Json, unsigned Token, void *Output, size_t Size)
{
    const VC_JSON_TOKEN *T;
    size_t Length, i, Position = 0;
    uint8_t *Bytes = Output;
    unsigned Bits = 0, Value = 0;
    char Canonical[129];
    if (!Token || Token > Json->Count || Size > 96)
        return 0;
    T = &Json->Tokens[Token - 1];
    Length = ((Size + 2) / 3) * 4;
    if (T->Type != '"' || T->End - T->Start != Length + 2)
        return 0;
    for (i = 0; i < Length; ++i)
    {
        char Ch = Json->Text[T->Start + 1 + i];
        const char *Match;
        if (Ch == '=')
            break;
        Match = Ch ? strchr(VcBase64Alphabet, Ch) : NULL;
        if (!Match)
            return 0;
        Value = (Value << 6) | (unsigned)(Match - VcBase64Alphabet);
        Bits += 6;
        if (Bits >= 8)
        {
            Bits -= 8;
            if (Position == Size)
                return 0;
            Bytes[Position++] = (uint8_t)(Value >> Bits);
        }
    }
    if (Position != Size)
        return 0;
    VcBase64(Output, Size, Canonical);
    return !memcmp(Canonical, Json->Text + T->Start + 1, Length);
}
