/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verify the persisted first-run result in the new user's session
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <windows.h>
#include <lm.h>
#include <sddl.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static unsigned Checks, Failures;

static void Check(BOOL Condition, const char *Name)
{
    ++Checks;
    if (!Condition) ++Failures;
    printf("%s: %s\n", Condition ? "PASS" : "FAIL", Name);
}

static BYTE *CaptureClient(HWND Window, SIZE_T *Size)
{
    RECT Rect;
    BITMAPINFO Bitmap = {0};
    HDC Source = GetDC(Window), Target = Source ? CreateCompatibleDC(Source) : NULL;
    HBITMAP Image = NULL, Previous;
    void *Bits;
    BYTE *Copy = NULL;
    GetClientRect(Window, &Rect);
    *Size = (SIZE_T)Rect.right * Rect.bottom * 4;
    Bitmap.bmiHeader.biSize = sizeof(Bitmap.bmiHeader);
    Bitmap.bmiHeader.biWidth = Rect.right;
    Bitmap.bmiHeader.biHeight = -Rect.bottom;
    Bitmap.bmiHeader.biPlanes = 1;
    Bitmap.bmiHeader.biBitCount = 32;
    Bitmap.bmiHeader.biCompression = BI_RGB;
    if (Target) Image = CreateDIBSection(Source, &Bitmap, DIB_RGB_COLORS, &Bits, NULL, 0);
    if (Image)
    {
        Previous = SelectObject(Target, Image);
        if (BitBlt(Target, 0, 0, Rect.right, Rect.bottom, Source, 0, 0, SRCCOPY) && GdiFlush())
        {
            Copy = HeapAlloc(GetProcessHeap(), 0, *Size);
            if (Copy) CopyMemory(Copy, Bits, *Size);
        }
        SelectObject(Target, Previous);
        DeleteObject(Image);
    }
    if (Target) DeleteDC(Target);
    if (Source) ReleaseDC(Window, Source);
    return Copy;
}

static void CheckComboRepaint(void)
{
    WNDCLASSW Class = {0};
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_STANDARD_CLASSES};
    HWND Parent, Combo;
    BYTE *Before, *After;
    SIZE_T BeforeSize, AfterSize;
    unsigned Index;
    InitCommonControlsEx(&Controls);
    Class.lpfnWndProc = DefWindowProcW;
    Class.hInstance = GetModuleHandleW(NULL);
    Class.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    Class.lpszClassName = L"ReliefWin.Oobe.ControlTest";
    RegisterClassW(&Class);
    Parent = CreateWindowW(Class.lpszClassName, L"OOBE control test", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           40, 40, 600, 180, NULL, NULL, Class.hInstance, NULL);
    Check(Parent != NULL, "control test window created");
    if (!Parent) return;
    Combo = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                          20, 20, 500, 100, Parent, NULL, Class.hInstance, NULL);
    Check(Combo != NULL, "dropdown created");
    if (!Combo) { DestroyWindow(Parent); return; }
    SetWindowTheme(Combo, L"ContentLight", NULL);
    Check(GetWindowTheme(Combo) != NULL, "dropdown has an active visual style");
    SendMessageW(Combo, CB_ADDSTRING, 0, (LPARAM)L"A long old selection that must be fully erased");
    SendMessageW(Combo, CB_ADDSTRING, 0, (LPARAM)L"B");
    SendMessageW(Combo, CB_SETCURSEL, 0, 0);
    SetFocus(Combo);
    RedrawWindow(Parent, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    for (Index = 0; Index < 2; ++Index)
    {
        SendMessageW(Combo, CB_SETCURSEL, 0, 0);
        RedrawWindow(Combo, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        if (Index == 0) SendMessageW(Combo, CB_SETCURSEL, 1, 0);
        else SendMessageW(Combo, WM_KEYDOWN, VK_DOWN, 0);
        Check(SendMessageW(Combo, CB_GETCURSEL, 0, 0) == 1, "dropdown selection changed");
        Before = CaptureClient(Combo, &BeforeSize);
        RedrawWindow(Combo, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        After = CaptureClient(Combo, &AfterSize);
        Check(Before && After && BeforeSize == AfterSize && !memcmp(Before, After, BeforeSize),
              Index == 0 ? "programmatic selection erases old text" : "keyboard selection erases old text");
        if (Before) HeapFree(GetProcessHeap(), 0, Before);
        if (After) HeapFree(GetProcessHeap(), 0, After);
    }
    DestroyWindow(Parent);
    UnregisterClassW(Class.lpszClassName, Class.hInstance);
}

static void CheckAliasName(WELL_KNOWN_SID_TYPE Type)
{
    BYTE Sid[SECURITY_MAX_SID_SIZE], Resolved[SECURITY_MAX_SID_SIZE];
    WCHAR Name[256], Domain[256];
    DWORD Size = sizeof(Sid), NameSize = _countof(Name), DomainSize = _countof(Domain);
    SID_NAME_USE Use;
    PLOCALGROUP_INFO_0 Info = NULL;
    BOOL Success = CreateWellKnownSid(Type, NULL, Sid, &Size);
    Check(Success, "builtin alias SID is available");
    if (!Success) return;
    Success = LookupAccountSidW(NULL, Sid, Name, &NameSize, Domain, &DomainSize, &Use);
    Check(Success && Use == SidTypeAlias, "builtin alias SID resolves to an alias name");
    if (!Success) return;
    Check(NetLocalGroupGetInfo(NULL, Name, 0, (LPBYTE *)&Info) == NERR_Success,
          "resolved alias name exists in the local account database");
    if (Info) NetApiBufferFree(Info);
    Size = sizeof(Resolved);
    DomainSize = _countof(Domain);
    Check(LookupAccountNameW(NULL, Name, Resolved, &Size, Domain, &DomainSize, &Use) &&
          Use == SidTypeAlias && EqualSid(Sid, Resolved), "alias name resolves to the original SID");
    Check(Size == GetLengthSid(Sid), "successful account lookup returns actual SID length");
}

int wmain(int Count, WCHAR **Args)
{
    WCHAR User[UNLEN + 1], Value[MAX_PATH];
    DWORD Size, Complete, Error, Entries = 0, Total = 0, Index;
    PUSER_INFO_1 Info = NULL;
    PLOCALGROUP_USERS_INFO_0 Groups = NULL;
    BOOL Admin = FALSE, Ordinary = FALSE;
    if (Count != 2)
    {
        puts("Usage: oobe_test <expected-user|/aliases|/controls>");
        return 2;
    }
    if (!wcscmp(Args[1], L"/controls"))
    {
        CheckComboRepaint();
        printf("%u checks, %u failures\n", Checks, Failures);
        return Failures ? 1 : 0;
    }
    CheckAliasName(WinBuiltinAdministratorsSid);
    CheckAliasName(WinBuiltinUsersSid);
    if (!wcscmp(Args[1], L"/aliases"))
    {
        printf("%u checks, %u failures\n", Checks, Failures);
        return Failures ? 1 : 0;
    }
    Size = _countof(User);
    Check(GetUserNameW(User, &Size) && !_wcsicmp(User, Args[1]), "new account is logged in");
    Check(GetShellWindow() != NULL, "desktop shell is running");
    Error = NetUserGetInfo(NULL, Args[1], 1, (LPBYTE *)&Info);
    Check(Error == NERR_Success, "local account exists");
    if (Error == NERR_Success)
    {
        Check((Info->usri1_flags & UF_NORMAL_ACCOUNT) != 0, "normal local account");
        Check(!(Info->usri1_flags & UF_ACCOUNTDISABLE), "account is enabled");
        Check(!(Info->usri1_flags & UF_PASSWD_NOTREQD), "password required");
        NetApiBufferFree(Info);
    }
    Error = NetUserGetLocalGroups(NULL, Args[1], 0, 0, (LPBYTE *)&Groups, MAX_PREFERRED_LENGTH, &Entries, &Total);
    Check(Error == NERR_Success, "group memberships can be queried");
    if (Error == NERR_Success)
    {
        for (Index = 0; Index < Entries; ++Index)
        {
            BYTE Sid[SECURITY_MAX_SID_SIZE];
            WCHAR Domain[256];
            DWORD SidSize = sizeof(Sid), DomainSize = _countof(Domain);
            SID_NAME_USE Use;
            if (LookupAccountNameW(NULL, Groups[Index].lgrui0_name, Sid, &SidSize, Domain, &DomainSize, &Use))
            {
                Admin |= IsWellKnownSid(Sid, WinBuiltinAdministratorsSid);
                Ordinary |= IsWellKnownSid(Sid, WinBuiltinUsersSid);
            }
        }
        NetApiBufferFree(Groups);
    }
    Check(Admin, "member of Administrators");
    Check(Ordinary, "member of Users");
    Size = sizeof(Complete);
    Error = RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\ReliefWin\\OOBE", L"Completed",
                         RRF_RT_REG_DWORD, NULL, &Complete, &Size);
    Check(Error == ERROR_SUCCESS && Complete == 1, "completion is persisted");
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                         L"OOBE", RRF_RT_REG_SZ, NULL, Value, &Size);
    Check(Error == ERROR_FILE_NOT_FOUND, "first-run entry was removed");
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
                         L"AutoAdminLogon", RRF_RT_REG_SZ, NULL, Value, &Size);
    Check(Error == ERROR_SUCCESS && !wcscmp(Value, L"0"), "automatic sign-in is disabled");
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
                         L"DefaultPassword", RRF_RT_REG_SZ, NULL, Value, &Size);
    Check(Error == ERROR_FILE_NOT_FOUND, "no plaintext sign-in password");
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"PreferredUILanguages",
                         RRF_RT_REG_MULTI_SZ, NULL, Value, &Size);
    Check(Error == ERROR_SUCCESS && !wcscmp(Value, L"zh-CN"), "new user inherited Chinese UI language");
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload", L"1",
                         RRF_RT_REG_SZ, NULL, Value, &Size);
    Check(Error == ERROR_SUCCESS && Value[0], "new user inherited keyboard selection");
    printf("%u checks, %u failures\n", Checks, Failures);
    return Failures ? 1 : 0;
}
