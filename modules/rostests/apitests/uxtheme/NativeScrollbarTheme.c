/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Native standalone scrollbar theme rendering and tracking lifetime
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <apitest.h>
#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <undocuser.h>

#define SCROLL_SURFACE 512
#define SCROLL_BACKGROUND RGB(181, 57, 166)
#define SCROLL_CUSTOM RGB(29, 143, 91)

#define TEST_TOP_ARROW 1
#define TEST_TOP_TRACK 2
#define TEST_THUMB 3
#define TEST_BOTTOM_TRACK 4
#define TEST_BOTTOM_ARROW 5

typedef int (WINAPI *PSET_SCROLL_APP_MODE)(int);
typedef BOOLEAN (WINAPI *PALLOW_SCROLL_DARK)(HWND, BOOLEAN);

typedef struct
{
    HDC Dc;
    HBITMAP Bitmap;
    HBITMAP OldBitmap;
    DWORD *Pixels;
} SCROLL_SURFACE_DATA;

typedef struct
{
    HBRUSH Background;
    HBRUSH CustomBrush;
    BOOL UseCustomBrush;
    HTHEME Reference;
    SCROLL_SURFACE_DATA Actual;
    SCROLL_SURFACE_DATA Expected;
    HWND Target;
    HWND DestroyOnErase;
    HWND DestroyOnColor;
    HWND ThemeOnErase;
    LPCWSTR NextApplication;
    HRESULT SwitchResult;
    UINT DestroyCount;
    UINT SwitchCount;
    UINT LineCount;
    UINT EndCount;
    UINT TrackCount;
    UINT PositionCount;
    int DragDelta;
    BOOL InspectTracking;
    UINT Palette;
    USERAPIHOOKPROC ShutdownHook;
    BOOL ShutdownOnErase;
    UINT ShutdownCount;
} SCROLL_PARENT;

static void PumpScrollbarMessages(void)
{
    MSG Message;

    while (PeekMessageW(&Message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessageW(&Message);
    }
}

static BOOL GetScrollbarRects(HWND Window, SCROLLBARINFO *Info, RECT *First,
                              RECT *Track, RECT *Thumb, RECT *Last)
{
    POINT Origin = {0};
    BOOL Vertical = (GetWindowLongW(Window, GWL_STYLE) & SBS_VERT) != 0;

    ZeroMemory(Info, sizeof(*Info));
    Info->cbSize = sizeof(*Info);
    if (!GetScrollBarInfo(Window, OBJID_CLIENT, Info) || !ClientToScreen(Window, &Origin))
        return FALSE;
    OffsetRect(&Info->rcScrollBar, -Origin.x, -Origin.y);
    *First = *Track = *Thumb = *Last = Info->rcScrollBar;
    if (Vertical)
    {
        First->bottom = First->top + Info->dxyLineButton;
        Last->top = Last->bottom - Info->dxyLineButton;
        Track->top = First->bottom;
        Track->bottom = Last->top;
        Thumb->top += Info->xyThumbTop;
        Thumb->bottom = Info->rcScrollBar.top + Info->xyThumbBottom;
    }
    else
    {
        First->right = First->left + Info->dxyLineButton;
        Last->left = Last->right - Info->dxyLineButton;
        Track->left = First->right;
        Track->right = Last->left;
        Thumb->left += Info->xyThumbTop;
        Thumb->right = Info->rcScrollBar.left + Info->xyThumbBottom;
    }
    return TRUE;
}

static void DrawExpectedScrollbar(HWND Window, SCROLL_PARENT *Parent, BOOL PressedArrow,
                                  BOOL PressedThumb, int ThumbOffset)
{
    SCROLLBARINFO Info;
    RECT First, Track, Thumb, Last, Upper, Lower, Client;
    BOOL Vertical = (GetWindowLongW(Window, GWL_STYLE) & SBS_VERT) != 0;
    int FirstState = Vertical ? ABS_UPNORMAL : ABS_LEFTNORMAL;
    int LastState = Vertical ? ABS_DOWNNORMAL : ABS_RIGHTNORMAL;
    int State = IsWindowEnabled(Window) ? SCRBS_NORMAL : SCRBS_DISABLED;
    HRESULT Result;

    GetClientRect(Window, &Client);
    FillRect(Parent->Expected.Dc, &Client, Parent->Background);
    if (!GetScrollbarRects(Window, &Info, &First, &Track, &Thumb, &Last))
        return;
    if (!IsWindowEnabled(Window) || (Info.rgstate[TEST_TOP_ARROW] & STATE_SYSTEM_UNAVAILABLE))
        FirstState += ABS_UPDISABLED - ABS_UPNORMAL;
    else if (PressedArrow)
        FirstState += ABS_UPPRESSED - ABS_UPNORMAL;
    if (!IsWindowEnabled(Window) || (Info.rgstate[TEST_BOTTOM_ARROW] & STATE_SYSTEM_UNAVAILABLE))
        LastState += ABS_UPDISABLED - ABS_UPNORMAL;
    Result = DrawThemeBackground(Parent->Reference, Parent->Expected.Dc, SBP_ARROWBTN, FirstState, &First, NULL);
    ok(Result == S_OK, "palette %u cannot draw expected first arrow: %08lx\n", Parent->Palette, Result);
    Result = DrawThemeBackground(Parent->Reference, Parent->Expected.Dc, SBP_ARROWBTN, LastState, &Last, NULL);
    ok(Result == S_OK, "palette %u cannot draw expected last arrow: %08lx\n", Parent->Palette, Result);
    if (Parent->UseCustomBrush)
        FillRect(Parent->Expected.Dc, &Track, Parent->CustomBrush);
    else
        DrawThemeBackground(Parent->Reference, Parent->Expected.Dc,
                            Vertical ? SBP_LOWERTRACKVERT : SBP_LOWERTRACKHORZ, State, &Track, NULL);
    if (Info.xyThumbTop <= 0 || Info.xyThumbBottom <= Info.xyThumbTop)
        return;
    OffsetRect(&Thumb, Vertical ? 0 : ThumbOffset, Vertical ? ThumbOffset : 0);
    Upper = Lower = Track;
    if (Vertical)
    {
        Upper.bottom = Thumb.top;
        Lower.top = Thumb.bottom;
    }
    else
    {
        Upper.right = Thumb.left;
        Lower.left = Thumb.right;
    }
    if (!Parent->UseCustomBrush)
    {
        DrawThemeBackground(Parent->Reference, Parent->Expected.Dc,
                            Vertical ? SBP_UPPERTRACKVERT : SBP_UPPERTRACKHORZ, State, &Upper, NULL);
        DrawThemeBackground(Parent->Reference, Parent->Expected.Dc,
                            Vertical ? SBP_LOWERTRACKVERT : SBP_LOWERTRACKHORZ, State, &Lower, NULL);
    }
    Result = DrawThemeBackground(Parent->Reference, Parent->Expected.Dc,
                                Vertical ? SBP_THUMBBTNVERT : SBP_THUMBBTNHORZ,
                                PressedThumb ? SCRBS_PRESSED : State, &Thumb, NULL);
    ok(Result == S_OK, "palette %u cannot draw expected thumb: %08lx\n", Parent->Palette, Result);
}

static DWORD CheckScrollbarPixelsMode(HWND Window, SCROLL_PARENT *Parent, const char *Phase,
                                      BOOL PressedArrow, BOOL PressedThumb, int ThumbOffset, BOOL Themed)
{
    RECT Client;
    HDC Dc;
    LONG x, y, FirstX = -1, FirstY = -1;
    DWORD Differences = 0, Signature = 2166136261u, ActualColor = 0, ExpectedColor = 0;

    GetClientRect(Window, &Client);
    ok(Client.right > 0 && Client.bottom > 0 && Client.right <= SCROLL_SURFACE && Client.bottom <= SCROLL_SURFACE,
       "palette %u %s invalid client size %ld,%ld\n", Parent->Palette, Phase, Client.right, Client.bottom);
    if (Client.right <= 0 || Client.bottom <= 0 || Client.right > SCROLL_SURFACE || Client.bottom > SCROLL_SURFACE)
        return 0;
    DrawExpectedScrollbar(Window, Parent, PressedArrow, PressedThumb, ThumbOffset);
    Dc = GetDC(Window);
    ok(Dc != NULL, "palette %u %s cannot acquire native scrollbar DC\n", Parent->Palette, Phase);
    if (!Dc)
        return 0;
    ok(BitBlt(Parent->Actual.Dc, 0, 0, Client.right, Client.bottom, Dc, 0, 0, SRCCOPY),
       "palette %u %s cannot capture native scrollbar\n", Parent->Palette, Phase);
    ReleaseDC(Window, Dc);
    GdiFlush();
    for (y = 0; y < Client.bottom; y++)
    {
        for (x = 0; x < Client.right; x++)
        {
            DWORD Actual = Parent->Actual.Pixels[y * SCROLL_SURFACE + x] & 0x00ffffff;
            DWORD Expected = Parent->Expected.Pixels[y * SCROLL_SURFACE + x] & 0x00ffffff;

            Signature = (Signature ^ Actual) * 16777619u;
            if (Actual == Expected)
                continue;
            if (!Differences)
            {
                FirstX = x;
                FirstY = y;
                ActualColor = Actual;
                ExpectedColor = Expected;
            }
            Differences++;
        }
    }
    trace("NATIVE_SCROLL palette=%u phase=%s vertical=%d signature=%08lx differences=%lu\n",
          Parent->Palette, Phase, (GetWindowLongW(Window, GWL_STYLE) & SBS_VERT) != 0, Signature, Differences);
    if (Themed)
        ok(!Differences, "palette %u %s has %lu mismatched native pixels; first (%ld,%ld) %06lx expected %06lx\n",
           Parent->Palette, Phase, Differences, FirstX, FirstY, ActualColor, ExpectedColor);
    else
        ok(Differences != 0, "palette %u %s ignored classic rendering optout\n", Parent->Palette, Phase);
    return Signature;
}

static DWORD CheckScrollbarPixels(HWND Window, SCROLL_PARENT *Parent, const char *Phase,
                                  BOOL PressedArrow, BOOL PressedThumb, int ThumbOffset)
{
    return CheckScrollbarPixelsMode(Window, Parent, Phase, PressedArrow, PressedThumb, ThumbOffset, TRUE);
}

static LRESULT CALLBACK ScrollbarParentProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    SCROLL_PARENT *Parent = (SCROLL_PARENT *)GetWindowLongPtrW(Window, GWLP_USERDATA);

    if (Message == WM_NCCREATE)
    {
        Parent = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Parent);
    }
    if (Parent && (Message == WM_ERASEBKGND || Message == WM_PRINTCLIENT))
    {
        RECT Rect;
        HWND Target;

        GetClientRect(Window, &Rect);
        FillRect((HDC)WParam, &Rect, Parent->Background);
        if (Parent->ShutdownOnErase)
        {
            HTHEME Owner = GetWindowTheme(Parent->Target);
            SIZE Size;

            Parent->ShutdownOnErase = FALSE;
            Parent->ShutdownCount++;
            ok(Parent->ShutdownHook(uahShutdown, NULL), "cannot stop theme owner lifetime inside native paint\n");
            ok(!GetWindowTheme(Parent->Target), "theme shutdown retained a native window owner property\n");
            ok(Owner && GetThemePartSize(Owner, (HDC)WParam, SBP_THUMBBTNHORZ,
                                        SCRBS_NORMAL, NULL, TS_TRUE, &Size) == S_OK,
               "theme shutdown invalidated the renderer scoped reference before its callback returned\n");
        }
        Target = Parent->DestroyOnErase;
        if (Target)
        {
            Parent->DestroyOnErase = NULL;
            Parent->DestroyCount++;
            ok(DestroyWindow(Target), "cannot destroy scrollbar in parent background callback\n");
        }
        Target = Parent->ThemeOnErase;
        if (Target)
        {
            Parent->ThemeOnErase = NULL;
            Parent->SwitchCount++;
            Parent->SwitchResult = SetWindowTheme(Target, Parent->NextApplication, NULL);
        }
        return TRUE;
    }
    if (Parent && Message == WM_CTLCOLORSCROLLBAR)
    {
        HWND Target = Parent->DestroyOnColor;

        if (Target)
        {
            Parent->DestroyOnColor = NULL;
            Parent->DestroyCount++;
            ok(DestroyWindow(Target), "cannot destroy scrollbar in control color callback\n");
        }
        if (Parent->UseCustomBrush)
            return (LRESULT)Parent->CustomBrush;
    }
    if (Parent && (Message == WM_HSCROLL || Message == WM_VSCROLL))
    {
        UINT Code = LOWORD(WParam);

        ok((HWND)LParam == Parent->Target, "scroll notification identifies %p instead of control %p\n",
           (HWND)LParam, Parent->Target);
        if (Code == SB_LINEUP)
        {
            Parent->LineCount++;
            if (Parent->InspectTracking)
                CheckScrollbarPixels((HWND)LParam, Parent, "arrow-pressed", TRUE, FALSE, 0);
        }
        else if (Code == SB_THUMBTRACK)
        {
            if (Parent->InspectTracking)
            {
                SendMessageW((HWND)LParam, WM_MOUSELEAVE, 0, 0);
                CheckScrollbarPixels((HWND)LParam, Parent, Parent->TrackCount ? "thumb-moving" : "thumb-pressed",
                                     FALSE, TRUE, Parent->TrackCount ? Parent->DragDelta : 0);
            }
            Parent->TrackCount++;
        }
        else if (Code == SB_THUMBPOSITION)
            Parent->PositionCount++;
        else if (Code == SB_ENDSCROLL)
            Parent->EndCount++;
        return 0;
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

static BOOL CreateScrollSurface(SCROLL_SURFACE_DATA *Surface)
{
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), SCROLL_SURFACE, -SCROLL_SURFACE, 1, 32, BI_RGB}};

    Surface->Dc = CreateCompatibleDC(NULL);
    Surface->Bitmap = Surface->Dc ? CreateDIBSection(Surface->Dc, &Info, DIB_RGB_COLORS,
                                                   (void **)&Surface->Pixels, NULL, 0) : NULL;
    if (!Surface->Dc || !Surface->Bitmap || !Surface->Pixels)
        return FALSE;
    Surface->OldBitmap = SelectObject(Surface->Dc, Surface->Bitmap);
    return Surface->OldBitmap && Surface->OldBitmap != HGDI_ERROR;
}

static void DestroyScrollSurface(SCROLL_SURFACE_DATA *Surface)
{
    if (Surface->OldBitmap && Surface->OldBitmap != HGDI_ERROR)
        SelectObject(Surface->Dc, Surface->OldBitmap);
    if (Surface->Bitmap)
        DeleteObject(Surface->Bitmap);
    if (Surface->Dc)
        DeleteDC(Surface->Dc);
}

static HWND CreateNativeScrollbar(HWND Window, SCROLL_PARENT *Parent, BOOL Vertical, UINT Palette)
{
    HWND Control = CreateWindowExW(0, L"ScrollBar", L"", WS_CHILD | WS_VISIBLE | (Vertical ? SBS_VERT : SBS_HORZ),
                                   Vertical ? 380 : 20, Vertical ? 20 : 250,
                                   Vertical ? 20 : 320, Vertical ? 200 : 20,
                                   Window, (HMENU)(INT_PTR)(300 + Vertical), GetModuleHandleW(NULL), NULL);
    SCROLLINFO Info = {sizeof(Info), SIF_RANGE | SIF_PAGE | SIF_POS, 0, 200, 30, 60};

    ok(Control != NULL, "palette %u cannot create native axis %d scrollbar\n", Palette, Vertical);
    if (!Control)
        return NULL;
    ok(GetWindowTheme(Control) != NULL, "palette %u native scrollbar has no creation lifetime theme owner\n", Palette);
    if (Palette)
        ok(SetWindowTheme(Control, L"ContentLight", NULL) == S_OK, "cannot select light scrollbar theme\n");
    SetScrollInfo(Control, SB_CTL, &Info, TRUE);
    RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    Parent->Target = Control;
    return Control;
}

static void CheckScrollbarSetters(HWND Window, SCROLL_PARENT *Parent)
{
    SCROLLINFO Info = {sizeof(Info), SIF_POS};

    Info.nPos = 70;
    SetScrollInfo(Window, SB_CTL, &Info, TRUE);
    UpdateWindow(Window);
    ok(GetScrollPos(Window, SB_CTL) == 70, "SetScrollInfo did not retain native position\n");
    CheckScrollbarPixels(Window, Parent, "SetScrollInfo", FALSE, FALSE, 0);
    SetScrollPos(Window, SB_CTL, 80, TRUE);
    UpdateWindow(Window);
    ok(GetScrollPos(Window, SB_CTL) == 80, "SetScrollPos did not retain native position\n");
    CheckScrollbarPixels(Window, Parent, "SetScrollPos", FALSE, FALSE, 0);
    Info.nPos = 90;
    SendMessageW(Window, SBM_SETSCROLLINFO, TRUE, (LPARAM)&Info);
    UpdateWindow(Window);
    ok(SendMessageW(Window, SBM_GETPOS, 0, 0) == 90, "SBM_SETSCROLLINFO did not retain native position\n");
    CheckScrollbarPixels(Window, Parent, "SBM_SETSCROLLINFO", FALSE, FALSE, 0);
    SendMessageW(Window, SBM_SETPOS, 100, TRUE);
    UpdateWindow(Window);
    ok(GetScrollPos(Window, SB_CTL) == 100, "SBM_SETPOS did not retain native position\n");
    CheckScrollbarPixels(Window, Parent, "SBM_SETPOS", FALSE, FALSE, 0);
    SendMessageW(Window, SBM_SETRANGEREDRAW, 0, 240);
    UpdateWindow(Window);
    Info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    ok(GetScrollInfo(Window, SB_CTL, &Info) && Info.nMin == 0 && Info.nMax == 240 && Info.nPage == 30 && Info.nPos == 100,
       "SBM_SETRANGEREDRAW changed unrelated state\n");
    CheckScrollbarPixels(Window, Parent, "SBM_SETRANGEREDRAW", FALSE, FALSE, 0);
    ok(EnableScrollBar(Window, SB_CTL, ESB_DISABLE_LTUP), "cannot disable first scrollbar arrow\n");
    CheckScrollbarPixels(Window, Parent, "arrow-disabled", FALSE, FALSE, 0);
    ok(EnableScrollBar(Window, SB_CTL, ESB_ENABLE_BOTH), "cannot reenable scrollbar arrows\n");
    CheckScrollbarPixels(Window, Parent, "arrows-enabled", FALSE, FALSE, 0);
    EnableWindow(Window, FALSE);
    UpdateWindow(Window);
    CheckScrollbarPixels(Window, Parent, "control-disabled", FALSE, FALSE, 0);
    EnableWindow(Window, TRUE);
    UpdateWindow(Window);
    CheckScrollbarPixels(Window, Parent, "control-enabled", FALSE, FALSE, 0);
}

static void CheckScrollbarBoundaries(HWND Window, SCROLL_PARENT *Parent)
{
    SCROLLINFO Info = {sizeof(Info), SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL};
    RECT Client;

    SetScrollInfo(Window, SB_CTL, &Info, TRUE);
    UpdateWindow(Window);
    CheckScrollbarPixels(Window, Parent, "empty-range", FALSE, FALSE, 0);
    Info.nMax = 12;
    Info.nPage = 40;
    Info.nPos = 90;
    SetScrollInfo(Window, SB_CTL, &Info, TRUE);
    UpdateWindow(Window);
    CheckScrollbarPixels(Window, Parent, "page-covers-range", FALSE, FALSE, 0);
    Info.nMax = 240;
    Info.nPage = 30;
    Info.nPos = 100;
    SetScrollInfo(Window, SB_CTL, &Info, TRUE);
    UpdateWindow(Window);
    CheckScrollbarPixels(Window, Parent, "range-restored", FALSE, FALSE, 0);
    GetClientRect(Window, &Client);
    ok(SetWindowPos(Window, NULL, 0, 0, 20, 20, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE),
       "cannot reduce native scrollbar to its arrow bounds\n");
    RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    CheckScrollbarPixels(Window, Parent, "empty-track", FALSE, FALSE, 0);
    ok(SetWindowPos(Window, NULL, 0, 0, Client.right, Client.bottom,
                    SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE), "cannot restore native scrollbar bounds\n");
    RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    CheckScrollbarPixels(Window, Parent, "bounds-restored", FALSE, FALSE, 0);
}

static void CheckScrollbarTracking(HWND Window, SCROLL_PARENT *Parent)
{
    SCROLLBARINFO Info;
    RECT First, Track, Thumb, Last;
    POINT Click, Move;
    BOOL Vertical = (GetWindowLongW(Window, GWL_STYLE) & SBS_VERT) != 0;
    SCROLLINFO Position = {sizeof(Position), SIF_POS};
    int OriginalPosition;

    GetScrollInfo(Window, SB_CTL, &Position);
    OriginalPosition = Position.nPos;
    if (!GetScrollbarRects(Window, &Info, &First, &Track, &Thumb, &Last))
    {
        ok(FALSE, "cannot query native scrollbar tracking geometry\n");
        return;
    }
    PumpScrollbarMessages();
    Parent->InspectTracking = TRUE;
    Parent->LineCount = Parent->EndCount = 0;
    Click.x = (First.left + First.right) / 2;
    Click.y = (First.top + First.bottom) / 2;
    ok(PostMessageW(Window, WM_LBUTTONUP, 0, MAKELPARAM(Click.x, Click.y)), "cannot queue scrollbar arrow release\n");
    SendMessageW(Window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(Click.x, Click.y));
    ok(Parent->LineCount && Parent->EndCount, "native arrow tracking omitted line or end notification: %u,%u\n",
       Parent->LineCount, Parent->EndCount);
    ok(GetCapture() != Window, "native arrow tracking retained capture after release\n");
    CheckScrollbarPixels(Window, Parent, "arrow-released", FALSE, FALSE, 0);
    Parent->TrackCount = Parent->PositionCount = Parent->EndCount = 0;
    Parent->DragDelta = 36;
    Click.x = (Thumb.left + Thumb.right) / 2;
    Click.y = (Thumb.top + Thumb.bottom) / 2;
    Move = Click;
    if (Vertical)
        Move.y += Parent->DragDelta;
    else
        Move.x += Parent->DragDelta;
    ok(PostMessageW(Window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(Move.x, Move.y)), "cannot queue native thumb drag\n");
    ok(PostMessageW(Window, WM_LBUTTONUP, 0, MAKELPARAM(Move.x, Move.y)), "cannot queue native thumb release\n");
    SendMessageW(Window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(Click.x, Click.y));
    Parent->InspectTracking = FALSE;
    ok(Parent->TrackCount >= 2 && Parent->PositionCount && Parent->EndCount,
       "native thumb tracking omitted notifications: track %u position %u end %u\n",
       Parent->TrackCount, Parent->PositionCount, Parent->EndCount);
    ok(GetCapture() != Window, "native thumb tracking retained capture after release\n");
    ok(GetScrollPos(Window, SB_CTL) == OriginalPosition, "native tracking changed application-owned position\n");
    CheckScrollbarPixels(Window, Parent, "thumb-released", FALSE, FALSE, 0);
}

static void CheckScrollbarDc(HWND Window, SCROLL_PARENT *Parent, BOOL Destroy)
{
    HDC Dc = Parent->Actual.Dc;
    POINT BeforeOrigin, AfterOrigin, BeforeBrush, AfterBrush;
    RECT BeforeClip, AfterClip;
    COLORREF Text, Background;
    int Saved = SaveDC(Dc);

    ok(Saved != 0, "cannot preserve scrollbar DC fixture\n");
    if (!Saved)
        return;
    SetViewportOrgEx(Dc, 7, 9, NULL);
    SetBrushOrgEx(Dc, 3, 5, NULL);
    IntersectClipRect(Dc, 2, 2, 280, 280);
    SetTextColor(Dc, RGB(11, 31, 73));
    SetBkColor(Dc, RGB(43, 23, 19));
    Text = GetTextColor(Dc);
    Background = GetBkColor(Dc);
    GetViewportOrgEx(Dc, &BeforeOrigin);
    GetBrushOrgEx(Dc, &BeforeBrush);
    GetClipBox(Dc, &BeforeClip);
    if (Destroy)
        Parent->DestroyOnErase = Window;
    SendMessageW(Window, WM_PRINTCLIENT, (WPARAM)Dc, PRF_CLIENT | PRF_ERASEBKGND);
    GetViewportOrgEx(Dc, &AfterOrigin);
    GetBrushOrgEx(Dc, &AfterBrush);
    GetClipBox(Dc, &AfterClip);
    ok(BeforeOrigin.x == AfterOrigin.x && BeforeOrigin.y == AfterOrigin.y,
       "native scrollbar callback changed viewport origin\n");
    ok(BeforeBrush.x == AfterBrush.x && BeforeBrush.y == AfterBrush.y,
       "native scrollbar callback changed brush origin\n");
    ok(EqualRect(&BeforeClip, &AfterClip), "native scrollbar callback changed clip bounds\n");
    ok(Text == GetTextColor(Dc) && Background == GetBkColor(Dc), "native scrollbar callback changed DC colors\n");
    ok(RestoreDC(Dc, Saved), "cannot restore scrollbar DC fixture\n");
    if (Destroy)
        ok(!IsWindow(Window) && Parent->DestroyCount, "native scrollbar survived destructive parent callback\n");
}

static void CheckScrollbarModeSwitch(HWND Window, SCROLL_PARENT *Parent, PSET_SCROLL_APP_MODE SetMode)
{
    HTHEME Owner = GetWindowTheme(Window);

    SetMode(3);
    CloseThemeData(Parent->Reference);
    Parent->Reference = OpenThemeData(NULL, L"ContentLight::ScrollBar");
    ok(Parent->Reference != NULL, "cannot open independent light theme during app mode switch\n");
    RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    if (Parent->Reference)
        CheckScrollbarPixels(Window, Parent, "mode-without-theme-message", FALSE, FALSE, 0);
    ok(Owner && GetWindowTheme(Window) == Owner,
       "scoped native paint replaced its lifetime owner after an app mode switch\n");
    SetMode(2);
    CloseThemeData(Parent->Reference);
    Parent->Reference = OpenThemeData(NULL, L"ScrollBar");
    ok(Parent->Reference != NULL, "cannot restore independent dark theme after app mode switch\n");
    RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    if (Parent->Reference)
        CheckScrollbarPixels(Window, Parent, "mode-restored", FALSE, FALSE, 0);
    ok(Owner && GetWindowTheme(Window) == Owner,
       "native scrollbar lifetime owner changed when restoring app mode\n");
}

static void CheckScrollbarDestructiveTracking(HWND Window, SCROLL_PARENT *Parent)
{
    SCROLLBARINFO Info;
    RECT First, Track, Thumb, Last;
    POINT Click;

    if (!GetScrollbarRects(Window, &Info, &First, &Track, &Thumb, &Last))
    {
        ok(FALSE, "cannot query scrollbar before destructive tracking\n");
        return;
    }
    Click.x = (First.left + First.right) / 2;
    Click.y = (First.top + First.bottom) / 2;
    Parent->DestroyCount = 0;
    Parent->DestroyOnErase = Window;
    ok(PostMessageW(Window, WM_LBUTTONUP, 0, MAKELPARAM(Click.x, Click.y)),
       "cannot queue fallback release for destructive tracking\n");
    SendMessageW(Window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(Click.x, Click.y));
    Parent->DestroyOnErase = NULL;
    ok(!IsWindow(Window) && Parent->DestroyCount,
       "native tracking survived destructive background callback\n");
    ok(GetCapture() != Window, "destroyed scrollbar retained native tracking capture\n");
    PumpScrollbarMessages();
}

static void CheckScrollbarLifetime(HWND Window, SCROLL_PARENT *Parent)
{
    HTHEME Owner = GetWindowTheme(Window);
    DWORD Before, After;
    SIZE Size;
    UINT Frame;

    RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    Before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    for (Frame = 0; Frame < 64; Frame++)
        SendMessageW(Window, WM_PRINTCLIENT, (WPARAM)Parent->Actual.Dc, PRF_CLIENT | PRF_ERASEBKGND);
    After = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    ok(After <= Before + 1, "native scrollbar leaked GDI resources over 64 paints: %lu to %lu\n", Before, After);
    ok(Owner && GetWindowTheme(Window) == Owner, "scoped scrollbar paints replaced the window lifetime owner\n");
    ok(GetThemePartSize(Owner, Parent->Actual.Dc, SBP_THUMBBTNHORZ, SCRBS_NORMAL, NULL, TS_TRUE, &Size) == S_OK,
       "native scrollbar window lifetime theme is unusable after repeated paints\n");
}

static void RunNativeScrollbarTest(BOOL Shutdown, BOOL RenderingOnly)
{
    WNDCLASSW Class = {0};
    SCROLL_PARENT Parent = {0};
    HMODULE Module = GetModuleHandleW(L"uxtheme.dll");
    PSET_SCROLL_APP_MODE SetMode = Module ? (PSET_SCROLL_APP_MODE)GetProcAddress(Module, MAKEINTRESOURCEA(135)) : NULL;
    PALLOW_SCROLL_DARK AllowDark = Module ? (PALLOW_SCROLL_DARK)GetProcAddress(Module, MAKEINTRESOURCEA(133)) : NULL;
    HWND Window = NULL, Control = NULL, PreviousFocus = GetFocus();
    HTHEME Bootstrap = NULL;
    POINT Cursor;
    BOOL HaveCursor = GetCursorPos(&Cursor), Registered = FALSE, ModeChanged = FALSE, SurfacesReady;
    UINT Palette, Axis;
    int PreviousMode = 0;
    DWORD Signatures[2] = {0};

    trace("NativeScrollbarTheme checks the ReliefWin native ScrollBar product contract; Windows 11 parity is unverified\n");
    if (!IsThemeActive())
    {
        skip("No visual style is active\n");
        return;
    }
    ok(SetMode && AllowDark, "native scrollbar palette entry points unavailable\n");
    if (!SetMode || !AllowDark)
        return;
    PreviousMode = SetMode(2);
    ModeChanged = TRUE;
    Bootstrap = OpenThemeData(NULL, L"ScrollBar");
    ok(Bootstrap != NULL, "cannot initialize native scrollbar theme hooks\n");
    if (!Bootstrap)
        goto Cleanup;
    CloseThemeData(Bootstrap);
    Bootstrap = NULL;
    Parent.Background = CreateSolidBrush(SCROLL_BACKGROUND);
    Parent.CustomBrush = CreateSolidBrush(SCROLL_CUSTOM);
    Class.lpfnWndProc = ScrollbarParentProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.lpszClassName = L"ReliefWinNativeScrollbarTheme";
    Registered = RegisterClassW(&Class) != 0;
    ok(Registered && Parent.Background && Parent.CustomBrush, "cannot allocate native scrollbar parent resources\n");
    if (!Registered || !Parent.Background || !Parent.CustomBrush)
        goto Cleanup;
    Window = CreateWindowExW(0, Class.lpszClassName, L"", WS_POPUP | WS_VISIBLE, 20, 20, 440, 310,
                             NULL, NULL, Class.hInstance, &Parent);
    SurfacesReady = Window && CreateScrollSurface(&Parent.Actual) && CreateScrollSurface(&Parent.Expected);
    ok(SurfacesReady, "cannot allocate native scrollbar capture resources\n");
    if (!SurfacesReady)
        goto Cleanup;
    ok(AllowDark(Window, TRUE), "cannot allow dark native scrollbar palette\n");
    SetCursorPos(30, 30);
    UpdateWindow(Window);
    if (Shutdown)
    {
        COLORREF Outside;

        Parent.ShutdownHook = (USERAPIHOOKPROC)GetProcAddress(Module, "ThemeInitApiHook");
        ok(Parent.ShutdownHook != NULL, "native shutdown fixture requires the ReliefWin theme hook entry point\n");
        if (!Parent.ShutdownHook)
            goto Cleanup;
        Control = CreateNativeScrollbar(Window, &Parent, FALSE, 0);
        if (!Control)
            goto Cleanup;
        FillRect(Parent.Actual.Dc, &(RECT){0, 0, SCROLL_SURFACE, SCROLL_SURFACE}, Parent.CustomBrush);
        Parent.ShutdownOnErase = TRUE;
        CheckScrollbarDc(Control, &Parent, FALSE);
        ok(Parent.ShutdownCount == 1 && !GetWindowTheme(Control),
           "native paint did not execute the deterministic owner shutdown interleaving\n");
        GdiFlush();
        Outside = GetPixel(Parent.Actual.Dc, 37, 12);
        ok(Outside == SCROLL_CUSTOM, "aborted native paint fell back onto the changed window: %06lx\n", Outside);
        ok(GetPixel(Parent.Actual.Dc, 10, 12) == SCROLL_BACKGROUND,
           "native theme painted an arrow after its owner was stopped\n");
        goto Cleanup;
    }
    for (Palette = 0; Palette < 2; Palette++)
    {
        Parent.Palette = Palette;
        Parent.Reference = OpenThemeData(NULL, Palette ? L"ContentLight::ScrollBar" : L"ScrollBar");
        ok(Parent.Reference != NULL, "palette %u cannot open independent scrollbar reference\n", Palette);
        if (!Parent.Reference)
            continue;
        for (Axis = 0; Axis < 2; Axis++)
        {
            Control = CreateNativeScrollbar(Window, &Parent, Axis != 0, Palette);
            if (!Control)
                continue;
            Signatures[Palette] ^= CheckScrollbarPixels(Control, &Parent, "created", FALSE, FALSE, 0);
            CheckScrollbarSetters(Control, &Parent);
            CheckScrollbarTracking(Control, &Parent);
            CheckScrollbarBoundaries(Control, &Parent);
            CheckScrollbarDc(Control, &Parent, FALSE);
            Parent.UseCustomBrush = TRUE;
            RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            CheckScrollbarPixels(Control, &Parent, "custom-track-brush", FALSE, FALSE, 0);
            Parent.UseCustomBrush = FALSE;
            RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            if (!RenderingOnly)
            {
                CheckScrollbarLifetime(Control, &Parent);
                if (!Palette)
                    CheckScrollbarModeSwitch(Control, &Parent, SetMode);
            }
            ok(SetWindowTheme(Control, L"", L"") == S_OK && !GetWindowTheme(Control),
               "native scrollbar theme optout retained an owner\n");
            RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            CheckScrollbarPixelsMode(Control, &Parent, "classic-optout", FALSE, FALSE, 0, FALSE);
            ok(SetWindowTheme(Control, Palette ? L"ContentLight" : NULL, NULL) == S_OK && GetWindowTheme(Control),
               "native scrollbar failed to restore its lifetime theme owner\n");
            RedrawWindow(Control, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            CheckScrollbarPixels(Control, &Parent, "optout-restored", FALSE, FALSE, 0);
            if (RenderingOnly)
            {
                DestroyWindow(Control);
                Control = NULL;
                Parent.Target = NULL;
                continue;
            }
            Parent.ThemeOnErase = Control;
            Parent.NextApplication = Palette ? NULL : L"ContentLight";
            Parent.SwitchCount = 0;
            CheckScrollbarDc(Control, &Parent, FALSE);
            ok(Parent.SwitchCount && Parent.SwitchResult == S_OK && GetWindowTheme(Control),
               "native scrollbar failed theme replacement inside parent callback\n");
            Parent.ThemeOnErase = NULL;
            CloseThemeData(Parent.Reference);
            Parent.Reference = OpenThemeData(NULL, Palette ? L"ScrollBar" : L"ContentLight::ScrollBar");
            ok(Parent.Reference != NULL, "cannot reopen independent theme after callback palette switch\n");
            UpdateWindow(Control);
            if (Parent.Reference)
                CheckScrollbarPixels(Control, &Parent, "callback-theme-switch", FALSE, FALSE, 0);
            CloseThemeData(Parent.Reference);
            Parent.Reference = OpenThemeData(NULL, Palette ? L"ContentLight::ScrollBar" : L"ScrollBar");
            Parent.DestroyCount = 0;
            CheckScrollbarDc(Control, &Parent, TRUE);
            Parent.DestroyOnErase = NULL;
            if (IsWindow(Control))
                DestroyWindow(Control);
            Control = NULL;
            Parent.Target = NULL;
            Control = CreateNativeScrollbar(Window, &Parent, Axis != 0, Palette);
            if (Control)
            {
                Parent.DestroyCount = 0;
                Parent.DestroyOnColor = Control;
                SendMessageW(Control, WM_PRINTCLIENT, (WPARAM)Parent.Actual.Dc, PRF_CLIENT | PRF_ERASEBKGND);
                ok(!IsWindow(Control) && Parent.DestroyCount, "native scrollbar survived destructive color callback\n");
                Parent.DestroyOnColor = NULL;
                if (IsWindow(Control))
                    DestroyWindow(Control);
                Control = NULL;
                Parent.Target = NULL;
            }
            Control = CreateNativeScrollbar(Window, &Parent, Axis != 0, Palette);
            if (Control)
            {
                CheckScrollbarDestructiveTracking(Control, &Parent);
                if (IsWindow(Control))
                    DestroyWindow(Control);
                Control = NULL;
                Parent.Target = NULL;
            }
        }
        CloseThemeData(Parent.Reference);
        Parent.Reference = NULL;
    }
    ok(Signatures[0] && Signatures[1] && Signatures[0] != Signatures[1],
       "native scrollbar palettes produced equal signatures %08lx,%08lx\n", Signatures[0], Signatures[1]);

Cleanup:
    Parent.DestroyOnErase = Parent.DestroyOnColor = Parent.ThemeOnErase = NULL;
    Parent.ShutdownOnErase = FALSE;
    if (Control && IsWindow(Control))
        DestroyWindow(Control);
    if (Window)
        DestroyWindow(Window);
    if (Parent.Reference)
        CloseThemeData(Parent.Reference);
    if (Bootstrap)
        CloseThemeData(Bootstrap);
    if (ModeChanged)
        SetMode(PreviousMode);
    DestroyScrollSurface(&Parent.Actual);
    DestroyScrollSurface(&Parent.Expected);
    if (Registered)
        UnregisterClassW(Class.lpszClassName, Class.hInstance);
    if (Parent.Background)
        DeleteObject(Parent.Background);
    if (Parent.CustomBrush)
        DeleteObject(Parent.CustomBrush);
    if (HaveCursor)
        SetCursorPos(Cursor.x, Cursor.y);
    if (PreviousFocus && IsWindow(PreviousFocus))
        SetFocus(PreviousFocus);
    PumpScrollbarMessages();
}

START_TEST(NativeScrollbarTheme)
{
    RunNativeScrollbarTest(FALSE, FALSE);
}

START_TEST(NativeScrollbarThemeShutdown)
{
    RunNativeScrollbarTest(TRUE, FALSE);
}

START_TEST(NativeScrollbarRendering)
{
    RunNativeScrollbarTest(FALSE, TRUE);
}
