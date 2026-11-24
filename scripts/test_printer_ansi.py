#!/usr/bin/env python3
# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise actual printer ANSI entry points with documented encoding and I/O mocks
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from pathlib import Path
import re
import resource
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = 'subsystems/win/win32k/printing/base/winspool/printers.c'
BASELINE = 'a676708adf7c492ecd32e35c2b35ec1d3edb0a28'


def function(source, name, declaration):
    start = source.index(name + '(')
    cursor = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[cursor] == '{') - (source[cursor] == '}')
        cursor += 1
    return declaration + ' ' + source[start:cursor] + '\n'


PREFIX = r'''
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
typedef uint16_t WCHAR;
typedef uint16_t WORD;
typedef WORD LANGID;
typedef uint64_t DWORDLONG;
typedef int32_t LONG;
typedef void *HWND;
typedef char CHAR, *PSTR, *LPSTR;
typedef WCHAR *PWSTR, *LPWSTR;
typedef const WCHAR *LPCWSTR, *PCWSTR;
typedef const char *LPCSTR;
typedef uintptr_t ULONG_PTR;
typedef uint32_t DWORD, *PDWORD, *LPDWORD;
typedef unsigned char BYTE, *PBYTE, *LPBYTE;
typedef int BOOL, INT;
typedef size_t SIZE_T;
typedef void *HANDLE, *PSECURITY_DESCRIPTOR;
typedef struct { LONG cx, cy; } SIZEL;
typedef struct { LONG left, top, right, bottom; } RECTL;
typedef struct { DWORD dwLowDateTime, dwHighDateTime; } FILETIME;
typedef struct { WORD Length, MaximumLength; PWSTR Buffer; } UNICODE_STRING;
#define WINAPI
#define TRUE 1
#define FALSE 0
#define CP_ACP 0
#define ERROR_SUCCESS 0
#define ERROR_ACCESS_DENIED 5
#define ERROR_NOT_ENOUGH_MEMORY 8
#define ERROR_INVALID_DATA 13
#define ERROR_INVALID_PARAMETER 87
#define ERROR_INVALID_LEVEL 124
#define ERROR_INSUFFICIENT_BUFFER 122
#define ERROR_ARITHMETIC_OVERFLOW 534
#define ERROR_NO_UNICODE_TRANSLATION 1113
#define CopyMemory memcpy
#define ZeroMemory(pointer,bytes) memset(pointer,0,bytes)
#define _countof(array) (sizeof(array)/sizeof((array)[0]))
#define MAXDWORD UINT32_MAX
#define GetProcessHeap() hProcessHeap
#define FIELD_OFFSET(type, member) offsetof(type, member)
#define min(a,b) ((a)<(b)?(a):(b))
#define TRACE(...) ((void)0)
#define ERR(...) ((void)0)
#define FIXME(...) ((void)0)
static HANDLE hProcessHeap = (HANDLE)1;
static DWORD LastError;
static unsigned Checks, Failures, Allocations, LiveAllocations, ConversionCalls;
static unsigned FailAllocation, FailConversion;
static unsigned ActiveCodePage = 936;
static int ForceWFailure;
static const char *CaseName;
#define EXPECT(x) do { ++Checks; if (!(x)) { if (Failures < 25) fprintf(stderr, "%s:%d: %s\n", CaseName, __LINE__, #x); ++Failures; } } while (0)
static DWORD GetLastError(void) { return LastError; }
static void SetLastError(DWORD error) { LastError = error; }
static size_t wide_len(const WCHAR *text)
{
    const WCHAR *end = text;
    while (*end) ++end;
    return end - text;
}
#define wcslen wide_len
typedef union { max_align_t Alignment; size_t Bytes; } ALLOCATION_HEADER;
static void *HeapAlloc(HANDLE heap, DWORD flags, SIZE_T bytes)
{
    ALLOCATION_HEADER *allocation;
    EXPECT(heap == hProcessHeap && flags == 0);
    ++Allocations;
    if (FailAllocation && Allocations == FailAllocation) return NULL;
    allocation = malloc(sizeof(*allocation) + bytes + 8);
    if (!allocation) abort();
    allocation->Bytes = bytes;
    memset(allocation + 1, 0xcd, bytes);
    memset((BYTE *)(allocation + 1) + bytes, 0xee, 8);
    ++LiveAllocations;
    return allocation + 1;
}
static BOOL HeapFree(HANDLE heap, DWORD flags, void *pointer)
{
    ALLOCATION_HEADER *allocation = (ALLOCATION_HEADER *)pointer - 1;
    size_t i;
    EXPECT(heap == hProcessHeap && flags == 0 && LiveAllocations > 0);
    for (i = 0; i < 8; ++i) EXPECT(((BYTE *)pointer)[allocation->Bytes + i] == 0xee);
    --LiveAllocations;
    free(allocation);
    return TRUE;
}
static int WideCharToMultiByte(unsigned codepage, DWORD flags, const WCHAR *text,
                               int count, char *output, int capacity, void *def, void *used)
{
    char converted[4096];
    char *input = (char *)text, *target = converted;
    size_t input_bytes, output_bytes = sizeof(converted), result;
    iconv_t codec;
    EXPECT(codepage == CP_ACP && flags == 0 && count == -1 && !def && !used);
    ++ConversionCalls;
    if (FailConversion && ConversionCalls == FailConversion)
    {
        SetLastError(ERROR_NO_UNICODE_TRANSLATION);
        return 0;
    }
    input_bytes = (wide_len(text) + 1) * sizeof(WCHAR);
    codec = iconv_open(ActiveCodePage == 936 ? "CP936" : "UTF-8", "UTF-16LE");
    if (codec == (iconv_t)-1) abort();
    result = iconv(codec, &input, &input_bytes, &target, &output_bytes);
    iconv_close(codec);
    EXPECT(result != (size_t)-1 && input_bytes == 0);
    result = sizeof(converted) - output_bytes;
    if (!capacity) return (int)result;
    if ((size_t)capacity < result)
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    memcpy(output, converted, result);
    return (int)result;
}
static int MultiByteToWideChar(unsigned codepage, DWORD flags, const char *text,
                               int count, WCHAR *output, int capacity)
{
    size_t length = strlen(text), i;
    EXPECT(codepage == CP_ACP && flags == 0 && count == -1);
    if(!capacity) return (int)length+1;
    EXPECT((size_t)capacity >= length + 1);
    for (i = 0; i <= length; ++i) output[i] = (unsigned char)text[i];
    return (int)length + 1;
}
static int StringCchCopyA(char *output, size_t count, const char *input)
{
    size_t i = 0;
    if (!count) return -1;
    while (i + 1 < count && input[i]) { output[i] = input[i]; ++i; }
    output[i] = 0;
    return input[i] ? -1 : 0;
}
static PWSTR AsciiToUnicode(UNICODE_STRING *target, PSTR text)
{
    EXPECT(!text);
    memset(target,0,sizeof(*target));
    return NULL;
}
static void RtlFreeUnicodeString(UNICODE_STRING *text) { EXPECT(!text->Buffer); }
static DWORD UnicodeToAnsiZZInPlace(PWSTR text) { EXPECT(!text); return ERROR_SUCCESS; }
'''


BODY = r'''
typedef struct {
    DWORD Level;
    SIZE_T Size, Count, Offsets[11], DevmodeOffset;
} FIELDS;
#define OFF(n, field) offsetof(PRINTER_INFO_##n##W, field)
static const FIELDS Layouts[] = {
    {1, sizeof(PRINTER_INFO_1W), 3, {OFF(1,pDescription),OFF(1,pName),OFF(1,pComment)}},
    {2, sizeof(PRINTER_INFO_2W), 11, {OFF(2,pServerName),OFF(2,pPrinterName),OFF(2,pShareName),OFF(2,pPortName),OFF(2,pDriverName),OFF(2,pComment),OFF(2,pLocation),OFF(2,pSepFile),OFF(2,pPrintProcessor),OFF(2,pDatatype),OFF(2,pParameters)},OFF(2,pDevMode)},
    {4, sizeof(PRINTER_INFO_4W), 2, {OFF(4,pPrinterName),OFF(4,pServerName)}},
    {5, sizeof(PRINTER_INFO_5W), 2, {OFF(5,pPrinterName),OFF(5,pPortName)}},
    {7, sizeof(PRINTER_INFO_7W), 1, {OFF(7,pszObjectGUID)}},
    {8, sizeof(PRINTER_INFO_8W), 0, {0}},
    {9, sizeof(PRINTER_INFO_9W), 0, {0}}
};
static const FIELDS JobLayout = {2, sizeof(JOB_INFO_2W), 0, {0}, offsetof(JOB_INFO_2W,pDevMode)};
static const FIELDS FormLayout = {1,sizeof(FORM_INFO_1W),1,{offsetof(FORM_INFO_1W,pName)}};
static const FIELDS DriverLayout = {1,sizeof(DRIVER_INFO_1W),1,{offsetof(DRIVER_INFO_1W,pName)}};
static const FIELDS AddJobLayout = {1,sizeof(ADDJOB_INFO_1W),1,{offsetof(ADDJOB_INFO_1W,Path)}};
static const WCHAR Empty[] = {0};
static const WCHAR Ascii[] = {'p','r','i','n','t','e','r',0};
static const WCHAR Chinese[] = {0x4e2d,0};
static const WCHAR TwoChinese[] = {0x4e2d,0x6587,0};
static const WCHAR Mixed[] = {'A',0x4e2d,'B',0x6587,0};
static const WCHAR Guid[] = {'{','0','0','0','0','0','0','0','0','-','0','0','0','0','-','0','0','0','0','-','0','0','0','0','-','0','0','0','0','0','0','0','0','0','0','0','0','}',0};
static WCHAR LongChinese[97];
static const WCHAR *const Inputs[] = {Ascii, Empty, Chinese, TwoChinese, Mixed, LongChinese, NULL};
static const unsigned char AnsiAscii[] = "printer";
static const unsigned char AnsiEmpty[] = "";
static const unsigned char AnsiChinese[] = {0xd6,0xd0,0};
static const unsigned char AnsiTwoChinese[] = {0xd6,0xd0,0xce,0xc4,0};
static const unsigned char AnsiMixed[] = {'A',0xd6,0xd0,'B',0xce,0xc4,0};
static unsigned char AnsiLongChinese[193];
static const unsigned char *Expected[] = {AnsiAscii, AnsiEmpty, AnsiChinese, AnsiTwoChinese, AnsiMixed, AnsiLongChinese, NULL};
static const FIELDS *CurrentLayout;
static unsigned Records, Sample, SingleField;
static BYTE Header[1024];
static DWORD LastNeeded, WCalls;
static unsigned IncludeDevmode, InvalidDevmode;
static FORM_INFO_2W FormSources[2];
static const FORM_INFO_2A *SubmittedForm;
static unsigned FormSubmissionCalls;
static WCHAR SubmittedWide[3][32];
static char SubmittedKeyword[64];
static void make_devmode(BYTE *storage);
static const WCHAR *field_text(unsigned field)
{
    if (SingleField && field + 1 != SingleField) return NULL;
    if (CurrentLayout->Level == 7 && Inputs[Sample]) return Guid;
    return Inputs[Sample];
}
static BOOL provide_w(BYTE *buffer, DWORD capacity, DWORD *needed, DWORD *returned)
{
    size_t cursor = Records * CurrentLayout->Size, record, field;
    ++WCalls;
    if (ForceWFailure) { SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    for (record = 0; record < Records; ++record)
        for (field = 0; field < CurrentLayout->Count; ++field)
            if (field_text(field)) cursor += (wide_len(field_text(field)) + 1) * sizeof(WCHAR);
    if (IncludeDevmode)
        cursor = (cursor + _Alignof(DEVMODEW)-1) / _Alignof(DEVMODEW) * _Alignof(DEVMODEW) + Records * (sizeof(DEVMODEW)+16);
    *needed = LastNeeded = cursor;
    if (returned) *returned = 0;
    if (!buffer || capacity < cursor) { SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
    memset(buffer, 0, cursor);
    cursor = Records * CurrentLayout->Size;
    for (record = 0; record < Records; ++record)
    {
        for (field = 0; field < CurrentLayout->Count; ++field)
        {
            const WCHAR *text = field_text(field);
            WCHAR *pointer = NULL;
            if (text)
            {
                size_t bytes = (wide_len(text) + 1) * sizeof(WCHAR);
                pointer = (WCHAR *)(buffer + cursor);
                memcpy(pointer, text, bytes);
                cursor += bytes;
            }
            memcpy(buffer + record * CurrentLayout->Size + CurrentLayout->Offsets[field], &pointer, sizeof(pointer));
        }
    }
    if (IncludeDevmode)
    {
        cursor = (cursor + _Alignof(DEVMODEW)-1) / _Alignof(DEVMODEW) * _Alignof(DEVMODEW);
        for (record = 0; record < Records; ++record)
        {
            DEVMODEW *mode = (DEVMODEW *)(buffer + cursor);
            make_devmode((BYTE *)mode);
            if (InvalidDevmode == 1) mode->dmSize = FIELD_OFFSET(DEVMODEW,dmFields)-1;
            if (InvalidDevmode == 2)
            {
                for (field = 0; field < 17; ++field) mode->dmDeviceName[field] = 0x4e2d;
                mode->dmDeviceName[17] = 0;
            }
            memcpy(buffer + record * CurrentLayout->Size + CurrentLayout->DevmodeOffset, &mode, sizeof(mode));
            cursor += sizeof(DEVMODEW)+16;
        }
    }
    EXPECT(Records * CurrentLayout->Size <= sizeof(Header));
    memcpy(Header, buffer, Records * CurrentLayout->Size);
    if (returned) *returned = Records;
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}
static BOOL EnumPrintersW(DWORD flags, PWSTR name, DWORD level, PBYTE buffer,
                          DWORD capacity, PDWORD needed, PDWORD returned)
{
    EXPECT(flags == 2 && level == CurrentLayout->Level);
    if (name) EXPECT(name[0] == 's' && name[1] == 'e');
    return provide_w(buffer, capacity, needed, returned);
}
static BOOL GetPrinterW(HANDLE printer, DWORD level, LPBYTE buffer, DWORD capacity, LPDWORD needed)
{
    EXPECT(printer == (HANDLE)2 && level == CurrentLayout->Level);
    return provide_w(buffer, capacity, needed, NULL);
}
static BOOL EnumJobsW(HANDLE printer, DWORD first, DWORD count, DWORD level, PBYTE buffer,
                      DWORD capacity, PDWORD needed, PDWORD returned)
{
    EXPECT(printer == (HANDLE)2 && first == 0 && count == 1 && level == 2);
    return provide_w(buffer, capacity, needed, returned);
}
static BOOL GetJobW(HANDLE printer, DWORD job, DWORD level, PBYTE buffer, DWORD capacity, PDWORD needed)
{
    EXPECT(printer == (HANDLE)2 && job == 1 && level == 2);
    return provide_w(buffer, capacity, needed, NULL);
}
static LONG DocumentPropertiesW(HWND window, HANDLE printer, PWSTR name, PDEVMODEW output, PDEVMODEW input, DWORD mode)
{
    EXPECT(!window && printer == (HANDLE)2 && !name && !input);
    ++WCalls;
    if (!output) { EXPECT(mode == 0); return sizeof(DEVMODEW)+16; }
    EXPECT(mode == 2);
    make_devmode((BYTE *)output);
    if (InvalidDevmode == 1) output->dmSize = FIELD_OFFSET(DEVMODEW,dmFields)-1;
    return 1;
}
static void RosConvertAnsiDevModeToUnicodeDevmode(PDEVMODEA input, PDEVMODEW *output)
{
    EXPECT(!input && !output);
}
static BOOL EnumFormsW(HANDLE printer,DWORD level,PBYTE buffer,DWORD capacity,PDWORD needed,PDWORD returned)
{
    EXPECT(printer==(HANDLE)2 && (level==1 || level==2));
    if(level==2) return provide_forms_w(buffer,capacity,needed,returned);
    return provide_w(buffer,capacity,needed,returned);
}
static BOOL GetFormW(HANDLE printer,PWSTR name,DWORD level,PBYTE buffer,DWORD capacity,PDWORD needed)
{
    EXPECT(printer==(HANDLE)2 && !name && (level==1 || level==2));
    if(level==2) return provide_forms_w(buffer,capacity,needed,NULL);
    return provide_w(buffer,capacity,needed,NULL);
}
static BOOL submit_form_w(HANDLE printer,DWORD level,PBYTE buffer)
{
    PFORM_INFO_2W form=(PFORM_INFO_2W)buffer;
    const WCHAR *wide[3]={form->pName,form->pMuiDll,form->pDisplayName};
    unsigned field;
    size_t bytes;
    ++FormSubmissionCalls;
    EXPECT(printer==(HANDLE)2 && level==2);
    EXPECT(form->pKeyword==SubmittedForm->pKeyword);
    EXPECT(form->Flags==SubmittedForm->Flags && form->StringType==SubmittedForm->StringType);
    EXPECT(form->dwResourceId==SubmittedForm->dwResourceId && form->wLangId==SubmittedForm->wLangId);
    FormSources[0]=*form;
    for(field=0;field<3;++field)
    {
        bytes=(wide_len(wide[field])+1)*sizeof(WCHAR);
        EXPECT(bytes<=sizeof(SubmittedWide[field]));
        memcpy(SubmittedWide[field],wide[field],bytes);
    }
    FormSources[0].pName=SubmittedWide[0];
    FormSources[0].pMuiDll=SubmittedWide[1];
    FormSources[0].pDisplayName=SubmittedWide[2];
    if(form->pKeyword)
    {
        bytes=strlen(form->pKeyword)+1;
        EXPECT(bytes<=sizeof(SubmittedKeyword));
        memcpy(SubmittedKeyword,form->pKeyword,bytes);
        FormSources[0].pKeyword=SubmittedKeyword;
    }
    if(ForceWFailure) { SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}
static BOOL AddFormW(HANDLE printer,DWORD level,PBYTE buffer)
{
    return submit_form_w(printer,level,buffer);
}
static BOOL SetFormW(HANDLE printer,PWSTR name,DWORD level,PBYTE buffer)
{
    EXPECT(name && name[0]=='p' && name[1]=='a' && name[5]==0);
    return submit_form_w(printer,level,buffer);
}
static BOOL EnumPrinterDriversW(PWSTR name,PWSTR environment,DWORD level,PBYTE buffer,DWORD capacity,PDWORD needed,PDWORD returned)
{
    EXPECT(!name && !environment && level==1);
    return provide_w(buffer,capacity,needed,returned);
}
static BOOL GetPrinterDriverW(HANDLE printer,PWSTR environment,DWORD level,PBYTE buffer,DWORD capacity,PDWORD needed)
{
    EXPECT(printer==(HANDLE)2 && !environment && level==1);
    return provide_w(buffer,capacity,needed,NULL);
}
static BOOL AddJobW(HANDLE printer,DWORD level,PBYTE buffer,DWORD capacity,PDWORD needed)
{
    EXPECT(printer==(HANDLE)2 && level==1);
    return provide_w(buffer,capacity,needed,NULL);
}
'''


TESTS = r'''
static void reset(void)
{
    EXPECT(LiveAllocations == 0);
    Allocations = ConversionCalls = FailAllocation = FailConversion = WCalls = 0;
    ActiveCodePage = 936;
    ForceWFailure = SingleField = 0;
    IncludeDevmode = InvalidDevmode = 0;
    LastError = 0;
}
static void api_case(int enumerate, unsigned layout, unsigned sample, unsigned allocation, unsigned conversion, int w_failure)
{
    union { max_align_t Alignment; BYTE Bytes[8192]; } output;
    DWORD needed = 0x12345678, returned = 0;
    BOOL success;
    size_t record, field, i;
    reset();
    CurrentLayout = &Layouts[layout]; Records = enumerate ? 2 : 1; Sample = sample;
    FailAllocation = allocation; FailConversion = conversion; ForceWFailure = w_failure;
    memset(output.Bytes, 0xaa, sizeof(output.Bytes));
    success = enumerate ? EnumPrintersA(2, "server", CurrentLayout->Level, output.Bytes, sizeof(output.Bytes)-16, &needed, &returned)
                        : GetPrinterA((HANDLE)2, CurrentLayout->Level, output.Bytes, sizeof(output.Bytes)-16, &needed);
    EXPECT(WCalls == 1 || (enumerate && allocation == 1));
    EXPECT(LiveAllocations == 0);
    for (i = sizeof(output.Bytes)-16; i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xaa);
    if (allocation || conversion || w_failure)
    {
        EXPECT(!success);
        EXPECT(GetLastError() == (w_failure ? ERROR_ACCESS_DENIED : allocation ? ERROR_NOT_ENOUGH_MEMORY : ERROR_NO_UNICODE_TRANSLATION));
        if (WCalls && !w_failure) EXPECT(needed == LastNeeded);
        return;
    }
    EXPECT(success && GetLastError() == ERROR_SUCCESS);
    EXPECT(needed == LastNeeded);
    if (enumerate) EXPECT(returned == Records);
    EXPECT(!memcmp(Header, output.Bytes, Records * CurrentLayout->Size));
    for (record = 0; record < Records; ++record)
        for (field = 0; field < CurrentLayout->Count; ++field)
        {
            char *text;
            memcpy(&text, output.Bytes + record * CurrentLayout->Size + CurrentLayout->Offsets[field], sizeof(text));
            if (!Inputs[sample]) EXPECT(!text);
            else if (CurrentLayout->Level == 7) EXPECT(!strcmp(text, "{00000000-0000-0000-0000-000000000000}"));
            else EXPECT(!strcmp(text, (const char *)Expected[sample]));
        }
}
static void api_utf8_boundary(void)
{
    union { max_align_t Alignment; BYTE Bytes[8192]; } output;
    DWORD needed, returned;
    size_t i, slot;
    BOOL success;
    int enumerate;
    for (enumerate = 0; enumerate <= 1; ++enumerate)
    {
        reset(); CurrentLayout = &Layouts[0]; Records = 1; Sample = 3; SingleField = 1; ActiveCodePage = 65001;
        memset(output.Bytes, 0xaa, sizeof(output.Bytes));
        success = enumerate ? EnumPrintersA(2, NULL, 1, output.Bytes, sizeof(output.Bytes), &needed, &returned)
                            : GetPrinterA((HANDLE)2, 1, output.Bytes, sizeof(output.Bytes), &needed);
        EXPECT(!success && GetLastError() == ERROR_INSUFFICIENT_BUFFER);
        EXPECT(needed == LastNeeded && LiveAllocations == 0);
        slot = CurrentLayout->Size;
        EXPECT(!memcmp(output.Bytes + slot, TwoChinese, sizeof(TwoChinese)));
        for (i = slot + sizeof(TwoChinese); i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xaa);
    }
}
int main(void)
{
    unsigned layout, sample, i;
    CaseName = "printer ANSI public entry points";
    for (i = 0; i < 96; ++i) { LongChinese[i] = 0x4e2d; AnsiLongChinese[i*2] = 0xd6; AnsiLongChinese[i*2+1] = 0xd0; }
    for (layout = 0; layout < 5; ++layout)
        for (sample = 0; sample < sizeof(Inputs)/sizeof(Inputs[0]); ++sample)
        {
            api_case(0, layout, sample, 0, 0, 0);
            if (layout < 4) api_case(1, layout, sample, 0, 0, 0);
        }
    for (layout = 0; layout < 5; ++layout)
    {
        api_case(0, layout, 2, 1, 0, 0);
        api_case(0, layout, 2, 0, 1, 0);
        api_case(0, layout, 2, 0, 2, 0);
        api_case(0, layout, 2, 0, 0, 1);
        if (layout < 4)
        {
            api_case(1, layout, 2, 1, 0, 0);
            api_case(1, layout, 2, 2, 0, 0);
            api_case(1, layout, 2, 0, 1, 0);
            api_case(1, layout, 2, 0, 2, 0);
            api_case(1, layout, 2, 0, 0, 1);
        }
    }
    api_utf8_boundary();
    devmode_tests();
    devmode_api_tests();
    consumer_api_tests();
    form_level2_tests();
    form_submission_tests();
    EXTRA_TESTS
    printf("%u printer ANSI checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
'''


FORM_PROVIDER = r'''
typedef FORM_INFO_2W FORM_INFO_LIST, *PFORM_INFO_LIST;
PROVIDER_IMPLEMENTATION
static BOOL provide_forms_w(BYTE *buffer,DWORD capacity,DWORD *needed,DWORD *returned);
'''


FORM_TESTS = r'''
static BOOL provide_forms_w(BYTE *buffer,DWORD capacity,DWORD *needed,DWORD *returned)
{
    unsigned record;
    PBYTE end;
    PFORM_INFO_2W form;
    ++WCalls;
    if(ForceWFailure) { SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    *needed=0;
    for(record=0;record<Records;++record) _LocalGetFormLevel2(&FormSources[record],NULL,NULL,needed);
    LastNeeded=*needed;
    if(returned) *returned=0;
    if(!buffer || capacity<*needed) { SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
    memset(buffer,0,capacity);
    form=(PFORM_INFO_2W)buffer;
    end=buffer+(returned?*needed:capacity);
    for(record=0;record<Records;++record) _LocalGetFormLevel2(&FormSources[record],&form,&end,needed);
    if(returned) *returned=Records;
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}
static void form_level2_tests(void)
{
    static const union { WCHAR Align; char Text[16]; } keywords[] = {
        {.Text=""},{.Text="x"},{.Text="xy"},{.Text="xyz"},{.Text="keyword"},{.Text="\xd6\xd0"}
    };
    union { max_align_t Alignment; BYTE Bytes[2048]; } output;
    unsigned keyword, records, record, field, api, failure, i, sample;
    DWORD needed, returned, expected_needed;
    PFORM_INFO_2W form, cursor;
    PBYTE end;
    BOOL success;
    const WCHAR *wide;
    const char *text;
    size_t bytes, aligned;
    const size_t offsets[]={offsetof(FORM_INFO_2W,pName),offsetof(FORM_INFO_2W,pMuiDll),offsetof(FORM_INFO_2W,pDisplayName)};
    CaseName="FORM_INFO_2 mixed ANSI and UTF16 provider packing";
    for(keyword=0;keyword<=_countof(keywords);++keyword)
        for(records=1;records<=2;++records)
            for(sample=0;sample<_countof(Inputs);++sample)
            {
                reset(); Records=records;
                text=keyword==_countof(keywords)?NULL:keywords[keyword].Text;
                bytes=text?strlen(text)+1:0;
                aligned=(bytes+sizeof(WCHAR)-1)/sizeof(WCHAR)*sizeof(WCHAR);
                expected_needed=records*(sizeof(FORM_INFO_2W)+aligned+(Inputs[sample]?3*(wide_len(Inputs[sample])+1)*sizeof(WCHAR):0));
                for(record=0;record<records;++record)
                {
                    memset(&FormSources[record],0,sizeof(FormSources[record]));
                    FormSources[record].pName=(PWSTR)Inputs[sample];
                    FormSources[record].pMuiDll=Inputs[sample];
                    FormSources[record].pDisplayName=Inputs[sample];
                    FormSources[record].pKeyword=text;
                    FormSources[record].Flags=record+1;
                    FormSources[record].Size.cx=210000+record;
                    FormSources[record].ImageableArea.bottom=297000+record;
                    FormSources[record].StringType=2;
                    FormSources[record].dwResourceId=100+record;
                }
                needed=0;
                for(record=0;record<records;++record) _LocalGetFormLevel2(&FormSources[record],NULL,NULL,&needed);
                EXPECT(needed==expected_needed && !(needed%sizeof(WCHAR)));
                EXPECT(needed+16<sizeof(output.Bytes));
                memset(output.Bytes,0xaa,sizeof(output.Bytes));
                form=cursor=(PFORM_INFO_2W)output.Bytes; end=output.Bytes+needed;
                for(record=0;record<records;++record) _LocalGetFormLevel2(&FormSources[record],&cursor,&end,&needed);
                EXPECT(cursor==form+records && end==output.Bytes+records*sizeof(*form));
                for(record=0;record<records;++record)
                {
                    EXPECT(form[record].Flags==record+1 && form[record].Size.cx==(LONG)(210000+record));
                    EXPECT(form[record].ImageableArea.bottom==(LONG)(297000+record));
                    EXPECT(form[record].StringType==2 && form[record].dwResourceId==100+record);
                    if(text)
                    {
                        EXPECT(!memcmp(form[record].pKeyword,text,bytes));
                        EXPECT((const BYTE *)form[record].pKeyword>=end && (const BYTE *)form[record].pKeyword+aligned<=output.Bytes+needed);
                        for(i=bytes;i<aligned;++i) EXPECT(!form[record].pKeyword[i]);
                    }
                    else EXPECT(!form[record].pKeyword);
                    for(field=0;field<_countof(offsets);++field)
                    {
                        memcpy(&wide,(BYTE *)&form[record]+offsets[field],sizeof(wide));
                        if(Inputs[sample])
                        {
                            EXPECT(!((uintptr_t)wide%_Alignof(WCHAR)));
                            EXPECT((const BYTE *)wide>=end && (const BYTE *)(wide+wide_len(wide)+1)<=output.Bytes+needed);
                            EXPECT(!memcmp(wide,Inputs[sample],(wide_len(Inputs[sample])+1)*sizeof(WCHAR)));
                        }
                        else EXPECT(!wide);
                    }
                }
                for(i=needed;i<sizeof(output.Bytes);++i) EXPECT(output.Bytes[i]==0xaa);
                for(api=0;api<2;++api)
                    for(failure=0;failure<=4;++failure)
                    {
                        reset(); Records=api?1:records;
                        if(failure==1) ForceWFailure=1;
                        if(failure==2 && Inputs[sample]) FailAllocation=1;
                        if(failure==3 && Inputs[sample]) FailConversion=1;
                        if(failure==4 && Inputs[sample]) FailConversion=2;
                        memset(output.Bytes,0xaa,sizeof(output.Bytes));
                        success=api?GetFormA((HANDLE)2,NULL,2,output.Bytes,sizeof(output.Bytes)-16,&needed)
                                   :EnumFormsA((HANDLE)2,2,output.Bytes,sizeof(output.Bytes)-16,&needed,&returned);
                        EXPECT(WCalls==1 && LiveAllocations==0);
                        for(i=sizeof(output.Bytes)-16;i<sizeof(output.Bytes);++i) EXPECT(output.Bytes[i]==0xaa);
                        if(failure==1 || (failure && Inputs[sample]))
                        {
                            EXPECT(!success);
                            EXPECT(GetLastError()==(failure==1?ERROR_ACCESS_DENIED:failure==2?ERROR_NOT_ENOUGH_MEMORY:ERROR_NO_UNICODE_TRANSLATION));
                            continue;
                        }
                        EXPECT(success && GetLastError()==ERROR_SUCCESS && needed==LastNeeded);
                        if(!api) EXPECT(returned==Records);
                        form=(PFORM_INFO_2W)output.Bytes;
                        for(record=0;record<Records;++record)
                        {
                            if(text) EXPECT(!memcmp(form[record].pKeyword,text,bytes));
                            else EXPECT(!form[record].pKeyword);
                            for(field=0;field<_countof(offsets);++field)
                            {
                                memcpy(&text,(BYTE *)&form[record]+offsets[field],sizeof(text));
                                if(Inputs[sample]) EXPECT(!strcmp(text,(const char *)Expected[sample]));
                                else EXPECT(!text);
                            }
                            text=keyword==_countof(keywords)?NULL:keywords[keyword].Text;
                        }
                    }
            }
}
static void form_submission_tests(void)
{
    static const char *keywords[]={NULL,"","x","xy","xyz","keyword","\xd6\xd0"};
    union { max_align_t Alignment; BYTE Bytes[1024]; } output;
    FORM_INFO_2A input;
    PFORM_INFO_2A form=(PFORM_INFO_2A)output.Bytes;
    unsigned api,keyword,failure;
    DWORD needed,returned;
    BOOL success;
    CaseName="FORM_INFO_2 Add/Set ANSI keyword contract";
    for(api=0;api<2;++api)
        for(keyword=0;keyword<_countof(keywords);++keyword)
            for(failure=0;failure<2;++failure)
            {
                reset(); Records=1; FormSubmissionCalls=0;
                memset(&input,0,sizeof(input));
                input.pName="paper"; input.pMuiDll="paper"; input.pDisplayName="paper";
                input.pKeyword=keywords[keyword]; input.StringType=2; input.Flags=1;
                input.dwResourceId=100; input.wLangId=0x0804;
                SubmittedForm=&input;
                ForceWFailure=failure;
                success=api?SetFormA((HANDLE)2,"paper",2,(PBYTE)&input):AddFormA((HANDLE)2,2,(PBYTE)&input);
                EXPECT(FormSubmissionCalls==1 && LiveAllocations==0);
                EXPECT(Allocations==(api?4:3));
                EXPECT(success==!failure && GetLastError()==(failure?ERROR_ACCESS_DENIED:ERROR_SUCCESS));
                if(failure) continue;
                EXPECT(api?GetFormA((HANDLE)2,NULL,2,output.Bytes,sizeof(output.Bytes),&needed)
                          :EnumFormsA((HANDLE)2,2,output.Bytes,sizeof(output.Bytes),&needed,&returned));
                EXPECT(!strcmp(form->pName,"paper") && !strcmp(form->pMuiDll,"paper") && !strcmp(form->pDisplayName,"paper"));
                if(keywords[keyword]) EXPECT(!strcmp(form->pKeyword,keywords[keyword]));
                else EXPECT(!form->pKeyword);
                EXPECT(LiveAllocations==0);
            }
}
'''


DEVMODE_TESTS = r'''
static void reset(void);
static void make_devmode(BYTE *storage)
{
    DEVMODEW *mode = (DEVMODEW *)storage;
    unsigned i;
    memset(storage, 0, sizeof(*mode) + 16);
    mode->dmDeviceName[0] = 'P'; mode->dmDeviceName[1] = 'R';
    mode->dmFormName[0] = 'A'; mode->dmFormName[1] = '4';
    mode->dmSpecVersion = 0x401; mode->dmDriverVersion = 0x123;
    mode->dmSize = sizeof(*mode); mode->dmDriverExtra = 16;
    mode->dmFields = DM_FORMNAME;
    mode->dmOrientation = 2; mode->dmCopies = 3;
    mode->dmLogPixels = 120; mode->dmPelsWidth = 1920;
    mode->dmPanningHeight = 1080;
    for (i = 0; i < 16; ++i) storage[sizeof(*mode) + i] = (BYTE)(i + 1);
}
static void devmode_tests(void)
{
    union { max_align_t Alignment; BYTE Bytes[sizeof(DEVMODEW)+48]; } input, output;
    BYTE original[sizeof(input.Bytes)], expected[sizeof(DEVMODEA)+16];
    DEVMODEW *wide = (DEVMODEW *)input.Bytes;
    DEVMODEA *ansi = (DEVMODEA *)output.Bytes;
    unsigned i, failure;
    DWORD error;
    CaseName = "DEVMODE conversion ownership";
    reset();
    EXPECT(RosConvertUnicodeDevModeToAnsiDevmode(NULL, ansi) == ERROR_SUCCESS);
    EXPECT(RosConvertUnicodeDevModeToAnsiDevmode(wide, NULL) == ERROR_SUCCESS);
    EXPECT(Allocations == 0 && ConversionCalls == 0);
    make_devmode(input.Bytes); memset(output.Bytes, 0xab, sizeof(output.Bytes));
    memcpy(original, input.Bytes, sizeof(original));
    error = RosConvertUnicodeDevModeToAnsiDevmode(wide, ansi);
    EXPECT(error == ERROR_SUCCESS && LiveAllocations == 0);
    EXPECT(!memcmp(input.Bytes, original, sizeof(original)));
    EXPECT(ansi->dmSize == sizeof(DEVMODEA) && ansi->dmDriverExtra == 16);
    EXPECT(ansi->dmSpecVersion == 0x401 && ansi->dmDriverVersion == 0x123);
    EXPECT(ansi->dmFields == DM_FORMNAME && ansi->dmOrientation == 2 && ansi->dmCopies == 3);
    EXPECT(ansi->dmLogPixels == 120 && ansi->dmPelsWidth == 1920 && ansi->dmPanningHeight == 1080);
    EXPECT(!strcmp((char *)ansi->dmDeviceName, "PR") && !strcmp((char *)ansi->dmFormName, "A4"));
    for (i = 0; i < 16; ++i) EXPECT(output.Bytes[sizeof(DEVMODEA)+i] == i+1);
    for (i = sizeof(DEVMODEA)+16; i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xab);
    memcpy(expected, output.Bytes, sizeof(expected));
    reset(); make_devmode(input.Bytes);
    EXPECT(RosConvertUnicodeDevModeToAnsiDevmode(wide, (DEVMODEA *)wide) == ERROR_SUCCESS);
    EXPECT(!memcmp(input.Bytes, expected, sizeof(expected)) && LiveAllocations == 0);
    for (failure = 1; failure <= 5; ++failure)
    {
        reset(); make_devmode(input.Bytes); memset(output.Bytes, 0xab, sizeof(output.Bytes));
        if (failure == 1) wide->dmSize = FIELD_OFFSET(DEVMODEW, dmFields)-1;
        if (failure == 2) FailAllocation = 1;
        if (failure == 3) FailConversion = 1;
        if (failure == 4) FailConversion = 2;
        if (failure == 5)
        {
            for (i = 0; i < 17; ++i) wide->dmDeviceName[i] = 0x4e2d;
            wide->dmDeviceName[17] = 0;
        }
        error = RosConvertUnicodeDevModeToAnsiDevmode(wide, ansi);
        EXPECT(error == (failure == 1 ? ERROR_INVALID_DATA : failure == 2 ? ERROR_NOT_ENOUGH_MEMORY : failure == 5 ? ERROR_INSUFFICIENT_BUFFER : ERROR_NO_UNICODE_TRANSLATION));
        EXPECT(GetLastError() == error && LiveAllocations == 0);
        for (i = 0; i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xab);
    }
    reset(); make_devmode(input.Bytes); memset(output.Bytes, 0xab, sizeof(output.Bytes));
    for (i = 0; i < 17; ++i) wide->dmFormName[i] = 0x4e2d;
    wide->dmFormName[17] = 0;
    EXPECT(RosConvertUnicodeDevModeToAnsiDevmode(wide, ansi) == ERROR_INSUFFICIENT_BUFFER);
    EXPECT(LiveAllocations == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    for (i = 0; i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xab);
    reset(); make_devmode(input.Bytes);
    wide->dmDeviceName[0] = 0x4e2d; wide->dmDeviceName[1] = 0;
    EXPECT(RosConvertUnicodeDevModeToAnsiDevmode(wide, ansi) == ERROR_SUCCESS);
    EXPECT(!memcmp(ansi->dmDeviceName, AnsiChinese, sizeof(AnsiChinese)) && LiveAllocations == 0);
}
static void devmode_api_tests(void)
{
    union { max_align_t Alignment; BYTE Bytes[8192]; } output;
    DWORD needed, returned, error;
    LONG result;
    unsigned api, failure, i;
    CaseName = "DEVMODE public caller failure propagation";
    for (api = 0; api < 7; ++api)
        for (failure = 0; failure <= 4; ++failure)
        {
            reset(); Records = 1; Sample = 6; IncludeDevmode = 1;
            CurrentLayout = api == 0 || api == 1 ? &Layouts[1] : api == 2 ? &Layouts[5] : api == 3 ? &Layouts[6] : &JobLayout;
            if (failure == 1) InvalidDevmode = 1;
            if (failure == 2) FailAllocation = api == 6 ? 2 : 1;
            if (failure == 3) FailConversion = 1;
            if (failure == 4) FailConversion = 2;
            memset(output.Bytes, 0xab, sizeof(output.Bytes));
            if (api == 0) result = GetPrinterA((HANDLE)2, 2, output.Bytes, sizeof(output.Bytes), &needed);
            else if (api == 1) result = EnumPrintersA(2,NULL,2,output.Bytes,sizeof(output.Bytes),&needed,&returned);
            else if (api == 2 || api == 3) result = GetPrinterA((HANDLE)2,api == 2 ? 8 : 9,output.Bytes,sizeof(output.Bytes),&needed);
            else if (api == 4) result = GetJobA((HANDLE)2,1,2,output.Bytes,sizeof(output.Bytes),&needed);
            else if (api == 5) result = EnumJobsA((HANDLE)2,0,1,2,output.Bytes,sizeof(output.Bytes),&needed,&returned);
            else result = DocumentPropertiesA(NULL,(HANDLE)2,NULL,(DEVMODEA *)output.Bytes,NULL,2);
            EXPECT(LiveAllocations == 0);
            if (!failure)
            {
                DEVMODEA *converted;
                EXPECT(result == 1);
                if (api == 6) converted = (DEVMODEA *)output.Bytes;
                else memcpy(&converted,output.Bytes+CurrentLayout->DevmodeOffset,sizeof(converted));
                EXPECT(converted->dmSize == sizeof(DEVMODEA) && converted->dmDriverExtra == 16);
                EXPECT(!strcmp((char *)converted->dmDeviceName,"PR"));
                for (i = 0; i < 16; ++i) EXPECT(((BYTE *)converted)[sizeof(DEVMODEA)+i] == i+1);
            }
            else
            {
                error = failure == 1 ? ERROR_INVALID_DATA : failure == 2 ? ERROR_NOT_ENOUGH_MEMORY : ERROR_NO_UNICODE_TRANSLATION;
                EXPECT(result == (api == 6 ? -1 : 0));
                EXPECT(GetLastError() == error);
                if (api == 6)
                    for (i = 0; i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xab);
            }
        }
}
static void consumer_api_tests(void)
{
    union { max_align_t Alignment; BYTE Bytes[8192]; } output;
    unsigned api, failure;
    DWORD needed, returned, error;
    BOOL result;
    char *text;
    CaseName="shared printer helper public consumers";
    for(api=0;api<5;++api)
        for(failure=0;failure<=4;++failure)
        {
            reset(); Records=1; Sample=2;
            CurrentLayout=api<2?&FormLayout:api<4?&DriverLayout:&AddJobLayout;
            if(failure==1) ForceWFailure=1;
            if(failure==2) FailAllocation=1;
            if(failure==3) FailConversion=1;
            if(failure==4) FailConversion=2;
            memset(output.Bytes,0xab,sizeof(output.Bytes));
            if(api==0) result=EnumFormsA((HANDLE)2,1,output.Bytes,sizeof(output.Bytes),&needed,&returned);
            else if(api==1) result=GetFormA((HANDLE)2,NULL,1,output.Bytes,sizeof(output.Bytes),&needed);
            else if(api==2) result=EnumPrinterDriversA(NULL,NULL,1,output.Bytes,sizeof(output.Bytes),&needed,&returned);
            else if(api==3) result=GetPrinterDriverA((HANDLE)2,NULL,1,output.Bytes,sizeof(output.Bytes),&needed);
            else result=AddJobA((HANDLE)2,1,output.Bytes,sizeof(output.Bytes),&needed);
            EXPECT(WCalls==1 && LiveAllocations==0);
            if(failure)
            {
                error=failure==1?ERROR_ACCESS_DENIED:failure==2?ERROR_NOT_ENOUGH_MEMORY:ERROR_NO_UNICODE_TRANSLATION;
                EXPECT(!result && GetLastError()==error);
            }
            else
            {
                EXPECT(result && GetLastError()==ERROR_SUCCESS && needed==LastNeeded);
                memcpy(&text,output.Bytes+CurrentLayout->Offsets[0],sizeof(text));
                EXPECT(!strcmp(text,(const char *)AnsiChinese));
            }
        }
}
'''


HELPER_TEST = r'''
    {
        union { max_align_t Alignment; BYTE Bytes[32]; } output;
        const unsigned char utf8[] = {0xe4,0xb8,0xad,0};
        reset(); EXPECT(CONVERTER(NULL) == ERROR_SUCCESS && Allocations == 0);
        memset(output.Bytes, 0xab, sizeof(output.Bytes)); memcpy(output.Bytes, Chinese, sizeof(Chinese));
        ActiveCodePage = 65001;
        EXPECT(CONVERTER((PWSTR)output.Bytes) == ERROR_SUCCESS);
        EXPECT(!memcmp(output.Bytes, utf8, sizeof(utf8)));
        for (i = sizeof(utf8); i < sizeof(output.Bytes); ++i) EXPECT(output.Bytes[i] == 0xab);
        EXPECT(LiveAllocations == 0);
    }
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--baseline', metavar='REF', help='use historical public entry points with current shared-converter fixtures to isolate output/error propagation regressions')
    parser.add_argument('--cc', default='cc')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--mutation', choices=('unsafe_devmode_null', 'ignored_devmode_name', 'ignored_ansi_failure', 'wide_form_keyword', 'ansi_form_keyword'))
    args = parser.parse_args()
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    source = (subprocess.check_output(['git', 'show', args.baseline + ':' + SOURCE], cwd=ROOT).decode()
              if args.baseline else (ROOT / SOURCE).read_text())
    header = (ROOT / 'sdk/include/psdk/winspool.h').read_text()
    gdi = (ROOT / 'sdk/include/psdk/wingdi.h').read_text()
    devmode_declarations = [
        '#define WINVER 0x0a00\n#define _WIN32_WINNT 0x0a00\n',
        '#define _ANONYMOUS_UNION\n#define _ANONYMOUS_STRUCT\n',
        '#define DUMMYSTRUCTNAME1\n#define DUMMYSTRUCTNAME2\n#define DUMMYUNIONNAME1\n#define DUMMYUNIONNAME2\n',
        'typedef struct { int32_t x, y; } POINTL;\n',
    ]
    for name in ('CCHDEVICENAME', 'CCHFORMNAME', 'DM_FORMNAME'):
        devmode_declarations.append(re.search(r'^#define\s+' + name + r'\b[^\n]*', gdi, re.MULTILINE).group() + '\n')
    for suffix in ('A', 'W'):
        devmode_declarations.append(re.search(r'typedef struct _devicemode' + suffix + r'\s*\{.*?\}\s*DEVMODE' + suffix + r'[^;]*;', gdi, re.DOTALL).group())
    declarations = []
    for level in (1, 2, 4, 5, 7, 8, 9):
        for suffix in ('A', 'W'):
            name = 'PRINTER_INFO_' + str(level) + suffix
            match = re.search(r'typedef struct _' + name + r'\s*\{.*?\}\s*' + name + r'[^;]*;', header, re.DOTALL)
            if not match:
                raise RuntimeError('Missing public printer structure: ' + name)
            declarations.append(match.group())
    system = (ROOT / 'sdk/include/psdk/minwinbase.h').read_text()
    declarations.insert(0, re.search(r'typedef struct _SYSTEMTIME\s*\{.*?\}\s*SYSTEMTIME[^;]*;', system, re.DOTALL).group())
    for name in ('JOB_INFO_1W', 'JOB_INFO_2A', 'JOB_INFO_2W', 'FORM_INFO_1W', 'FORM_INFO_2A', 'FORM_INFO_2W', 'DRIVER_INFO_1W', 'DRIVER_INFO_8W', 'ADDJOB_INFO_1W'):
        declarations.append(re.search(r'typedef struct _' + name + r'\s*\{.*?\}\s*' + name + r'[^;]*;', header, re.DOTALL).group())
    converter = '_ConvertPrinterStringToAnsiInPlace'
    helper = function(source, converter, 'static DWORD') if converter + '(' in source else ''
    if not helper:
        converter = 'UnicodeToAnsiInPlace'
        helper = function((ROOT / 'subsystems/win/win32k/printing/base/winspool/utils.c').read_text(), converter, 'static DWORD')
    extra = HELPER_TEST.replace('CONVERTER', converter) if helper else ''
    devmode = (ROOT / 'subsystems/win/win32k/printing/base/winspool/devmode.c').read_text()
    if args.mutation == 'unsafe_devmode_null':
        target = '    if (!pTmp)\n        return GetLastError();\n'
        assert devmode.count(target) == 1
        devmode = devmode.replace(target, '', 1)
    elif args.mutation == 'ignored_devmode_name':
        target = '    if (!WideCharToMultiByte(CP_ACP, 0, dmW->dmDeviceName, -1, (LPSTR)dmA->dmDeviceName, CCHDEVICENAME, NULL, NULL))\n        goto Failure;'
        assert devmode.count(target) == 1
        devmode = devmode.replace(target, '    WideCharToMultiByte(CP_ACP, 0, dmW->dmDeviceName, -1, (LPSTR)dmA->dmDeviceName, CCHDEVICENAME, NULL, NULL);', 1)
    elif args.mutation == 'ignored_ansi_failure':
        assert helper.count('if (!cbConverted)') == 1
        helper = helper.replace('if (!cbConverted)', 'if (0)', 1)
    provider = (ROOT/'subsystems/win/win32k/printing/providers/localspl/forms.c').read_text()
    if args.mutation == 'wide_form_keyword':
        provider = subprocess.check_output(['git','show',BASELINE+':subsystems/win/win32k/printing/providers/localspl/forms.c'],cwd=ROOT).decode()
    packing = function((ROOT/'subsystems/win/win32k/printing/base/spoolss/tools.c').read_text(),'PackStrings','static PBYTE WINAPI')
    packing += re.search(r'static DWORD dwFormInfo2Offsets\[\]\s*=\s*\{.*?\};',provider,re.DOTALL).group()
    packing += function(provider,'_LocalGetFormLevel2','static void')
    test = PREFIX + '\n'.join(devmode_declarations) + '\n'.join(declarations) + FORM_PROVIDER.replace('PROVIDER_IMPLEMENTATION',packing) + BODY + helper
    test += function(devmode, '_ConvertToDevmodeA', 'static DEVMODEA *')
    test += function(devmode, 'RosConvertUnicodeDevModeToAnsiDevmode', 'static DWORD')
    test += function(source, 'EnumPrintersA', 'static BOOL WINAPI')
    test += function(source, 'GetPrinterA', 'static BOOL WINAPI')
    test += function(source, 'DocumentPropertiesA', 'static LONG WINAPI')
    jobs_path = 'subsystems/win/win32k/printing/base/winspool/jobs.c'
    jobs = (subprocess.check_output(['git', 'show', args.baseline + ':' + jobs_path], cwd=ROOT).decode()
            if args.baseline else (ROOT / jobs_path).read_text())
    test += function(jobs, 'EnumJobsA', 'static BOOL WINAPI')
    test += function(jobs, 'GetJobA', 'static BOOL WINAPI')
    test += function(jobs, 'AddJobA', 'static BOOL WINAPI')
    for filename, names in (('forms.c', ('EnumFormsA','GetFormA','AddFormA','SetFormA')), ('printerdrivers.c',('EnumPrinterDriversA','GetPrinterDriverA'))):
        path = 'subsystems/win/win32k/printing/base/winspool/' + filename
        content = (subprocess.check_output(['git','show',args.baseline+':'+path],cwd=ROOT).decode()
                   if args.baseline else (ROOT/path).read_text())
        if filename=='forms.c' and args.mutation=='ansi_form_keyword':
            content=subprocess.check_output(['git','show',BASELINE+':'+path],cwd=ROOT).decode()
        for name in names:
            test += function(content,name,'static BOOL WINAPI')
    test += DEVMODE_TESTS + FORM_TESTS + TESTS.replace('EXTRA_TESTS', extra)
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        (path / 'test.c').write_text(test)
        command = [args.cc, '-std=c11', '-Wall', '-Wextra', '-Wno-unused-function', '-Wno-missing-field-initializers', '-fno-strict-aliasing', '-g', str(path / 'test.c'), '-o', str(path / 'test')]
        if args.sanitize:
            command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
        subprocess.run(command, check=True)
        return subprocess.run([str(path / 'test')]).returncode


if __name__ == '__main__':
    raise SystemExit(main())
