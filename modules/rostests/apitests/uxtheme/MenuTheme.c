/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test the MENU visual style class
 * COPYRIGHT:   Copyright 2026 Ostap Brehin <osbre@protonmail.com>
 */

#include <apitest.h>
#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

#define TEXT_WIDTH 64
#define TEXT_HEIGHT 24
#define TEXT_BACKGROUND 0x00202020

static void
test_text_color(HTHEME Theme, int Part, int State, COLORREF Expected)
{
    COLORREF Color = 0xdeadbe;
    HRESULT hr;

    hr = GetThemeColor(Theme, Part, State, TMT_TEXTCOLOR, &Color);
    ok(hr == S_OK, "part %d state %d: GetThemeColor returned 0x%08lx\n", Part, State, hr);
    if (GetSysColor(COLOR_MENUTEXT) == RGB(0, 0, 0))
        ok(Color == Expected, "part %d state %d: text color 0x%06lx, expected 0x%06lx\n", Part, State, Color, Expected);
}

static void
test_draw_text(HTHEME Theme, int Part, int State)
{
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), TEXT_WIDTH, -TEXT_HEIGHT, 1, 32, BI_RGB}};
    RECT Rect = {0, 0, TEXT_WIDTH, TEXT_HEIGHT};
    RECT ReferenceRect = Rect;
    DTTOPTS Options = {sizeof(Options)};
    HBITMAP Bitmap[2] = {NULL, NULL}, OldBitmap[2] = {NULL, NULL};
    DWORD *Bits[2] = {NULL, NULL};
    HDC Dc[2] = {NULL, NULL};
    LOGFONTW Font;
    HFONT ThemeFont = NULL;
    HGDIOBJ ReferenceFont, OldFont = NULL;
    HRESULT hr;
    int i, j, Ink = 0, Differences = 0, Height;

    for (i = 0; i < 2; i++)
    {
        Dc[i] = CreateCompatibleDC(NULL);
        ok(Dc[i] != NULL, "CreateCompatibleDC %d failed\n", i);
        if (!Dc[i])
            goto Cleanup;
        Bitmap[i] = CreateDIBSection(Dc[i], &Info, DIB_RGB_COLORS, (void **)&Bits[i], NULL, 0);
        ok(Bitmap[i] != NULL, "CreateDIBSection %d failed\n", i);
        if (!Bitmap[i])
            goto Cleanup;
        OldBitmap[i] = SelectObject(Dc[i], Bitmap[i]);
        ok(OldBitmap[i] != NULL && OldBitmap[i] != HGDI_ERROR, "SelectObject bitmap %d failed\n", i);
        if (!OldBitmap[i] || OldBitmap[i] == HGDI_ERROR)
            goto Cleanup;
        for (j = 0; j < TEXT_WIDTH * TEXT_HEIGHT; j++)
            Bits[i][j] = TEXT_BACKGROUND;
    }

    hr = GetThemeFont(Theme, Dc[0], Part, State, TMT_FONT, &Font);
    if (SUCCEEDED(hr))
    {
        ThemeFont = CreateFontIndirectW(&Font);
        ok(ThemeFont != NULL, "cannot create the themed reference font\n");
        if (!ThemeFont)
            goto Cleanup;
        ReferenceFont = ThemeFont;
    }
    else
        ReferenceFont = GetCurrentObject(Dc[0], OBJ_FONT);
    ok(ReferenceFont != NULL, "cannot obtain the reference font\n");
    if (!ReferenceFont)
        goto Cleanup;
    OldFont = SelectObject(Dc[1], ReferenceFont);
    ok(OldFont != NULL && OldFont != HGDI_ERROR, "cannot select the reference font\n");
    if (!OldFont || OldFont == HGDI_ERROR)
        goto Cleanup;

    Options.dwFlags = DTT_TEXTCOLOR;
    Options.crText = RGB(255, 255, 255);
    hr = DrawThemeTextEx(Theme, Dc[0], Part, State, L"File", -1, DT_SINGLELINE | DT_CENTER | DT_VCENTER, &Rect, &Options);
    ok(hr == S_OK, "part %d state %d: DrawThemeTextEx returned 0x%08lx\n", Part, State, hr);
    if (hr != S_OK)
        goto Cleanup;
    if (SetTextColor(Dc[1], Options.crText) == CLR_INVALID || !SetBkMode(Dc[1], TRANSPARENT))
    {
        ok(FALSE, "cannot configure the reference text colors\n");
        goto Cleanup;
    }
    Height = DrawTextW(Dc[1], L"File", -1, &ReferenceRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    ok(Height > 0, "part %d state %d: DrawTextW reference failed\n", Part, State);
    if (Height <= 0)
        goto Cleanup;
    GdiFlush();
    for (i = 0; i < TEXT_WIDTH * TEXT_HEIGHT; i++)
    {
        if ((Bits[1][i] & 0x00ffffff) != TEXT_BACKGROUND)
            Ink++;
        if ((Bits[0][i] & 0x00ffffff) != (Bits[1][i] & 0x00ffffff))
            Differences++;
    }
    ok(!Differences, "part %d state %d: themed text differs from the requested RGB reference at %d pixels\n",
       Part, State, Differences);
    ok(Ink > 0, "part %d state %d: reference text is empty\n", Part, State);
    ok(Ink < TEXT_WIDTH * TEXT_HEIGHT / 2, "part %d state %d: reference changed %d of %d pixels\n",
       Part, State, Ink, TEXT_WIDTH * TEXT_HEIGHT);

Cleanup:
    if (Dc[1] && OldFont && OldFont != HGDI_ERROR)
        SelectObject(Dc[1], OldFont);
    for (i = 0; i < 2; i++)
    {
        if (Dc[i])
        {
            if (OldBitmap[i] && OldBitmap[i] != HGDI_ERROR)
                SelectObject(Dc[i], OldBitmap[i]);
            DeleteDC(Dc[i]);
        }
        if (Bitmap[i])
            DeleteObject(Bitmap[i]);
    }
    if (ThemeFont)
        DeleteObject(ThemeFont);
}

START_TEST(MenuTheme)
{
    WNDCLASSW Class = {0};
    HTHEME Theme;
    HWND Window;

    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        return;
    }

    Class.lpfnWndProc = DefWindowProcW;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.lpszClassName = L"MenuThemeTest";
    RegisterClassW(&Class);
    Window = CreateWindowExW(0, Class.lpszClassName, L"MenuTheme", WS_OVERLAPPEDWINDOW,
                             0, 0, 200, 100, NULL, NULL, Class.hInstance, NULL);
    ok(Window != NULL, "CreateWindowExW failed: %lu\n", GetLastError());
    if (!Window)
        return;

    SetLastError(0xdeadbeef);
    Theme = OpenThemeData(Window, VSCLASS_MENU);
    ok(Theme != NULL, "OpenThemeData(MENU) failed: 0x%08lx\n", GetLastError());
    if (Theme)
    {
        ok(IsThemePartDefined(Theme, MENU_BARITEM, 0), "MENU_BARITEM is not defined\n");
        ok(IsThemePartDefined(Theme, MENU_POPUPITEM, 0), "MENU_POPUPITEM is not defined\n");

        test_text_color(Theme, MENU_BARITEM, MBI_NORMAL, RGB(0, 0, 0));
        test_text_color(Theme, MENU_BARITEM, MBI_HOT, RGB(0, 0, 0));
        test_text_color(Theme, MENU_BARITEM, MBI_PUSHED, RGB(0, 0, 0));
        test_text_color(Theme, MENU_BARITEM, MBI_DISABLED, RGB(0x6d, 0x6d, 0x6d));
        test_text_color(Theme, MENU_BARITEM, MBI_DISABLEDHOT, RGB(0x6d, 0x6d, 0x6d));
        test_text_color(Theme, MENU_BARITEM, MBI_DISABLEDPUSHED, RGB(0x6d, 0x6d, 0x6d));
        test_text_color(Theme, MENU_POPUPITEM, MPI_NORMAL, RGB(0, 0, 0));
        test_text_color(Theme, MENU_POPUPITEM, MPI_HOT, RGB(0, 0, 0));
        test_text_color(Theme, MENU_POPUPITEM, MPI_DISABLED, RGB(0x9f, 0x9f, 0x9f));
        test_text_color(Theme, MENU_POPUPITEM, MPI_DISABLEDHOT, RGB(0x9f, 0x9f, 0x9f));

        test_draw_text(Theme, MENU_BARITEM, MBI_NORMAL);
        test_draw_text(Theme, MENU_POPUPITEM, MPI_NORMAL);

        CloseThemeData(Theme);
    }

    DestroyWindow(Window);
    UnregisterClassW(Class.lpszClassName, Class.hInstance);
}
