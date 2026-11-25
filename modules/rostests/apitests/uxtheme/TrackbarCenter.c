/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Native round trackbar channel and thumb alignment
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <apitest.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <uxtheme.h>
#include <vsstyle.h>

#define TRACK_SURFACE 256

typedef BOOL (WINAPI *PINIT_TRACK_CONTROLS)(const INITCOMMONCONTROLSEX *);
typedef int (WINAPI *PSET_TRACK_APP_MODE)(int);
typedef BOOLEAN (WINAPI *PALLOW_TRACK_DARK)(HWND, BOOLEAN);
typedef HTHEME (WINAPI *POPEN_TRACK_FOR_DPI)(HWND, LPCWSTR, UINT);

typedef struct
{
    HBRUSH Background;
    RECT Channel;
    RECT Thumb;
    UINT ChannelCount;
    UINT ThumbCount;
} TRACK_PARENT;

static LRESULT CALLBACK TrackParentProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    TRACK_PARENT *Parent = (TRACK_PARENT *)GetWindowLongPtrW(Window, GWLP_USERDATA);

    if (Message == WM_NCCREATE)
    {
        Parent = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Parent);
    }
    if (Parent && Message == WM_ERASEBKGND)
    {
        RECT Rect;

        GetClientRect(Window, &Rect);
        FillRect((HDC)WParam, &Rect, Parent->Background);
        return TRUE;
    }
    if (Parent && Message == WM_CTLCOLORSTATIC)
        return (LRESULT)Parent->Background;
    if (Parent && Message == WM_NOTIFY)
    {
        NMCUSTOMDRAW *Draw = (NMCUSTOMDRAW *)LParam;

        if (Draw->hdr.code == NM_CUSTOMDRAW)
        {
            if (Draw->dwDrawStage == CDDS_PREPAINT)
                return CDRF_NOTIFYITEMDRAW;
            if (Draw->dwDrawStage == CDDS_ITEMPREPAINT)
            {
                if (Draw->dwItemSpec == TBCD_CHANNEL)
                {
                    Parent->Channel = Draw->rc;
                    Parent->ChannelCount++;
                }
                else if (Draw->dwItemSpec == TBCD_THUMB)
                {
                    Parent->Thumb = Draw->rc;
                    Parent->ThumbCount++;
                }
            }
        }
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

static BOOL FindTrackPixels(const DWORD *Pixels, const RECT *Client, DWORD Color, RECT *Bounds)
{
    BOOL Found = FALSE;
    int x, y;

    SetRect(Bounds, Client->right, Client->bottom, 0, 0);
    for (y = 0; y < Client->bottom; y++)
    {
        for (x = 0; x < Client->right; x++)
        {
            if ((Pixels[y * TRACK_SURFACE + x] & 0x00ffffff) != Color)
                continue;
            Bounds->left = min(Bounds->left, x);
            Bounds->right = max(Bounds->right, x + 1);
            Bounds->top = min(Bounds->top, y);
            Bounds->bottom = max(Bounds->bottom, y + 1);
            Found = TRUE;
        }
    }
    return Found;
}

static DWORD CheckTrackCenter(HWND Window, TRACK_PARENT *Parent, HDC Dc, DWORD *Pixels,
                              UINT Palette, const char *Phase)
{
    static const DWORD TrackColors[] = {0x0043474e, 0x00cdcdcd};
    static const DWORD ThumbColors[] = {0x004cc2ff, 0x000067c0};
    RECT Client, Channel, Thumb, TrackInk, ThumbInk;
    BOOL Vertical = (GetWindowLongW(Window, GWL_STYLE) & TBS_VERT) != 0;
    BOOL HasTrack, HasThumb;
    LONG TrackCenter, ThumbCenter;
    DWORD Signature = 2166136261u;
    int x, y;

    GetClientRect(Window, &Client);
    ok(Client.right > 0 && Client.bottom > 0 && Client.right <= TRACK_SURFACE && Client.bottom <= TRACK_SURFACE,
       "palette %u %s invalid capture bounds %ld,%ld\n", Palette, Phase, Client.right, Client.bottom);
    if (Client.right <= 0 || Client.bottom <= 0 || Client.right > TRACK_SURFACE || Client.bottom > TRACK_SURFACE)
        return 0;
    GdiFlush();
    for (y = 0; y < Client.bottom; y++)
        for (x = 0; x < Client.right; x++)
            Pixels[y * TRACK_SURFACE + x] = 0x00b539a6;
    Parent->ChannelCount = Parent->ThumbCount = 0;
    SendMessageW(Window, WM_PRINTCLIENT, (WPARAM)Dc, PRF_CLIENT | PRF_ERASEBKGND);
    GdiFlush();
    for (y = 0; y < Client.bottom; y++)
        for (x = 0; x < Client.right; x++)
            Signature = (Signature ^ (Pixels[y * TRACK_SURFACE + x] & 0x00ffffff)) * 16777619u;
    SendMessageW(Window, TBM_GETCHANNELRECT, 0, (LPARAM)&Channel);
    SendMessageW(Window, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
    HasTrack = FindTrackPixels(Pixels, &Client, TrackColors[Palette], &TrackInk);
    HasThumb = FindTrackPixels(Pixels, &Client, ThumbColors[Palette], &ThumbInk);
    ok(HasTrack && HasThumb, "palette %u %s missing native track %d or thumb %d\n", Palette, Phase, HasTrack, HasThumb);
    ok(Parent->ChannelCount && EqualRect(&Parent->Channel, &Channel),
       "palette %u %s channel custom draw disagrees with TBM_GETCHANNELRECT\n", Palette, Phase);
    ok(Parent->ThumbCount && EqualRect(&Parent->Thumb, &Thumb),
       "palette %u %s thumb custom draw disagrees with TBM_GETTHUMBRECT\n", Palette, Phase);
    if (!HasTrack || !HasThumb)
        return Signature;
    ok(ThumbInk.left >= Thumb.left && ThumbInk.top >= Thumb.top &&
       ThumbInk.right <= Thumb.right && ThumbInk.bottom <= Thumb.bottom,
       "palette %u %s thumb pixels escape TBM_GETTHUMBRECT\n", Palette, Phase);
    if (Vertical)
    {
        TrackCenter = TrackInk.left + TrackInk.right - 1;
        ThumbCenter = ThumbInk.left + ThumbInk.right - 1;
        ok(TrackInk.left == Channel.left && TrackInk.right == Channel.right,
           "palette %u %s visible track x=%ld..%ld differs from channel %ld..%ld\n", Palette, Phase,
           TrackInk.left, TrackInk.right, Channel.left, Channel.right);
    }
    else
    {
        TrackCenter = TrackInk.top + TrackInk.bottom - 1;
        ThumbCenter = ThumbInk.top + ThumbInk.bottom - 1;
        ok(TrackInk.top == Channel.top && TrackInk.bottom == Channel.bottom,
           "palette %u %s visible track y=%ld..%ld differs from channel %ld..%ld\n", Palette, Phase,
           TrackInk.top, TrackInk.bottom, Channel.top, Channel.bottom);
    }
    trace("TRACK_CENTER palette=%u phase=%s vertical=%d track_center2=%ld thumb_center2=%ld\n",
          Palette, Phase, Vertical, TrackCenter, ThumbCenter);
    ok(TrackCenter == ThumbCenter,
       "palette %u %s track and round thumb are off axis: twice centers %ld and %ld, delta %ld half pixels\n",
       Palette, Phase, TrackCenter, ThumbCenter, ThumbCenter - TrackCenter);
    return Signature;
}

static void CheckTrackPositions(HWND Window, TRACK_PARENT *Parent, HDC Dc, DWORD *Pixels,
                                UINT Palette, UINT Style, const char *Phase)
{
    static const LONG Positions[] = {0, 50, 100};
    char Label[96];
    RECT Previous = {0}, Thumb;
    BOOL Vertical = (GetWindowLongW(Window, GWL_STYLE) & TBS_VERT) != 0;
    UINT i;

    for (i = 0; i < ARRAYSIZE(Positions); i++)
    {
        SendMessageW(Window, TBM_SETPOS, TRUE, Positions[i]);
        sprintf(Label, "style%u-%s-pos%ld", Style, Phase, Positions[i]);
        CheckTrackCenter(Window, Parent, Dc, Pixels, Palette, Label);
        SendMessageW(Window, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
        ok(SendMessageW(Window, TBM_GETPOS, 0, 0) == Positions[i],
           "palette %u %s did not retain its position\n", Palette, Label);
        if (i)
            ok(Vertical ? Thumb.top > Previous.top : Thumb.left > Previous.left,
               "palette %u %s thumb did not advance along its axis\n", Palette, Label);
        Previous = Thumb;
    }
}

static void CheckTrackDpi(HDC Dc, UINT Palette, POPEN_TRACK_FOR_DPI OpenForDpi)
{
    static const UINT Dpis[] = {96, 144, 192};
    static const int Parts[] = {TKP_THUMB, TKP_THUMBBOTTOM, TKP_THUMBTOP,
                                TKP_THUMBVERT, TKP_THUMBLEFT, TKP_THUMBRIGHT};
    UINT Dpi, Part;

    if (!OpenForDpi)
        return;
    for (Dpi = 0; Dpi < ARRAYSIZE(Dpis); Dpi++)
    {
        HTHEME Theme = OpenForDpi(NULL, L"Trackbar", Dpis[Dpi]);

        ok(Theme != NULL, "palette %u cannot open trackbar at %u DPI\n", Palette, Dpis[Dpi]);
        if (!Theme)
            continue;
        for (Part = 0; Part < ARRAYSIZE(Parts); Part++)
        {
            SIZE Source = {0}, Draw = {0};
            LONG Expected = MulDiv(20, Dpis[Dpi], 96);
            HRESULT Result;

            Result = GetThemePartSize(Theme, Dc, Parts[Part], TUS_NORMAL, NULL, TS_TRUE, &Source);
            ok(Result == S_OK && Source.cx == 20 && Source.cy == 20,
               "palette %u part %d at %u DPI source is %ld,%ld, result %08lx\n",
               Palette, Parts[Part], Dpis[Dpi], Source.cx, Source.cy, Result);
            Result = GetThemePartSize(Theme, Dc, Parts[Part], TUS_NORMAL, NULL, TS_DRAW, &Draw);
            ok(Result == S_OK && Draw.cx == Expected && Draw.cy == Expected,
               "palette %u part %d at %u DPI draw size is %ld,%ld, expected %ld, result %08lx\n",
               Palette, Parts[Part], Dpis[Dpi], Draw.cx, Draw.cy, Expected, Result);
        }
        CloseThemeData(Theme);
    }
}

static void ReopenTrackTheme(HWND Window, UINT Palette, UINT Style)
{
    HRESULT Result = SetWindowTheme(Window, L"", L"");

    ok(Result == S_OK && GetWindowTheme(Window) == NULL,
       "palette %u style %u cannot disable the trackbar theme, result %08lx\n", Palette, Style, Result);
    Result = SetWindowTheme(Window, NULL, NULL);
    ok(Result == S_OK && GetWindowTheme(Window) != NULL,
       "palette %u style %u cannot restore the trackbar theme, result %08lx\n", Palette, Style, Result);
}

static void CheckTrackPaintResources(HWND Window, HDC Dc)
{
    DWORD Before, After;
    UINT Pass;

    for (Pass = 0; Pass < 4; Pass++)
        SendMessageW(Window, WM_PRINTCLIENT, (WPARAM)Dc, PRF_CLIENT | PRF_ERASEBKGND);
    GdiFlush();
    Before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    ok(Before != 0, "cannot read GDI resources after trackbar paint warmup\n");
    for (Pass = 0; Pass < 64; Pass++)
        SendMessageW(Window, WM_PRINTCLIENT, (WPARAM)Dc, PRF_CLIENT | PRF_ERASEBKGND);
    GdiFlush();
    After = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    trace("TRACK_PAINT_RESOURCES frames=64 before=%lu after=%lu\n", Before, After);
    ok(After != 0 && After <= Before,
       "64 native trackbar paints increased GDI resources from %lu to %lu\n", Before, After);
}

static DWORD CheckTrackMatrix(HWND ParentWindow, TRACK_PARENT *Parent, HDC Dc, DWORD *Pixels,
                              UINT Palette)
{
    static const DWORD Styles[] = {TBS_AUTOTICKS, TBS_AUTOTICKS | TBS_TOP,
                                   TBS_AUTOTICKS | TBS_BOTH, TBS_NOTICKS,
                                   TBS_TOP | TBS_NOTICKS, TBS_BOTH | TBS_NOTICKS,
                                   TBS_AUTOTICKS | TBS_ENABLESELRANGE};
    static const UINT Lengths[] = {20, 21, 40};
    DWORD Signature = 0;
    UINT Axis, Style, Fixed, Length;

    for (Axis = 0; Axis < 2; Axis++)
    {
        for (Style = 0; Style < ARRAYSIZE(Styles); Style++)
        {
            for (Fixed = 0; Fixed < 2; Fixed++)
            {
                DWORD Flags = WS_CHILD | WS_VISIBLE | Styles[Style] | (Axis ? TBS_VERT : 0) |
                              (Fixed ? TBS_FIXEDLENGTH : 0);
                HWND Window = CreateWindowExW(0, TRACKBAR_CLASSW, L"", Flags, 0, 0,
                                                Axis ? 64 : 224, Axis ? 224 : 64,
                                                ParentWindow, NULL, GetModuleHandleW(NULL), NULL);
                char Phase[64];
                RECT BeforeChannel, BeforeThumb, AfterChannel, AfterThumb;

                ok(Window != NULL, "palette %u axis %u style %u fixed %u cannot create a native trackbar\n",
                   Palette, Axis, Style, Fixed);
                if (!Window)
                    continue;
                ok(GetWindowTheme(Window) != NULL, "palette %u axis %u style %u fixed %u has no theme\n",
                   Palette, Axis, Style, Fixed);
                ok(GetDpiForWindow(Window) == 96, "native pixel matrix requires the 96 DPI test display\n");
                SendMessageW(Window, TBM_SETRANGE, FALSE, MAKELPARAM(0, 100));
                SendMessageW(Window, TBM_SETPOS, TRUE, 50);
                sprintf(Phase, "axis%u-style%u-fixed%u-created", Axis, Style, Fixed);
                if (!Axis && !Style && !Fixed)
                {
                    Signature = CheckTrackCenter(Window, Parent, Dc, Pixels, Palette, Phase);
                    if (!Palette)
                        CheckTrackPaintResources(Window, Dc);
                }
                else
                    CheckTrackCenter(Window, Parent, Dc, Pixels, Palette, Phase);
                if (Fixed)
                {
                    for (Length = 0; Length < ARRAYSIZE(Lengths); Length++)
                    {
                        SendMessageW(Window, TBM_SETTHUMBLENGTH, Lengths[Length], 0);
                        ok(SendMessageW(Window, TBM_GETTHUMBLENGTH, 0, 0) == Lengths[Length],
                           "palette %u axis %u style %u did not set thumb length %u\n",
                           Palette, Axis, Style, Lengths[Length]);
                        sprintf(Phase, "axis%u-fixed%u", Axis, Lengths[Length]);
                        CheckTrackPositions(Window, Parent, Dc, Pixels, Palette, Style, Phase);
                        SendMessageW(Window, WM_THEMECHANGED, 0, 0);
                        ReopenTrackTheme(Window, Palette, Style);
                        sprintf(Phase, "axis%u-fixed%u-themechange", Axis, Lengths[Length]);
                        CheckTrackPositions(Window, Parent, Dc, Pixels, Palette, Style, Phase);
                    }
                }
                else
                {
                    CheckTrackPositions(Window, Parent, Dc, Pixels, Palette, Style, "created");
                    ok(SetWindowPos(Window, NULL, 0, 0, Axis ? 80 : 192, Axis ? 192 : 80,
                                    SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE),
                       "palette %u axis %u style %u cannot resize\n", Palette, Axis, Style);
                    CheckTrackPositions(Window, Parent, Dc, Pixels, Palette, Style, "resized");
                    SendMessageW(Window, TBM_GETCHANNELRECT, 0, (LPARAM)&BeforeChannel);
                    SendMessageW(Window, TBM_GETTHUMBRECT, 0, (LPARAM)&BeforeThumb);
                    SendMessageW(Window, WM_THEMECHANGED, 0, 0);
                    ReopenTrackTheme(Window, Palette, Style);
                    sprintf(Phase, "axis%u-style%u-themechange", Axis, Style);
                    CheckTrackCenter(Window, Parent, Dc, Pixels, Palette, Phase);
                    SendMessageW(Window, TBM_GETCHANNELRECT, 0, (LPARAM)&AfterChannel);
                    SendMessageW(Window, TBM_GETTHUMBRECT, 0, (LPARAM)&AfterThumb);
                    ok(EqualRect(&BeforeChannel, &AfterChannel) && EqualRect(&BeforeThumb, &AfterThumb),
                       "palette %u axis %u style %u changed geometry when reopening the same theme\n",
                       Palette, Axis, Style);
                    CheckTrackPositions(Window, Parent, Dc, Pixels, Palette, Style, "themechange");
                }
                DestroyWindow(Window);
            }
        }
    }
    return Signature;
}

START_TEST(TrackbarCenter)
{
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_BAR_CLASSES};
    ACTCTXW Context = {sizeof(Context)};
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), TRACK_SURFACE, -TRACK_SURFACE, 1, 32, BI_RGB}};
    WNDCLASSW Class = {0};
    TRACK_PARENT Parent = {0};
    HMODULE UxTheme = GetModuleHandleW(L"uxtheme.dll"), Module = NULL;
    PSET_TRACK_APP_MODE SetMode = UxTheme ? (PSET_TRACK_APP_MODE)GetProcAddress(UxTheme, MAKEINTRESOURCEA(135)) : NULL;
    PALLOW_TRACK_DARK AllowDark = UxTheme ? (PALLOW_TRACK_DARK)GetProcAddress(UxTheme, MAKEINTRESOURCEA(133)) : NULL;
    POPEN_TRACK_FOR_DPI OpenForDpi = UxTheme ? (POPEN_TRACK_FOR_DPI)GetProcAddress(UxTheme, "OpenThemeDataForDpi") : NULL;
    PINIT_TRACK_CONTROLS Initialize;
    HANDLE Activation = INVALID_HANDLE_VALUE;
    ULONG_PTR Cookie = 0;
    HWND Window = NULL;
    HDC Dc = NULL;
    HBITMAP Bitmap = NULL, Old = NULL;
    DWORD *Pixels = NULL;
    DWORD Signatures[2] = {0};
    WCHAR Path[MAX_PATH];
    BOOL Activated = FALSE, ModeChanged = FALSE, Registered = FALSE, Initialized;
    int PreviousMode = 0;
    UINT Palette;

    trace("TrackbarCenter checks the ReliefWin round theme contract; Windows 11 pixel parity is not asserted\n");
    trace("Native pixels use the 96 DPI display; 144/192 DPI checks query resource sizes only\n");
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        return;
    }
    ok(SetMode && AllowDark, "palette selection entry points are unavailable\n");
    if (!SetMode || !AllowDark)
        return;
    if (!GetModuleFileNameW(NULL, Path, ARRAYSIZE(Path)))
    {
        ok(FALSE, "cannot locate the common-controls manifest\n");
        return;
    }
    Context.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
    Context.lpSource = Path;
    Context.lpResourceName = MAKEINTRESOURCEW(2);
    Activation = CreateActCtxW(&Context);
    ok(Activation != INVALID_HANDLE_VALUE, "cannot create the common-controls activation context\n");
    if (Activation == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Activated = ActivateActCtx(Activation, &Cookie);
    ok(Activated, "cannot activate themed common controls\n");
    if (!Activated)
        goto Cleanup;
    Module = LoadLibraryW(L"comctl32.dll");
    Initialize = Module ? (PINIT_TRACK_CONTROLS)GetProcAddress(Module, "InitCommonControlsEx") : NULL;
    Initialized = Initialize && Initialize(&Controls);
    ok(Initialized, "cannot initialize native trackbars\n");
    if (!Initialized)
        goto Cleanup;
    Parent.Background = CreateSolidBrush(RGB(181, 57, 166));
    Class.lpfnWndProc = TrackParentProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.lpszClassName = L"ReliefWinTrackbarCenter";
    Registered = RegisterClassW(&Class) != 0;
    ok(Registered, "cannot register the trackbar parent\n");
    if (!Registered || !Parent.Background)
        goto Cleanup;
    Window = CreateWindowExW(0, Class.lpszClassName, L"", WS_POPUP, 0, 0,
                             TRACK_SURFACE, TRACK_SURFACE, NULL, NULL, Class.hInstance, &Parent);
    Dc = CreateCompatibleDC(NULL);
    Bitmap = Dc ? CreateDIBSection(Dc, &Info, DIB_RGB_COLORS, (void **)&Pixels, NULL, 0) : NULL;
    ok(Window && Dc && Bitmap && Pixels, "cannot allocate native trackbar fixtures\n");
    if (!Window || !Dc || !Bitmap || !Pixels)
        goto Cleanup;
    Old = SelectObject(Dc, Bitmap);
    ok(Old && Old != HGDI_ERROR, "cannot select the trackbar capture bitmap\n");
    if (!Old || Old == HGDI_ERROR)
    {
        Old = NULL;
        goto Cleanup;
    }
    PreviousMode = SetMode(2);
    ModeChanged = TRUE;
    ok(AllowDark(Window, TRUE), "cannot allow dark content on the test parent\n");
    ok(OpenForDpi != NULL, "OpenThemeDataForDpi is unavailable\n");
    for (Palette = 0; Palette < 2; Palette++)
    {
        SetMode(Palette ? 3 : 2);
        Signatures[Palette] = CheckTrackMatrix(Window, &Parent, Dc, Pixels, Palette);
        CheckTrackDpi(Dc, Palette, OpenForDpi);
    }
    ok(Signatures[0] != 0 && Signatures[1] != 0 && Signatures[0] != Signatures[1],
       "dark and light native trackbar signatures are %08lx and %08lx\n", Signatures[0], Signatures[1]);

Cleanup:
    if (Window)
        DestroyWindow(Window);
    if (ModeChanged)
        SetMode(PreviousMode);
    if (Old)
        SelectObject(Dc, Old);
    if (Bitmap)
        DeleteObject(Bitmap);
    if (Dc)
        DeleteDC(Dc);
    if (Registered)
        UnregisterClassW(Class.lpszClassName, Class.hInstance);
    if (Parent.Background)
        DeleteObject(Parent.Background);
    if (Module)
        FreeLibrary(Module);
    if (Activated)
        DeactivateActCtx(0, Cookie);
    if (Activation != INVALID_HANDLE_VALUE)
        ReleaseActCtx(Activation);
}

typedef struct
{
    HWND Trackbar;
    BOOL Armed;
    BOOL Counting;
    BOOL Destroyed;
    UINT DestroyCount;
    UINT SecondMoves;
} TRACK_BUDDY_CONTEXT;

typedef struct
{
    TRACK_BUDDY_CONTEXT *Context;
    BOOL First;
} TRACK_BUDDY_NODE;

static LRESULT CALLBACK TrackBuddyProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    TRACK_BUDDY_NODE *Node = (TRACK_BUDDY_NODE *)GetWindowLongPtrW(Window, GWLP_USERDATA);

    if (Message == WM_NCCREATE)
    {
        Node = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Node);
    }
    if (Node && Message == WM_WINDOWPOSCHANGED)
    {
        TRACK_BUDDY_CONTEXT *Context = Node->Context;

        if (Node->First && Context->Armed)
        {
            Context->Armed = FALSE;
            Context->DestroyCount++;
            Context->Destroyed = DestroyWindow(Context->Trackbar);
        }
        else if (!Node->First && Context->Counting && !(((WINDOWPOS *)LParam)->flags & SWP_NOMOVE))
            Context->SecondMoves++;
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

START_TEST(TrackbarBuddyLifetime)
{
    static const char *Actions[] = {"size", "style", "theme", "buddy"};
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_BAR_CLASSES};
    ACTCTXW ActivationContext = {sizeof(ActivationContext)};
    WNDCLASSW Class = {0};
    HANDLE Activation = INVALID_HANDLE_VALUE;
    ULONG_PTR Cookie = 0;
    HMODULE Module = NULL;
    PINIT_TRACK_CONTROLS Initialize;
    HWND Parent = NULL;
    WCHAR Path[MAX_PATH];
    BOOL Activated = FALSE, Registered = FALSE, Initialized;
    UINT Axis, Action;

    trace("TrackbarBuddyLifetime checks destruction during buddy positioning on the fixed implementation\n");
    trace("No Windows 11 baseline or intentional unfixed use-after-free run is available\n");
    if (!GetModuleFileNameW(NULL, Path, ARRAYSIZE(Path)))
    {
        ok(FALSE, "cannot locate the trackbar buddy test manifest\n");
        return;
    }
    ActivationContext.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
    ActivationContext.lpSource = Path;
    ActivationContext.lpResourceName = MAKEINTRESOURCEW(2);
    Activation = CreateActCtxW(&ActivationContext);
    ok(Activation != INVALID_HANDLE_VALUE, "cannot create the buddy test activation context\n");
    if (Activation == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Activated = ActivateActCtx(Activation, &Cookie);
    ok(Activated, "cannot activate themed trackbar buddy tests\n");
    if (!Activated)
        goto Cleanup;
    Module = LoadLibraryW(L"comctl32.dll");
    Initialize = Module ? (PINIT_TRACK_CONTROLS)GetProcAddress(Module, "InitCommonControlsEx") : NULL;
    Initialized = Initialize && Initialize(&Controls);
    ok(Initialized, "cannot initialize trackbar buddy controls\n");
    if (!Initialized)
        goto Cleanup;
    Class.lpfnWndProc = TrackBuddyProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.lpszClassName = L"ReliefWinTrackbarBuddyLifetime";
    Registered = RegisterClassW(&Class) != 0;
    ok(Registered, "cannot register the trackbar buddy class\n");
    if (!Registered)
        goto Cleanup;
    Parent = CreateWindowExW(0, L"Static", L"", WS_POPUP, 0, 0, 320, 320,
                             NULL, NULL, Class.hInstance, NULL);
    ok(Parent != NULL, "cannot create the trackbar buddy parent\n");
    if (!Parent)
        goto Cleanup;
    for (Axis = 0; Axis < 2; Axis++)
    {
        for (Action = 0; Action < ARRAYSIZE(Actions); Action++)
        {
            TRACK_BUDDY_CONTEXT Context = {0};
            TRACK_BUDDY_NODE Nodes[2] = {{&Context, TRUE}, {&Context, FALSE}};
            HWND Buddies[2] = {NULL, NULL};
            DWORD Style = WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | (Axis ? TBS_VERT : 0);
            RECT BeforeSecond = {0}, AfterSecond = {0};
            UINT Buddy;

            Context.Trackbar = CreateWindowExW(0, TRACKBAR_CLASSW, L"", Style, 60, 60,
                                                Axis ? 64 : 180, Axis ? 180 : 64,
                                                Parent, NULL, Class.hInstance, NULL);
            for (Buddy = 0; Buddy < ARRAYSIZE(Buddies); Buddy++)
                Buddies[Buddy] = CreateWindowExW(0, Class.lpszClassName, L"", WS_CHILD | WS_VISIBLE,
                                                 0, 0, 20, 16, Parent, NULL, Class.hInstance, &Nodes[Buddy]);
            ok(Context.Trackbar && Buddies[0] && Buddies[1],
               "axis %u %s cannot create the buddy fixtures\n", Axis, Actions[Action]);
            if (!Context.Trackbar || !Buddies[0] || !Buddies[1])
                goto CaseCleanup;
            SendMessageW(Context.Trackbar, TBM_SETBUDDY, TRUE, (LPARAM)Buddies[0]);
            SendMessageW(Context.Trackbar, TBM_SETBUDDY, FALSE, (LPARAM)Buddies[1]);
            ok((HWND)SendMessageW(Context.Trackbar, TBM_GETBUDDY, TRUE, 0) == Buddies[0] &&
               (HWND)SendMessageW(Context.Trackbar, TBM_GETBUDDY, FALSE, 0) == Buddies[1],
               "axis %u %s did not attach both buddy windows\n", Axis, Actions[Action]);
            ok(SetWindowPos(Buddies[0], NULL, 3, 5, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE) &&
               SetWindowPos(Buddies[1], NULL, 7, 9, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE),
               "axis %u %s cannot prime buddy positions\n", Axis, Actions[Action]);
            ok(GetWindowRect(Buddies[1], &BeforeSecond),
               "axis %u %s cannot snapshot the second buddy\n", Axis, Actions[Action]);
            Context.Counting = TRUE;
            Context.Armed = TRUE;
            switch (Action)
            {
                case 0:
                    SetWindowPos(Context.Trackbar, NULL, 0, 0, Axis ? 72 : 192, Axis ? 192 : 72,
                                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                    break;
                case 1:
                    SetWindowLongPtrW(Context.Trackbar, GWL_STYLE, Style | TBS_BOTH);
                    break;
                case 2:
                    SendMessageW(Context.Trackbar, WM_THEMECHANGED, 0, 0);
                    break;
                case 3:
                    SendMessageW(Context.Trackbar, TBM_SETBUDDY, TRUE, (LPARAM)Buddies[0]);
                    break;
            }
            Context.Armed = FALSE;
            Context.Counting = FALSE;
            ok(Context.DestroyCount == 1 && Context.Destroyed && !IsWindow(Context.Trackbar),
               "axis %u %s callback count %u, destroyed %d, trackbar alive %d\n",
               Axis, Actions[Action], Context.DestroyCount, Context.Destroyed, IsWindow(Context.Trackbar));
            ok(IsWindow(Buddies[0]) && IsWindow(Buddies[1]) && GetWindowRect(Buddies[1], &AfterSecond),
               "axis %u %s lost a sibling buddy window\n", Axis, Actions[Action]);
            ok(Context.SecondMoves == 0 && EqualRect(&BeforeSecond, &AfterSecond),
               "axis %u %s moved the second buddy after trackbar destruction, notifications %u\n",
               Axis, Actions[Action], Context.SecondMoves);

CaseCleanup:
            Context.Armed = FALSE;
            Context.Counting = FALSE;
            if (Context.Trackbar && IsWindow(Context.Trackbar))
                DestroyWindow(Context.Trackbar);
            for (Buddy = 0; Buddy < ARRAYSIZE(Buddies); Buddy++)
                if (Buddies[Buddy])
                    DestroyWindow(Buddies[Buddy]);
        }
    }

Cleanup:
    if (Parent)
        DestroyWindow(Parent);
    if (Registered)
        UnregisterClassW(Class.lpszClassName, Class.hInstance);
    if (Module)
        FreeLibrary(Module);
    if (Activated)
        DeactivateActCtx(0, Cookie);
    if (Activation != INVALID_HANDLE_VALUE)
        ReleaseActCtx(Activation);
}
