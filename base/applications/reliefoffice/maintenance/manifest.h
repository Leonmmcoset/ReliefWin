/*
 * PROJECT:     ReliefOffice maintenance
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Fixed installed file and association manifest
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

static const WCHAR *const OfficeFiles[] =
{
    L"reliefword.exe",
    L"reliefexcel.exe",
    L"reliefppt.exe",
    L"relieftodo.exe",
    L"reliefdiagram.exe",
    L"reliefmathpp.exe",
    L"reliefmath.exe",
    L"reliefmath-render.exe",
    L"ReliefMath\\STIXTwoMath-Regular.otf",
    L"reliefoffice.inf",
    L"UPSTREAM_NOTICE",
    L"UPSTREAM_LICENSE",
    L"THIRD_PARTY_LICENSES.txt",
    L"reliefoffice-maintenance.exe",
};

typedef struct _OFFICE_APPLICATION
{
    PCWSTR Name;
    PCWSTR Executable;
    PCWSTR Extension;
    PCWSTR ProgId;
} OFFICE_APPLICATION;

static const OFFICE_APPLICATION OfficeApplications[] =
{
    {L"ReliefWord", L"reliefword.exe", L".rword", L"ReliefOffice.Word"},
    {L"ReliefExcel", L"reliefexcel.exe", L".rexl", L"ReliefOffice.Excel"},
    {L"ReliefPPT", L"reliefppt.exe", L".rppt", L"ReliefOffice.PPT"},
    {L"ReliefTodo", L"relieftodo.exe", L".rtodo", L"ReliefOffice.Todo"},
    {L"ReliefDiagram", L"reliefdiagram.exe", L".rdiagram", L"ReliefOffice.Diagram"},
    {L"ReliefMathPP", L"reliefmathpp.exe", L".rmathpp", L"ReliefOffice.MathPP"},
    {L"ReliefMath", L"reliefmath.exe", L".rmath", L"ReliefOffice.Math"},
};
