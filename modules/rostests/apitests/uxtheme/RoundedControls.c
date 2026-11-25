/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Rounded theme rendering, native control behavior and interactive gallery
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 */

#include <apitest.h>
#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

#define SURFACE_WIDTH 192
#define SURFACE_HEIGHT 144
#define CONTROL_ID_BASE 200

typedef int (WINAPI *PSET_ROUNDED_APP_MODE)(int);
typedef BOOLEAN (WINAPI *PALLOW_ROUNDED_DARK_MODE)(HWND, BOOLEAN);

enum ROUNDED_APP_MODE
{
    RoundedAppDefault, RoundedAppAllowDark, RoundedAppForceDark, RoundedAppForceLight
};

typedef struct
{
    COLORREF TextColor;
    DWORD Signature;
    BOOL Valid;
} ROUNDED_PALETTE_SAMPLE;

enum CONTROL_INDEX
{
    BUTTON_NORMAL, BUTTON_DISABLED, BUTTON_PRESSED, BUTTON_DEFAULT,
    EDIT_NORMAL, EDIT_DISABLED, COMBO_EDIT, COMBO_READONLY,
    CHECK_NORMAL, CHECK_MIXED, RADIO_FIRST, RADIO_SECOND,
    SCROLL_HORIZONTAL, PROGRESS_NORMAL, TAB_NORMAL, TRACK_NORMAL,
    SPIN_BUDDY_EDIT, SPIN_BUDDY, SPIN_ALONE, LIST_NORMAL, TREE_NORMAL,
    STATUS_LABEL, CONTROL_COUNT
};

typedef struct
{
    HWND Window;
    HWND Controls[CONTROL_COUNT];
    HBRUSH Background;
    COLORREF BackgroundColor;
    UINT Commands;
    HTREEITEM TreeRoot;
    HTREEITEM TreeLeaf;
    HWND DestroyOnErase;
    UINT ParentDestroyCount;
    BOOL ParentDestroyResult;
    HWND ThemeOnErase;
    const WCHAR *ThemeApplication;
    UINT ParentThemeCount;
    HRESULT ParentThemeResult;
} GALLERY;

typedef struct
{
    const WCHAR *ClassName;
    int Part;
    int States;
    BOOL TrueSize;
    BOOL Vertical;
    UINT CornerMask;
    UINT DistinctStates;
    UINT EmptyStates;
} PART_CASE;

static const PART_CASE Parts[] =
{
    { L"Button", BP_PUSHBUTTON, 5, FALSE, FALSE, 15, 28 },
    { L"Button", BP_CHECKBOX, 12, TRUE, FALSE, 15, 272 },
    { L"Button", BP_RADIOBUTTON, 8, TRUE, FALSE, 15, 16 },
    { L"Edit", EP_EDITTEXT, 8, FALSE, FALSE, 15, 24 },
    { L"Edit", EP_EDITBORDER_NOSCROLL, 4, FALSE, FALSE, 15, 12 },
    { L"Edit", EP_EDITBORDER_HSCROLL, 4, FALSE, FALSE, 15, 12 },
    { L"Edit", EP_EDITBORDER_VSCROLL, 4, FALSE, FALSE, 15, 12 },
    { L"Edit", EP_EDITBORDER_HVSCROLL, 4, FALSE, FALSE, 15, 12 },
    { L"ComboBox", 0, 4, FALSE, FALSE, 15, 0 },
    { L"ComboBox", CP_DROPDOWNBUTTONRIGHT, 4, FALSE, FALSE, 10, 12 },
    { L"ComboBox", CP_DROPDOWNBUTTONLEFT, 4, FALSE, FALSE, 5, 12 },
    { L"ComboBox", CP_BORDER, 4, FALSE, FALSE, 15, 12 },
    { L"ComboBox", CP_READONLY, 4, FALSE, FALSE, 15, 12 },
    { L"ComboBox", CP_DROPDOWNBUTTON, 4, FALSE, FALSE, 10, 12 },
    { L"ScrollBar", SBP_ARROWBTN, 16, FALSE, FALSE, 15, 0 },
    { L"ScrollBar", SBP_THUMBBTNHORZ, 4, FALSE, FALSE, 15, 0 },
    { L"ScrollBar", SBP_THUMBBTNVERT, 4, FALSE, TRUE, 15, 0 },
    { L"ScrollBar", SBP_LOWERTRACKHORZ, 4, FALSE, FALSE, 15, 0 },
    { L"ScrollBar", SBP_UPPERTRACKHORZ, 4, FALSE, FALSE, 15, 0 },
    { L"ScrollBar", SBP_LOWERTRACKVERT, 4, FALSE, TRUE, 15, 0 },
    { L"ScrollBar", SBP_UPPERTRACKVERT, 4, FALSE, TRUE, 15, 0 },
    { L"Progress", PP_BAR, 1, FALSE, FALSE, 15, 0 },
    { L"Progress", PP_BARVERT, 1, FALSE, TRUE, 15, 0 },
    { L"Progress", PP_FILL, 4, FALSE, FALSE, 15, 6 },
    { L"Progress", PP_FILLVERT, 4, FALSE, TRUE, 15, 6 },
    { L"Tab", TABP_TABITEM, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TABITEMLEFTEDGE, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TABITEMRIGHTEDGE, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TABITEMBOTHEDGE, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TOPTABITEM, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TOPTABITEMLEFTEDGE, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TOPTABITEMRIGHTEDGE, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_TOPTABITEMBOTHEDGE, 5, FALSE, FALSE, 3, 12 },
    { L"Tab", TABP_PANE, 1, FALSE, FALSE, 15, 0 },
    { L"Spin", SPNP_UP, 4, FALSE, FALSE, 3, 12 },
    { L"Spin", SPNP_DOWN, 4, FALSE, FALSE, 12, 12 },
    { L"Spin", SPNP_UPHORZ, 4, FALSE, FALSE, 10, 12 },
    { L"Spin", SPNP_DOWNHORZ, 4, FALSE, FALSE, 5, 12 },
    { L"Trackbar", TKP_THUMB, 5, TRUE, FALSE, 15, 24 },
    { L"Trackbar", TKP_THUMBVERT, 5, TRUE, TRUE, 15, 24 },
    { L"Trackbar", TKP_TRACK, 1, FALSE, FALSE, 15, 0 },
    { L"Trackbar", TKP_TRACKVERT, 1, FALSE, TRUE, 15, 0 },
    { L"ListView", LVP_LISTITEM, 6, FALSE, FALSE, 15, 54, 9 },
    { L"TreeView", TVP_TREEITEM, 6, FALSE, FALSE, 15, 54, 9 }
};

static void Pump(DWORD Duration)
{
    DWORD Start = GetTickCount();
    MSG Message;

    do
    {
        while (PeekMessageW(&Message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessageW(&Message);
        }
        if (GetTickCount() - Start >= Duration)
            break;
        MsgWaitForMultipleObjects(0, NULL, FALSE, 10, QS_ALLINPUT);
    } while (TRUE);
}

static void SampleRoundedPalette(HDC Dc, DWORD *Bits, UINT Palette, ROUNDED_PALETTE_SAMPLE *Sample)
{
    RECT Rect = {12, 12, 108, 44};
    HTHEME Theme = OpenThemeData(NULL, L"Button");
    HRESULT ColorResult, DrawResult;
    UINT Changed = 0;
    int x, y;

    Sample->TextColor = CLR_INVALID;
    Sample->Signature = 2166136261u;
    Sample->Valid = FALSE;
    ok(Theme != NULL, "palette %u cannot open the Button theme sample\n", Palette);
    if (!Theme)
        return;
    ColorResult = GetThemeColor(Theme, BP_PUSHBUTTON, PBS_NORMAL, TMT_TEXTCOLOR, &Sample->TextColor);
    ok(ColorResult == S_OK && Sample->TextColor != CLR_INVALID,
       "palette %u Button text color is %08lx, result %08lx\n", Palette, Sample->TextColor, ColorResult);
    GdiFlush();
    for (y = 0; y < SURFACE_HEIGHT; y++)
        for (x = 0; x < SURFACE_WIDTH; x++)
            Bits[y * SURFACE_WIDTH + x] = 0x00db3595;
    DrawResult = DrawThemeBackground(Theme, Dc, BP_PUSHBUTTON, PBS_NORMAL, &Rect, NULL);
    GdiFlush();
    for (y = Rect.top; y < Rect.bottom; y++)
    {
        for (x = Rect.left; x < Rect.right; x++)
        {
            DWORD Pixel = Bits[y * SURFACE_WIDTH + x] & 0x00ffffff;

            Changed += Pixel != 0x00db3595;
            Sample->Signature = (Sample->Signature ^ Pixel) * 16777619u;
        }
    }
    ok(DrawResult == S_OK && Changed != 0,
       "palette %u Button sample drew %u pixels, result %08lx\n", Palette, Changed, DrawResult);
    Sample->Valid = ColorResult == S_OK && Sample->TextColor != CLR_INVALID && DrawResult == S_OK && Changed != 0;
    trace("ROUNDED_PALETTE palette=%u text=%08lx pixels=%08lx\n", Palette, Sample->TextColor, Sample->Signature);
    CloseThemeData(Theme);
}

static void TestPart(HWND Window, const PART_CASE *Part, HDC Dc, DWORD *Bits, DWORD *First)
{
    static const DWORD Backgrounds[] = { 0x00db3595, 0x0024bdd7 };
    HTHEME Theme = OpenThemeData(Window, Part->ClassName);
    int State, Scale, Pass, x, y;
    HRESULT Result;

    ok(Theme != NULL, "%ls part %d: cannot open theme\n", Part->ClassName, Part->Part);
    if (!Theme)
        return;
    ok(IsThemePartDefined(Theme, Part->Part, 0), "%ls part %d is missing\n", Part->ClassName, Part->Part);
    for (Scale = 0; Scale < (Part->TrueSize ? 1 : 2); Scale++)
    {
        RECT Rect = { 12, 12, 32, 30 };
        DWORD NormalHash = 0;

        if (Scale)
        {
            Rect.right = 144;
            Rect.bottom = 52;
        }
        if (Part->Vertical)
        {
            Rect.right = Scale ? 40 : 30;
            Rect.bottom = Scale ? 116 : 64;
        }
        if (Part->TrueSize)
        {
            SIZE Size = {0};

            Result = GetThemePartSize(Theme, Dc, Part->Part, 1, NULL, TS_TRUE, &Size);
            ok(Result == S_OK && Size.cx > 0 && Size.cy > 0 && Size.cx < 160 && Size.cy < 112,
               "%ls part %d: invalid native size %ld,%ld, result %08lx\n", Part->ClassName, Part->Part,
               Size.cx, Size.cy, Result);
            if (FAILED(Result) || Size.cx <= 0 || Size.cy <= 0 || Size.cx >= 160 || Size.cy >= 112)
                break;
            Rect.right = Rect.left + Size.cx;
            Rect.bottom = Rect.top + Size.cy;
        }
        for (State = 1; State <= Part->States; State++)
        {
            UINT Partial = 0;
            BOOL Empty = (Part->EmptyStates & (1u << (State - 1))) != 0;

            ok(IsThemeBackgroundPartiallyTransparent(Theme, Part->Part, State),
               "%ls part %d state %d has no transparent background\n", Part->ClassName, Part->Part, State);
            for (Pass = 0; Pass < ARRAYSIZE(Backgrounds); Pass++)
            {
                UINT Changed = 0, Escaped = 0;
                DWORD Hash = 2166136261u;
                DWORD Background = Backgrounds[Pass];

                GdiFlush();
                for (y = 0; y < SURFACE_HEIGHT; y++)
                    for (x = 0; x < SURFACE_WIDTH; x++)
                        Bits[y * SURFACE_WIDTH + x] = Background;
                Result = DrawThemeBackground(Theme, Dc, Part->Part, State, &Rect, NULL);
                ok(Result == S_OK, "%ls part %d state %d scale %d: draw %08lx\n", Part->ClassName,
                   Part->Part, State, Scale, Result);
                GdiFlush();
                for (y = 0; y < SURFACE_HEIGHT; y++)
                {
                    for (x = 0; x < SURFACE_WIDTH; x++)
                    {
                        UINT Index = y * SURFACE_WIDTH + x;
                        DWORD Pixel = Bits[Index] & 0x00ffffff;

                        if (x < Rect.left || x >= Rect.right || y < Rect.top || y >= Rect.bottom)
                            Escaped += Pixel != Background;
                        else
                        {
                            Changed += Pixel != Background;
                            Hash = (Hash ^ Pixel) * 16777619u;
                            if (Pass && Pixel != Background && First[Index] != Backgrounds[0] &&
                                Pixel != First[Index])
                                Partial++;
                        }
                    }
                }
                ok(Empty ? Changed == 0 : Changed != 0,
                   "%ls part %d state %d scale %d background %d painted %u pixels, expected %s\n",
                   Part->ClassName, Part->Part, State, Scale, Pass, Changed, Empty ? "empty" : "visible");
                ok(Escaped == 0, "%ls part %d state %d scale %d painted %u pixels outside the target\n",
                   Part->ClassName, Part->Part, State, Scale, Escaped);
                ok((!(Part->CornerMask & 1) || (Bits[Rect.top * SURFACE_WIDTH + Rect.left] & 0x00ffffff) == Background) &&
                   (!(Part->CornerMask & 2) || (Bits[Rect.top * SURFACE_WIDTH + Rect.right - 1] & 0x00ffffff) == Background) &&
                   (!(Part->CornerMask & 4) || (Bits[(Rect.bottom - 1) * SURFACE_WIDTH + Rect.left] & 0x00ffffff) == Background) &&
                   (!(Part->CornerMask & 8) || (Bits[(Rect.bottom - 1) * SURFACE_WIDTH + Rect.right - 1] & 0x00ffffff) == Background),
                   "%ls part %d state %d scale %d background %d: rounded corners cover the parent\n",
                   Part->ClassName, Part->Part, State, Scale, Pass);
                if (Part->CornerMask != 15)
                    ok(((Part->CornerMask & 1) || (Bits[Rect.top * SURFACE_WIDTH + Rect.left] & 0x00ffffff) != Background) &&
                       ((Part->CornerMask & 2) || (Bits[Rect.top * SURFACE_WIDTH + Rect.right - 1] & 0x00ffffff) != Background) &&
                       ((Part->CornerMask & 4) || (Bits[(Rect.bottom - 1) * SURFACE_WIDTH + Rect.left] & 0x00ffffff) != Background) &&
                       ((Part->CornerMask & 8) || (Bits[(Rect.bottom - 1) * SURFACE_WIDTH + Rect.right - 1] & 0x00ffffff) != Background),
                       "%ls part %d state %d scale %d background %d: shared straight corners have gaps\n",
                       Part->ClassName, Part->Part, State, Scale, Pass);
                if (!Pass)
                {
                    for (y = 0; y < SURFACE_HEIGHT; y++)
                        for (x = 0; x < SURFACE_WIDTH; x++)
                            First[y * SURFACE_WIDTH + x] = Bits[y * SURFACE_WIDTH + x] & 0x00ffffff;
                    if (State == 1)
                        NormalHash = Hash;
                    else if (Part->DistinctStates & (1u << (State - 1)))
                        ok(Hash != NormalHash, "%ls part %d state %d scale %d matches the normal state\n",
                           Part->ClassName, Part->Part, State, Scale);
                }
            }
            ok(Empty ? Partial == 0 : Partial != 0,
               "%ls part %d state %d scale %d: %u partially transparent pixels, expected %s\n",
               Part->ClassName, Part->Part, State, Scale, Partial, Empty ? "none" : "antialiased edges");
        }
    }
    CloseThemeData(Theme);
}

static LRESULT CALLBACK GalleryProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    GALLERY *Gallery = (GALLERY *)GetWindowLongPtrW(Window, GWLP_USERDATA);
    HDC Dc;
    RECT Rect;
    PAINTSTRUCT Paint;

    if (Message == WM_NCCREATE)
    {
        Gallery = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Gallery);
    }
    if (!Gallery)
        return DefWindowProcW(Window, Message, WParam, LParam);
    switch (Message)
    {
        case WM_ERASEBKGND:
            if (Gallery->DestroyOnErase)
            {
                HWND Target = Gallery->DestroyOnErase;

                Gallery->DestroyOnErase = NULL;
                Gallery->ParentDestroyCount++;
                Gallery->ParentDestroyResult = DestroyWindow(Target);
            }
            if (Gallery->ThemeOnErase)
            {
                HWND Target = Gallery->ThemeOnErase;

                Gallery->ThemeOnErase = NULL;
                Gallery->ParentThemeCount++;
                Gallery->ParentThemeResult = SetWindowTheme(Target, Gallery->ThemeApplication, NULL);
            }
            GetClientRect(Window, &Rect);
            FillRect((HDC)WParam, &Rect, Gallery->Background);
            return TRUE;
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSTATIC:
            SetBkMode((HDC)WParam, TRANSPARENT);
            SetTextColor((HDC)WParam, GetSysColor(COLOR_WINDOWTEXT));
            return (LRESULT)Gallery->Background;
        case WM_CTLCOLOREDIT:
            SetBkColor((HDC)WParam, GetSysColor(COLOR_WINDOW));
            SetTextColor((HDC)WParam, GetSysColor(COLOR_WINDOWTEXT));
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        case WM_COMMAND:
            if (HIWORD(WParam) == BN_CLICKED && LOWORD(WParam) == CONTROL_ID_BASE + BUTTON_NORMAL)
                Gallery->Commands++;
            if (Gallery->Controls[STATUS_LABEL])
                SetWindowTextW(Gallery->Controls[STATUS_LABEL], L"操作已完成，可以继续体验其他控件。");
            return 0;
        case WM_HSCROLL:
            if ((HWND)LParam == Gallery->Controls[SCROLL_HORIZONTAL])
            {
                SCROLLINFO Scroll = {sizeof(Scroll), SIF_ALL};

                if (GetScrollInfo((HWND)LParam, SB_CTL, &Scroll))
                {
                    switch (LOWORD(WParam))
                    {
                        case SB_LINELEFT: Scroll.nPos--; break;
                        case SB_LINERIGHT: Scroll.nPos++; break;
                        case SB_PAGELEFT: Scroll.nPos -= Scroll.nPage; break;
                        case SB_PAGERIGHT: Scroll.nPos += Scroll.nPage; break;
                        case SB_THUMBPOSITION:
                        case SB_THUMBTRACK: Scroll.nPos = Scroll.nTrackPos; break;
                        case SB_TOP: Scroll.nPos = Scroll.nMin; break;
                        case SB_BOTTOM: Scroll.nPos = Scroll.nMax; break;
                    }
                    Scroll.fMask = SIF_POS;
                    SetScrollInfo((HWND)LParam, SB_CTL, &Scroll, TRUE);
                }
            }
            return 0;
        case WM_PAINT:
            Dc = BeginPaint(Window, &Paint);
            GetClientRect(Window, &Rect);
            FillRect(Dc, &Rect, Gallery->Background);
            SelectObject(Dc, GetStockObject(DEFAULT_GUI_FONT));
            SetBkMode(Dc, TRANSPARENT);
            SetTextColor(Dc, GetSysColor(COLOR_WINDOWTEXT));
            TextOutW(Dc, 28, 20, L"ReliefWin 原生控件", lstrlenW(L"ReliefWin 原生控件"));
            TextOutW(Dc, 28, 46, L"圆角按钮、输入框、选择器及进度条", lstrlenW(L"圆角按钮、输入框、选择器及进度条"));
            EndPaint(Window, &Paint);
            return 0;
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

static HWND AddControl(GALLERY *Gallery, int Index, DWORD ExStyle, const WCHAR *ClassName,
                       const WCHAR *Text, DWORD Style, int x, int y, int Width, int Height)
{
    HWND Control = CreateWindowExW(ExStyle, ClassName, Text, WS_CHILD | WS_VISIBLE | Style,
                                   x, y, Width, Height, Gallery->Window,
                                   (HMENU)(UINT_PTR)(CONTROL_ID_BASE + Index), GetModuleHandleW(NULL), NULL);

    Gallery->Controls[Index] = Control;
    ok(Control != NULL, "%ls control %d could not be created, error %lu\n", ClassName, Index, GetLastError());
    if (Control)
        SendMessageW(Control, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    return Control;
}

static BOOL CreateGallery(GALLERY *Gallery, BOOL Showcase)
{
    WNDCLASSW Class = {0};
    TCITEMW Item = {0};
    LVCOLUMNW Column = {0};
    LVITEMW ListItem = {0};
    TVINSERTSTRUCTW TreeItem = {0};
    SCROLLINFO Scroll = {sizeof(Scroll), SIF_RANGE | SIF_PAGE | SIF_POS, 0, 100, 20, 35};
    int i;

    ZeroMemory(Gallery, sizeof(*Gallery));
    Gallery->BackgroundColor = Showcase ? GetSysColor(COLOR_BTNFACE) : RGB(219, 53, 149);
    Gallery->Background = CreateSolidBrush(Gallery->BackgroundColor);
    ok(Gallery->Background != NULL, "cannot create the parent background brush\n");
    if (!Gallery->Background)
        return FALSE;
    Class.lpfnWndProc = GalleryProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    Class.lpszClassName = L"ReliefWinRoundedControls";
    ok(RegisterClassW(&Class) != 0, "cannot register gallery, error %lu\n", GetLastError());
    Gallery->Window = CreateWindowExW(WS_EX_TOPMOST, Class.lpszClassName, L"ReliefWin 控件体验",
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
                                      220, 70, 780, 780, NULL, NULL, Class.hInstance, Gallery);
    ok(Gallery->Window != NULL, "cannot create gallery, error %lu\n", GetLastError());
    if (!Gallery->Window)
        return FALSE;
    AddControl(Gallery, BUTTON_NORMAL, 0, L"Button", L"普通按钮", BS_PUSHBUTTON | WS_TABSTOP, 28, 82, 160, 34);
    AddControl(Gallery, BUTTON_DISABLED, 0, L"Button", L"暂不可用", BS_PUSHBUTTON | WS_DISABLED, 204, 82, 160, 34);
    AddControl(Gallery, BUTTON_PRESSED, 0, L"Button", L"按下状态", BS_PUSHBUTTON, 380, 82, 160, 34);
    AddControl(Gallery, BUTTON_DEFAULT, 0, L"Button", L"主要操作", BS_DEFPUSHBUTTON | WS_TABSTOP, 556, 82, 160, 34);
    AddControl(Gallery, EDIT_NORMAL, WS_EX_CLIENTEDGE, L"Edit", L"在这里输入文字", ES_AUTOHSCROLL | WS_TABSTOP, 28, 148, 336, 32);
    AddControl(Gallery, EDIT_DISABLED, WS_EX_CLIENTEDGE, L"Edit", L"不可编辑的文本", ES_AUTOHSCROLL | WS_DISABLED, 380, 148, 336, 32);
    AddControl(Gallery, COMBO_EDIT, 0, L"ComboBox", L"", CBS_DROPDOWN | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP, 28, 214, 336, 180);
    AddControl(Gallery, COMBO_READONLY, 0, L"ComboBox", L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP, 380, 214, 336, 180);
    AddControl(Gallery, CHECK_NORMAL, 0, L"Button", L"启用此选项", BS_AUTOCHECKBOX | WS_TABSTOP, 28, 274, 160, 32);
    AddControl(Gallery, CHECK_MIXED, 0, L"Button", L"部分选中", BS_AUTO3STATE | WS_TABSTOP, 204, 274, 160, 32);
    AddControl(Gallery, RADIO_FIRST, 0, L"Button", L"选项一", BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP, 380, 274, 160, 32);
    AddControl(Gallery, RADIO_SECOND, 0, L"Button", L"选项二", BS_AUTORADIOBUTTON | WS_TABSTOP, 556, 274, 160, 32);
    AddControl(Gallery, SCROLL_HORIZONTAL, 0, L"ScrollBar", L"", SBS_HORZ | WS_TABSTOP, 28, 340, 336, 20);
    AddControl(Gallery, PROGRESS_NORMAL, 0, PROGRESS_CLASSW, L"", PBS_SMOOTH, 380, 340, 336, 20);
    AddControl(Gallery, TAB_NORMAL, 0, WC_TABCONTROLW, L"", WS_TABSTOP, 28, 400, 688, 100);
    AddControl(Gallery, TRACK_NORMAL, 0, TRACKBAR_CLASSW, L"", TBS_AUTOTICKS | WS_TABSTOP, 28, 538, 336, 40);
    AddControl(Gallery, SPIN_BUDDY_EDIT, WS_EX_CLIENTEDGE, L"Edit", L"17", ES_NUMBER | WS_TABSTOP, 380, 538, 160, 32);
    AddControl(Gallery, SPIN_BUDDY, 0, UPDOWN_CLASSW, L"", UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 520, 538, 20, 32);
    AddControl(Gallery, SPIN_ALONE, 0, UPDOWN_CLASSW, L"", UDS_ARROWKEYS | WS_TABSTOP, 580, 538, 28, 40);
    AddControl(Gallery, LIST_NORMAL, 0, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_TABSTOP,
               28, 606, 336, 80);
    AddControl(Gallery, TREE_NORMAL, 0, WC_TREEVIEWW, L"", TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT |
               TVS_SHOWSELALWAYS | WS_TABSTOP, 380, 606, 336, 80);
    AddControl(Gallery, STATUS_LABEL, 0, L"Static", L"点击按钮，编辑文字，或选择不同的选项。", SS_LEFT, 28, 702, 688, 24);
    for (i = 0; i < CONTROL_COUNT; i++)
        if (!Gallery->Controls[i])
            return FALSE;
    for (i = COMBO_EDIT; i <= COMBO_READONLY; i++)
    {
        SendMessageW(Gallery->Controls[i], CB_ADDSTRING, 0, (LPARAM)L"简洁与舒适");
        SendMessageW(Gallery->Controls[i], CB_ADDSTRING, 0, (LPARAM)L"专注与高效");
        SendMessageW(Gallery->Controls[i], CB_SETCURSEL, 0, 0);
    }
    SendMessageW(Gallery->Controls[BUTTON_PRESSED], BM_SETSTATE, TRUE, 0);
    SendMessageW(Gallery->Controls[CHECK_MIXED], BM_SETCHECK, BST_INDETERMINATE, 0);
    SendMessageW(Gallery->Controls[RADIO_FIRST], BM_SETCHECK, BST_CHECKED, 0);
    SetScrollInfo(Gallery->Controls[SCROLL_HORIZONTAL], SB_CTL, &Scroll, TRUE);
    SendMessageW(Gallery->Controls[PROGRESS_NORMAL], PBM_SETRANGE32, 0, 100);
    SendMessageW(Gallery->Controls[PROGRESS_NORMAL], PBM_SETPOS, 63, 0);
    Item.mask = TCIF_TEXT;
    Item.pszText = L"概览";
    SendMessageW(Gallery->Controls[TAB_NORMAL], TCM_INSERTITEMW, 0, (LPARAM)&Item);
    Item.pszText = L"个性化";
    SendMessageW(Gallery->Controls[TAB_NORMAL], TCM_INSERTITEMW, 1, (LPARAM)&Item);
    SendMessageW(Gallery->Controls[TRACK_NORMAL], TBM_SETRANGE, FALSE, MAKELPARAM(0, 100));
    SendMessageW(Gallery->Controls[TRACK_NORMAL], TBM_SETPOS, TRUE, 45);
    SendMessageW(Gallery->Controls[SPIN_BUDDY], UDM_SETBUDDY, (WPARAM)Gallery->Controls[SPIN_BUDDY_EDIT], 0);
    SendMessageW(Gallery->Controls[SPIN_BUDDY], UDM_SETRANGE32, 0, 100);
    SendMessageW(Gallery->Controls[SPIN_BUDDY], UDM_SETPOS32, 0, 17);
    SendMessageW(Gallery->Controls[SPIN_ALONE], UDM_SETRANGE32, 0, 100);
    SendMessageW(Gallery->Controls[SPIN_ALONE], UDM_SETPOS32, 0, 31);
    Column.mask = LVCF_TEXT | LVCF_WIDTH;
    Column.pszText = L"项目";
    Column.cx = 332;
    ok(SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_INSERTCOLUMNW, 0, (LPARAM)&Column) == 0,
       "cannot insert native list column\n");
    ListItem.mask = LVIF_TEXT | LVIF_STATE;
    ListItem.pszText = L"已选中的项目";
    ListItem.state = LVIS_SELECTED | LVIS_FOCUSED;
    ListItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    ok(SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_INSERTITEMW, 0, (LPARAM)&ListItem) == 0,
       "cannot insert first native list item\n");
    ListItem.iItem = 1;
    ListItem.pszText = L"普通项目";
    ListItem.state = 0;
    ok(SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_INSERTITEMW, 0, (LPARAM)&ListItem) == 1,
       "cannot insert second native list item\n");
    TreeItem.hParent = TVI_ROOT;
    TreeItem.hInsertAfter = TVI_LAST;
    TreeItem.item.mask = TVIF_TEXT;
    TreeItem.item.pszText = L"ReliefWin";
    Gallery->TreeRoot = (HTREEITEM)SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_INSERTITEMW, 0, (LPARAM)&TreeItem);
    ok(Gallery->TreeRoot != NULL, "cannot insert native tree root\n");
    TreeItem.hParent = Gallery->TreeRoot;
    TreeItem.item.pszText = L"个性化设置";
    Gallery->TreeLeaf = (HTREEITEM)SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_INSERTITEMW, 0, (LPARAM)&TreeItem);
    ok(Gallery->TreeLeaf != NULL, "cannot insert native tree child\n");
    SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_EXPAND, TVE_EXPAND, (LPARAM)Gallery->TreeRoot);
    SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_SELECTITEM, TVGN_CARET, (LPARAM)Gallery->TreeLeaf);
    SetForegroundWindow(Gallery->Window);
    SetFocus(Gallery->Controls[BUTTON_DEFAULT]);
    RedrawWindow(Gallery->Window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
    Pump(250);
    return TRUE;
}

static void CheckNativeCorners(const GALLERY *Gallery, int Index)
{
    HWND Control = Gallery->Controls[Index];
    HDC Dc;
    RECT Rect;
    POINT Points[4];
    int i;

    ok(GetWindowTheme(Control) != NULL, "native control %d has no visual style\n", Index);
    if (!GetWindowRect(Control, &Rect))
    {
        ok(FALSE, "native control %d: no window rectangle\n", Index);
        return;
    }
    Points[0].x = Points[2].x = 0;
    Points[1].x = Points[3].x = Rect.right - Rect.left - 1;
    Points[0].y = Points[1].y = 0;
    Points[2].y = Points[3].y = Rect.bottom - Rect.top - 1;
    Dc = GetWindowDC(Control);
    ok(Dc != NULL, "native control %d: no window DC\n", Index);
    if (!Dc)
        return;
    for (i = 0; i < ARRAYSIZE(Points); i++)
    {
        COLORREF Actual = GetPixel(Dc, Points[i].x, Points[i].y);

        ok(Actual == Gallery->BackgroundColor,
           "native control %d corner %ld,%ld: %06lx, parent %06lx\n", Index, Points[i].x, Points[i].y,
           Actual, Gallery->BackgroundColor);
    }
    ReleaseDC(Control, Dc);
}

static void CheckProgressCycle(const GALLERY *Gallery)
{
    static const int Positions[] = {100, 0, 63};
    HWND Control = Gallery->Controls[PROGRESS_NORMAL];
    COLORREF Centers[ARRAYSIZE(Positions)];
    RECT Rect;
    HDC Dc;
    UINT i;

    for (i = 0; i < ARRAYSIZE(Positions); i++)
    {
        SendMessageW(Control, PBM_SETPOS, Positions[i], 0);
        RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        Pump(50);
        ok(SendMessageW(Control, PBM_GETPOS, 0, 0) == Positions[i],
           "native progress did not retain position %d\n", Positions[i]);
        CheckNativeCorners(Gallery, PROGRESS_NORMAL);
        Centers[i] = CLR_INVALID;
        if (GetClientRect(Control, &Rect))
        {
            Dc = GetDC(Control);
            if (Dc)
            {
                Centers[i] = GetPixel(Dc, (Rect.left + Rect.right) / 2, (Rect.top + Rect.bottom) / 2);
                ReleaseDC(Control, Dc);
            }
        }
        ok(Centers[i] != CLR_INVALID, "cannot sample progress position %d\n", Positions[i]);
    }
    ok(Centers[0] != Centers[1], "native progress retained the full fill after reset: %06lx\n", Centers[1]);
    ok(Centers[2] == Centers[0], "native progress center after refill is %06lx, expected %06lx\n",
       Centers[2], Centers[0]);
}

static void CheckProgressMarquee(const GALLERY *Gallery)
{
    HWND Control = Gallery->Controls[PROGRESS_NORMAL];
    LONG_PTR Style = GetWindowLongPtrW(Control, GWL_STYLE);
    RECT Rect = {0};
    DWORD FirstHash = 0;
    UINT Frame, ValidFrames = 0, ChangedFrames = 0;

    ok(GetClientRect(Control, &Rect) && Rect.right - Rect.left > 10 && Rect.bottom - Rect.top > 10,
       "native progress has no interior for marquee sampling\n");

    SetWindowLongPtrW(Control, GWL_STYLE, Style | PBS_MARQUEE);
    ok(SendMessageW(Control, PBM_SETMARQUEE, TRUE, 10), "native progress could not start marquee mode\n");
    for (Frame = 0; Frame < 10; Frame++)
    {
        HDC Dc;

        Pump(40);
        RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        trace("native progress marquee frame %u\n", Frame);
        CheckNativeCorners(Gallery, PROGRESS_NORMAL);
        Dc = GetDC(Control);
        ok(Dc != NULL, "cannot sample native progress marquee frame %u\n", Frame);
        if (Dc)
        {
            DWORD Hash = 2166136261u;
            UINT Missing = 0;
            int x, y;
            COLORREF Center = GetPixel(Dc, (Rect.left + Rect.right) / 2, (Rect.top + Rect.bottom) / 2);

            ok(Center != CLR_INVALID, "native progress marquee frame %u has no center pixel\n", Frame);
            for (y = Rect.top + 5; y < Rect.bottom - 5; y += 3)
            {
                for (x = Rect.left + 5; x < Rect.right - 5; x += 7)
                {
                    COLORREF Pixel = GetPixel(Dc, x, y);

                    Missing += Pixel == CLR_INVALID;
                    Hash = (Hash ^ Pixel) * 16777619u;
                }
            }
            ok(Missing == 0, "native progress marquee frame %u has %u unreadable interior pixels\n", Frame, Missing);
            if (!Missing && Center != CLR_INVALID)
            {
                if (!ValidFrames)
                    FirstHash = Hash;
                else
                    ChangedFrames += Hash != FirstHash;
                ValidFrames++;
            }
            ReleaseDC(Control, Dc);
        }
    }
    ok(ValidFrames >= 2 && ChangedFrames != 0,
       "native progress marquee did not animate: %u valid frames, %u changed frame hashes\n", ValidFrames, ChangedFrames);
    SendMessageW(Control, PBM_SETMARQUEE, FALSE, 0);
    SetWindowLongPtrW(Control, GWL_STYLE, Style);
    SendMessageW(Control, PBM_SETPOS, 63, 0);
    RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
    Pump(50);
    ok(SendMessageW(Control, PBM_GETPOS, 0, 0) == 63, "native progress lost its position after marquee mode\n");
    CheckNativeCorners(Gallery, PROGRESS_NORMAL);
}

static void TestNative(GALLERY *Gallery)
{
    static const int Rounded[] = { BUTTON_NORMAL, BUTTON_DISABLED, BUTTON_PRESSED, BUTTON_DEFAULT,
                                    EDIT_NORMAL, EDIT_DISABLED, COMBO_EDIT, COMBO_READONLY,
                                    PROGRESS_NORMAL, SPIN_ALONE };
    WCHAR Text[64];
    SCROLLINFO Scroll = {sizeof(Scroll), SIF_RANGE | SIF_PAGE | SIF_POS};
    LVITEMW ListItem = {0};
    TVITEMW TreeItem = {0};
    BOOL Error = TRUE;
    int Position;
    UINT i, Before;

    for (i = 0; i < ARRAYSIZE(Rounded); i++)
        CheckNativeCorners(Gallery, Rounded[i]);
    Before = Gallery->Commands;
    SendMessageW(Gallery->Controls[BUTTON_NORMAL], BM_CLICK, 0, 0);
    ok(Gallery->Commands == Before + 1, "native push button did not send BN_CLICKED\n");
    SendMessageW(Gallery->Controls[CHECK_NORMAL], BM_CLICK, 0, 0);
    ok(SendMessageW(Gallery->Controls[CHECK_NORMAL], BM_GETCHECK, 0, 0) == BST_CHECKED,
       "native checkbox did not toggle on\n");
    SendMessageW(Gallery->Controls[CHECK_NORMAL], BM_CLICK, 0, 0);
    ok(SendMessageW(Gallery->Controls[CHECK_NORMAL], BM_GETCHECK, 0, 0) == BST_UNCHECKED,
       "native checkbox did not toggle off\n");
    SendMessageW(Gallery->Controls[RADIO_SECOND], BM_CLICK, 0, 0);
    ok(SendMessageW(Gallery->Controls[RADIO_SECOND], BM_GETCHECK, 0, 0) == BST_CHECKED &&
       SendMessageW(Gallery->Controls[RADIO_FIRST], BM_GETCHECK, 0, 0) == BST_UNCHECKED,
       "native radio group did not transfer selection\n");
    SetFocus(Gallery->Controls[EDIT_NORMAL]);
    ok(GetFocus() == Gallery->Controls[EDIT_NORMAL], "native edit did not receive focus\n");
    SetWindowTextW(Gallery->Controls[EDIT_NORMAL], L"Relief");
    SendMessageW(Gallery->Controls[EDIT_NORMAL], EM_SETSEL, 6, 6);
    SendMessageW(Gallery->Controls[EDIT_NORMAL], EM_REPLACESEL, TRUE, (LPARAM)L"Win");
    GetWindowTextW(Gallery->Controls[EDIT_NORMAL], Text, ARRAYSIZE(Text));
    ok(!lstrcmpW(Text, L"ReliefWin"), "native edit contains %ls\n", Text);
    for (i = COMBO_EDIT; i <= COMBO_READONLY; i++)
    {
        SendMessageW(Gallery->Controls[i], CB_SETCURSEL, 1, 0);
        ok(SendMessageW(Gallery->Controls[i], CB_GETCURSEL, 0, 0) == 1,
           "native combo %u did not select the second item\n", i);
        SendMessageW(Gallery->Controls[i], CB_SHOWDROPDOWN, TRUE, 0);
        Pump(50);
        ok(SendMessageW(Gallery->Controls[i], CB_GETDROPPEDSTATE, 0, 0),
           "native combo %u did not open its list\n", i);
        SendMessageW(Gallery->Controls[i], CB_SHOWDROPDOWN, FALSE, 0);
    }
    ok(GetScrollInfo(Gallery->Controls[SCROLL_HORIZONTAL], SB_CTL, &Scroll) && Scroll.nMin == 0 &&
       Scroll.nMax == 100 && Scroll.nPage == 20 && Scroll.nPos == 35,
       "native scrollbar lost its range, page or position\n");
    SendMessageW(Gallery->Controls[TAB_NORMAL], TCM_SETCURSEL, 1, 0);
    ok(SendMessageW(Gallery->Controls[TAB_NORMAL], TCM_GETCURSEL, 0, 0) == 1,
       "native tab did not select the second page\n");
    ok(SendMessageW(Gallery->Controls[PROGRESS_NORMAL], PBM_GETPOS, 0, 0) == 63,
       "native progress position is incorrect\n");
    CheckProgressCycle(Gallery);
    CheckProgressMarquee(Gallery);
    ok(SendMessageW(Gallery->Controls[TRACK_NORMAL], TBM_GETPOS, 0, 0) == 45,
       "native trackbar position is incorrect\n");
    Position = (int)SendMessageW(Gallery->Controls[SPIN_BUDDY], UDM_GETPOS32, 0, (LPARAM)&Error);
    ok(Position == 17 && !Error, "native buddy spin position is %d, conversion error %d\n", Position, Error);
    Position = (int)SendMessageW(Gallery->Controls[SPIN_ALONE], UDM_GETPOS32, 0, 0);
    ok(Position == 31, "native standalone spin position is %d\n", Position);
    ListItem.state = LVIS_SELECTED | LVIS_FOCUSED;
    ListItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    ok(SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_SETITEMSTATE, 1, (LPARAM)&ListItem),
       "native list did not accept selection\n");
    ok(SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_GETNEXTITEM, -1, LVNI_SELECTED) == 1 &&
       SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_GETSELECTEDCOUNT, 0, 0) == 1,
       "native single-select list did not select only the second item\n");
    ListItem.iSubItem = 0;
    ListItem.pszText = Text;
    ListItem.cchTextMax = ARRAYSIZE(Text);
    SendMessageW(Gallery->Controls[LIST_NORMAL], LVM_GETITEMTEXTW, 1, (LPARAM)&ListItem);
    ok(!lstrcmpW(Text, L"普通项目"), "native list changed the selected text to %ls\n", Text);
    SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_SELECTITEM, TVGN_CARET, (LPARAM)Gallery->TreeRoot);
    SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_SELECTITEM, TVGN_CARET, (LPARAM)Gallery->TreeLeaf);
    ok((HTREEITEM)SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_GETNEXTITEM, TVGN_CARET, 0) == Gallery->TreeLeaf,
       "native tree did not transfer selection to its child\n");
    TreeItem.mask = TVIF_TEXT | TVIF_STATE;
    TreeItem.hItem = Gallery->TreeLeaf;
    TreeItem.stateMask = TVIS_SELECTED;
    TreeItem.pszText = Text;
    TreeItem.cchTextMax = ARRAYSIZE(Text);
    ok(SendMessageW(Gallery->Controls[TREE_NORMAL], TVM_GETITEMW, 0, (LPARAM)&TreeItem) &&
       (TreeItem.state & TVIS_SELECTED) && !lstrcmpW(Text, L"个性化设置"),
       "native tree lost its child selection or text\n");
    RedrawWindow(Gallery->Window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
    Pump(100);
    CheckNativeCorners(Gallery, EDIT_NORMAL);
    CheckNativeCorners(Gallery, COMBO_EDIT);
    CheckNativeCorners(Gallery, COMBO_READONLY);
}

static void CheckThemeRegionResources(HWND Window, HDC Dc)
{
    HTHEME Theme = OpenThemeData(Window, L"Progress");
    RECT Rect = {0, 0, 80, 20};
    DWORD Before = 0, After;
    UINT i;

    ok(Theme != NULL, "cannot open the progress theme for region resource checks\n");
    if (!Theme)
        return;
    for (i = 0; i <= 32; i++)
    {
        HRGN Region = NULL;
        HRESULT Result = GetThemeBackgroundRegion(Theme, Dc, PP_FILL, PBFS_NORMAL, &Rect, &Region);

        ok(Result == S_OK && Region != NULL, "theme region iteration %u returned %08lx, region %p\n",
           i, Result, Region);
        if (Region)
            ok(DeleteObject(Region), "cannot release theme region iteration %u\n", i);
        if (!i)
        {
            GdiFlush();
            Before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
        }
    }
    GdiFlush();
    After = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    ok(Before != 0, "cannot count the process GDI resources\n");
    ok(After == Before, "repeated theme regions changed GDI resource usage from %lu to %lu\n", Before, After);
    CloseThemeData(Theme);
}

static void CheckParentDestruction(GALLERY *Gallery, HDC Dc, const WCHAR *Application, const WCHAR *ClassName)
{
    UINT Pass;

    for (Pass = 0; Pass < 2; Pass++)
    {
        HWND Control = CreateWindowExW(0, ClassName, L"", WS_CHILD | WS_VISIBLE,
                                       640, 538, 24, 40, Gallery->Window, NULL, GetModuleHandleW(NULL), NULL);
        UINT Before = Gallery->ParentDestroyCount;

        ok(Control != NULL, "cannot create %ls for parent destruction pass %u\n", ClassName, Pass);
        if (!Control)
            continue;
        ok(SetWindowTheme(Control, Application, NULL) == S_OK,
           "cannot set %ls palette for parent destruction pass %u\n", ClassName, Pass);
        Gallery->ParentDestroyResult = FALSE;
        Gallery->DestroyOnErase = Control;
        if (Pass)
            RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        else
            SendMessageW(Control, WM_PRINTCLIENT, (WPARAM)Dc, PRF_CLIENT);
        Gallery->DestroyOnErase = NULL;
        ok(Gallery->ParentDestroyCount == Before + 1 && Gallery->ParentDestroyResult && !IsWindow(Control),
           "%ls parent destruction pass %u: callbacks %u, destroyed %d, alive %d\n",
           ClassName, Pass, Gallery->ParentDestroyCount - Before, Gallery->ParentDestroyResult, IsWindow(Control));
        if (IsWindow(Control))
            DestroyWindow(Control);
    }
}

static void CheckProgressParentThemeChange(GALLERY *Gallery, HDC Dc, const WCHAR *Application)
{
    HWND Control = Gallery->Controls[PROGRESS_NORMAL];
    UINT Pass;

    for (Pass = 0; Pass < 2; Pass++)
    {
        UINT Before = Gallery->ParentThemeCount;

        ok(SetWindowTheme(Control, Application, NULL) == S_OK,
           "cannot set the progress palette for parent theme change pass %u\n", Pass);
        Gallery->ParentThemeResult = E_FAIL;
        Gallery->ThemeApplication = Application ? NULL : L"ContentLight";
        Gallery->ThemeOnErase = Control;
        if (Pass)
            RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        else
            SendMessageW(Control, WM_PRINTCLIENT, (WPARAM)Dc, PRF_CLIENT);
        Gallery->ThemeOnErase = NULL;
        ok(Gallery->ParentThemeCount == Before + 1 && Gallery->ParentThemeResult == S_OK && IsWindow(Control),
           "progress parent theme change pass %u: callbacks %u, result %08lx, alive %d\n",
           Pass, Gallery->ParentThemeCount - Before, Gallery->ParentThemeResult, IsWindow(Control));
        ok(GetWindowTheme(Control) != NULL && SendMessageW(Control, PBM_GETPOS, 0, 0) == 63,
           "progress lost its theme or position during parent theme change pass %u\n", Pass);
        RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        CheckNativeCorners(Gallery, PROGRESS_NORMAL);
    }
}

static void RunRounded(BOOL Showcase, BOOL Lifetime)
{
    typedef BOOL (WINAPI *PINIT_COMMON_CONTROLS_EX)(const INITCOMMONCONTROLSEX *);
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_WIN95_CLASSES};
    ACTCTXW Context = {sizeof(Context)};
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), SURFACE_WIDTH, -SURFACE_HEIGHT, 1, 32, BI_RGB}};
    GALLERY Gallery = {0};
    ROUNDED_PALETTE_SAMPLE Samples[2] = {{0}};
    PINIT_COMMON_CONTROLS_EX Initialize;
    PSET_ROUNDED_APP_MODE SetMode = NULL;
    PALLOW_ROUNDED_DARK_MODE AllowDark = NULL;
    WCHAR Path[MAX_PATH], Capture[16];
    HANDLE Activation = INVALID_HANDLE_VALUE;
    HMODULE Module = NULL, UxTheme;
    ULONG_PTR Cookie = 0;
    BOOL Activated = FALSE, Initialized, ModeChanged = FALSE;
    HDC Dc = NULL;
    HBITMAP Bitmap = NULL, Old = NULL;
    DWORD *Bits = NULL, *First = NULL;
    int PreviousMode = RoundedAppDefault;
    UINT i, Palette;

    trace("RoundedControls checks the ReliefWin product style; no Windows 11 parity baseline is available\n");
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        return;
    }
    if (!GetModuleFileNameW(NULL, Path, ARRAYSIZE(Path)))
    {
        ok(FALSE, "cannot locate the test manifest\n");
        return;
    }
    Context.lpSource = Path;
    Context.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
    Context.lpResourceName = MAKEINTRESOURCEW(2);
    Activation = CreateActCtxW(&Context);
    ok(Activation != INVALID_HANDLE_VALUE, "CreateActCtxW failed, error %lu\n", GetLastError());
    if (Activation == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Activated = ActivateActCtx(Activation, &Cookie);
    ok(Activated, "ActivateActCtx failed, error %lu\n", GetLastError());
    if (!Activated)
        goto Cleanup;
    Module = LoadLibraryW(L"comctl32.dll");
    ok(Module != NULL, "cannot load themed common controls\n");
    Initialize = Module ? (PINIT_COMMON_CONTROLS_EX)GetProcAddress(Module, "InitCommonControlsEx") : NULL;
    Initialized = Initialize != NULL && Initialize(&Controls);
    ok(Initialized, "cannot initialize themed common controls\n");
    if (!Initialized || !CreateGallery(&Gallery, Showcase))
        goto Cleanup;
    if (GetModuleFileNameW(Module, Path, ARRAYSIZE(Path)))
        trace("RoundedControls common-controls module: %ls\n", Path);
    if (Showcase)
    {
        DWORD Duration = 20000, Start = GetTickCount();

        if (GetEnvironmentVariableW(L"UXTHEME_ROUNDED_CAPTURE_MS", Capture, ARRAYSIZE(Capture)))
            Duration = min(wcstoul(Capture, NULL, 10), 180000);
        trace("ROUNDED_SHOWCASE_READY hwnd=%p duration=%lu\n", Gallery.Window, Duration);
        while (IsWindow(Gallery.Window) && GetTickCount() - Start < Duration)
            Pump(25);
        goto Cleanup;
    }
    UxTheme = GetModuleHandleW(L"uxtheme.dll");
    SetMode = UxTheme ? (PSET_ROUNDED_APP_MODE)GetProcAddress(UxTheme, MAKEINTRESOURCEA(135)) : NULL;
    AllowDark = UxTheme ? (PALLOW_ROUNDED_DARK_MODE)GetProcAddress(UxTheme, MAKEINTRESOURCEA(133)) : NULL;
    ok(SetMode && AllowDark, "the existing ReliefWin palette selection ordinals are unavailable\n");
    if (!SetMode || !AllowDark)
        goto Cleanup;
    PreviousMode = SetMode(RoundedAppForceDark);
    ModeChanged = TRUE;
    ok(AllowDark(Gallery.Window, TRUE), "cannot allow dark content on the gallery root\n");
    trace("Palette selection follows ReliefWin's existing private ordinal consumer contract\n");
    Dc = CreateCompatibleDC(NULL);
    Bitmap = Dc ? CreateDIBSection(Dc, &Info, DIB_RGB_COLORS, (void **)&Bits, NULL, 0) : NULL;
    First = HeapAlloc(GetProcessHeap(), 0, SURFACE_WIDTH * SURFACE_HEIGHT * sizeof(*First));
    ok(Dc != NULL && Bitmap != NULL && First != NULL, "cannot allocate theme test surface\n");
    if (!Dc || !Bitmap || !First)
        goto Cleanup;
    Old = SelectObject(Dc, Bitmap);
    for (Palette = 0; Palette < 2; Palette++)
    {
        const WCHAR *Application = Palette ? L"ContentLight" : NULL;

        SetMode(Palette ? RoundedAppForceLight : RoundedAppForceDark);
        trace("RoundedControls palette %u\n", Palette);
        ok(SetWindowTheme(Gallery.Window, Application, NULL) == S_OK, "cannot select palette %u\n", Palette);
        SampleRoundedPalette(Dc, Bits, Palette, &Samples[Palette]);
        if (Lifetime)
        {
            CheckParentDestruction(&Gallery, Dc, Application, PROGRESS_CLASSW);
            CheckProgressParentThemeChange(&Gallery, Dc, Application);
            continue;
        }
        for (i = 0; i < ARRAYSIZE(Parts); i++)
            TestPart(Gallery.Window, &Parts[i], Dc, Bits, First);
        CheckThemeRegionResources(Gallery.Window, Dc);
        for (i = 0; i < CONTROL_COUNT; i++)
            ok(SetWindowTheme(Gallery.Controls[i], Application, NULL) == S_OK,
               "cannot select palette %u for native control %u\n", Palette, i);
        SendMessageW(Gallery.Controls[RADIO_FIRST], BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(Gallery.Controls[RADIO_SECOND], BM_SETCHECK, BST_UNCHECKED, 0);
        RedrawWindow(Gallery.Window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
        Pump(100);
        TestNative(&Gallery);
        CheckParentDestruction(&Gallery, Dc, Application, UPDOWN_CLASSW);
    }
    ok(Samples[0].Valid && Samples[1].Valid && Samples[0].TextColor != Samples[1].TextColor,
       "dark/light Button text colors did not differ: %08lx, %08lx\n", Samples[0].TextColor, Samples[1].TextColor);
    ok(Samples[0].Valid && Samples[1].Valid && Samples[0].Signature != Samples[1].Signature,
       "dark/light Button pixels did not differ: %08lx, %08lx\n", Samples[0].Signature, Samples[1].Signature);

Cleanup:
    if (Old)
        SelectObject(Dc, Old);
    if (Bitmap)
        DeleteObject(Bitmap);
    if (Dc)
        DeleteDC(Dc);
    if (First)
        HeapFree(GetProcessHeap(), 0, First);
    if (Gallery.Window && IsWindow(Gallery.Window))
        DestroyWindow(Gallery.Window);
    if (ModeChanged)
        SetMode(PreviousMode);
    if (Gallery.Background)
        DeleteObject(Gallery.Background);
    UnregisterClassW(L"ReliefWinRoundedControls", GetModuleHandleW(NULL));
    if (Module)
        FreeLibrary(Module);
    if (Activated)
        DeactivateActCtx(0, Cookie);
    if (Activation != INVALID_HANDLE_VALUE)
        ReleaseActCtx(Activation);
}

START_TEST(RoundedControls)
{
    RunRounded(FALSE, FALSE);
}

START_TEST(RoundedControlsShowcase)
{
    RunRounded(TRUE, FALSE);
}

START_TEST(RoundedControlLifetime)
{
    RunRounded(FALSE, TRUE);
}

START_TEST(RoundedControlDpi)
{
    typedef HTHEME (WINAPI *POPENTHEMEDATAFORDPI)(HWND, LPCWSTR, UINT);
    static const UINT Dpis[] = {96, 144, 192};
    static const struct
    {
        const WCHAR *Classes[2];
        int Part;
        int State;
        LONG NativeSize;
    } Cases[] =
    {
        {{L"Button", L"ContentLight::Button"}, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, 16},
        {{L"Button", L"ContentLight::Button"}, BP_RADIOBUTTON, RBS_UNCHECKEDNORMAL, 16},
        {{L"Trackbar", L"ContentLight::Trackbar"}, TKP_THUMB, TUS_NORMAL, 20},
        {{L"Trackbar", L"ContentLight::Trackbar"}, TKP_THUMBBOTTOM, TUBS_NORMAL, 20},
        {{L"Trackbar", L"ContentLight::Trackbar"}, TKP_THUMBTOP, TUTS_NORMAL, 20},
        {{L"Trackbar", L"ContentLight::Trackbar"}, TKP_THUMBVERT, TUVS_NORMAL, 20},
        {{L"Trackbar", L"ContentLight::Trackbar"}, TKP_THUMBLEFT, TUVLS_NORMAL, 20},
        {{L"Trackbar", L"ContentLight::Trackbar"}, TKP_THUMBRIGHT, TUVRS_NORMAL, 20}
    };
    HMODULE Module = GetModuleHandleW(L"uxtheme.dll");
    POPENTHEMEDATAFORDPI OpenForDpi = Module ?
        (POPENTHEMEDATAFORDPI)GetProcAddress(Module, "OpenThemeDataForDpi") : NULL;
    PSET_ROUNDED_APP_MODE SetMode = Module ?
        (PSET_ROUNDED_APP_MODE)GetProcAddress(Module, MAKEINTRESOURCEA(135)) : NULL;
    HDC Dc;
    int PreviousMode;
    UINT Palette, Index, Dpi;

    trace("RoundedControlDpi checks the ReliefWin product style; no Windows 11 parity baseline is available\n");
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        return;
    }
    ok(OpenForDpi != NULL && SetMode != NULL, "DPI or existing ReliefWin palette selection entry point is missing\n");
    if (!OpenForDpi || !SetMode)
        return;
    Dc = GetDC(NULL);
    ok(Dc != NULL, "cannot get the display DC for rounded control DPI checks\n");
    if (!Dc)
        return;
    PreviousMode = SetMode(RoundedAppForceDark);
    trace("DPI palette selection follows ReliefWin's existing private ordinal consumer contract\n");
    for (Palette = 0; Palette < 2; Palette++)
    {
        SetMode(Palette ? RoundedAppForceLight : RoundedAppForceDark);
        for (Index = 0; Index < ARRAYSIZE(Cases); Index++)
        {
            const WCHAR *ClassName = Cases[Index].Classes[Palette];

            for (Dpi = 0; Dpi < ARRAYSIZE(Dpis); Dpi++)
            {
                HTHEME Theme = OpenForDpi(NULL, ClassName, Dpis[Dpi]);
                LONG Expected = MulDiv(Cases[Index].NativeSize, Dpis[Dpi], 96);
                SIZE True = {0}, Draw = {0};
                HRESULT Result;

                ok(Theme != NULL, "%ls part %d cannot open at %u DPI\n", ClassName, Cases[Index].Part, Dpis[Dpi]);
                if (!Theme)
                    continue;
                Result = GetThemePartSize(Theme, Dc, Cases[Index].Part, Cases[Index].State, NULL, TS_TRUE, &True);
                ok(Result == S_OK && True.cx == Cases[Index].NativeSize && True.cy == Cases[Index].NativeSize,
                   "%ls part %d source size at %u DPI is %ld,%ld, result %08lx, expected %ld,%ld\n",
                   ClassName, Cases[Index].Part, Dpis[Dpi], True.cx, True.cy, Result,
                   Cases[Index].NativeSize, Cases[Index].NativeSize);
                Result = GetThemePartSize(Theme, Dc, Cases[Index].Part, Cases[Index].State, NULL, TS_DRAW, &Draw);
                ok(Result == S_OK && Draw.cx == Expected && Draw.cy == Expected,
                   "%ls part %d draw size at %u DPI is %ld,%ld, result %08lx, expected %ld,%ld\n",
                   ClassName, Cases[Index].Part, Dpis[Dpi], Draw.cx, Draw.cy, Result, Expected, Expected);
                CloseThemeData(Theme);
            }
        }
    }
    SetMode(PreviousMode);
    ReleaseDC(NULL, Dc);
}

typedef struct
{
    HBRUSH Background;
    HBRUSH Marker;
    RECT MarkerRect;
    UINT EraseCount;
    UINT PrintCount;
} PARENT_BACKGROUND_CONTEXT;

static LRESULT CALLBACK ParentBackgroundProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    PARENT_BACKGROUND_CONTEXT *Context = (PARENT_BACKGROUND_CONTEXT *)GetWindowLongPtrW(Window, GWLP_USERDATA);

    if (Message == WM_NCCREATE)
    {
        Context = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Context);
    }
    if (Context && (Message == WM_ERASEBKGND || Message == WM_PRINTCLIENT))
    {
        RECT Rect;

        if (Message == WM_ERASEBKGND)
            Context->EraseCount++;
        else
            Context->PrintCount++;
        GetClientRect(Window, &Rect);
        FillRect((HDC)WParam, &Rect, Context->Background);
        FillRect((HDC)WParam, &Context->MarkerRect, Context->Marker);
        return 1;
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

START_TEST(ParentBackgroundClip)
{
    static const RECT Target = {4, 3, 26, 18};
    static const RECT ExistingClip = {6, 4, 24, 16};
    static const POINT Viewport = {24, 20};
    WNDCLASSW Class = {0};
    PARENT_BACKGROUND_CONTEXT Context = {0};
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), SURFACE_WIDTH, -SURFACE_HEIGHT, 1, 32, BI_RGB}};
    HWND Parent = NULL, Child = NULL;
    HDC Dc = NULL;
    HBITMAP Bitmap = NULL, Old = NULL;
    HRGN Before = NULL, After = NULL, Initial = NULL;
    DWORD *Bits = NULL;
    POINT Origin = {0};
    UINT Case;

    trace("ParentBackgroundClip checks the documented child-coordinate drawing bounds; Windows 11 baseline unavailable\n");
    Class.lpfnWndProc = ParentBackgroundProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.lpszClassName = L"ReliefWinParentBackgroundClip";
    Context.Background = CreateSolidBrush(RGB(37, 103, 151));
    Context.Marker = CreateSolidBrush(RGB(209, 71, 39));
    ok(RegisterClassW(&Class) != 0, "cannot register the parent background test class\n");
    Parent = CreateWindowExW(0, Class.lpszClassName, L"", WS_POPUP, 0, 0, 160, 120,
                             NULL, NULL, Class.hInstance, &Context);
    Child = Parent ? CreateWindowExW(0, L"Static", L"", WS_CHILD, 19, 13, 32, 24,
                                    Parent, NULL, Class.hInstance, NULL) : NULL;
    Dc = CreateCompatibleDC(NULL);
    Bitmap = Dc ? CreateDIBSection(Dc, &Info, DIB_RGB_COLORS, (void **)&Bits, NULL, 0) : NULL;
    Before = CreateRectRgn(0, 0, 0, 0);
    After = CreateRectRgn(0, 0, 0, 0);
    Initial = CreateRectRgn(Viewport.x + ExistingClip.left, Viewport.y + ExistingClip.top,
                            Viewport.x + ExistingClip.right, Viewport.y + ExistingClip.bottom);
    ok(Parent && Child && Dc && Bitmap && Before && After && Initial && Context.Background && Context.Marker,
       "cannot allocate parent background clipping fixtures\n");
    if (!Parent || !Child || !Dc || !Bitmap || !Before || !After || !Initial ||
        !Context.Background || !Context.Marker)
        goto Cleanup;
    Old = SelectObject(Dc, Bitmap);
    MapWindowPoints(Child, Parent, &Origin, 1);
    SetRect(&Context.MarkerRect, Origin.x + 8, Origin.y + 6, Origin.x + 12, Origin.y + 10);
    for (Case = 0; Case < 4; Case++)
    {
        RECT DrawRect = {0, 0, 32, 24};
        RECT *Requested = Case & 1 ? (RECT *)&Target : NULL;
        BOOL Clipped = (Case & 2) != 0;
        UINT InsideErrors = 0, OutsideErrors = 0;
        UINT EraseCount = Context.EraseCount, PrintCount = Context.PrintCount;
        POINT Restored = {0};
        int BeforeType, AfterType, x, y;
        HRESULT Result;

        if (Requested)
            DrawRect = *Requested;
        for (y = 0; y < SURFACE_HEIGHT; y++)
            for (x = 0; x < SURFACE_WIDTH; x++)
                Bits[y * SURFACE_WIDTH + x] = 0x004875a6;
        ok(SetViewportOrgEx(Dc, Viewport.x, Viewport.y, NULL), "case %u cannot set the viewport\n", Case);
        ok(SelectClipRgn(Dc, Clipped ? Initial : NULL) != ERROR, "case %u cannot set the initial clip\n", Case);
        BeforeType = GetClipRgn(Dc, Before);
        ok(BeforeType == (Clipped ? 1 : 0), "case %u initial clip type %d\n", Case, BeforeType);
        Result = DrawThemeParentBackground(Child, Dc, Requested);
        GdiFlush();
        ok(Result == S_OK, "case %u DrawThemeParentBackground returned %08lx\n", Case, Result);
        trace("case %u parent callbacks: erase %u, print %u\n", Case,
              Context.EraseCount - EraseCount, Context.PrintCount - PrintCount);
        for (y = 0; y < SURFACE_HEIGHT; y++)
        {
            for (x = 0; x < SURFACE_WIDTH; x++)
            {
                POINT Point = {x - Viewport.x, y - Viewport.y};
                BOOL Inside = PtInRect(&DrawRect, Point) && (!Clipped || PtInRect(&ExistingClip, Point));
                DWORD Expected = Inside ? (Point.x >= 8 && Point.x < 12 && Point.y >= 6 && Point.y < 10 ?
                                          0x00d14727 : 0x00256797) : 0x004875a6;

                if ((Bits[y * SURFACE_WIDTH + x] & 0x00ffffff) != Expected)
                {
                    if (Inside)
                        InsideErrors++;
                    else
                        OutsideErrors++;
                }
            }
        }
        ok(!OutsideErrors, "case %u changed %u pixels outside the requested child area\n", Case, OutsideErrors);
        ok(!InsideErrors, "case %u mapped %u pixels incorrectly inside the requested child area\n", Case, InsideErrors);
        ok(GetViewportOrgEx(Dc, &Restored) && Restored.x == Viewport.x && Restored.y == Viewport.y,
           "case %u viewport after drawing is %ld,%ld\n", Case, Restored.x, Restored.y);
        AfterType = GetClipRgn(Dc, After);
        ok(AfterType == BeforeType && (AfterType != 1 || EqualRgn(Before, After)),
           "case %u changed the application clipping region, before %d, after %d\n", Case, BeforeType, AfterType);
    }

Cleanup:
    if (Old)
        SelectObject(Dc, Old);
    if (Bitmap)
        DeleteObject(Bitmap);
    if (Dc)
        DeleteDC(Dc);
    if (Before)
        DeleteObject(Before);
    if (After)
        DeleteObject(After);
    if (Initial)
        DeleteObject(Initial);
    if (Parent)
        DestroyWindow(Parent);
    if (Context.Background)
        DeleteObject(Context.Background);
    if (Context.Marker)
        DeleteObject(Context.Marker);
    UnregisterClassW(Class.lpszClassName, Class.hInstance);
}

typedef struct
{
    HWND Page;
    HWND Controls[4];
} PARENT_SIBLING_CONTEXT;

static INT_PTR CALLBACK ParentSiblingPageProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    PARENT_SIBLING_CONTEXT *Context;

    if (Message != WM_INITDIALOG)
        return FALSE;
    Context = (PARENT_SIBLING_CONTEXT *)((PROPSHEETPAGEW *)LParam)->lParam;
    Context->Page = Window;
    Context->Controls[0] = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
                                           18, 18, 48, 24, Window, NULL, GetModuleHandleW(NULL), NULL);
    Context->Controls[1] = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"Sibling edit",
                                           WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                           18, 58, 180, 28, Window, NULL, GetModuleHandleW(NULL), NULL);
    Context->Controls[2] = CreateWindowExW(0, L"Button", L"Earlier button", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                           18, 104, 100, 30, Window, NULL, GetModuleHandleW(NULL), NULL);
    Context->Controls[3] = CreateWindowExW(0, L"Button", L"Last button", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                           142, 104, 100, 30, Window, NULL, GetModuleHandleW(NULL), NULL);
    return TRUE;
}

static void SampleParentSibling(HWND Window, COLORREF *Samples, BOOL Compare, UINT Control, UINT Pass)
{
    HDC Dc = GetDC(Window);
    RECT Rect;
    UINT x, y;

    ok(Dc != NULL, "control %u pass %u cannot acquire a sampling DC\n", Control, Pass);
    if (!Dc)
        return;
    GetClientRect(Window, &Rect);
    for (y = 0; y < 3; y++)
    {
        for (x = 0; x < 3; x++)
        {
            UINT Index = y * 3 + x;
            COLORREF Actual = GetPixel(Dc, (x + 1) * Rect.right / 4, (y + 1) * Rect.bottom / 4);

            if (Compare)
                ok(Actual == Samples[Index], "control %u pass %u sample %u changed from %08lx to %08lx\n",
                   Control, Pass, Index, Samples[Index], Actual);
            else
                Samples[Index] = Actual;
            ok(Actual != CLR_INVALID, "control %u pass %u sample %u is unavailable\n", Control, Pass, Index);
        }
    }
    ReleaseDC(Window, Dc);
}

START_TEST(ParentBackgroundSiblings)
{
    typedef BOOL (WINAPI *PINIT_COMMON_CONTROLS_EX)(const INITCOMMONCONTROLSEX *);
    typedef INT_PTR (WINAPI *PPROPERTY_SHEET_W)(LPCPROPSHEETHEADERW);
    struct
    {
        DLGTEMPLATE Dialog;
        WORD Menu;
        WORD Class;
        WORD Title;
    } Template = {{WS_CHILD | WS_VISIBLE | DS_CONTROL, 0, 0, 0, 0, 180, 110}, 0, 0, 0};
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_WIN95_CLASSES};
    ACTCTXW Context = {sizeof(Context)};
    PROPSHEETPAGEW Page = {sizeof(Page)};
    PROPSHEETHEADERW Header = {sizeof(Header)};
    PARENT_SIBLING_CONTEXT Siblings = {0};
    PINIT_COMMON_CONTROLS_EX Initialize;
    PPROPERTY_SHEET_W CreateSheet;
    HANDLE Activation = INVALID_HANDLE_VALUE;
    HMODULE Module = NULL;
    HWND Sheet = NULL;
    ULONG_PTR Cookie = 0;
    BOOL Activated = FALSE, Initialized;
    WCHAR Path[MAX_PATH];
    COLORREF Samples[3][9];
    UINT i;

    trace("ParentBackgroundSiblings checks native property-page sibling preservation; Windows 11 baseline unavailable\n");
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        return;
    }
    if (!GetModuleFileNameW(NULL, Path, ARRAYSIZE(Path)))
    {
        ok(FALSE, "cannot locate the themed controls manifest\n");
        return;
    }
    Context.lpSource = Path;
    Context.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
    Context.lpResourceName = MAKEINTRESOURCEW(2);
    Activation = CreateActCtxW(&Context);
    ok(Activation != INVALID_HANDLE_VALUE, "cannot create the themed controls activation context\n");
    if (Activation == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Activated = ActivateActCtx(Activation, &Cookie);
    ok(Activated, "cannot activate themed controls\n");
    if (!Activated)
        goto Cleanup;
    Module = LoadLibraryW(L"comctl32.dll");
    Initialize = Module ? (PINIT_COMMON_CONTROLS_EX)GetProcAddress(Module, "InitCommonControlsEx") : NULL;
    CreateSheet = Module ? (PPROPERTY_SHEET_W)GetProcAddress(Module, "PropertySheetW") : NULL;
    Initialized = Initialize && CreateSheet && Initialize(&Controls);
    ok(Initialized, "cannot initialize themed property sheets\n");
    if (!Initialized)
        goto Cleanup;
    Page.dwFlags = PSP_DLGINDIRECT | PSP_USETITLE;
    Page.hInstance = GetModuleHandleW(NULL);
    Page.pResource = &Template.Dialog;
    Page.pszTitle = L"Sibling controls";
    Page.pfnDlgProc = ParentSiblingPageProc;
    Page.lParam = (LPARAM)&Siblings;
    Header.dwFlags = PSH_MODELESS | PSH_NOAPPLYNOW | PSH_PROPSHEETPAGE;
    Header.hInstance = Page.hInstance;
    Header.pszCaption = L"Parent-background clipping";
    Header.nPages = 1;
    Header.ppsp = &Page;
    Sheet = (HWND)CreateSheet(&Header);
    ok(Sheet && Sheet != (HWND)-1 && IsWindow(Sheet), "cannot create the sibling property sheet\n");
    if (!Sheet || Sheet == (HWND)-1 || !IsWindow(Sheet))
    {
        Sheet = NULL;
        goto Cleanup;
    }
    ok(Siblings.Page && !(GetWindowLongW(Siblings.Page, GWL_STYLE) & WS_CLIPCHILDREN),
       "the property page is missing or clips child windows\n");
    for (i = 0; i < ARRAYSIZE(Siblings.Controls); i++)
    {
        ok(Siblings.Controls[i] != NULL, "cannot create sibling control %u\n", i);
        if (!Siblings.Controls[i])
            goto Cleanup;
    }
    ShowWindow(Sheet, SW_SHOWNORMAL);
    SetForegroundWindow(Sheet);
    SetFocus(Siblings.Controls[3]);
    RedrawWindow(Sheet, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
    Pump(100);
    for (i = 0; i < ARRAYSIZE(Samples); i++)
    {
        RedrawWindow(Siblings.Controls[i], NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW);
        SampleParentSibling(Siblings.Controls[i], Samples[i], FALSE, i, 0);
    }
    RedrawWindow(Sheet, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
    GdiFlush();
    for (i = 0; i < ARRAYSIZE(Samples); i++)
        SampleParentSibling(Siblings.Controls[i], Samples[i], TRUE, i, 1);
    RedrawWindow(Siblings.Controls[3], NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW);
    GdiFlush();
    for (i = 0; i < ARRAYSIZE(Samples); i++)
        SampleParentSibling(Siblings.Controls[i], Samples[i], TRUE, i, 2);

Cleanup:
    if (Sheet)
        DestroyWindow(Sheet);
    if (Module)
        FreeLibrary(Module);
    if (Activated)
        DeactivateActCtx(0, Cookie);
    if (Activation != INVALID_HANDLE_VALUE)
        ReleaseActCtx(Activation);
}
