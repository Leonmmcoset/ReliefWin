/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Trackbar painting callback destruction and theme replacement
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <apitest.h>
#include <limits.h>
#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>

#define PAINT_WIDTH 192
#define PAINT_HEIGHT 96
#define PAINT_CLIENT_WIDTH 160
#define PAINT_CLIENT_HEIGHT 64
#define PAINT_SENTINEL 0x00d12b97

typedef BOOL (WINAPI *PINIT_PAINT_CONTROLS)(const INITCOMMONCONTROLSEX *);

enum PAINT_ACTION
{
    PaintDestroy,
    PaintRecycle,
    PaintDisableTheme,
    PaintRestoreTheme
};

enum PAINT_ROUTE
{
    PaintPrint,
    PaintNative,
    PaintSetPosition,
    PaintMouseDown,
    PaintMouseUp
};

typedef struct
{
    const char *Name;
    UINT Message;
    DWORD Stage;
    DWORD_PTR Item;
} PAINT_CASE;

typedef struct
{
    HBRUSH Background;
    HWND Target;
    HWND OriginalTarget;
    HWND OwnerWindow;
    HWND Replacement;
    const PAINT_CASE *Case;
    enum PAINT_ACTION Action;
    BOOL Armed;
    BOOL Destroyed;
    UINT TriggerCount;
    UINT MatchCount;
    UINT SkipMatches;
    UINT AfterDestroy;
    UINT RecycleCallbacks;
    UINT RecycleCount;
    BOOL RecycleFailed;
    HRESULT ThemeResult;
    UINT TooltipMessage;
    WNDPROC TooltipProc;
} PAINT_PARENT;

typedef struct
{
    HDC Dc;
    HBITMAP Bitmap;
    HBITMAP OldBitmap;
    DWORD *Pixels;
} PAINT_SURFACE;

static const PAINT_CASE PaintCases[] =
{
    {"color", WM_CTLCOLORSTATIC, 0, 0},
    {"prepaint", WM_NOTIFY, CDDS_PREPAINT, 0},
    {"preerase", WM_NOTIFY, CDDS_PREERASE, 0},
    {"posterase", WM_NOTIFY, CDDS_POSTERASE, 0},
    {"channel-pre", WM_NOTIFY, CDDS_ITEMPREPAINT, TBCD_CHANNEL},
    {"channel-post", WM_NOTIFY, CDDS_ITEMPOSTPAINT, TBCD_CHANNEL},
    {"tics-pre", WM_NOTIFY, CDDS_ITEMPREPAINT, TBCD_TICS},
    {"tics-post", WM_NOTIFY, CDDS_ITEMPOSTPAINT, TBCD_TICS},
    {"thumb-pre", WM_NOTIFY, CDDS_ITEMPREPAINT, TBCD_THUMB},
    {"thumb-post", WM_NOTIFY, CDDS_ITEMPOSTPAINT, TBCD_THUMB},
    {"postpaint", WM_NOTIFY, CDDS_POSTPAINT, 0}
};

static const char *PaintRouteNames[] = {"print", "paint", "setpos", "mousedown", "mouseup"};

static void TriggerPaintRecycle(PAINT_PARENT *Parent)
{
    HWND Original = Parent->Target;
    UINT i;

    Parent->Destroyed = DestroyWindow(Original);
    Parent->OriginalTarget = Original;
    if (!Parent->Destroyed)
    {
        Parent->RecycleFailed = TRUE;
        return;
    }

    Parent->Destroyed = FALSE;
    for (i = 0; i < 32770; i++)
    {
        HWND Trackbar = CreateWindowExW(0, TRACKBAR_CLASSW, L"", WS_CHILD,
                                        4, 4, PAINT_CLIENT_WIDTH, PAINT_CLIENT_HEIGHT,
                                        Parent->OwnerWindow, NULL, GetModuleHandleW(NULL), NULL);

        if (!Trackbar)
        {
            Parent->RecycleFailed = TRUE;
            break;
        }
        Parent->RecycleCount++;
        if (Trackbar == Original)
        {
            Parent->Replacement = Trackbar;
            Parent->Target = Trackbar;
            Parent->Destroyed = TRUE;
            SendMessageW(Trackbar, TBM_SETPOS, FALSE, 77);
            return;
        }
        DestroyWindow(Trackbar);
    }

    Parent->Destroyed = TRUE;
    Parent->RecycleFailed = TRUE;
}

static void TriggerPaintAction(PAINT_PARENT *Parent)
{
    Parent->Armed = FALSE;
    Parent->TriggerCount++;
    if (Parent->Action == PaintDestroy)
        Parent->Destroyed = DestroyWindow(Parent->Target);
    else if (Parent->Action == PaintRecycle)
        TriggerPaintRecycle(Parent);
    else if (Parent->Action == PaintDisableTheme)
        Parent->ThemeResult = SetWindowTheme(Parent->Target, L"", L"");
    else
        Parent->ThemeResult = SetWindowTheme(Parent->Target, NULL, NULL);
}

static void CheckPaintCallback(PAINT_PARENT *Parent, UINT Message, DWORD Stage, DWORD_PTR Item)
{
    if (Parent->Armed && Parent->Case && Parent->Case->Message == Message &&
        Parent->Case->Stage == Stage && Parent->Case->Item == Item)
    {
        Parent->MatchCount++;
        if (Parent->MatchCount > Parent->SkipMatches)
            TriggerPaintAction(Parent);
    }
}

static LRESULT CALLBACK PaintParentProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    PAINT_PARENT *Parent = (PAINT_PARENT *)GetWindowLongPtrW(Window, GWLP_USERDATA);

    if (Message == WM_NCCREATE)
    {
        Parent = ((CREATESTRUCTW *)LParam)->lpCreateParams;
        SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Parent);
    }
    if (Parent && Parent->Destroyed && Parent->Action == PaintDestroy)
    {
        if (Message == WM_NOTIFY && ((NMHDR *)LParam)->hwndFrom == Parent->Target)
            Parent->AfterDestroy++;
        else if ((Message == WM_HSCROLL || Message == WM_VSCROLL) && (HWND)LParam == Parent->Target)
            Parent->AfterDestroy++;
        else if (Message == WM_CTLCOLORSTATIC && (HWND)LParam == Parent->Target)
            Parent->AfterDestroy++;
        else if (Message == WM_NOTIFYFORMAT && (HWND)WParam == Parent->Target)
            Parent->AfterDestroy++;
    }
    if (Parent && Parent->Destroyed && Parent->Action == PaintRecycle)
    {
        if (Message == WM_NOTIFY && ((NMHDR *)LParam)->hwndFrom == Parent->OriginalTarget)
            Parent->RecycleCallbacks++;
        else if ((Message == WM_HSCROLL || Message == WM_VSCROLL) && (HWND)LParam == Parent->OriginalTarget)
            Parent->RecycleCallbacks++;
        else if (Message == WM_CTLCOLORSTATIC && (HWND)LParam == Parent->OriginalTarget)
            Parent->RecycleCallbacks++;
        else if (Message == WM_NOTIFYFORMAT && (HWND)WParam == Parent->OriginalTarget)
            Parent->RecycleCallbacks++;
    }
    if (Parent && Message == WM_CTLCOLORSTATIC)
    {
        if ((HWND)LParam == Parent->Target)
            CheckPaintCallback(Parent, Message, 0, 0);
        return (LRESULT)Parent->Background;
    }
    if (Parent && Message == WM_NOTIFY)
    {
        NMHDR *Header = (NMHDR *)LParam;

        if (Header->hwndFrom == Parent->Target && Header->code == NM_OUTOFMEMORY)
            CheckPaintCallback(Parent, Message, Header->code, 0);
        else if (Header->hwndFrom == Parent->Target && Header->code == NM_RELEASEDCAPTURE)
            CheckPaintCallback(Parent, Message, Header->code, 0);
        else if (Header->code == NM_CUSTOMDRAW)
        {
            NMCUSTOMDRAW *Draw = (NMCUSTOMDRAW *)LParam;

            if (Draw->hdr.hwndFrom == Parent->Target)
                CheckPaintCallback(Parent, Message, Draw->dwDrawStage,
                                   Draw->dwDrawStage & CDDS_ITEM ? Draw->dwItemSpec : 0);
            if (Draw->dwDrawStage == CDDS_PREPAINT)
                return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYPOSTERASE;
            if (Draw->dwDrawStage == CDDS_ITEMPREPAINT)
                return CDRF_NOTIFYPOSTPAINT;
        }
    }
    if (Parent && Parent->Armed && Parent->Case)
    {
        if ((Message == WM_KILLFOCUS || Message == WM_CAPTURECHANGED) && Parent->Case->Message == Message)
            CheckPaintCallback(Parent, Message, 0, 0);
        else if ((Message == WM_HSCROLL || Message == WM_VSCROLL) && (HWND)LParam == Parent->Target &&
                 Parent->Case->Message == Message && LOWORD(WParam) == Parent->Case->Stage)
            CheckPaintCallback(Parent, Message, LOWORD(WParam), 0);
        else if (Message == WM_NOTIFYFORMAT && Parent->Case->Message == Message &&
                 (DWORD)LParam == Parent->Case->Stage)
        {
            Parent->Target = (HWND)WParam;
            CheckPaintCallback(Parent, Message, (DWORD)LParam, 0);
        }
    }
    return DefWindowProcW(Window, Message, WParam, LParam);
}

static LRESULT CALLBACK PaintTooltipProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    PAINT_PARENT *Parent = (PAINT_PARENT *)GetPropW(Window, L"ReliefWinPaintParent");
    WNDPROC Previous = Parent->TooltipProc;

    if (Parent->Destroyed && Parent->Action == PaintDestroy)
        Parent->AfterDestroy++;

    if (Message == WM_NCDESTROY)
    {
        SetWindowLongPtrW(Window, GWLP_WNDPROC, (LONG_PTR)Previous);
        RemovePropW(Window, L"ReliefWinPaintParent");
    }
    if (Parent->Armed && Message == Parent->TooltipMessage)
    {
        TriggerPaintAction(Parent);
        return 0;
    }
    return CallWindowProcW(Previous, Window, Message, WParam, LParam);
}

static void ClearPaintSurface(PAINT_SURFACE *Surface)
{
    UINT i;

    GdiFlush();
    for (i = 0; i < PAINT_WIDTH * PAINT_HEIGHT; i++)
        Surface->Pixels[i] = PAINT_SENTINEL;
}

static DWORD PaintSurfaceSignature(PAINT_SURFACE *Surface, UINT *Changed)
{
    DWORD Signature = 2166136261u;
    UINT x, y;

    GdiFlush();
    *Changed = 0;
    for (y = 0; y < PAINT_CLIENT_HEIGHT; y++)
    {
        for (x = 0; x < PAINT_CLIENT_WIDTH; x++)
        {
            DWORD Pixel = Surface->Pixels[y * PAINT_WIDTH + x] & 0x00ffffff;

            *Changed += Pixel != PAINT_SENTINEL;
            Signature = (Signature ^ Pixel) * 16777619u;
        }
    }
    return Signature;
}

static HWND CreatePaintTrackbar(HWND Window, PAINT_PARENT *Parent, DWORD Style)
{
    HWND Trackbar;

    Parent->Armed = FALSE;
    Parent->Target = NULL;
    Trackbar = CreateWindowExW(0, TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | Style,
                               4, 4, PAINT_CLIENT_WIDTH, PAINT_CLIENT_HEIGHT,
                               Window, NULL, GetModuleHandleW(NULL), NULL);
    ok(Trackbar != NULL, "cannot create a paint-lifetime trackbar\n");
    if (Trackbar)
    {
        SendMessageW(Trackbar, TBM_SETPOS, TRUE, 50);
        UpdateWindow(Trackbar);
    }
    Parent->Target = Trackbar;
    return Trackbar;
}

static void ArmPaintCallbackSkip(PAINT_PARENT *Parent, const PAINT_CASE *Case,
                                enum PAINT_ACTION Action, UINT SkipMatches)
{
    Parent->Case = Case;
    Parent->Action = Action;
    Parent->Destroyed = FALSE;
    Parent->TriggerCount = 0;
    Parent->MatchCount = 0;
    Parent->SkipMatches = SkipMatches;
    Parent->AfterDestroy = 0;
    Parent->OriginalTarget = NULL;
    Parent->RecycleCallbacks = 0;
    Parent->Replacement = NULL;
    Parent->RecycleCount = 0;
    Parent->RecycleFailed = FALSE;
    Parent->ThemeResult = E_FAIL;
    Parent->Armed = TRUE;
}

static void ArmPaintCallback(PAINT_PARENT *Parent, const PAINT_CASE *Case, enum PAINT_ACTION Action)
{
    ArmPaintCallbackSkip(Parent, Case, Action, 0);
}

static void InvokeTrackbarPaint(HWND Window, PAINT_SURFACE *Surface, enum PAINT_ROUTE Route)
{
    if (Route == PaintNative)
    {
        InvalidateRect(Window, NULL, TRUE);
        UpdateWindow(Window);
    }
    else if (Route == PaintSetPosition)
        SendMessageW(Window, TBM_SETPOS, TRUE, 75);
    else if (Route == PaintMouseDown)
    {
        RECT Thumb;

        SendMessageW(Window, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
        SendMessageW(Window, WM_LBUTTONDOWN, MK_LBUTTON,
                     MAKELPARAM((Thumb.left + Thumb.right) / 2, (Thumb.top + Thumb.bottom) / 2));
    }
    else if (Route == PaintMouseUp)
        SendMessageW(Window, WM_LBUTTONUP, 0, 0);
    else
        SendMessageW(Window, WM_PRINTCLIENT, (WPARAM)Surface->Dc, PRF_CLIENT | PRF_ERASEBKGND);
}

static void CheckPaintDestruction(HWND Window, PAINT_PARENT *Parent, PAINT_SURFACE *Surface,
                                  const PAINT_CASE *Case, enum PAINT_ROUTE Route)
{
    HWND Trackbar = CreatePaintTrackbar(Window, Parent, 0);
    UINT Changed;

    if (!Trackbar)
        return;
    ClearPaintSurface(Surface);
    ArmPaintCallback(Parent, Case, PaintDestroy);
    InvokeTrackbarPaint(Trackbar, Surface, Route);
    Parent->Armed = FALSE;
    ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
       "%s %s did not synchronously destroy the trackbar: count %u result %d alive %d\n",
       PaintRouteNames[Route], Case->Name, Parent->TriggerCount,
       Parent->Destroyed, IsWindow(Trackbar));
    ok(!Parent->AfterDestroy, "%s %s sent %u draw callbacks after destruction\n",
       PaintRouteNames[Route], Case->Name, Parent->AfterDestroy);
    if (Route == PaintPrint)
    {
        PaintSurfaceSignature(Surface, &Changed);
        ok(!Changed, "print %s committed %u pixels after destruction\n", Case->Name, Changed);
    }
    if (IsWindow(Trackbar))
        DestroyWindow(Trackbar);
    Parent->Target = NULL;
}

static void CheckTooltipDestruction(HWND Window, PAINT_PARENT *Parent, PAINT_SURFACE *Surface,
                                    UINT Message, enum PAINT_ROUTE Route)
{
    HWND Trackbar = CreatePaintTrackbar(Window, Parent, TBS_TOOLTIPS);
    HWND Tooltip;
    RECT Thumb;
    UINT Changed;
    BOOL PropertySet;

    if (!Trackbar)
        return;
    Tooltip = (HWND)SendMessageW(Trackbar, TBM_GETTOOLTIPS, 0, 0);
    ok(Tooltip != NULL, "cannot get the trackbar tooltip\n");
    if (!Tooltip)
        goto Cleanup;
    PropertySet = SetPropW(Tooltip, L"ReliefWinPaintParent", Parent);
    ok(PropertySet, "cannot attach tooltip lifetime state\n");
    if (!PropertySet)
        goto Cleanup;
    Parent->TooltipProc = (WNDPROC)SetWindowLongPtrW(Tooltip, GWLP_WNDPROC, (LONG_PTR)PaintTooltipProc);
    ok(Parent->TooltipProc != NULL, "cannot subclass the trackbar tooltip\n");
    if (!Parent->TooltipProc)
    {
        RemovePropW(Tooltip, L"ReliefWinPaintParent");
        goto Cleanup;
    }
    if (Route != PaintMouseDown)
    {
        SendMessageW(Trackbar, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
        SendMessageW(Trackbar, WM_LBUTTONDOWN, MK_LBUTTON,
                     MAKELPARAM((Thumb.left + Thumb.right) / 2, (Thumb.top + Thumb.bottom) / 2));
        ok(GetCapture() == Trackbar, "tooltip test did not enter thumb dragging\n");
    }
    ClearPaintSurface(Surface);
    ArmPaintCallback(Parent, NULL, PaintDestroy);
    Parent->TooltipMessage = Message;
    InvokeTrackbarPaint(Trackbar, Surface, Route);
    Parent->Armed = FALSE;
    ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
       "%s tooltip %04x did not destroy during painting: count %u result %d alive %d\n",
       PaintRouteNames[Route], Message, Parent->TriggerCount,
       Parent->Destroyed, IsWindow(Trackbar));
    ok(!Parent->AfterDestroy, "%s tooltip %04x sent %u callbacks after destruction\n",
       PaintRouteNames[Route], Message, Parent->AfterDestroy);
    if (Route == PaintPrint)
    {
        PaintSurfaceSignature(Surface, &Changed);
        ok(!Changed, "print tooltip %04x committed %u pixels after destruction\n", Message, Changed);
    }

Cleanup:
    Parent->Armed = FALSE;
    if (IsWindow(Trackbar))
        DestroyWindow(Trackbar);
    Parent->Target = NULL;
}

static void CheckPaintThemeReplacement(HWND Window, PAINT_PARENT *Parent, PAINT_SURFACE *Surface,
                                       const PAINT_CASE *Case, enum PAINT_ACTION Action, enum PAINT_ROUTE Route)
{
    HWND Trackbar = CreatePaintTrackbar(Window, Parent, 0);
    DWORD Before, During, After;
    UINT ChangedBefore, ChangedDuring, ChangedAfter;

    if (!Trackbar)
        return;
    if (Action == PaintRestoreTheme)
        ok(SetWindowTheme(Trackbar, L"", L"") == S_OK && !GetWindowTheme(Trackbar),
           "cannot disable the initial trackbar theme\n");
    ClearPaintSurface(Surface);
    SendMessageW(Trackbar, WM_PRINTCLIENT, (WPARAM)Surface->Dc, PRF_CLIENT | PRF_ERASEBKGND);
    Before = PaintSurfaceSignature(Surface, &ChangedBefore);
    ClearPaintSurface(Surface);
    ArmPaintCallback(Parent, Case, Action);
    InvokeTrackbarPaint(Trackbar, Surface, Route);
    Parent->Armed = FALSE;
    ok(Parent->TriggerCount == 1 && Parent->ThemeResult == S_OK && IsWindow(Trackbar),
       "%s %s action %u failed theme replacement: count %u result %08lx\n",
       PaintRouteNames[Route], Case->Name, Action, Parent->TriggerCount, Parent->ThemeResult);
    if (!IsWindow(Trackbar))
        goto Cleanup;
    ok((GetWindowTheme(Trackbar) != NULL) == (Action == PaintRestoreTheme),
       "%s %s action %u left the wrong theme state\n", PaintRouteNames[Route], Case->Name, Action);
    if (Route != PaintPrint)
    {
        HDC Dc = GetDC(Trackbar);

        ok(Dc != NULL, "cannot capture the repainted trackbar\n");
        if (Dc)
        {
            ok(BitBlt(Surface->Dc, 0, 0, PAINT_CLIENT_WIDTH, PAINT_CLIENT_HEIGHT, Dc, 0, 0, SRCCOPY),
               "cannot capture trackbar theme replacement\n");
            ReleaseDC(Trackbar, Dc);
        }
    }
    During = PaintSurfaceSignature(Surface, &ChangedDuring);
    ClearPaintSurface(Surface);
    SendMessageW(Trackbar, WM_PRINTCLIENT, (WPARAM)Surface->Dc, PRF_CLIENT | PRF_ERASEBKGND);
    After = PaintSurfaceSignature(Surface, &ChangedAfter);
    ok(ChangedBefore && ChangedDuring && ChangedAfter && During == After && Before != After,
       "%s %s action %u rendered stale theme pixels: %08lx/%u expected %08lx/%u previous %08lx/%u\n",
       PaintRouteNames[Route], Case->Name, Action,
       During, ChangedDuring, After, ChangedAfter, Before, ChangedBefore);

Cleanup:
    Parent->Armed = FALSE;
    if (IsWindow(Trackbar))
        DestroyWindow(Trackbar);
    Parent->Target = NULL;
}

static void CheckMouseDownDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const PAINT_CASE Cases[] =
    {
        {"focus", WM_KILLFOCUS, 0, 0},
        {"capture", WM_CAPTURECHANGED, 0, 0},
        {"page", WM_HSCROLL, TB_PAGEDOWN, 0}
    };
    UINT Case;

    for (Case = 0; Case < ARRAYSIZE(Cases); Case++)
    {
        HWND Trackbar = CreatePaintTrackbar(Window, Parent, 0);
        RECT Thumb, Channel;
        LONG x, y;

        if (!Trackbar)
            continue;
        SendMessageW(Trackbar, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
        SendMessageW(Trackbar, TBM_GETCHANNELRECT, 0, (LPARAM)&Channel);
        x = (Thumb.left + Thumb.right) / 2;
        y = (Thumb.top + Thumb.bottom) / 2;
        if (Case == 0)
        {
            SetFocus(Window);
            ok(GetFocus() == Window, "cannot set up parent focus destruction\n");
        }
        else
        {
            SetFocus(Trackbar);
            if (Case == 1)
            {
                SetCapture(Window);
                ok(GetCapture() == Window, "cannot set up parent capture destruction\n");
            }
            else
                x = (Thumb.right + Channel.right) / 2;
        }
        ArmPaintCallback(Parent, &Cases[Case], PaintDestroy);
        SendMessageW(Trackbar, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
        Parent->Armed = FALSE;
        ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
           "mousedown %s did not destroy in the synchronous callback: count %u result %d alive %d\n",
           Cases[Case].Name, Parent->TriggerCount, Parent->Destroyed, IsWindow(Trackbar));
        ok(!Parent->AfterDestroy, "mousedown %s sent %u draw callbacks after destruction\n",
           Cases[Case].Name, Parent->AfterDestroy);
        if (IsWindow(Trackbar))
            DestroyWindow(Trackbar);
        Parent->Target = NULL;
    }
}

static void CheckMouseMoveDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const struct
    {
        PAINT_CASE Case;
        DWORD Style;
    } Cases[] =
    {
        {{"thumbtrack-horizontal", WM_HSCROLL, TB_THUMBTRACK, 0}, 0},
        {{"thumbtrack-vertical", WM_VSCROLL, TB_THUMBTRACK, 0}, TBS_VERT}
    };
    UINT i;

    for (i = 0; i < ARRAYSIZE(Cases); i++)
    {
        HWND Trackbar = CreatePaintTrackbar(Window, Parent, Cases[i].Style);
        RECT Thumb, Channel;
        LONG x, y;

        if (!Trackbar)
            continue;
        SendMessageW(Trackbar, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
        SendMessageW(Trackbar, TBM_GETCHANNELRECT, 0, (LPARAM)&Channel);
        x = (Thumb.left + Thumb.right) / 2;
        y = (Thumb.top + Thumb.bottom) / 2;
        SendMessageW(Trackbar, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
        ok(GetCapture() == Trackbar, "%s setup did not enter thumb dragging\n", Cases[i].Case.Name);
        if (Cases[i].Style & TBS_VERT)
            y = Channel.bottom - 2;
        else
            x = Channel.right - 2;
        ArmPaintCallback(Parent, &Cases[i].Case, PaintDestroy);
        SendMessageW(Trackbar, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x, y));
        Parent->Armed = FALSE;
        ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
           "%s did not destroy synchronously: count %u result %d alive %d\n",
           Cases[i].Case.Name, Parent->TriggerCount, Parent->Destroyed, IsWindow(Trackbar));
        ok(!Parent->AfterDestroy, "%s sent %u notifications after destruction\n",
           Cases[i].Case.Name, Parent->AfterDestroy);
        if (IsWindow(Trackbar))
            DestroyWindow(Trackbar);
        Parent->Target = NULL;
    }
}

static void CheckKeyDownDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const struct
    {
        PAINT_CASE Case;
        UINT Key;
        DWORD Style;
    } Cases[] =
    {
        {{"line-horizontal", WM_HSCROLL, TB_LINEDOWN, 0}, VK_RIGHT, 0},
        {{"page-horizontal", WM_HSCROLL, TB_PAGEDOWN, 0}, VK_NEXT, 0},
        {{"home-horizontal", WM_HSCROLL, TB_TOP, 0}, VK_HOME, 0},
        {{"end-horizontal", WM_HSCROLL, TB_BOTTOM, 0}, VK_END, 0},
        {{"line-vertical", WM_VSCROLL, TB_LINEUP, 0}, VK_UP, TBS_VERT},
        {{"page-vertical", WM_VSCROLL, TB_PAGEDOWN, 0}, VK_NEXT, TBS_VERT},
        {{"home-vertical", WM_VSCROLL, TB_TOP, 0}, VK_HOME, TBS_VERT},
        {{"end-vertical", WM_VSCROLL, TB_BOTTOM, 0}, VK_END, TBS_VERT}
    };
    UINT i;

    for (i = 0; i < ARRAYSIZE(Cases); i++)
    {
        HWND Trackbar = CreatePaintTrackbar(Window, Parent, Cases[i].Style);

        if (!Trackbar)
            continue;
        ArmPaintCallback(Parent, &Cases[i].Case, PaintDestroy);
        SendMessageW(Trackbar, WM_KEYDOWN, Cases[i].Key, 0);
        Parent->Armed = FALSE;
        ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
           "keydown %s did not destroy synchronously: count %u result %d alive %d\n",
           Cases[i].Case.Name, Parent->TriggerCount, Parent->Destroyed, IsWindow(Trackbar));
        ok(!Parent->AfterDestroy, "keydown %s sent %u notifications after destruction\n",
           Cases[i].Case.Name, Parent->AfterDestroy);
        if (IsWindow(Trackbar))
            DestroyWindow(Trackbar);
        Parent->Target = NULL;
    }
}

static void CheckMouseUpDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const PAINT_CASE Cases[] =
    {
        {"thumbposition", WM_HSCROLL, TB_THUMBPOSITION, 0},
        {"endtrack", WM_HSCROLL, TB_ENDTRACK, 0},
        {"capture-endtrack", WM_HSCROLL, TB_ENDTRACK, 0},
        {"released-capture", WM_NOTIFY, NM_RELEASEDCAPTURE, 0},
        {"autopage-endtrack", WM_HSCROLL, TB_ENDTRACK, 0},
        {"autopage-capture-endtrack", WM_HSCROLL, TB_ENDTRACK, 0}
    };
    UINT i;

    for (i = 0; i < ARRAYSIZE(Cases); i++)
    {
        HWND Trackbar = CreatePaintTrackbar(Window, Parent, 0);
        RECT Thumb, Channel;
        UINT SkipMatches = i == 2 || i == 5 ? 1 : 0;

        if (!Trackbar)
            continue;
        if (i < 4)
        {
            SendMessageW(Trackbar, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
            SendMessageW(Trackbar, WM_LBUTTONDOWN, MK_LBUTTON,
                         MAKELPARAM((Thumb.left + Thumb.right) / 2, (Thumb.top + Thumb.bottom) / 2));
            ok(GetCapture() == Trackbar, "mouseup %s setup did not enter thumb dragging\n", Cases[i].Name);
        }
        else
        {
            SendMessageW(Trackbar, TBM_GETTHUMBRECT, 0, (LPARAM)&Thumb);
            SendMessageW(Trackbar, TBM_GETCHANNELRECT, 0, (LPARAM)&Channel);
            SendMessageW(Trackbar, WM_LBUTTONDOWN, MK_LBUTTON,
                         MAKELPARAM((Thumb.right + Channel.right) / 2,
                                    (Thumb.top + Thumb.bottom) / 2));
            ok(GetCapture() == Trackbar, "mouseup %s setup did not enter auto-page\n", Cases[i].Name);
        }
        ArmPaintCallbackSkip(Parent, &Cases[i], PaintDestroy, SkipMatches);
        SendMessageW(Trackbar, WM_LBUTTONUP, 0, 0);
        Parent->Armed = FALSE;
        ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
           "mouseup %s did not destroy synchronously: count %u result %d alive %d matches %u\n",
           Cases[i].Name, Parent->TriggerCount, Parent->Destroyed, IsWindow(Trackbar), Parent->MatchCount);
        ok(Parent->MatchCount == SkipMatches + 1, "mouseup %s matched %u callbacks, expected %u\n",
           Cases[i].Name, Parent->MatchCount, SkipMatches + 1);
        ok(!Parent->AfterDestroy, "mouseup %s sent %u notifications after destruction\n",
           Cases[i].Name, Parent->AfterDestroy);
        if (IsWindow(Trackbar))
            DestroyWindow(Trackbar);
        Parent->Target = NULL;
    }
}

static void CheckNotifyFormatDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const PAINT_CASE Case = {"notifyformat", WM_NOTIFYFORMAT, NF_QUERY, 0};
    HWND Trackbar;

    Parent->Target = NULL;
    ArmPaintCallback(Parent, &Case, PaintDestroy);
    Trackbar = CreateWindowExW(0, TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                               4, 4, PAINT_CLIENT_WIDTH, PAINT_CLIENT_HEIGHT,
                               Window, NULL, GetModuleHandleW(NULL), NULL);
    Parent->Armed = FALSE;
    ok(Parent->TriggerCount == 1 && Parent->Destroyed && Parent->Target && !IsWindow(Parent->Target),
       "WM_NOTIFYFORMAT did not destroy the trackbar during creation: count %u result %d target %p\n",
       Parent->TriggerCount, Parent->Destroyed, Parent->Target);
    ok(!Parent->AfterDestroy, "WM_NOTIFYFORMAT sent %u notifications after destruction\n", Parent->AfterDestroy);
    if (Trackbar && IsWindow(Trackbar))
        DestroyWindow(Trackbar);
    Parent->Target = NULL;
}

static void CheckSetTicFrequencyDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const PAINT_CASE Case = {"ticfreq-prepaint", WM_NOTIFY, CDDS_PREPAINT, 0};
    HWND Trackbar = CreatePaintTrackbar(Window, Parent, 0);

    if (!Trackbar)
        return;
    ArmPaintCallback(Parent, &Case, PaintDestroy);
    SendMessageW(Trackbar, TBM_SETTICFREQ, 2, 0);
    Parent->Armed = FALSE;
    ok(Parent->TriggerCount == 1 && Parent->Destroyed && !IsWindow(Trackbar),
       "tic frequency repaint did not destroy synchronously: count %u result %d alive %d\n",
       Parent->TriggerCount, Parent->Destroyed, IsWindow(Trackbar));
    ok(!Parent->AfterDestroy, "tic frequency sent %u notifications after destruction\n", Parent->AfterDestroy);
    if (IsWindow(Trackbar))
        DestroyWindow(Trackbar);
    Parent->Target = NULL;
}

static void CheckTicOverflowDestruction(HWND Window, PAINT_PARENT *Parent)
{
    static const PAINT_CASE Case = {"tic-overflow-outofmemory", WM_NOTIFY, NM_OUTOFMEMORY, 0};
    HWND Trackbar = CreatePaintTrackbar(Window, Parent, TBS_AUTOTICKS);

    if (!Trackbar)
        return;
    ArmPaintCallback(Parent, &Case, PaintDestroy);
    SendMessageW(Trackbar, TBM_SETRANGEMAX, TRUE, LONG_MAX);
    Parent->Armed = FALSE;
    ok(Parent->MatchCount == 1 && Parent->TriggerCount == 1,
       "tic overflow sent %u matching notifications and triggered %u times\n",
       Parent->MatchCount, Parent->TriggerCount);
    ok(Parent->Destroyed && !IsWindow(Trackbar),
       "tic overflow did not synchronously destroy the trackbar: result %d alive %d\n",
       Parent->Destroyed, IsWindow(Trackbar));
    ok(!Parent->AfterDestroy,
       "tic overflow sent %u notifications or drawing callbacks after destruction\n",
       Parent->AfterDestroy);
    if (IsWindow(Trackbar))
        DestroyWindow(Trackbar);
    Parent->Target = NULL;
}

static void CheckPaintRecycledWindow(HWND Window, PAINT_PARENT *Parent, PAINT_SURFACE *Surface)
{
    static const PAINT_CASE Case = {"recycled-window-prepaint", WM_NOTIFY, CDDS_PREPAINT, 0};
    HWND Trackbar = CreatePaintTrackbar(Window, Parent, 0);
    UINT Changed;

    if (!Trackbar)
        return;
    Parent->OwnerWindow = Window;
    ClearPaintSurface(Surface);
    ArmPaintCallback(Parent, &Case, PaintRecycle);
    InvokeTrackbarPaint(Trackbar, Surface, PaintPrint);
    Parent->Armed = FALSE;
    PaintSurfaceSignature(Surface, &Changed);
    ok(Parent->TriggerCount == 1 && Parent->Destroyed && !Parent->RecycleFailed &&
       Parent->Replacement == Trackbar && IsWindow(Parent->Replacement),
       "paint callback did not recycle the original HWND: count %u result %d failed %d replacement %p original %p cycles %u\n",
       Parent->TriggerCount, Parent->Destroyed, Parent->RecycleFailed,
       Parent->Replacement, Trackbar, Parent->RecycleCount);
    ok(Parent->RecycleCount >= 32767, "recycled HWND after only %u allocations\n", Parent->RecycleCount);
    ok(!Parent->RecycleCallbacks, "old paint sent %u callbacks after the HWND was recycled\n",
       Parent->RecycleCallbacks);
    ok(!Changed, "old paint committed %u pixels after the HWND was recycled\n", Changed);
    if (Parent->Replacement && IsWindow(Parent->Replacement))
    {
        ok(SendMessageW(Parent->Replacement, TBM_GETPOS, 0, 0) == 77,
           "recycled trackbar position changed after the old paint returned\n");
        DestroyWindow(Parent->Replacement);
    }
    Parent->Replacement = NULL;
    Parent->Target = NULL;
}

START_TEST(TrackbarPaintLifetime)
{
    static const UINT TooltipMessages[] = {TTM_UPDATETIPTEXTW, TTM_GETBUBBLESIZE,
                                           TTM_TRACKPOSITION, TTM_TRACKACTIVATE};
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_BAR_CLASSES};
    ACTCTXW Context = {sizeof(Context)};
    BITMAPINFO Info = {{sizeof(BITMAPINFOHEADER), PAINT_WIDTH, -PAINT_HEIGHT, 1, 32, BI_RGB}};
    WNDCLASSW Class = {0};
    PAINT_PARENT Parent = {0};
    PAINT_SURFACE Surface = {0};
    HANDLE Activation = INVALID_HANDLE_VALUE;
    HMODULE Module = NULL;
    PINIT_PAINT_CONTROLS Initialize;
    ULONG_PTR Cookie = 0;
    HWND Window = NULL;
    WCHAR Path[MAX_PATH];
    BOOL Activated = FALSE, Registered = FALSE, Initialized;
    DWORD Before, After;
    UINT Pass, Route, Case, Message, Action;

    trace("TrackbarPaintLifetime exercises callback destruction on the fixed implementation\n");
    trace("No Windows 11 baseline or intentional unfixed use-after-free run is available\n");
    if (!GetModuleFileNameW(NULL, Path, ARRAYSIZE(Path)))
    {
        ok(FALSE, "cannot locate the paint-lifetime common-controls manifest\n");
        return;
    }
    Context.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
    Context.lpSource = Path;
    Context.lpResourceName = MAKEINTRESOURCEW(2);
    Activation = CreateActCtxW(&Context);
    ok(Activation != INVALID_HANDLE_VALUE, "cannot create the paint-lifetime activation context\n");
    if (Activation == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Activated = ActivateActCtx(Activation, &Cookie);
    ok(Activated, "cannot activate themed trackbar controls\n");
    if (!Activated)
        goto Cleanup;
    Module = LoadLibraryW(L"comctl32.dll");
    Initialize = Module ? (PINIT_PAINT_CONTROLS)GetProcAddress(Module, "InitCommonControlsEx") : NULL;
    Initialized = Initialize && Initialize(&Controls);
    ok(Initialized, "cannot initialize paint-lifetime trackbars\n");
    if (!Initialized)
        goto Cleanup;
    Parent.Background = CreateSolidBrush(RGB(37, 126, 91));
    Class.lpfnWndProc = PaintParentProc;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.lpszClassName = L"ReliefWinTrackbarPaintLifetime";
    Class.hbrBackground = Parent.Background;
    Registered = RegisterClassW(&Class) != 0;
    ok(Registered && Parent.Background, "cannot register the paint-lifetime parent\n");
    if (!Registered || !Parent.Background)
        goto Cleanup;
    Window = CreateWindowExW(0, Class.lpszClassName, L"", WS_POPUP | WS_VISIBLE,
                             8, 8, PAINT_WIDTH, PAINT_HEIGHT, NULL, NULL, Class.hInstance, &Parent);
    Parent.OwnerWindow = Window;
    Surface.Dc = CreateCompatibleDC(NULL);
    Surface.Bitmap = Surface.Dc ? CreateDIBSection(Surface.Dc, &Info, DIB_RGB_COLORS,
                                                  (void **)&Surface.Pixels, NULL, 0) : NULL;
    ok(Window && Surface.Dc && Surface.Bitmap && Surface.Pixels, "cannot create paint-lifetime fixtures\n");
    if (!Window || !Surface.Dc || !Surface.Bitmap || !Surface.Pixels)
        goto Cleanup;
    Surface.OldBitmap = SelectObject(Surface.Dc, Surface.Bitmap);
    ok(Surface.OldBitmap && Surface.OldBitmap != HGDI_ERROR, "cannot select paint-lifetime capture bitmap\n");
    if (!Surface.OldBitmap || Surface.OldBitmap == HGDI_ERROR)
    {
        Surface.OldBitmap = NULL;
        goto Cleanup;
    }
    UpdateWindow(Window);
    Before = 0;
    CheckNotifyFormatDestruction(Window, &Parent);
    CheckPaintRecycledWindow(Window, &Parent, &Surface);
    for (Pass = 0; Pass < 3; Pass++)
    {
        for (Route = PaintPrint; Route <= PaintMouseUp; Route++)
        {
            if (Route != PaintMouseDown && Route != PaintMouseUp)
                for (Case = 0; Case < ARRAYSIZE(PaintCases); Case++)
                    CheckPaintDestruction(Window, &Parent, &Surface, &PaintCases[Case], Route);
            for (Message = 0; Message < ARRAYSIZE(TooltipMessages); Message++)
                if (Route == PaintMouseUp ? TooltipMessages[Message] == TTM_TRACKACTIVATE :
                    (Route == PaintMouseDown || TooltipMessages[Message] != TTM_TRACKACTIVATE))
                    CheckTooltipDestruction(Window, &Parent, &Surface, TooltipMessages[Message], Route);
        }
        CheckMouseDownDestruction(Window, &Parent);
        CheckMouseMoveDestruction(Window, &Parent);
        CheckKeyDownDestruction(Window, &Parent);
        CheckMouseUpDestruction(Window, &Parent);
        CheckSetTicFrequencyDestruction(Window, &Parent);
        CheckTicOverflowDestruction(Window, &Parent);
        GdiFlush();
        if (!Pass)
            Before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    }
    After = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    ok(Before && After <= Before + 2, "callback destruction leaked GDI objects: %lu to %lu\n", Before, After);
    if (IsThemeActive())
    {
        for (Route = PaintPrint; Route <= PaintNative; Route++)
            for (Case = 0; Case < 2; Case++)
                for (Action = PaintDisableTheme; Action <= PaintRestoreTheme; Action++)
                    CheckPaintThemeReplacement(Window, &Parent, &Surface, &PaintCases[Case], Action, Route);
    }
    else
        skip("No visual style is active for callback theme replacement\n");

Cleanup:
    Parent.Armed = FALSE;
    if (Window)
        DestroyWindow(Window);
    if (Surface.OldBitmap)
        SelectObject(Surface.Dc, Surface.OldBitmap);
    if (Surface.Bitmap)
        DeleteObject(Surface.Bitmap);
    if (Surface.Dc)
        DeleteDC(Surface.Dc);
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
