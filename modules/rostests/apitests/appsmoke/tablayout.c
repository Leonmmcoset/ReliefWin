/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verify application pages stay inside the tab display area
 * COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>
 */

#include "appsmoke.h"
#include <commctrl.h>
#include <winnls.h>
#include "../../../../base/applications/taskmgr/resource.h"

typedef struct _TASKMGR_WINDOW
{
    DWORD ProcessId;
    HWND Window;
} TASKMGR_WINDOW;

static BOOL CALLBACK FindTaskmgr(HWND Window, LPARAM Parameter)
{
    TASKMGR_WINDOW *Search = (TASKMGR_WINDOW *)Parameter;
    DWORD ProcessId;

    GetWindowThreadProcessId(Window, &ProcessId);
    if (ProcessId == Search->ProcessId && GetDlgItem(Window, IDC_TAB))
    {
        Search->Window = Window;
        return FALSE;
    }
    return TRUE;
}

static BOOL TaskmgrMessage(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam, DWORD_PTR *Result)
{
    BOOL Success = SendMessageTimeoutW(Window, Message, WParam, LParam,
                                      SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, Result) != 0;

    ok(Success, "Message %#x to %p failed: %lu\n", Message, Window, GetLastError());
    return Success;
}

static HWND TaskmgrPage(HWND Window, int ControlId)
{
    HWND Page = NULL;

    while ((Page = FindWindowExW(Window, Page, L"#32770", NULL)) != NULL)
    {
        if (GetDlgItem(Page, ControlId))
            return Page;
    }
    return NULL;
}

static BOOL TaskmgrDisplay(HWND Window, HWND Tab, HANDLE Process, RECT *Remote, RECT *Display)
{
    DWORD_PTR Result;
    BOOL Success;

    Success = GetClientRect(Tab, Display) &&
              WriteProcessMemory(Process, Remote, Display, sizeof(*Display), NULL);
    ok(Success, "Cannot prepare the tab display rectangle: %lu\n", GetLastError());
    if (!Success || !TaskmgrMessage(Tab, TCM_ADJUSTRECT, FALSE, (LPARAM)Remote, &Result))
        return FALSE;
    Success = ReadProcessMemory(Process, Remote, Display, sizeof(*Display), NULL);
    ok(Success, "Cannot read the tab display rectangle: %lu\n", GetLastError());
    if (!Success)
        return FALSE;
    MapWindowPoints(Tab, Window, (POINT *)Display, 2);
    return TRUE;
}

static void CheckTaskmgrPages(HWND Window, HWND Tab, HANDLE Process, RECT *Remote, const char *Stage)
{
    static const int Controls[] = { IDC_APPLIST, IDC_PROCESSLIST, IDC_CPU_USAGE_FRAME };
    HWND Pages[ARRAYSIZE(Controls)];
    RECT Display, PageRect;
    DWORD_PTR Result;
    UINT Index, Other;

    for (Index = 0; Index < ARRAYSIZE(Controls); Index++)
    {
        Pages[Index] = TaskmgrPage(Window, Controls[Index]);
        ok(Pages[Index] != NULL, "%s: page %u was not created\n", Stage, Index);
        if (!Pages[Index])
            return;
    }

    for (Index = 0; Index < ARRAYSIZE(Controls); Index++)
    {
        if (!TaskmgrMessage(Tab, TCM_SETCURFOCUS, Index, 0, &Result) ||
            !TaskmgrDisplay(Window, Tab, Process, Remote, &Display))
            return;
        trace("%s tab %u display %ld,%ld,%ld,%ld\n", Stage, Index,
              Display.left, Display.top, Display.right, Display.bottom);
        ok(!IsRectEmpty(&Display), "%s: the tab display area is empty\n", Stage);

        for (Other = 0; Other < ARRAYSIZE(Controls); Other++)
        {
            BOOL Success = GetWindowRect(Pages[Other], &PageRect);

            ok(Success, "%s: cannot measure page %u: %lu\n", Stage, Other, GetLastError());
            if (!Success)
                continue;
            MapWindowPoints(NULL, Window, (POINT *)&PageRect, 2);
            ok(PageRect.left >= Display.left && PageRect.top >= Display.top &&
               PageRect.right <= Display.right && PageRect.bottom <= Display.bottom,
               "%s tab %u: page %u (%ld,%ld,%ld,%ld) covers the tab frame outside %ld,%ld,%ld,%ld\n",
               Stage, Index, Other, PageRect.left, PageRect.top, PageRect.right, PageRect.bottom,
               Display.left, Display.top, Display.right, Display.bottom);
            ok(IsWindowVisible(Pages[Other]) == (Other == Index),
               "%s tab %u: page %u visibility is %d\n", Stage, Index, Other,
               IsWindowVisible(Pages[Other]));
        }
    }
}

START_TEST(taskmgr_layout)
{
    STARTUPINFOW Startup = { sizeof(Startup) };
    PROCESS_INFORMATION Process;
    TASKMGR_WINDOW Search = { 0 };
    WINDOWPLACEMENT Placement = { sizeof(Placement) };
    WCHAR Path[MAX_PATH], Command[MAX_PATH + 4];
    RECT Original, Client, *Remote = NULL;
    HWND Tab;
    DWORD_PTR Result, OriginalSelection = 0, OriginalItemSize;
    LONG_PTR OriginalStyle;
    DWORD Started, Wait;
    HANDLE Mutex;
    BOOL Created;
    char **Arguments;
    int Count = winetest_get_mainargs(&Arguments);

    if (Count < 3)
    {
        skip("Pass the path to the ReliefWin taskmgr.exe as the third argument\n");
        return;
    }
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, Arguments[2], -1, Path, ARRAYSIZE(Path)))
    {
        ok(FALSE, "Invalid task manager path: %lu\n", GetLastError());
        return;
    }
    Mutex = OpenMutexW(SYNCHRONIZE, FALSE, L"taskmgrros");
    if (Mutex)
    {
        CloseHandle(Mutex);
        skip("Close the existing classic task manager before running this test\n");
        return;
    }
    wsprintfW(Command, L"\"%s\"", Path);
    Created = CreateProcessW(Path, Command, NULL, NULL, FALSE, 0, NULL, NULL, &Startup, &Process);
    ok(Created, "Cannot start %s: %lu\n", wine_dbgstr_w(Path), GetLastError());
    if (!Created)
        return;
    CloseHandle(Process.hThread);
    Wait = WaitForInputIdle(Process.hProcess, 10000);
    ok(Wait == 0, "The task manager did not become idle: %#lx\n", Wait);
    if (Wait != 0)
        goto Cleanup;
    Search.ProcessId = Process.dwProcessId;
    Started = GetTickCount();
    do
    {
        EnumWindows(FindTaskmgr, (LPARAM)&Search);
        if (Search.Window && TaskmgrPage(Search.Window, IDC_CPU_USAGE_FRAME))
            break;
        if (WaitForSingleObject(Process.hProcess, 50) != WAIT_TIMEOUT)
            break;
    } while (GetTickCount() - Started < 10000);
    ok(Search.Window != NULL, "The task manager window did not appear\n");
    if (!Search.Window)
        goto Cleanup;
    Tab = GetDlgItem(Search.Window, IDC_TAB);
    Remote = VirtualAllocEx(Process.hProcess, NULL, sizeof(*Remote), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    ok(Remote != NULL, "Cannot allocate the remote tab rectangle: %lu\n", GetLastError());
    if (!Remote || !TaskmgrMessage(Tab, TCM_GETCURSEL, 0, 0, &OriginalSelection))
        goto Cleanup;
    Created = GetWindowPlacement(Search.Window, &Placement);
    ok(Created, "Cannot save the window placement: %lu\n", GetLastError());
    if (!Created)
        goto Cleanup;
    if (!TaskmgrMessage(Search.Window, WM_SYSCOMMAND, SC_RESTORE, 0, &Result))
        goto Cleanup;
    Created = GetWindowRect(Search.Window, &Original);
    ok(Created, "Cannot measure the task manager window: %lu\n", GetLastError());
    if (!Created)
        goto Cleanup;
    CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "initial");

    Created = SetWindowPos(Search.Window, NULL, 0, 0, Original.right - Original.left + 137,
                           Original.bottom - Original.top + 83, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    ok(Created, "Cannot enlarge the task manager: %lu\n", GetLastError());
    CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "larger");
    Created = SetWindowPos(Search.Window, NULL, 0, 0, Original.right - Original.left,
                           Original.bottom - Original.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    ok(Created, "Cannot restore the task manager size: %lu\n", GetLastError());
    CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "restored");
    TaskmgrMessage(Search.Window, WM_SYSCOMMAND, SC_MAXIMIZE, 0, &Result);
    ok(IsZoomed(Search.Window), "The task manager was not maximized\n");
    CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "maximized");
    TaskmgrMessage(Search.Window, WM_SYSCOMMAND, SC_RESTORE, 0, &Result);
    ok(!IsZoomed(Search.Window), "The task manager was not restored\n");
    CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "unmaximized");

    OriginalStyle = GetWindowLongPtrW(Tab, GWL_STYLE);
    SetLastError(ERROR_SUCCESS);
    Result = SetWindowLongPtrW(Tab, GWL_STYLE, OriginalStyle | TCS_FIXEDWIDTH);
    ok(Result || GetLastError() == ERROR_SUCCESS, "Cannot set fixed-width tabs: %lu\n", GetLastError());
    if (TaskmgrMessage(Tab, TCM_SETITEMSIZE, 0, MAKELPARAM(100, 48), &OriginalItemSize))
    {
        Created = GetClientRect(Search.Window, &Client);
        ok(Created, "Cannot measure the task manager client: %lu\n", GetLastError());
        if (!Created)
            goto Cleanup;
        TaskmgrMessage(Search.Window, WM_SIZE, SIZE_RESTORED, MAKELPARAM(Client.right, Client.bottom), &Result);
        CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "taller tabs");
        TaskmgrMessage(Tab, TCM_SETITEMSIZE, 0, OriginalItemSize, &Result);
        SetLastError(ERROR_SUCCESS);
        Result = SetWindowLongPtrW(Tab, GWL_STYLE, OriginalStyle);
        ok(Result || GetLastError() == ERROR_SUCCESS, "Cannot restore the tab style: %lu\n", GetLastError());
        TaskmgrMessage(Search.Window, WM_SIZE, SIZE_RESTORED, MAKELPARAM(Client.right, Client.bottom), &Result);
        CheckTaskmgrPages(Search.Window, Tab, Process.hProcess, Remote, "restored tab height");
    }
    TaskmgrMessage(Tab, TCM_SETCURFOCUS, OriginalSelection, 0, &Result);
    Created = SetWindowPlacement(Search.Window, &Placement);
    ok(Created, "Cannot restore the window placement: %lu\n", GetLastError());

Cleanup:
    if (Search.Window)
        PostMessageW(Search.Window, WM_CLOSE, 0, 0);
    Wait = WaitForSingleObject(Process.hProcess, 5000);
    ok(Wait == WAIT_OBJECT_0, "The task manager did not close: %#lx\n", Wait);
    if (Wait != WAIT_OBJECT_0)
    {
        TerminateProcess(Process.hProcess, 1);
        WaitForSingleObject(Process.hProcess, 5000);
    }
    CloseHandle(Process.hProcess);
}

typedef struct _TAB_PAGE_CHECK
{
    HWND Window;
    HWND Tab;
    RECT Display;
    UINT VisiblePages;
    UINT Index;
    const char *Stage;
} TAB_PAGE_CHECK;

static BOOL CALLBACK FindTabbedWindow(HWND Window, LPARAM Parameter)
{
    TASKMGR_WINDOW *Search = (TASKMGR_WINDOW *)Parameter;
    DWORD ProcessId;

    GetWindowThreadProcessId(Window, &ProcessId);
    if (ProcessId == Search->ProcessId && FindWindowExW(Window, NULL, WC_TABCONTROLW, NULL))
    {
        Search->Window = Window;
        return FALSE;
    }
    return TRUE;
}

static BOOL CALLBACK CheckTabPage(HWND Page, LPARAM Parameter)
{
    TAB_PAGE_CHECK *Check = (TAB_PAGE_CHECK *)Parameter;
    WCHAR Class[32];
    RECT PageRect, Client, ControlRect;
    HWND Parent = GetParent(Page);
    HWND Control;
    BOOL Success;

    if ((Parent != Check->Window && Parent != Check->Tab) || !IsWindowVisible(Page) ||
        !GetClassNameW(Page, Class, ARRAYSIZE(Class)) || lstrcmpW(Class, L"#32770"))
        return TRUE;
    Check->VisiblePages++;
    Success = GetWindowRect(Page, &PageRect);
    ok(Success, "%s tab %u: cannot measure page %p: %lu\n", Check->Stage, Check->Index, Page, GetLastError());
    if (!Success)
        return TRUE;
    MapWindowPoints(NULL, Check->Window, (POINT *)&PageRect, 2);
    trace("%s tab %u: page %ld,%ld,%ld,%ld display %ld,%ld,%ld,%ld\n",
          Check->Stage, Check->Index, PageRect.left, PageRect.top, PageRect.right, PageRect.bottom,
          Check->Display.left, Check->Display.top, Check->Display.right, Check->Display.bottom);
    ok(!IsRectEmpty(&PageRect) && PageRect.left >= Check->Display.left && PageRect.top >= Check->Display.top &&
       PageRect.right <= Check->Display.right && PageRect.bottom <= Check->Display.bottom,
       "%s tab %u: page (%ld,%ld,%ld,%ld) covers the frame outside %ld,%ld,%ld,%ld\n",
       Check->Stage, Check->Index, PageRect.left, PageRect.top, PageRect.right, PageRect.bottom,
       Check->Display.left, Check->Display.top, Check->Display.right, Check->Display.bottom);
    Success = GetClientRect(Page, &Client);
    ok(Success, "%s: cannot measure the page client: %lu\n", Check->Stage, GetLastError());
    if (!Success)
        return TRUE;
    for (Control = GetWindow(Page, GW_CHILD); Control; Control = GetWindow(Control, GW_HWNDNEXT))
    {
        if (!IsWindowVisible(Control))
            continue;
        Success = GetWindowRect(Control, &ControlRect);
        ok(Success, "%s: cannot measure control %p: %lu\n", Check->Stage, Control, GetLastError());
        if (!Success)
            continue;
        MapWindowPoints(NULL, Page, (POINT *)&ControlRect, 2);
        ok(ControlRect.left >= Client.left && ControlRect.top >= Client.top &&
           ControlRect.right <= Client.right && ControlRect.bottom <= Client.bottom,
           "%s tab %u: control %d (%ld,%ld,%ld,%ld) is clipped outside %ld,%ld,%ld,%ld\n",
           Check->Stage, Check->Index, GetDlgCtrlID(Control), ControlRect.left, ControlRect.top,
           ControlRect.right, ControlRect.bottom, Client.left, Client.top, Client.right, Client.bottom);
    }
    return TRUE;
}

static void CheckTabbedWindow(HWND Window, HWND Tab, HANDLE Process, RECT *Remote, const char *Stage)
{
    TAB_PAGE_CHECK Check = {0};
    DWORD_PTR Count, Result;

    if (!TaskmgrMessage(Tab, TCM_GETITEMCOUNT, 0, 0, &Count))
        return;
    ok(Count > 0 && Count <= 256, "%s: unexpected tab count %I64u\n", Stage, (ULONGLONG)Count);
    if (!Count || Count > 256)
        return;
    Check.Window = Window;
    Check.Tab = Tab;
    Check.Stage = Stage;
    for (Check.Index = 0; Check.Index < Count; Check.Index++)
    {
        if (!TaskmgrMessage(Tab, TCM_SETCURFOCUS, Check.Index, 0, &Result) ||
            !TaskmgrDisplay(Window, Tab, Process, Remote, &Check.Display))
            return;
        ok(!IsRectEmpty(&Check.Display), "%s: empty tab display area\n", Stage);
        Check.VisiblePages = 0;
        EnumChildWindows(Window, CheckTabPage, (LPARAM)&Check);
        ok(Check.VisiblePages == 1, "%s tab %u: %u visible pages\n", Stage, Check.Index, Check.VisiblePages);
    }
}

START_TEST(tab_layout)
{
    STARTUPINFOW Startup = {sizeof(Startup)};
    PROCESS_INFORMATION Process;
    TASKMGR_WINDOW Search = {0};
    WCHAR Path[MAX_PATH], Command[MAX_PATH + 4];
    RECT Client, *Remote;
    HWND Tab;
    LONG_PTR Style;
    DWORD_PTR OriginalSize, Result;
    DWORD Started, Wait;
    BOOL Success;
    char **Arguments;
    int Count = winetest_get_mainargs(&Arguments);

    if (Count < 3)
    {
        skip("Pass the path to the ReliefWin application as the third argument\n");
        return;
    }
    Success = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, Arguments[2], -1, Path, ARRAYSIZE(Path)) != 0;
    ok(Success, "Invalid application path: %lu\n", GetLastError());
    if (!Success)
        return;
    wsprintfW(Command, L"\"%s\"", Path);
    Success = CreateProcessW(Path, Command, NULL, NULL, FALSE, 0, NULL, NULL, &Startup, &Process);
    ok(Success, "Cannot start %s: %lu\n", wine_dbgstr_w(Path), GetLastError());
    if (!Success)
        return;
    CloseHandle(Process.hThread);
    Wait = WaitForInputIdle(Process.hProcess, 10000);
    ok(Wait == 0, "The application did not become idle: %#lx\n", Wait);
    if (Wait != 0)
        goto Cleanup;
    Search.ProcessId = Process.dwProcessId;
    Started = GetTickCount();
    do
    {
        EnumWindows(FindTabbedWindow, (LPARAM)&Search);
        if (Search.Window || WaitForSingleObject(Process.hProcess, 50) != WAIT_TIMEOUT)
            break;
    } while (GetTickCount() - Started < 10000);
    ok(Search.Window != NULL, "The tabbed application window did not appear\n");
    if (!Search.Window)
        goto Cleanup;
    Tab = FindWindowExW(Search.Window, NULL, WC_TABCONTROLW, NULL);
    Remote = VirtualAllocEx(Process.hProcess, NULL, sizeof(*Remote), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    ok(Remote != NULL, "Cannot allocate the remote display rectangle: %lu\n", GetLastError());
    if (!Remote)
        goto Cleanup;
    CheckTabbedWindow(Search.Window, Tab, Process.hProcess, Remote, "initial");
    Style = GetWindowLongPtrW(Tab, GWL_STYLE);
    SetLastError(ERROR_SUCCESS);
    Result = SetWindowLongPtrW(Tab, GWL_STYLE, Style | TCS_FIXEDWIDTH);
    ok(Result || GetLastError() == ERROR_SUCCESS, "Cannot set fixed-width tabs: %lu\n", GetLastError());
    if (TaskmgrMessage(Tab, TCM_SETITEMSIZE, 0, MAKELPARAM(80, 48), &OriginalSize))
    {
        Success = GetClientRect(Search.Window, &Client);
        ok(Success, "Cannot measure the application client: %lu\n", GetLastError());
        if (!Success)
            goto Cleanup;
        TaskmgrMessage(Search.Window, WM_SIZE, SIZE_RESTORED, MAKELPARAM(Client.right, Client.bottom), &Result);
        CheckTabbedWindow(Search.Window, Tab, Process.hProcess, Remote, "taller tabs");
        if (Count > 3 && !strcmp(Arguments[3], "--snapshot"))
        {
            TaskmgrMessage(Tab, TCM_SETCURFOCUS, 1, 0, &Result);
            Success = SetForegroundWindow(Search.Window);
            ok(Success, "Cannot bring the snapshot window to the foreground\n");
            trace("Snapshot ready: %s\n", Arguments[2]);
            fflush(stdout);
            FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
            WaitForSingleObject(Process.hProcess, 30000);
            goto Cleanup;
        }
        TaskmgrMessage(Tab, TCM_SETITEMSIZE, 0, OriginalSize, &Result);
        SetLastError(ERROR_SUCCESS);
        Result = SetWindowLongPtrW(Tab, GWL_STYLE, Style);
        ok(Result || GetLastError() == ERROR_SUCCESS, "Cannot restore the tab style: %lu\n", GetLastError());
        TaskmgrMessage(Search.Window, WM_SIZE, SIZE_RESTORED, MAKELPARAM(Client.right, Client.bottom), &Result);
        CheckTabbedWindow(Search.Window, Tab, Process.hProcess, Remote, "restored tabs");
    }

Cleanup:
    if (Search.Window)
        PostMessageW(Search.Window, WM_CLOSE, 0, 0);
    Wait = WaitForSingleObject(Process.hProcess, 5000);
    ok(Wait == WAIT_OBJECT_0, "The application did not close: %#lx\n", Wait);
    if (Wait != WAIT_OBJECT_0)
    {
        TerminateProcess(Process.hProcess, 1);
        WaitForSingleObject(Process.hProcess, 5000);
    }
    CloseHandle(Process.hProcess);
}
