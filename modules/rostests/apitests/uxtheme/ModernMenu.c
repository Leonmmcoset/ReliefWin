/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Modern native menu appearance and public menu interaction contracts
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <apitest.h>
#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <dwmapi.h>

#define MENU_TEST_TIMER 71
#define MENU_FIRST 301
#define MENU_DISABLED 302
#define MENU_CHECKED 303
#define MENU_RADIO 304
#define MENU_LAST 305
#define MENU_CHILD_FIRST 401
#define MENU_CHILD_DISABLED 402
#define MENU_CHILD_LAST 403
#define MENU_BACKGROUND RGB(177, 57, 166)
#define MENU_SURFACE_WIDTH 768
#define MENU_SURFACE_HEIGHT 768
#define MENU_SENTINEL 0x00b139a6

typedef int (WINAPI *PSET_MENU_APP_MODE)(int);
typedef BOOLEAN (WINAPI *PALLOW_MENU_DARK)(HWND, BOOLEAN);

typedef struct
{
    HDC Dc;
    HBITMAP Bitmap;
    HBITMAP Old;
    DWORD *Pixels;
} MENU_SURFACE;

typedef struct
{
    HWND Root;
    HWND Child;
    HMENU RootMenu;
    HMENU ChildMenu;
    UINT Count;
} MENU_WINDOWS;

typedef struct
{
    UINT Key;
    BOOL ChildVisible;
} MENU_STEP;

typedef struct
{
    HWND Window;
    HBRUSH Background;
    HMENU Bar;
    HMENU Root;
    HMENU Child;
    BOOL RootAttached;
    BOOL ChildAttached;
    BOOL Running;
    BOOL TimedOut;
    BOOL Appearance;
    BOOL Showcase;
    BOOL Captured;
    BOOL Scroll;
    BOOL Rtl;
    BOOL Tiny;
    BOOL WorkArea;
    BOOL ScreenCheck;
    BOOL ScreenSampled;
    BOOL BackgroundChanged;
    UINT BackgroundChange;
    UINT Palette;
    UINT Step;
    UINT Ticks;
    UINT Commands;
    UINT LastCommand;
    UINT MenuSelections;
    UINT InitPopups;
    UINT UninitPopups;
    UINT RootCount;
    UINT DisabledState;
    DWORD Start;
    DWORD Duration;
    const MENU_STEP *Steps;
    UINT StepCount;
    MENU_SURFACE Before;
    MENU_SURFACE After;
    MENU_SURFACE Reference;
    DWORD Signature;
    COLORREF ScreenBefore;
    UINT ScreenPosition;
} MENU_TEST;

static DWORD MenuPixel(COLORREF Color)
{
    return RGB(GetBValue(Color), GetGValue(Color), GetRValue(Color));
}

static BOOL CreateMenuSurface(MENU_SURFACE *Surface)
{
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), MENU_SURFACE_WIDTH, -MENU_SURFACE_HEIGHT, 1, 32, BI_RGB}};

    ZeroMemory(Surface, sizeof(*Surface));
    Surface->Dc = CreateCompatibleDC(NULL);
    Surface->Bitmap = Surface->Dc ? CreateDIBSection(Surface->Dc, &Info, DIB_RGB_COLORS,
                                                    (void **)&Surface->Pixels, NULL, 0) : NULL;
    if (!Surface->Dc || !Surface->Bitmap)
        return FALSE;
    Surface->Old = SelectObject(Surface->Dc, Surface->Bitmap);
    return Surface->Old != NULL;
}

static void DestroyMenuSurface(MENU_SURFACE *Surface)
{
    if (Surface->Old)
        SelectObject(Surface->Dc, Surface->Old);
    if (Surface->Bitmap)
        DeleteObject(Surface->Bitmap);
    if (Surface->Dc)
        DeleteDC(Surface->Dc);
    ZeroMemory(Surface, sizeof(*Surface));
}

static void ClearMenuSurface(MENU_SURFACE *Surface, DWORD Color)
{
    UINT i;

    for (i = 0; i < MENU_SURFACE_WIDTH * MENU_SURFACE_HEIGHT; i++)
        Surface->Pixels[i] = Color;
}

static DWORD MenuSurfaceSignature(MENU_SURFACE *Surface, const RECT *Rect)
{
    DWORD Signature = 2166136261u;
    int x, y;

    for (y = Rect->top; y < Rect->bottom; y++)
        for (x = Rect->left; x < Rect->right; x++)
            Signature = (Signature ^ (Surface->Pixels[y * MENU_SURFACE_WIDTH + x] & 0x00ffffff)) * 16777619u;
    return Signature;
}

static BOOL CaptureNativeMenu(HWND Popup, MENU_SURFACE *Surface, RECT *Client)
{
    if (!GetClientRect(Popup, Client) || IsRectEmpty(Client) ||
        Client->right >= MENU_SURFACE_WIDTH || Client->bottom >= MENU_SURFACE_HEIGHT)
    {
        ok(FALSE, "native menu client is outside the test capture surface\n");
        return FALSE;
    }
    ClearMenuSurface(Surface, MENU_SENTINEL);
    SendMessageW(Popup, WM_PRINTCLIENT, (WPARAM)Surface->Dc, PRF_CLIENT | PRF_ERASEBKGND);
    GdiFlush();
    return TRUE;
}

static BOOL CALLBACK FindMenuWindow(HWND Window, LPARAM Parameter)
{
    MENU_WINDOWS *Windows = (MENU_WINDOWS *)Parameter;
    MENUBARINFO Info = {sizeof(Info)};
    WCHAR Name[32];

    if (!IsWindowVisible(Window) || !GetClassNameW(Window, Name, ARRAYSIZE(Name)) || wcscmp(Name, L"#32768"))
        return TRUE;
    if (!GetMenuBarInfo(Window, OBJID_CLIENT, 0, &Info))
        return TRUE;
    if (Info.hMenu == Windows->RootMenu)
        Windows->Root = Window;
    else if (Info.hMenu == Windows->ChildMenu)
        Windows->Child = Window;
    Windows->Count++;
    return TRUE;
}

static MENU_WINDOWS GetMenuWindows(MENU_TEST *Test)
{
    MENU_WINDOWS Windows = {0};

    Windows.RootMenu = Test->Root;
    Windows.ChildMenu = Test->Child;
    EnumThreadWindows(GetCurrentThreadId(), FindMenuWindow, (LPARAM)&Windows);
    return Windows;
}

static BOOL GetMenuClientScreenRect(HWND Popup, RECT *Client)
{
    if (!GetClientRect(Popup, Client))
        return FALSE;
    SetLastError(ERROR_SUCCESS);
    return MapWindowPoints(Popup, NULL, (POINT *)Client, 2) != 0 || GetLastError() == ERROR_SUCCESS;
}

static void CheckMenuPosition(HWND Popup, HMENU Menu, BOOL Scroll)
{
    MONITORINFO Monitor = {sizeof(Monitor)};
    RECT Window, Item, Previous = {0};
    HMONITOR Handle;
    int i, Count = GetMenuItemCount(Menu);

    if (!GetWindowRect(Popup, &Window))
    {
        ok(FALSE, "GetWindowRect for popup failed\n");
        return;
    }
    Handle = MonitorFromWindow(Popup, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(Handle, &Monitor))
    {
        ok(FALSE, "GetMonitorInfoW failed\n");
        return;
    }
    ok(Count >= 0, "GetMenuItemCount failed for tracked menu\n");
    ok(Window.left >= Monitor.rcWork.left && Window.top >= Monitor.rcWork.top &&
       Window.right <= Monitor.rcWork.right && Window.bottom <= Monitor.rcWork.bottom,
       "popup (%ld,%ld)-(%ld,%ld) is outside work area (%ld,%ld)-(%ld,%ld)\n",
       Window.left, Window.top, Window.right, Window.bottom,
       Monitor.rcWork.left, Monitor.rcWork.top, Monitor.rcWork.right, Monitor.rcWork.bottom);
    for (i = 0; i < Count; i++)
    {
        BOOL Result = GetMenuItemRect(NULL, Menu, i, &Item);

        ok(Result, "GetMenuItemRect(%u) failed: %lu\n", i, GetLastError());
        if (!Result)
            continue;
        ok(!IsRectEmpty(&Item) && (Scroll || (Item.left >= Window.left && Item.top >= Window.top &&
           Item.right <= Window.right && Item.bottom <= Window.bottom)),
           "item %u (%ld,%ld)-(%ld,%ld) is outside its menu\n",
           i, Item.left, Item.top, Item.right, Item.bottom);
        if (i)
            ok(Item.top >= Previous.bottom, "item %u overlaps the preceding row\n", i);
        Previous = Item;
    }
}

static void CheckMenuBarClientBounds(HWND Popup)
{
    MENUBARINFO Info = {sizeof(Info)};
    RECT Client;
    BOOL Result = GetMenuClientScreenRect(Popup, &Client) && GetMenuBarInfo(Popup, OBJID_CLIENT, 0, &Info);

    ok(Result, "cannot query the popup client and menu-bar rectangles\n");
    if (!Result)
        return;
    ok(EqualRect(&Client, &Info.rcBar),
       "popup client (%ld,%ld)-(%ld,%ld), GetMenuBarInfo (%ld,%ld)-(%ld,%ld)\n",
       Client.left, Client.top, Client.right, Client.bottom,
       Info.rcBar.left, Info.rcBar.top, Info.rcBar.right, Info.rcBar.bottom);
}

static void CheckMenuWindowRegion(HWND Popup)
{
    HRGN Region = CreateRectRgn(0, 0, 0, 0);
    RECT Window, Bounds;
    int Type, Width, Height;

    ok(Region != NULL, "cannot create the popup region query object\n");
    if (!Region)
        return;
    Type = GetWindowRgn(Popup, Region);
    if (Type == ERROR)
        trace("The popup has no application region; the DWM exterior clipping path is not tested by GetWindowRgn\n");
    else if (GetWindowRect(Popup, &Window))
    {
        Width = Window.right - Window.left;
        Height = Window.bottom - Window.top;
        GetRgnBox(Region, &Bounds);
        ok(Bounds.left == 0 && Bounds.top == 0 && Bounds.right == Width && Bounds.bottom == Height,
           "popup region (%ld,%ld)-(%ld,%ld) does not cover its %dx%d window\n",
           Bounds.left, Bounds.top, Bounds.right, Bounds.bottom, Width, Height);
        ok(!PtInRegion(Region, 0, 0) && !PtInRegion(Region, Width - 1, Height - 1),
           "popup region contains square exterior corners\n");
        ok(PtInRegion(Region, Width / 2, 0) && PtInRegion(Region, Width / 2, Height - 1) &&
           PtInRegion(Region, 0, Height / 2) && PtInRegion(Region, Width - 1, Height / 2),
           "popup region cuts off one or more straight side centers\n");
    }
    DeleteObject(Region);
}

static void CheckNativeMenuHitTests(MENU_TEST *Test, HWND Popup)
{
    RECT Item, Window;
    POINT Point;
    BOOL Result = GetMenuItemRect(NULL, Test->Root, 0, &Item) && GetWindowRect(Popup, &Window);

    ok(Result, "cannot obtain menu hit-test geometry\n");
    if (!Result)
        return;
    Point.y = (Item.top + Item.bottom) / 2;
    Point.x = Item.left;
    ok(MenuItemFromPoint(NULL, Test->Root, Point) == 0, "left item edge does not hit the first item\n");
    Point.x = Item.right - 1;
    ok(MenuItemFromPoint(NULL, Test->Root, Point) == 0, "right item edge does not hit the first item\n");
    Point.x = (Item.left + Item.right) / 2;
    Point.y = Item.top;
    ok(MenuItemFromPoint(NULL, Test->Root, Point) == 0, "top item edge does not hit the first item\n");
    Point.y = Item.bottom - 1;
    ok(MenuItemFromPoint(NULL, Test->Root, Point) == 0, "bottom item edge does not hit the first item\n");
    if (Item.top > Window.top)
    {
        Point.y = Window.top;
        ok(MenuItemFromPoint(NULL, Test->Root, Point) == -1, "popup top padding incorrectly hits an item\n");
    }
    if (GetMenuItemRect(NULL, Test->Root, Test->RootCount - 1, &Item) && Window.bottom > Item.bottom)
    {
        Point.y = Window.bottom - 1;
        ok(MenuItemFromPoint(NULL, Test->Root, Point) == -1, "popup bottom padding incorrectly hits an item\n");
    }
}

static BOOL GetMenuItemClientRect(HWND Popup, HMENU Menu, UINT Position, RECT *Rect)
{
    RECT Client;

    if (!GetMenuItemRect(NULL, Menu, Position, Rect) || !GetMenuClientScreenRect(Popup, &Client))
        return FALSE;
    OffsetRect(Rect, -Client.left, -Client.top);
    return TRUE;
}

static void CheckMenuBackgroundChange(MENU_TEST *Test, HWND Popup)
{
    DPI_AWARENESS_CONTEXT Previous;
    MENUINFO Info = {sizeof(Info)};
    RECT Window, Client, First, Last;
    LONG_PTR Style = GetWindowLongPtrW(Popup, GWL_STYLE);
    LONG_PTR ExStyle = GetWindowLongPtrW(Popup, GWL_EXSTYLE);
    BOOL Themed = Test->BackgroundChange == 1;
    int Margin;

    ok(Test->BackgroundChanged, "WM_INITMENUPOPUP did not change the root background\n");
    Info.fMask = MIM_BACKGROUND;
    ok(GetMenuInfo(Test->Root, &Info), "cannot query the changed popup background\n");
    ok(Info.hbrBack == (Themed ? NULL : Test->Background), "popup background differs from the callback result\n");
    ok(!!(Style & WS_BORDER) == !Themed && !!(ExStyle & WS_EX_DLGMODALFRAME) == !Themed,
       "callback background mode %u left popup style %Ix ex-style %Ix\n",
       Test->BackgroundChange, (UINT_PTR)Style, (UINT_PTR)ExStyle);
    Previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ok(Previous != NULL, "cannot enter physical coordinates for the changed popup frame\n");
    if (!Previous)
        return;
    Margin = MulDiv(6, GetDpiForWindow(Popup), 96);
    if (!GetWindowRect(Popup, &Window) || !GetMenuClientScreenRect(Popup, &Client) ||
        !GetMenuItemRect(NULL, Test->Root, 0, &First) ||
        !GetMenuItemRect(NULL, Test->Root, Test->RootCount - 1, &Last))
    {
        ok(FALSE, "cannot query the changed popup frame and item rectangles\n");
        goto Cleanup;
    }
    ok(First.left >= Client.left && First.top >= Client.top && First.right <= Client.right &&
       Last.left >= Client.left && Last.right <= Client.right && Last.bottom <= Client.bottom,
       "callback background mode %u placed items outside the client rectangle\n", Test->BackgroundChange);
    if (Themed)
    {
        ok(EqualRect(&Window, &Client), "callback cleared the background but retained a nonclient frame\n");
        ok(First.left - Client.left >= Margin && First.top - Client.top >= Margin &&
           Client.right - First.right >= Margin && Client.bottom - Last.bottom >= Margin,
           "callback cleared the background without restoring %d-pixel popup padding\n", Margin);
    }
    else
        ok(Client.left > Window.left && Client.top > Window.top &&
           Client.right < Window.right && Client.bottom < Window.bottom,
           "callback set a custom background without reserving its nonclient frame\n");

Cleanup:
    ok(SetThreadDpiAwarenessContext(Previous) != NULL, "cannot restore tracking DPI awareness after frame inspection\n");
}

static void CheckNativeMenuLayout(MENU_TEST *Test, HWND Popup, const RECT *Client)
{
    NONCLIENTMETRICSW Metrics = {sizeof(Metrics)};
    TEXTMETRICW Text;
    HDC Dc = GetDC(Popup);
    HFONT Font = NULL, Old = NULL;
    RECT Item = {0}, Last = {0}, Separator = {0};
    UINT Dpi = GetDpiForWindow(Popup);
    int Margin = MulDiv(6, Dpi, 96), Minimum = MulDiv(28, Dpi, 96);
    BOOL HaveText = FALSE;

    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(Metrics), &Metrics, 0))
        Font = CreateFontIndirectW(&Metrics.lfMenuFont);
    if (Dc && Font)
    {
        Old = SelectObject(Dc, Font);
        HaveText = GetTextMetricsW(Dc, &Text);
    }
    ok(GetMenuItemClientRect(Popup, Test->Root, 0, &Item), "cannot map first native menu item\n");
    ok(GetMenuItemClientRect(Popup, Test->Root, 6, &Last), "cannot map last native menu item\n");
    ok(GetMenuItemClientRect(Popup, Test->Root, 2, &Separator), "cannot map native separator\n");
    ok(Item.left >= Margin && Item.top >= Margin &&
       Client->right - Item.right >= Margin && Client->bottom - Last.bottom >= Margin,
       "palette %u popup padding: (%ld,%ld,%ld,%ld), expected at least %d\n", Test->Palette,
       Item.left, Item.top, Client->right - Item.right, Client->bottom - Last.bottom, Margin);
    ok(Item.bottom - Item.top >= Minimum, "palette %u row height %ld is below %d\n",
       Test->Palette, Item.bottom - Item.top, Minimum);
    if (HaveText)
        ok(Item.bottom - Item.top >= Text.tmHeight + 2 * Margin,
           "palette %u row height %ld does not reserve text height %ld + margins %d\n",
           Test->Palette, Item.bottom - Item.top, Text.tmHeight, Margin);
    ok(Separator.bottom - Separator.top == MulDiv(9, Dpi, 96),
       "palette %u separator height %ld, expected %d\n", Test->Palette,
       Separator.bottom - Separator.top, MulDiv(9, Dpi, 96));
    if (Old)
        SelectObject(Dc, Old);
    if (Font)
        DeleteObject(Font);
    if (Dc)
        ReleaseDC(Popup, Dc);
}

static void CheckNativeSeparator(MENU_TEST *Test, HWND Popup)
{
    RECT Separator;
    int x, y, Rows = 0;
    DWORD Background;

    if (!GetMenuItemClientRect(Popup, Test->Root, 2, &Separator) ||
        Separator.left < 0 || Separator.top < 0 || Separator.right > MENU_SURFACE_WIDTH ||
        Separator.bottom > MENU_SURFACE_HEIGHT || IsRectEmpty(&Separator))
    {
        ok(FALSE, "cannot inspect separator pixels\n");
        return;
    }
    x = (Separator.left + Separator.right) / 2;
    Background = Test->Before.Pixels[Separator.top * MENU_SURFACE_WIDTH + x] & 0x00ffffff;
    for (y = Separator.top; y < Separator.bottom; y++)
        if ((Test->Before.Pixels[y * MENU_SURFACE_WIDTH + x] & 0x00ffffff) != Background)
            Rows++;
    ok(Rows == 1, "palette %u separator has %d non-background rows, expected one\n", Test->Palette, Rows);
}

static BOOL FindMenuGlyph(MENU_SURFACE *Surface, const RECT *Search, DWORD Background, RECT *Bounds)
{
    BOOL Found = FALSE;
    int x, y;

    SetRectEmpty(Bounds);
    for (y = Search->top; y < Search->bottom; y++)
        for (x = Search->left; x < Search->right; x++)
            if ((Surface->Pixels[y * MENU_SURFACE_WIDTH + x] & 0x00ffffff) != Background)
            {
                if (!Found)
                    SetRect(Bounds, x, y, x + 1, y + 1);
                else
                {
                    Bounds->left = min(Bounds->left, x);
                    Bounds->top = min(Bounds->top, y);
                    Bounds->right = max(Bounds->right, x + 1);
                    Bounds->bottom = max(Bounds->bottom, y + 1);
                }
                Found = TRUE;
            }
    return Found;
}

static void CheckNativeMenuGlyph(MENU_TEST *Test, HWND Popup, UINT Position, int Part, int State)
{
    HTHEME Theme;
    RECT Item, Search, Actual, Expected, Target = {0, 0, 12, 12};
    DWORD Background;
    BOOL HaveActual, HaveExpected, Rtl = (GetWindowLongW(Popup, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    UINT Differences = 0;
    int x, y;

    if (GetDpiForWindow(Popup) != 96 || !GetMenuItemClientRect(Popup, Test->Root, Position, &Item) ||
        Item.left < 0 || Item.top < 0 || Item.right > MENU_SURFACE_WIDTH || Item.bottom > MENU_SURFACE_HEIGHT)
    {
        ok(FALSE, "native glyph comparison requires a valid menu at 96 DPI\n");
        return;
    }
    Theme = OpenThemeData(Test->Window, Test->Palette ? L"ContentLight::Menu" : L"Menu");
    ok(Theme != NULL, "cannot open the menu glyph reference\n");
    if (!Theme)
        return;
    ClearMenuSurface(&Test->Reference, MENU_SENTINEL);
    ok(DrawThemeBackground(Theme, Test->Reference.Dc, Part, State, &Target, NULL) == S_OK,
       "cannot draw the public Menu glyph reference\n");
    GdiFlush();
    CloseThemeData(Theme);
    Search = Item;
    if ((Part == MENU_POPUPCHECK) != Rtl)
        Search.right = min(Search.right, Search.left + 28);
    else
        Search.left = max(Search.left, Search.right - 24);
    Background = Test->Before.Pixels[Search.top * MENU_SURFACE_WIDTH + Search.left] & 0x00ffffff;
    HaveActual = FindMenuGlyph(&Test->Before, &Search, Background, &Actual);
    HaveExpected = FindMenuGlyph(&Test->Reference, &Target, MENU_SENTINEL, &Expected);
    ok(HaveActual && HaveExpected, "palette %u native glyph %u or its reference is empty\n", Test->Palette, Position);
    if (!HaveActual || !HaveExpected)
        return;
    ok(Actual.right - Actual.left == Expected.right - Expected.left &&
       Actual.bottom - Actual.top == Expected.bottom - Expected.top,
       "palette %u native glyph %u bounds %ldx%ld, expected %ldx%ld\n", Test->Palette, Position,
       Actual.right - Actual.left, Actual.bottom - Actual.top,
       Expected.right - Expected.left, Expected.bottom - Expected.top);
    if (Actual.right - Actual.left != Expected.right - Expected.left ||
        Actual.bottom - Actual.top != Expected.bottom - Expected.top)
        return;
    for (y = 0; y < Expected.bottom - Expected.top; y++)
        for (x = 0; x < Expected.right - Expected.left; x++)
        {
            BOOL Native = (Test->Before.Pixels[(Actual.top + y) * MENU_SURFACE_WIDTH + Actual.left + x] & 0x00ffffff) != Background;
            int ReferenceX = Part == MENU_POPUPSUBMENU && Rtl ? Expected.right - 1 - x : Expected.left + x;
            BOOL Reference = (Test->Reference.Pixels[(Expected.top + y) * MENU_SURFACE_WIDTH + ReferenceX] & 0x00ffffff) != MENU_SENTINEL;

            if (Native != Reference)
                Differences++;
        }
    ok(!Differences, "palette %u native glyph %u differs from the public theme mask at %u pixels\n",
       Test->Palette, Position, Differences);
}

static void CheckNativeMenuHighlight(MENU_TEST *Test, HWND Popup)
{
    RECT Client, Item;
    int x, y;

    if (!CaptureNativeMenu(Popup, &Test->After, &Client) ||
        !GetMenuItemClientRect(Popup, Test->Root, 0, &Item) || Item.left < 0 || Item.top < 0 ||
        Item.right > MENU_SURFACE_WIDTH || Item.bottom > MENU_SURFACE_HEIGHT || IsRectEmpty(&Item))
    {
        ok(FALSE, "cannot capture highlighted native menu item\n");
        return;
    }
    x = Item.left;
    y = Item.top;
    ok(Test->Before.Pixels[y * MENU_SURFACE_WIDTH + x] == Test->After.Pixels[y * MENU_SURFACE_WIDTH + x],
       "palette %u hot item changed its rounded top-left corner\n", Test->Palette);
    x = Item.left + MulDiv(5, GetDpiForWindow(Popup), 96);
    y = (Item.top + Item.bottom) / 2;
    ok(Test->Before.Pixels[y * MENU_SURFACE_WIDTH + x] != Test->After.Pixels[y * MENU_SURFACE_WIDTH + x],
       "palette %u hot item did not paint its left interior\n", Test->Palette);
    Test->Signature = MenuSurfaceSignature(&Test->After, &Client);
    trace("NATIVE_MENU palette=%u signature=%08lx client=%ldx%ld item=%ldx%ld\n",
          Test->Palette, Test->Signature, Client.right, Client.bottom,
          Item.right - Item.left, Item.bottom - Item.top);
}

static void CheckNativeMenuScroll(MENU_TEST *Test, HWND Popup)
{
    RECT Client, Item, Window;
    DWORD Background = MenuPixel(GetSysColor(COLOR_MENU));
    UINT i, Highlighted = 0, Differences = 0;
    POINT Arrow;
    int y;

    if (!CaptureNativeMenu(Popup, &Test->After, &Client) || Client.right < 8 || Client.bottom < 1)
    {
        ok(FALSE, "cannot capture scrolling menu\n");
        return;
    }
    ok(Client.bottom <= 180, "native scrolling menu height %ld exceeds its cyMax 180\n", Client.bottom);
    for (y = 8; y < Client.bottom - 8; y++)
    {
        if ((Test->After.Pixels[y * MENU_SURFACE_WIDTH + 2] & 0x00ffffff) != Background)
            Differences++;
        if ((Test->After.Pixels[y * MENU_SURFACE_WIDTH + Client.right - 3] & 0x00ffffff) != Background)
            Differences++;
    }
    ok(!Differences, "scroll step %u painted %u pixels in the left/right panel padding\n", Test->Step, Differences);
    if (Client.bottom >= 6)
        ok((Test->After.Pixels[2 * MENU_SURFACE_WIDTH + Client.right / 2] & 0x00ffffff) == Background &&
           (Test->After.Pixels[(Client.bottom - 3) * MENU_SURFACE_WIDTH + Client.right / 2] & 0x00ffffff) == Background,
           "scroll step %u painted top/bottom panel padding\n", Test->Step);
    if (!Test->Tiny && GetWindowRect(Popup, &Window))
    {
        Arrow.x = (Window.left + Window.right) / 2;
        Arrow.y = Window.top + MulDiv(6, GetDpiForWindow(Popup), 96);
        ok(MenuItemFromPoint(NULL, Test->Root, Arrow) == -1, "top scroll arrow incorrectly hits an item\n");
        Arrow.y = Window.bottom - MulDiv(6, GetDpiForWindow(Popup), 96) - 1;
        ok(MenuItemFromPoint(NULL, Test->Root, Arrow) == -1, "bottom scroll arrow incorrectly hits an item\n");
    }
    if (Test->Tiny)
    {
        Differences = 0;
        for (y = 0; y <= Client.bottom; y++)
            if ((Test->After.Pixels[y * MENU_SURFACE_WIDTH + Client.right] & 0x00ffffff) != MENU_SENTINEL)
                Differences++;
        for (i = 0; i < Client.right; i++)
            if ((Test->After.Pixels[Client.bottom * MENU_SURFACE_WIDTH + i] & 0x00ffffff) != MENU_SENTINEL)
                Differences++;
        ok(!Differences, "tiny menu height %ld painted %u pixels beyond its client rectangle\n", Client.bottom, Differences);
        trace("TINY_NATIVE_MENU client=%ldx%ld\n", Client.right, Client.bottom);
        return;
    }
    if (!Test->Step && GetMenuItemRect(NULL, Test->Root, 0, &Item) && GetWindowRect(Popup, &Window))
        ok(Item.top >= Window.top && Item.bottom <= Window.bottom,
           "reopened scrolling menu did not reset to the first row: (%ld,%ld), window (%ld,%ld)\n",
           Item.top, Item.bottom, Window.top, Window.bottom);
    for (i = 0; i < Test->RootCount; i++)
    {
        MENUITEMINFOW Info = {sizeof(Info)};
        POINT Point;

        Info.fMask = MIIM_STATE;
        if (!GetMenuItemInfoW(Test->Root, i, TRUE, &Info) || !(Info.fState & MFS_HILITE))
            continue;
        Highlighted++;
        if (!GetMenuItemRect(NULL, Test->Root, i, &Item))
        {
            ok(FALSE, "cannot obtain highlighted scrolling item %u\n", i);
            continue;
        }
        Point.x = (Item.left + Item.right) / 2;
        Point.y = (Item.top + Item.bottom) / 2;
        ok(MenuItemFromPoint(NULL, Test->Root, Point) == i,
           "scroll step %u highlighted item %u does not hit at its public rectangle center\n", Test->Step, i);
    }
    if (Test->Step)
        ok(Highlighted == 1, "scroll step %u has %u highlighted items\n", Test->Step, Highlighted);
}

static void PostMenuKey(HWND Window, UINT Key)
{
    ok(PostMessageW(Window, WM_KEYDOWN, Key, 1), "cannot post menu key %u\n", Key);
    ok(PostMessageW(Window, WM_KEYUP, Key, 0xc0000001), "cannot post menu key-up %u\n", Key);
}

static BOOL CheckNativeSelectionScreen(MENU_TEST *Test, HWND Popup)
{
    DPI_AWARENESS_CONTEXT Previous;
    MENUITEMINFOW Info = {sizeof(Info)};
    RECT Item, Client, ScreenClient;
    HDC Dc;
    POINT Point, ReferencePoint;
    COLORREF Pixel, Expected;
    BOOL Rtl, Ready = FALSE;
    UINT Dpi;

    if (Test->Step == 0)
        Test->ScreenPosition = Test->Steps[0].Key == VK_HOME ? 0 : Test->RootCount - 1;
    if (Test->Step > 1)
        return TRUE;
    if (Test->Step && !Test->ScreenSampled)
        return FALSE;
    Previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ok(Previous != NULL, "cannot enter physical coordinates for actual menu sampling\n");
    if (!Previous)
        return FALSE;
    if (!GetMenuItemRect(NULL, Test->Root, Test->ScreenPosition, &Item) ||
        !GetMenuClientScreenRect(Popup, &ScreenClient))
    {
        ok(FALSE, "cannot query the physical menu item rectangle\n");
        goto Cleanup;
    }
    Info.fMask = MIIM_STATE;
    if (!GetMenuItemInfoW(Test->Root, Test->ScreenPosition, TRUE, &Info))
    {
        ok(FALSE, "cannot query the public menu selection state\n");
        goto Cleanup;
    }
    if (Test->Step && !(Info.fState & MFS_HILITE))
    {
        if (Test->Ticks == 24)
            ok(FALSE, "public menu item %u did not become selected within the presentation limit\n", Test->ScreenPosition);
        goto Cleanup;
    }
    Dpi = GetDpiForWindow(Popup);
    Rtl = (GetWindowLongW(Popup, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    Point.x = Rtl ? Item.right - MulDiv(6, Dpi, 96) : Item.left + MulDiv(5, Dpi, 96);
    Point.y = (Item.top + Item.bottom) / 2;
    ReferencePoint.x = Point.x - ScreenClient.left;
    ReferencePoint.y = Point.y - ScreenClient.top;
    if (!CaptureNativeMenu(Popup, &Test->Reference, &Client) || !PtInRect(&Client, ReferencePoint))
    {
        ok(FALSE, "cannot capture the physical menu presentation reference\n");
        goto Cleanup;
    }
    Expected = GetPixel(Test->Reference.Dc, ReferencePoint.x, ReferencePoint.y);
    ok(Expected != CLR_INVALID, "cannot read the menu presentation reference\n");
    if (Expected == CLR_INVALID)
        goto Cleanup;
    if (Test->Step && Expected == Test->ScreenBefore)
    {
        if (Test->Ticks == 24)
            ok(FALSE, "selected menu item %u did not change its reference pixel within the presentation limit\n",
               Test->ScreenPosition);
        goto Cleanup;
    }
    GdiFlush();
    DwmFlush();
    Dc = GetDC(NULL);
    ok(Dc != NULL, "cannot obtain the actual screen for menu selection\n");
    if (!Dc)
        goto Cleanup;
    Pixel = GetPixel(Dc, Point.x, Point.y);
    ReleaseDC(NULL, Dc);
    ok(Pixel != CLR_INVALID, "cannot read actual menu pixel at (%ld,%ld)\n", Point.x, Point.y);
    Ready = Pixel != CLR_INVALID && Pixel == Expected;
    if (!Ready)
    {
        if (Test->Ticks == 24)
            ok(FALSE, "actual %u DPI %s menu presentation did not reach its reference at step %u (%ld,%ld): %06lx, expected %06lx\n",
               Dpi, Rtl ? "RTL" : "LTR", Test->Step, Point.x, Point.y, Pixel, Expected);
        goto Cleanup;
    }
    if (!Test->Step)
    {
        Test->ScreenBefore = Pixel;
        Test->ScreenSampled = TRUE;
    }
    else
        ok(Pixel != Test->ScreenBefore,
           "actual %u DPI %s menu selection did not change pixel (%ld,%ld) inside public item %u: %06lx\n",
           Dpi, Rtl ? "RTL" : "LTR", Point.x, Point.y, Test->ScreenPosition, Pixel);

Cleanup:
    ok(SetThreadDpiAwarenessContext(Previous) != NULL, "cannot restore menu tracking DPI awareness\n");
    return Ready;
}

static void AdvanceMenuTest(MENU_TEST *Test)
{
    MENU_WINDOWS Windows;
    RECT Client;

    if (!Test->Running)
        return;
    Test->Ticks++;
    if ((Test->Showcase && GetTickCount() - Test->Start >= Test->Duration) ||
        (!Test->Showcase && Test->Ticks > 24))
    {
        if (!Test->Showcase)
            Test->TimedOut = TRUE;
        EndMenu();
        PostMessageW(Test->Window, WM_CANCELMODE, 0, 0);
        return;
    }
    Windows = GetMenuWindows(Test);
    if (!Windows.Root)
    {
        if (!Test->Step)
            return;
        ok(FALSE, "native popup disappeared during step %u\n", Test->Step);
        EndMenu();
        return;
    }
    if (Test->Showcase)
    {
        if (Test->Step == 0)
            PostMenuKey(Test->Window, VK_END);
        else if (Test->Step == 1)
            PostMenuKey(Test->Window, VK_UP);
        else if (Test->Step == 2)
            PostMenuKey(Test->Window, VK_RIGHT);
        else if (Test->Step == 3)
            trace("MENU_SHOWCASE_READY root=%p child=%p duration=%lu\n", Windows.Root, Windows.Child, Test->Duration);
        Test->Step++;
        return;
    }
    if (Test->Step >= Test->StepCount)
        return;
    if (Test->ScreenCheck && !CheckNativeSelectionScreen(Test, Windows.Root))
        return;
    CheckMenuPosition(Windows.Root, Test->Root, Test->Scroll);
    CheckMenuBarClientBounds(Windows.Root);
    if (!Test->Step && !Test->Scroll)
        CheckNativeMenuHitTests(Test, Windows.Root);
    if (!Test->Step && Test->BackgroundChange)
        CheckMenuBackgroundChange(Test, Windows.Root);
    ok((Windows.Child != NULL) == Test->Steps[Test->Step].ChildVisible,
       "step %u child visibility is %d, expected %d\n", Test->Step,
       Windows.Child != NULL, Test->Steps[Test->Step].ChildVisible);
    if (Windows.Child)
    {
        CheckMenuPosition(Windows.Child, Test->Child, FALSE);
        CheckMenuBarClientBounds(Windows.Child);
    }
    if (Test->Appearance && !Test->Step && CaptureNativeMenu(Windows.Root, &Test->Before, &Client))
    {
        CheckNativeMenuLayout(Test, Windows.Root, &Client);
        CheckMenuWindowRegion(Windows.Root);
        CheckNativeSeparator(Test, Windows.Root);
        CheckNativeMenuGlyph(Test, Windows.Root, 3, MENU_POPUPCHECK, MC_CHECKMARKNORMAL);
        CheckNativeMenuGlyph(Test, Windows.Root, 4, MENU_POPUPCHECK, MC_BULLETNORMAL);
        CheckNativeMenuGlyph(Test, Windows.Root, 5, MENU_POPUPSUBMENU, MSM_NORMAL);
        Test->Captured = TRUE;
    }
    if (Test->Appearance && Test->Step == 1 && Test->Captured)
        CheckNativeMenuHighlight(Test, Windows.Root);
    if (Test->Scroll)
        CheckNativeMenuScroll(Test, Windows.Root);
    PostMenuKey(Test->Window, Test->Steps[Test->Step].Key);
    Test->Step++;
}

static LRESULT CALLBACK ModernMenuProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    MENU_TEST *Test = (MENU_TEST *)GetWindowLongPtrW(Window, GWLP_USERDATA);

    if (Message == WM_NCCREATE)
    {
        Test = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Test);
    }
    if (Test)
    {
        if (Message == WM_TIMER && WParam == MENU_TEST_TIMER)
        {
            AdvanceMenuTest(Test);
            return 0;
        }
        if (Message == WM_COMMAND)
        {
            Test->Commands++;
            Test->LastCommand = LOWORD(WParam);
            return 0;
        }
        if (Message == WM_MENUSELECT)
            Test->MenuSelections++;
        if (Message == WM_INITMENUPOPUP)
        {
            Test->InitPopups++;
            if ((HMENU)WParam == Test->Root && Test->BackgroundChange)
            {
                MENUINFO Info = {sizeof(Info)};

                Info.fMask = MIM_BACKGROUND;
                Info.hbrBack = Test->BackgroundChange == 2 ? Test->Background : NULL;
                Test->BackgroundChanged = SetMenuInfo(Test->Root, &Info);
                ok(Test->BackgroundChanged, "cannot change the background during WM_INITMENUPOPUP\n");
            }
        }
        if (Message == WM_UNINITMENUPOPUP)
            Test->UninitPopups++;
        if (Message == WM_ERASEBKGND)
        {
            RECT Client;

            GetClientRect(Window, &Client);
            FillRect((HDC)WParam, &Client, Test->Background);
            return TRUE;
        }
        if (Message == WM_PAINT)
        {
            PAINTSTRUCT Paint;
            RECT Client;

            BeginPaint(Window, &Paint);
            GetClientRect(Window, &Client);
            FillRect(Paint.hdc, &Client, Test->Background);
            if (Test->Showcase)
            {
                SetBkMode(Paint.hdc, TRANSPARENT);
                SetTextColor(Paint.hdc, RGB(255, 255, 255));
                TextOutW(Paint.hdc, 24, 24, L"ReliefWin 原生菜单", ARRAYSIZE(L"ReliefWin 原生菜单") - 1);
            }
            EndPaint(Window, &Paint);
            return 0;
        }
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

static BOOL CreateModernMenuTest(MENU_TEST *Test, BOOL Showcase)
{
    WNDCLASSW Class = {0};
    MENUITEMINFOW Item = {sizeof(Item)};

    ZeroMemory(Test, sizeof(*Test));
    Test->Showcase = Showcase;
    Test->Duration = 120000;
    Test->RootCount = 7;
    Test->Background = CreateSolidBrush(Showcase ? RGB(32, 32, 32) : MENU_BACKGROUND);
    Class.lpfnWndProc = ModernMenuProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    Class.lpszClassName = L"ReliefWinModernMenuTest";
    ok(RegisterClassW(&Class) != 0, "cannot register modern menu test window\n");
    Test->Window = CreateWindowExW(WS_EX_TOPMOST, Class.lpszClassName, L"ReliefWin 菜单体验",
                                   WS_OVERLAPPEDWINDOW | WS_VISIBLE, 160, 100, 850, 600,
                                   NULL, NULL, Class.hInstance, Test);
    Test->Root = CreatePopupMenu();
    Test->Child = CreatePopupMenu();
    Test->Bar = CreateMenu();
    ok(Test->Window && Test->Root && Test->Child && Test->Bar && Test->Background,
       "cannot allocate native menu test objects\n");
    if (!Test->Window || !Test->Root || !Test->Child || !Test->Bar || !Test->Background)
        return FALSE;
    ok(AppendMenuW(Test->Child, MF_STRING, MENU_CHILD_FIRST, L"第一个子命令(&F)"), "append child first failed\n");
    ok(AppendMenuW(Test->Child, MF_STRING | MF_GRAYED, MENU_CHILD_DISABLED, L"暂不可用(&D)"), "append child disabled failed\n");
    ok(AppendMenuW(Test->Child, MF_STRING, MENU_CHILD_LAST, L"最后一个子命令(&L)"), "append child last failed\n");
    ok(AppendMenuW(Test->Root, MF_STRING, MENU_FIRST, L"打开(&O)\tCtrl+O"), "append first failed\n");
    ok(AppendMenuW(Test->Root, MF_STRING | MF_GRAYED, MENU_DISABLED, L"暂不可用(&D)"), "append disabled failed\n");
    Item.fMask = MIIM_STATE;
    ok(GetMenuItemInfoW(Test->Root, MENU_DISABLED, FALSE, &Item), "cannot inspect fixture disabled state\n");
    Test->DisabledState = Item.fState & MFS_DISABLED;
    ok(Test->DisabledState != 0, "fixture disabled command is enabled\n");
    ok(AppendMenuW(Test->Root, MF_SEPARATOR, 0, NULL), "append separator failed\n");
    ok(AppendMenuW(Test->Root, MF_STRING | MF_CHECKED, MENU_CHECKED, L"显示状态栏(&S)"), "append checked failed\n");
    ok(AppendMenuW(Test->Root, MF_STRING | MF_CHECKED, MENU_RADIO, L"当前视图(&V)"), "append radio failed\n");
    Item.fMask = MIIM_FTYPE;
    Item.fType = MFT_RADIOCHECK;
    ok(SetMenuItemInfoW(Test->Root, MENU_RADIO, FALSE, &Item), "set radio type failed\n");
    Test->ChildAttached = AppendMenuW(Test->Root, MF_POPUP, (UINT_PTR)Test->Child, L"更多选项(&M)");
    ok(Test->ChildAttached, "append submenu failed\n");
    ok(AppendMenuW(Test->Root, MF_STRING, MENU_LAST, L"属性(&P)\tAlt+Enter"), "append last failed\n");
    ok(SetMenuDefaultItem(Test->Root, MENU_FIRST, FALSE), "set default command failed\n");
    Test->RootAttached = AppendMenuW(Test->Bar, MF_POPUP, (UINT_PTR)Test->Root, L"文件(&F)");
    ok(Test->RootAttached, "append menu bar popup failed\n");
    ok(AppendMenuW(Test->Bar, MF_STRING, 501, L"编辑(&E)"), "append second menu bar item failed\n");
    ok(SetMenu(Test->Window, Test->Bar), "cannot attach native menu bar\n");
    SetForegroundWindow(Test->Window);
    DrawMenuBar(Test->Window);
    UpdateWindow(Test->Window);
    {
        HDC Dc = GetDC(Test->Window);

        trace("MENU_DPI system=%u window=%u gdi=%d\n", GetDpiForSystem(), GetDpiForWindow(Test->Window),
              Dc ? GetDeviceCaps(Dc, LOGPIXELSX) : 0);
        if (Dc)
            ReleaseDC(Test->Window, Dc);
    }
    return Test->RootAttached && Test->ChildAttached;
}

static void DestroyModernMenuTest(MENU_TEST *Test)
{
    if (Test->Window)
    {
        KillTimer(Test->Window, MENU_TEST_TIMER);
        SetMenu(Test->Window, NULL);
        DestroyWindow(Test->Window);
    }
    if (Test->Bar)
        DestroyMenu(Test->Bar);
    if (Test->Root && !Test->RootAttached)
        DestroyMenu(Test->Root);
    if (Test->Child && !Test->ChildAttached)
        DestroyMenu(Test->Child);
    if (Test->Background)
        DeleteObject(Test->Background);
    DestroyMenuSurface(&Test->Before);
    DestroyMenuSurface(&Test->After);
    DestroyMenuSurface(&Test->Reference);
    UnregisterClassW(L"ReliefWinModernMenuTest", GetModuleHandleW(NULL));
}

static void CheckMenuState(MENU_TEST *Test)
{
    MENUITEMINFOW Item = {sizeof(Item)};

    ok(GetMenuItemCount(Test->Root) == Test->RootCount, "root item count changed\n");
    ok(GetMenuItemCount(Test->Child) == 3, "child item count changed\n");
    Item.fMask = MIIM_STATE;
    ok(GetMenuItemInfoW(Test->Root, MENU_DISABLED, FALSE, &Item), "cannot inspect disabled state\n");
    ok((Item.fState & MFS_DISABLED) != 0 && (Item.fState & MFS_DISABLED) == Test->DisabledState,
       "disabled command state changed from %x to %x\n", Test->DisabledState, Item.fState & MFS_DISABLED);
    ok(GetMenuItemInfoW(Test->Root, MENU_CHECKED, FALSE, &Item), "cannot inspect checked state\n");
    ok((Item.fState & MFS_CHECKED) != 0, "checked state was lost\n");
    Item.fMask = MIIM_STATE | MIIM_FTYPE;
    ok(GetMenuItemInfoW(Test->Root, MENU_RADIO, FALSE, &Item), "cannot inspect radio state\n");
    ok((Item.fState & MFS_CHECKED) && (Item.fType & MFT_RADIOCHECK), "radio state was lost\n");
    ok(GetMenuDefaultItem(Test->Root, FALSE, 0) == MENU_FIRST, "default command changed\n");
}

static UINT TrackModernMenu(MENU_TEST *Test, const MENU_STEP *Steps, UINT Count, POINT Point)
{
    MENU_WINDOWS Windows;
    UINT Result, Commands = Test->Commands;

    Test->Steps = Steps;
    Test->StepCount = Count;
    Test->Step = 0;
    Test->Ticks = 0;
    Test->Captured = FALSE;
    Test->ScreenSampled = FALSE;
    Test->BackgroundChanged = FALSE;
    Test->TimedOut = FALSE;
    Test->Running = TRUE;
    Test->Start = GetTickCount();
    if (!SetTimer(Test->Window, MENU_TEST_TIMER, Test->Showcase ? 180 : 75, NULL))
    {
        ok(FALSE, "cannot schedule menu inspection timer\n");
        Test->Running = FALSE;
        return 0;
    }
    Result = TrackPopupMenuEx(Test->Root, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NOANIMATION | (Test->Rtl ? TPM_LAYOUTRTL : 0) |
                             (Test->WorkArea ? TPM_WORKAREA : 0), Point.x, Point.y,
                             Test->Window, NULL);
    Test->Running = FALSE;
    KillTimer(Test->Window, MENU_TEST_TIMER);
    ok(!Test->TimedOut, "native menu tracking exceeded its deterministic step limit\n");
    if (!Test->Showcase)
    {
        ok(Test->Step == Count, "menu consumed %u of %u scheduled key steps\n", Test->Step, Count);
        ok(Test->Commands == Commands, "TPM_RETURNCMD sent an unexpected WM_COMMAND\n");
    }
    Windows = GetMenuWindows(Test);
    ok(!Windows.Root && !Windows.Child, "popup windows survived the tracking loop\n");
    CheckMenuState(Test);
    return Result;
}

static void CheckNativeMenuBar(MENU_TEST *Test)
{
    MENUBARINFO Info = {sizeof(Info)};
    RECT First = {0}, Second = {0};

    ok(GetMenuBarInfo(Test->Window, OBJID_MENU, 0, &Info), "cannot query menu bar\n");
    ok(Info.hMenu == Test->Bar && !IsRectEmpty(&Info.rcBar), "invalid menu bar metadata\n");
    ok(GetMenuItemRect(Test->Window, Test->Bar, 0, &First), "cannot query first menu bar item\n");
    ok(GetMenuItemRect(Test->Window, Test->Bar, 1, &Second), "cannot query second menu bar item\n");
    ok(First.left >= Info.rcBar.left && First.top >= Info.rcBar.top &&
       Second.right <= Info.rcBar.right && Second.bottom <= Info.rcBar.bottom,
       "menu bar items lie outside the bar\n");
    ok(First.right <= Second.left, "menu bar items overlap\n");
}

START_TEST(MenuInteraction)
{
    static const MENU_STEP First[] = {{VK_HOME, FALSE}, {VK_RETURN, FALSE}};
    static const MENU_STEP Last[] = {{VK_END, FALSE}, {VK_RETURN, FALSE}};
    static const MENU_STEP Child[] = {{VK_END, FALSE}, {VK_UP, FALSE}, {VK_RIGHT, FALSE},
                                       {VK_END, TRUE}, {VK_RETURN, TRUE}};
    static const MENU_STEP Cancel[] = {{VK_HOME, FALSE}, {VK_ESCAPE, FALSE}};
    static const MENU_STEP Back[] = {{VK_END, FALSE}, {VK_UP, FALSE}, {VK_RIGHT, FALSE},
                                      {VK_LEFT, TRUE}, {VK_ESCAPE, FALSE}};
    static const MENU_STEP RtlChild[] = {{VK_END, FALSE}, {VK_UP, FALSE}, {VK_RETURN, FALSE},
                                          {VK_END, TRUE}, {VK_RETURN, TRUE}};
    MENU_TEST Test;
    MENUINFO Info = {sizeof(Info)};
    MONITORINFO Monitor = {sizeof(Monitor)};
    POINT Point;
    UINT i, Result, Init, Uninit;

    trace("MenuInteraction uses public menu contracts; a Windows 11 baseline remains unverified\n");
    if (!CreateModernMenuTest(&Test, FALSE))
        goto Cleanup;
    if (!CreateMenuSurface(&Test.Reference))
    {
        ok(FALSE, "cannot allocate the menu presentation reference\n");
        goto Cleanup;
    }
    Test.ScreenCheck = TRUE;
    CheckNativeMenuBar(&Test);
    GetMonitorInfoW(MonitorFromWindow(Test.Window, MONITOR_DEFAULTTONEAREST), &Monitor);
    Point.x = Monitor.rcWork.left + 160;
    Point.y = Monitor.rcWork.top + 180;
    Result = TrackModernMenu(&Test, First, ARRAYSIZE(First), Point);
    ok(Result == MENU_FIRST, "Home/Enter returned %u, expected %u\n", Result, MENU_FIRST);
    Result = TrackModernMenu(&Test, Last, ARRAYSIZE(Last), Point);
    ok(Result == MENU_LAST, "End/Enter returned %u, expected %u\n", Result, MENU_LAST);
    for (i = 0; i < 4; i++)
    {
        Point.x = i & 1 ? Monitor.rcWork.right - 1 : Monitor.rcWork.left;
        Point.y = i & 2 ? Monitor.rcWork.bottom - 1 : Monitor.rcWork.top;
        Init = Test.InitPopups;
        Uninit = Test.UninitPopups;
        Result = TrackModernMenu(&Test, Child, ARRAYSIZE(Child), Point);
        ok(Result == MENU_CHILD_LAST, "corner %u submenu command returned %u, expected %u\n",
           i, Result, MENU_CHILD_LAST);
        ok(Test.InitPopups >= Init + 2 && Test.UninitPopups >= Uninit + 2,
           "corner %u did not initialize and uninitialize root and child\n", i);
    }
    Result = TrackModernMenu(&Test, Cancel, ARRAYSIZE(Cancel), Point);
    ok(Result == 0, "Escape returned command %u\n", Result);
    Result = TrackModernMenu(&Test, Back, ARRAYSIZE(Back), Point);
    ok(Result == 0, "Left/Escape returned command %u\n", Result);
    Test.WorkArea = TRUE;
    Point.x = Monitor.rcWork.left + (Monitor.rcWork.right - Monitor.rcWork.left) / 2;
    Point.y = Monitor.rcMonitor.bottom - 1;
    Result = TrackModernMenu(&Test, Cancel, ARRAYSIZE(Cancel), Point);
    ok(Result == 0, "TPM_WORKAREA taskbar anchor Escape returned command %u\n", Result);
    Test.WorkArea = FALSE;
    trace("Dynamic popup background checks use ReliefWin's product frame and padding rules\n");
    Point.x = Monitor.rcWork.left + 160;
    Point.y = Monitor.rcWork.top + 180;
    Info.fMask = MIM_BACKGROUND;
    for (i = 0; i < 2; i++)
    {
        Info.hbrBack = i ? NULL : Test.Background;
        ok(SetMenuInfo(Test.Root, &Info), "cannot set initial popup background for case %u\n", i);
        Test.BackgroundChange = i + 1;
        Init = Test.InitPopups;
        Uninit = Test.UninitPopups;
        Result = TrackModernMenu(&Test, Cancel, ARRAYSIZE(Cancel), Point);
        ok(Result == 0, "background callback case %u Escape returned command %u\n", i, Result);
        ok(Test.InitPopups == Init + 1 && Test.UninitPopups == Uninit + 1,
           "background callback case %u did not initialize and uninitialize the root exactly once\n", i);
    }
    Test.BackgroundChange = 0;
    Info.hbrBack = NULL;
    ok(SetMenuInfo(Test.Root, &Info), "cannot clear the callback fixture background\n");
    Test.Rtl = TRUE;
    for (i = 0; i < 2; i++)
    {
        Point.x = i ? Monitor.rcWork.right - 1 : Monitor.rcWork.left;
        Result = TrackModernMenu(&Test, RtlChild, ARRAYSIZE(RtlChild), Point);
        ok(Result == MENU_CHILD_LAST, "RTL edge %u submenu command returned %u, expected %u\n",
           i, Result, MENU_CHILD_LAST);
    }
    Test.Rtl = FALSE;
    SetWindowLongPtrW(Test.Window, GWL_EXSTYLE, GetWindowLongPtrW(Test.Window, GWL_EXSTYLE) | WS_EX_LAYOUTRTL);
    Result = TrackModernMenu(&Test, RtlChild, ARRAYSIZE(RtlChild), Point);
    ok(Result == MENU_CHILD_LAST, "owner-only RTL submenu command returned %u, expected %u\n", Result, MENU_CHILD_LAST);
    ok(Test.MenuSelections != 0, "menu selection notifications were not delivered\n");

Cleanup:
    DestroyModernMenuTest(&Test);
}

START_TEST(NativeMenuAppearance)
{
    static const MENU_STEP Steps[] = {{VK_HOME, FALSE}, {VK_ESCAPE, FALSE}};
    static const INT Indices[] = {COLOR_MENU, COLOR_MENUTEXT, COLOR_GRAYTEXT, COLOR_MENUBAR};
    static const COLORREF Colors[][4] =
    {
        {RGB(32, 32, 32), RGB(240, 240, 240), RGB(128, 128, 128), RGB(32, 32, 32)},
        {RGB(249, 249, 249), RGB(0, 0, 0), RGB(159, 159, 159), RGB(249, 249, 249)}
    };
    MENU_TEST Test;
    POINT Point = {240, 240};
    DWORD Signatures[2] = {0};
    COLORREF Previous[4];
    BOOL Changed = FALSE;
    UINT Palette, i;

    trace("NativeMenuAppearance checks ReliefWin's own menu geometry and pixels, not Windows 11 style parity\n");
    if (!CreateModernMenuTest(&Test, FALSE))
        goto Cleanup;
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        goto Cleanup;
    }
    ok(CreateMenuSurface(&Test.Before) && CreateMenuSurface(&Test.After) && CreateMenuSurface(&Test.Reference),
       "cannot allocate native menu surfaces\n");
    if (!Test.Before.Pixels || !Test.After.Pixels || !Test.Reference.Pixels)
        goto Cleanup;
    Test.Appearance = TRUE;
    for (i = 0; i < ARRAYSIZE(Indices); i++)
        Previous[i] = GetSysColor(Indices[i]);
    Changed = TRUE;
    for (Palette = 0; Palette < 2; Palette++)
    {
        Test.Palette = Palette;
        ok(SetSysColors(ARRAYSIZE(Indices), Indices, Colors[Palette]), "cannot set native menu palette %u\n", Palette);
        TrackModernMenu(&Test, Steps, ARRAYSIZE(Steps), Point);
        ok(Test.Captured, "palette %u did not capture its native popup\n", Palette);
        Signatures[Palette] = Test.Signature;
        SetWindowLongPtrW(Test.Window, GWL_EXSTYLE, GetWindowLongPtrW(Test.Window, GWL_EXSTYLE) | WS_EX_LAYOUTRTL);
        TrackModernMenu(&Test, Steps, ARRAYSIZE(Steps), Point);
        SetWindowLongPtrW(Test.Window, GWL_EXSTYLE, GetWindowLongPtrW(Test.Window, GWL_EXSTYLE) & ~WS_EX_LAYOUTRTL);
    }
    ok(Signatures[0] && Signatures[1] && Signatures[0] != Signatures[1],
       "native menu palette signatures did not differ: %08lx/%08lx\n", Signatures[0], Signatures[1]);

Cleanup:
    if (Changed)
    {
        ok(SetSysColors(ARRAYSIZE(Indices), Indices, Previous), "cannot restore system menu colors\n");
        for (i = 0; i < ARRAYSIZE(Indices); i++)
            ok(GetSysColor(Indices[i]) == Previous[i], "system menu color %d did not restore\n", Indices[i]);
    }
    DestroyModernMenuTest(&Test);
}

START_TEST(NativeMenuScroll)
{
    static const UINT TinyHeights[] = {1, 11, 12, 15, 32};
    static const MENU_STEP TinySteps[] = {{VK_ESCAPE, FALSE}};
    static const MENU_STEP Steps[] =
    {
        {VK_HOME, FALSE}, {VK_END, FALSE}, {VK_UP, FALSE}, {VK_DOWN, FALSE},
        {VK_HOME, FALSE}, {VK_DOWN, FALSE}, {VK_END, FALSE}, {VK_RETURN, FALSE}
    };
    MENU_TEST Test;
    MENUINFO Info = {sizeof(Info)};
    POINT Point = {240, 240};
    UINT i, Pass, Result;
    WCHAR Text[32];

    trace("NativeMenuScroll checks ReliefWin's padded scrolling menu drawing and public hit testing\n");
    if (!CreateModernMenuTest(&Test, FALSE))
        goto Cleanup;
    ok(CreateMenuSurface(&Test.After), "cannot allocate scroll capture surface\n");
    if (!Test.After.Pixels)
        goto Cleanup;
    for (i = 0; i < 48; i++)
    {
        wsprintfW(Text, L"额外命令 %u", i + 1);
        ok(AppendMenuW(Test.Root, MF_STRING, 601 + i, Text), "cannot append scrolling command %u\n", i);
    }
    Test.RootCount += 48;
    Info.fMask = MIM_MAXHEIGHT;
    Info.cyMax = 180;
    ok(SetMenuInfo(Test.Root, &Info), "cannot limit native menu height\n");
    Test.Scroll = TRUE;
    for (Pass = 0; Pass < 3; Pass++)
    {
        Result = TrackModernMenu(&Test, Steps, ARRAYSIZE(Steps), Point);
        ok(Result == 648, "scroll repeat %u returned %u instead of the last command 648\n", Pass, Result);
    }
    Test.Tiny = TRUE;
    for (i = 0; i < ARRAYSIZE(TinyHeights); i++)
    {
        Info.cyMax = TinyHeights[i];
        ok(SetMenuInfo(Test.Root, &Info), "cannot set cyMax=%u\n", TinyHeights[i]);
        Result = TrackModernMenu(&Test, TinySteps, ARRAYSIZE(TinySteps), Point);
        ok(Result == 0, "tiny menu cyMax=%u returned command %u\n", TinyHeights[i], Result);
    }

Cleanup:
    DestroyModernMenuTest(&Test);
}

static DWORD DrawMenuPart(MENU_SURFACE *Surface, HTHEME Theme, int Part, int State,
                          const RECT *Rect, BOOL Rounded, BOOL Empty)
{
    HRESULT Result;
    UINT Changed = 0, Outside = 0;
    int x, y;

    ClearMenuSurface(Surface, MENU_SENTINEL);
    Result = DrawThemeBackground(Theme, Surface->Dc, Part, State, Rect, NULL);
    ok(Result == S_OK, "menu part %d state %d draw returned %08lx\n", Part, State, Result);
    GdiFlush();
    for (y = Rect->top - 1; y <= Rect->bottom; y++)
        for (x = Rect->left - 1; x <= Rect->right; x++)
            if ((Surface->Pixels[y * MENU_SURFACE_WIDTH + x] & 0x00ffffff) != MENU_SENTINEL)
            {
                if (x < Rect->left || x >= Rect->right || y < Rect->top || y >= Rect->bottom)
                    Outside++;
                else
                    Changed++;
            }
    ok(!Outside, "menu part %d state %d drew %u outside pixels\n", Part, State, Outside);
    ok(Empty ? !Changed : Changed != 0, "menu part %d state %d changed %u pixels\n", Part, State, Changed);
    if (Rounded)
    {
        ok((Surface->Pixels[Rect->top * MENU_SURFACE_WIDTH + Rect->left] & 0x00ffffff) == MENU_SENTINEL &&
           (Surface->Pixels[(Rect->bottom - 1) * MENU_SURFACE_WIDTH + Rect->right - 1] & 0x00ffffff) == MENU_SENTINEL,
           "menu part %d state %d did not leave transparent opposite corners\n", Part, State);
        if (Part != MENU_POPUPBORDERS)
            ok((Surface->Pixels[((Rect->top + Rect->bottom) / 2) * MENU_SURFACE_WIDTH +
                                (Rect->left + Rect->right) / 2] & 0x00ffffff) != MENU_SENTINEL,
               "menu part %d state %d has an empty interior\n", Part, State);
    }
    return MenuSurfaceSignature(Surface, Rect);
}

START_TEST(MenuAppearance)
{
    static const int Sizes[][2] = {{180, 160}, {320, 220}};
    MENU_TEST Test;
    HMODULE Module = GetModuleHandleW(L"uxtheme.dll");
    PSET_MENU_APP_MODE SetMode = Module ? (PSET_MENU_APP_MODE)GetProcAddress(Module, MAKEINTRESOURCEA(135)) : NULL;
    PALLOW_MENU_DARK AllowDark = Module ? (PALLOW_MENU_DARK)GetProcAddress(Module, MAKEINTRESOURCEA(133)) : NULL;
    HTHEME Theme = NULL;
    RECT Rect;
    SIZE Size;
    MARGINS Margins = {0};
    DWORD Signatures[2] = {0};
    int Previous = 0;
    BOOL Changed = FALSE;
    UINT Palette, Index, State;

    trace("MenuAppearance pins the ReliefWin theme resource design; it is not a Windows 11 pixel baseline\n");
    if (!CreateModernMenuTest(&Test, FALSE))
        goto Cleanup;
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        goto Cleanup;
    }
    ok(SetMode && AllowDark, "ReliefWin palette selection ordinals are unavailable\n");
    if (!SetMode || !AllowDark)
        goto Cleanup;
    ok(CreateMenuSurface(&Test.Reference), "cannot allocate menu theme surface\n");
    if (!Test.Reference.Pixels)
        goto Cleanup;
    Previous = SetMode(2);
    Changed = TRUE;
    AllowDark(Test.Window, TRUE);
    for (Palette = 0; Palette < 2; Palette++)
    {
        SetMode(Palette ? 3 : 2);
        Theme = OpenThemeData(Test.Window, Palette ? L"ContentLight::Menu" : L"Menu");
        ok(Theme != NULL, "cannot open palette %u Menu theme\n", Palette);
        if (!Theme)
            continue;
        for (Index = 0; Index < ARRAYSIZE(Sizes); Index++)
        {
            SetRect(&Rect, 12, 12, 12 + Sizes[Index][0], 12 + Sizes[Index][1]);
            Signatures[Palette] = DrawMenuPart(&Test.Reference, Theme, MENU_POPUPBACKGROUND, 0, &Rect, TRUE, FALSE);
            DrawMenuPart(&Test.Reference, Theme, MENU_POPUPBORDERS, 0, &Rect, TRUE, FALSE);
        }
        ok(GetThemeMargins(Theme, Test.Reference.Dc, MENU_POPUPBACKGROUND, 0, TMT_CONTENTMARGINS, NULL, &Margins) == S_OK,
           "palette %u popup content margins missing\n", Palette);
        ok(Margins.cxLeftWidth == 6 && Margins.cxRightWidth == 6 &&
           Margins.cyTopHeight == 6 && Margins.cyBottomHeight == 6,
           "palette %u popup margins are %d,%d,%d,%d\n", Palette,
           Margins.cxLeftWidth, Margins.cxRightWidth, Margins.cyTopHeight, Margins.cyBottomHeight);
        SetRect(&Rect, 12, 12, 212, 40);
        for (State = MPI_NORMAL; State <= MPI_DISABLEDHOT; State++)
            DrawMenuPart(&Test.Reference, Theme, MENU_POPUPITEM, State, &Rect,
                         State == MPI_HOT || State == MPI_DISABLEDHOT,
                         State == MPI_NORMAL || State == MPI_DISABLED);
        for (State = MBI_NORMAL; State <= MBI_DISABLEDPUSHED; State++)
            DrawMenuPart(&Test.Reference, Theme, MENU_BARITEM, State, &Rect,
                         State != MBI_NORMAL && State != MBI_DISABLED,
                         State == MBI_NORMAL || State == MBI_DISABLED);
        SetRect(&Rect, 12, 12, 24, 24);
        for (State = MC_CHECKMARKNORMAL; State <= MC_BULLETDISABLED; State++)
            DrawMenuPart(&Test.Reference, Theme, MENU_POPUPCHECK, State, &Rect, FALSE, FALSE);
        for (State = MSM_NORMAL; State <= MSM_DISABLED; State++)
            DrawMenuPart(&Test.Reference, Theme, MENU_POPUPSUBMENU, State, &Rect, FALSE, FALSE);
        ok(GetThemePartSize(Theme, Test.Reference.Dc, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, NULL, TS_TRUE, &Size) == S_OK &&
           Size.cx == 12 && Size.cy == 12, "palette %u menu check is not native 12px\n", Palette);
        ok(GetThemePartSize(Theme, Test.Reference.Dc, MENU_POPUPSUBMENU, MSM_NORMAL, NULL, TS_TRUE, &Size) == S_OK &&
           Size.cx == 12 && Size.cy == 12, "palette %u submenu glyph is not native 12px\n", Palette);
        SetRect(&Rect, 12, 12, 212, 21);
        DrawMenuPart(&Test.Reference, Theme, MENU_POPUPSEPARATOR, 0, &Rect, FALSE, FALSE);
        CloseThemeData(Theme);
        Theme = NULL;
    }
    ok(Signatures[0] && Signatures[1] && Signatures[0] != Signatures[1],
       "menu theme dark/light panel signatures did not differ: %08lx/%08lx\n", Signatures[0], Signatures[1]);

Cleanup:
    if (Theme)
        CloseThemeData(Theme);
    if (Changed)
        SetMode(Previous);
    DestroyModernMenuTest(&Test);
}

START_TEST(MenuShowcase)
{
    MENU_TEST Test;
    WCHAR Duration[32];
    POINT Point = {220, 230};

    if (!CreateModernMenuTest(&Test, TRUE))
        goto Cleanup;
    if (GetEnvironmentVariableW(L"UXTHEME_MENU_CAPTURE_MS", Duration, ARRAYSIZE(Duration)))
        Test.Duration = min(wcstoul(Duration, NULL, 10), 180000);
    TrackModernMenu(&Test, NULL, 0, Point);

Cleanup:
    DestroyModernMenuTest(&Test);
}
