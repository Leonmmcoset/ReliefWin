/*
 * PROJECT:     ReliefWin Recovery Center
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Manual recovery user interface
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "recovery.h"
#include "resource.h"
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <strsafe.h>

static HINSTANCE Instance;
static BOOL Environment;
static UINT Page;
static WCHAR Windows[MAX_PATH];
static WCHAR BcdPath[MAX_PATH];
static const PCWSTR Logs[] = {L"..\\lastboot.log", L"..\\prevboot.log", L"debug.log", L"ntbtlog.txt", L"setupact.log", L"setuperr.log", L"setupapi.log", L"Panther\\setupact.log"};

static VOID
EnvironmentColors(VOID)
{
    static const struct { int Index; PCWSTR Name; } Colors[] = {
        {COLOR_BTNFACE, L"ButtonFace"}, {COLOR_BTNTEXT, L"ButtonText"},
        {COLOR_WINDOW, L"Window"}, {COLOR_WINDOWTEXT, L"WindowText"},
        {COLOR_MENUTEXT, L"MenuText"}, {COLOR_MENU, L"Menu"},
        {COLOR_GRAYTEXT, L"GrayText"}, {COLOR_HIGHLIGHT, L"Hilight"},
        {COLOR_HIGHLIGHTTEXT, L"HilightText"}, {COLOR_SCROLLBAR, L"Scrollbar"}};
    HKEY Key;
    WCHAR Value[64];
    DWORD Index, Type, Size, Red, Green, Blue;
    int Element;
    COLORREF Color;

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Colors", 0, KEY_QUERY_VALUE, &Key) != ERROR_SUCCESS)
        return;
    for (Index = 0; Index < ARRAYSIZE(Colors); Index++)
    {
        Size = sizeof(Value);
        if (RegQueryValueExW(Key, Colors[Index].Name, NULL, &Type, (PBYTE)Value, &Size) != ERROR_SUCCESS || Type != REG_SZ)
            continue;
        Value[ARRAYSIZE(Value) - 1] = 0;
        if (swscanf(Value, L"%lu %lu %lu", &Red, &Green, &Blue) != 3 || Red > 255 || Green > 255 || Blue > 255)
            continue;
        Element = Colors[Index].Index;
        Color = RGB(Red, Green, Blue);
        SetSysColors(1, &Element, &Color);
    }
    RegCloseKey(Key);
}

static PCWSTR
Text(UINT Id)
{
    static WCHAR Values[8][1024];
    static UINT Slot;
    PWSTR Value = Values[Slot++ % ARRAYSIZE(Values)];
    Value[0] = 0;
    LoadStringW(Instance, Id, Value, ARRAYSIZE(Values[0]));
    return Value;
}

static VOID
Status(HWND Dialog, DWORD Error)
{
    WCHAR System[256] = L"", Message[512];
    if (Error == ERROR_SUCCESS)
        SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_OK));
    else
    {
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, Error, 0,
                       System, ARRAYSIZE(System), NULL);
        StringCchPrintfW(Message, ARRAYSIZE(Message), Text(IDS_FAILED), Error, System);
        SetDlgItemTextW(Dialog, IDC_STATUS, Message);
    }
}

static BOOL
Confirm(HWND Dialog, UINT Id, PCWSTR First, PCWSTR Second)
{
    WCHAR Message[1024];
    StringCchPrintfW(Message, ARRAYSIZE(Message), Text(Id), First, Second);
    return MessageBoxW(Dialog, Message, Text(IDS_TITLE), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES;
}

static BOOL
FileDialog(HWND Dialog, PWSTR Path, UINT Title, BOOL Save)
{
    OPENFILENAMEW Open = {sizeof(Open)};
    Open.hwndOwner = Dialog;
    Open.lpstrFile = Path;
    Open.nMaxFile = MAX_PATH;
    Open.lpstrTitle = Text(Title);
    Open.lpstrFilter = L"BCD\0*.*\0\0";
    Open.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | (Save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
    return Save ? GetSaveFileNameW(&Open) : GetOpenFileNameW(&Open);
}

static VOID
FindBcd(HWND Dialog)
{
    WCHAR Volume[MAX_PATH], Path[MAX_PATH];
    HANDLE Find = FindFirstVolumeW(Volume, ARRAYSIZE(Volume));
    const PCWSTR Names[] = {L"EFI\\ReliefWin\\Boot\\BCD", L"Boot\\BCD"};
    UINT Index;

    if (Find == INVALID_HANDLE_VALUE) return;
    do
    {
        for (Index = 0; Index < ARRAYSIZE(Names); Index++)
        {
            if (SUCCEEDED(StringCchPrintfW(Path, ARRAYSIZE(Path), L"%s%s", Volume, Names[Index])) &&
                GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES)
                SendDlgItemMessageW(Dialog, IDC_BCD, CB_ADDSTRING, 0, (LPARAM)Path);
        }
    } while (FindNextVolumeW(Find, Volume, ARRAYSIZE(Volume)));
    FindVolumeClose(Find);
    if (SendDlgItemMessageW(Dialog, IDC_BCD, CB_GETCOUNT, 0, 0) == 1)
        SendDlgItemMessageW(Dialog, IDC_BCD, CB_SETCURSEL, 0, 0);
}

static VOID
LoadDrivers(HWND Dialog)
{
    HKEY Hive, Services;
    RC_DRIVER Driver;
    WCHAR Name[RC_DRIVER_NAME];
    DWORD Error, Index, Length;
    HWND List = GetDlgItem(Dialog, IDC_DRIVERS);
    LVITEMW Item = {0};
    int Row = 0;

    EnableWindow(GetDlgItem(Dialog, IDC_DISABLE), FALSE);
    EnableWindow(GetDlgItem(Dialog, IDC_UNDO), FALSE);
    ListView_DeleteAllItems(List);
    if (!Windows[0]) return;
    Error = RcOpenSystem(Windows, FALSE, &Hive, &Services);
    if (Error != ERROR_SUCCESS)
    {
        Status(Dialog, Error);
        return;
    }
    for (Index = 0; ; Index++)
    {
        Length = ARRAYSIZE(Name);
        Error = RegEnumKeyExW(Services, Index, Name, &Length, NULL, NULL, NULL, NULL);
        if (Error == ERROR_NO_MORE_ITEMS)
        {
            Error = ERROR_SUCCESS;
            break;
        }
        if (Error != ERROR_SUCCESS) break;
        Error = RcReadDriver(Services, Name, &Driver);
        if (Error == ERROR_NOT_SUPPORTED) continue;
        if (Error != ERROR_SUCCESS) break;
        Item.mask = LVIF_TEXT | LVIF_PARAM;
        Item.iItem = Row;
        Item.pszText = Name;
        Item.lParam = (Driver.Protected ? 1 : 0) | (Driver.CanUndo ? 2 : 0) |
                      (Driver.Start == SERVICE_DISABLED ? 4 : 0);
        if (ListView_InsertItem(List, &Item) == -1)
        {
            Error = ERROR_NOT_ENOUGH_MEMORY;
            break;
        }
        ListView_SetItemText(List, Row, 1, Driver.Display);
        ListView_SetItemText(List, Row, 2, (PWSTR)Text(IDS_BOOT_START + Driver.Start));
        ListView_SetItemText(List, Row, 3, (PWSTR)Text(Driver.CanUndo ? IDS_UNDO_AVAILABLE : Driver.Protected ? IDS_YES : IDS_NO));
        Row++;
    }
    RegCloseKey(Services);
    RegCloseKey(Hive);
    Status(Dialog, Error);
    if (Error == ERROR_SUCCESS && Row == 0)
        SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_NO_DRIVERS));
}

static VOID
SelectPage(HWND Dialog, UINT Next)
{
    static const int LogControls[] = {IDC_LOGS, IDC_READ_LOG, IDC_OUTPUT};
    static const int DriverControls[] = {IDC_DRIVERS, IDC_DISABLE, IDC_UNDO};
    static const int BootControls[] = {IDC_BCD, IDC_BROWSE_BCD, IDC_BACKUP, IDC_RESTORE};
    UINT Index;

    Page = Next;
    for (Index = 0; Index < ARRAYSIZE(LogControls); Index++)
        ShowWindow(GetDlgItem(Dialog, LogControls[Index]), Page == 0 ? SW_SHOW : SW_HIDE);
    for (Index = 0; Index < ARRAYSIZE(DriverControls); Index++)
        ShowWindow(GetDlgItem(Dialog, DriverControls[Index]), Page == 1 ? SW_SHOW : SW_HIDE);
    for (Index = 0; Index < ARRAYSIZE(BootControls); Index++)
        ShowWindow(GetDlgItem(Dialog, BootControls[Index]), Page == 2 ? SW_SHOW : SW_HIDE);
    SetDlgItemTextW(Dialog, IDC_PAGE_HELP, Text(Page == 0 ? IDS_LOG_HELP : Page == 1 ? IDS_DRIVER_HELP : IDS_BOOT_HELP));
    if (Page == 1) LoadDrivers(Dialog);
}

static VOID
AddTarget(HWND Dialog, PCWSTR Path)
{
    WCHAR System[MAX_PATH];
    HKEY Hive, Services;
    HWND Combo = GetDlgItem(Dialog, IDC_TARGET);
    int Item;

    if (FAILED(StringCchPrintfW(System, ARRAYSIZE(System), L"%s\\system32\\config\\SYSTEM", Path)) ||
        GetFileAttributesW(System) == INVALID_FILE_ATTRIBUTES)
        return;
    if (RcOpenSystem(Path, FALSE, &Hive, &Services) != ERROR_SUCCESS)
        return;
    RegCloseKey(Services);
    RegCloseKey(Hive);
    Item = (int)SendMessageW(Combo, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)Path);
    if (Item == CB_ERR)
        Item = (int)SendMessageW(Combo, CB_ADDSTRING, 0, (LPARAM)Path);
    if (Item != CB_ERR && Item != CB_ERRSPACE)
    {
        SendMessageW(Combo, CB_SETCURSEL, Item, 0);
        StringCchCopyW(Windows, ARRAYSIZE(Windows), Path);
    }
}

static VOID
FindTargets(HWND Dialog)
{
    WCHAR Path[] = L"C:\\Windows";
    DWORD Drives = GetLogicalDrives();
    for (Path[0] = L'A'; Path[0] <= L'Z'; Path[0]++)
    {
        if (Drives & (1u << (Path[0] - L'A')))
            AddTarget(Dialog, Path);
    }
    if (SendDlgItemMessageW(Dialog, IDC_TARGET, CB_GETCOUNT, 0, 0) > 0)
    {
        SendDlgItemMessageW(Dialog, IDC_TARGET, CB_SETCURSEL, 0, 0);
        SendDlgItemMessageW(Dialog, IDC_TARGET, CB_GETLBTEXT, 0, (LPARAM)Windows);
    }
}

static VOID
BrowseTarget(HWND Dialog)
{
    BROWSEINFOW Browse = {0};
    WCHAR Path[MAX_PATH];
    PIDLIST_ABSOLUTE Id;

    Browse.hwndOwner = Dialog;
    Browse.lpszTitle = Text(IDS_CHOOSE_TARGET);
    Browse.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    Id = SHBrowseForFolderW(&Browse);
    if (!Id) return;
    if (SHGetPathFromIDListW(Id, Path))
    {
        HKEY Hive, Services;
        DWORD Error = RcOpenSystem(Path, FALSE, &Hive, &Services);
        if (Error == ERROR_SUCCESS)
        {
            RegCloseKey(Services);
            RegCloseKey(Hive);
            AddTarget(Dialog, Path);
            SetDlgItemTextW(Dialog, IDC_OUTPUT, L"");
            SelectPage(Dialog, Page);
        }
        Status(Dialog, Error);
    }
    CoTaskMemFree(Id);
}

static VOID
ReadLog(HWND Dialog)
{
    WCHAR Path[MAX_PATH];
    PWSTR Value;
    BOOL Truncated;
    int Log = (int)SendDlgItemMessageW(Dialog, IDC_LOGS, CB_GETCURSEL, 0, 0);
    DWORD Error;

    if (!Windows[0] || Log < 0 || (UINT)Log >= ARRAYSIZE(Logs)) return;
    if (FAILED(StringCchPrintfW(Path, ARRAYSIZE(Path), L"%s\\%s", Windows, Logs[Log])))
    {
        Status(Dialog, ERROR_FILENAME_EXCED_RANGE);
        return;
    }
    SetDlgItemTextW(Dialog, IDC_OUTPUT, L"");
    Error = RcReadLog(Path, &Value, &Truncated);
    Status(Dialog, Error);
    if (Error != ERROR_SUCCESS) return;
    SetDlgItemTextW(Dialog, IDC_OUTPUT, Value);
    HeapFree(GetProcessHeap(), 0, Value);
    if (Truncated) SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_TRUNCATED));
}

static VOID
ChangeDriver(HWND Dialog, BOOL Undo)
{
    HWND List = GetDlgItem(Dialog, IDC_DRIVERS);
    int Selected = ListView_GetNextItem(List, -1, LVNI_SELECTED);
    WCHAR Name[RC_DRIVER_NAME];
    HKEY Hive, Services;
    DWORD Error;

    if (Selected < 0 || !Windows[0]) return;
    ListView_GetItemText(List, Selected, 0, Name, ARRAYSIZE(Name));
    if (!Confirm(Dialog, Undo ? IDS_CONFIRM_UNDO : IDS_CONFIRM_DISABLE, Name, Windows)) return;
    Error = RcOpenSystem(Windows, TRUE, &Hive, &Services);
    if (Error == ERROR_SUCCESS)
    {
        Error = RcChangeDriver(Services, Name, Undo);
        RegCloseKey(Services);
        RegCloseKey(Hive);
    }
    LoadDrivers(Dialog);
    Status(Dialog, Error);
}

static VOID
Backup(HWND Dialog, BOOL Restore)
{
    WCHAR Path[MAX_PATH] = L"";
    DWORD Error;
    GetDlgItemTextW(Dialog, IDC_BCD, BcdPath, ARRAYSIZE(BcdPath));
    if (!BcdPath[0]) return;
    if (!FileDialog(Dialog, Path, Restore ? IDS_OPEN_BACKUP : IDS_SAVE_BACKUP, !Restore)) return;
    if (!Confirm(Dialog, Restore ? IDS_CONFIRM_RESTORE : IDS_CONFIRM_BACKUP, BcdPath, Path)) return;
    Error = Restore ? RcRestoreBcd(BcdPath, Path) : RcBackupBcd(BcdPath, Path);
    Status(Dialog, Error);
}

static BOOL
Restart(HWND Dialog, BOOL Exiting)
{
    HANDLE Token;
    TOKEN_PRIVILEGES Privileges = {0};
    DWORD Error = ERROR_SUCCESS;

    if (!Confirm(Dialog, Exiting ? IDS_CONFIRM_EXIT : IDS_CONFIRM_RESTART, L"", L"")) return FALSE;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &Token))
        Error = GetLastError();
    else
    {
        Privileges.PrivilegeCount = 1;
        if (!LookupPrivilegeValueW(NULL, SE_SHUTDOWN_NAME, &Privileges.Privileges[0].Luid))
            Error = GetLastError();
        else
        {
            Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            SetLastError(ERROR_SUCCESS);
            AdjustTokenPrivileges(Token, FALSE, &Privileges, 0, NULL, NULL);
            Error = GetLastError();
        }
        CloseHandle(Token);
    }
    if (Error == ERROR_SUCCESS && !ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED))
        Error = GetLastError();
    Status(Dialog, Error);
    return Error == ERROR_SUCCESS;
}

static INT_PTR CALLBACK
DialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam)
{
    UINT Id;
    switch (Message)
    {
        case WM_INITDIALOG:
        {
            LVCOLUMNW Column = {LVCF_TEXT | LVCF_WIDTH};
            const UINT Names[] = {IDS_NAME, IDS_DISPLAY, IDS_START, IDS_PROTECTED};
            const int Widths[] = {180, 320, 120, 170};
            UINT Index;
            SendDlgItemMessageW(Dialog, IDC_DRIVERS, WM_SYSCOLORCHANGE, 0, 0);
            ListView_SetBkColor(GetDlgItem(Dialog, IDC_DRIVERS), GetSysColor(COLOR_WINDOW));
            ListView_SetTextBkColor(GetDlgItem(Dialog, IDC_DRIVERS), GetSysColor(COLOR_WINDOW));
            ListView_SetTextColor(GetDlgItem(Dialog, IDC_DRIVERS), GetSysColor(COLOR_WINDOWTEXT));
            ListView_SetExtendedListViewStyle(GetDlgItem(Dialog, IDC_DRIVERS), LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
            for (Index = 0; Index < ARRAYSIZE(Names); Index++)
            {
                Column.pszText = (PWSTR)Text(Names[Index]);
                Column.cx = MulDiv(Widths[Index], GetDpiForWindow(Dialog), 96);
                ListView_InsertColumn(GetDlgItem(Dialog, IDC_DRIVERS), Index, &Column);
            }
            SendDlgItemMessageW(Dialog, IDC_OUTPUT, EM_SETLIMITTEXT, 512 * 1024, 0);
            for (Index = 0; Index < ARRAYSIZE(Logs); Index++)
                SendDlgItemMessageW(Dialog, IDC_LOGS, CB_ADDSTRING, 0, (LPARAM)Logs[Index]);
            SendDlgItemMessageW(Dialog, IDC_LOGS, CB_SETCURSEL, 0, 0);
            SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_READY));
            FindTargets(Dialog);
            FindBcd(Dialog);
            SelectPage(Dialog, 0);
            return TRUE;
        }
        case WM_NOTIFY:
        {
            LPNMHDR Header = (LPNMHDR)LParam;
            if (Header->idFrom == IDC_DRIVERS && Header->code == LVN_ITEMCHANGED)
            {
                HWND List = Header->hwndFrom;
                LVITEMW Item = {LVIF_PARAM};
                Item.iItem = ListView_GetNextItem(List, -1, LVNI_SELECTED);
                if (Item.iItem >= 0 && ListView_GetItem(List, &Item))
                {
                    EnableWindow(GetDlgItem(Dialog, IDC_DISABLE), !(Item.lParam & (1 | 4)));
                    EnableWindow(GetDlgItem(Dialog, IDC_UNDO), (Item.lParam & 2) != 0);
                }
                else
                {
                    EnableWindow(GetDlgItem(Dialog, IDC_DISABLE), FALSE);
                    EnableWindow(GetDlgItem(Dialog, IDC_UNDO), FALSE);
                }
            }
            break;
        }
        case WM_COMMAND:
            Id = LOWORD(WParam);
            if (Id == IDC_TARGET && HIWORD(WParam) == CBN_SELCHANGE)
            {
                int Selected = (int)SendDlgItemMessageW(Dialog, IDC_TARGET, CB_GETCURSEL, 0, 0);
                if (Selected != CB_ERR)
                {
                    SendDlgItemMessageW(Dialog, IDC_TARGET, CB_GETLBTEXT, Selected, (LPARAM)Windows);
                    SetDlgItemTextW(Dialog, IDC_OUTPUT, L"");
                    SelectPage(Dialog, Page);
                }
                return TRUE;
            }
            if (HIWORD(WParam) != BN_CLICKED) break;
            switch (Id)
            {
                case IDC_LOG_PAGE: SelectPage(Dialog, 0); break;
                case IDC_DRIVER_PAGE: SelectPage(Dialog, 1); break;
                case IDC_BOOT_PAGE: SelectPage(Dialog, 2); break;
                case IDC_BROWSE_TARGET: BrowseTarget(Dialog); break;
                case IDC_READ_LOG: ReadLog(Dialog); break;
                case IDC_DISABLE: ChangeDriver(Dialog, FALSE); break;
                case IDC_UNDO: ChangeDriver(Dialog, TRUE); break;
                case IDC_BROWSE_BCD:
                    if (FileDialog(Dialog, BcdPath, IDS_CHOOSE_BCD, FALSE))
                        SetDlgItemTextW(Dialog, IDC_BCD, BcdPath);
                    break;
                case IDC_BACKUP: Backup(Dialog, FALSE); break;
                case IDC_RESTORE: Backup(Dialog, TRUE); break;
                case IDC_RESTART: Restart(Dialog, FALSE); break;
                case IDCANCEL:
                    if (!Environment) EndDialog(Dialog, 0);
                    else Restart(Dialog, TRUE);
                    break;
            }
            return TRUE;
        case WM_CLOSE:
            SendMessageW(Dialog, WM_COMMAND, IDCANCEL, 0);
            return TRUE;
    }
    return FALSE;
}

int WINAPI
wWinMain(HINSTANCE Current, HINSTANCE Previous, PWSTR CommandLine, int Show)
{
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_LISTVIEW_CLASSES};
    HANDLE Mutex;
    INT_PTR Result;
    int Count;
    PWSTR *Arguments;

    UNREFERENCED_PARAMETER(Previous);
    UNREFERENCED_PARAMETER(CommandLine);
    UNREFERENCED_PARAMETER(Show);
    Instance = Current;
    Arguments = CommandLineToArgvW(GetCommandLineW(), &Count);
    Environment = Arguments && Count == 2 && !_wcsicmp(Arguments[1], L"/environment");
    if (Arguments) LocalFree(Arguments);
    Mutex = CreateMutexW(NULL, FALSE, L"Local\\ReliefWinRecoveryCenter");
    if (!Mutex) return GetLastError();
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxW(NULL, Text(IDS_BUSY), Text(IDS_TITLE), MB_OK | MB_ICONINFORMATION);
        CloseHandle(Mutex);
        return ERROR_ALREADY_EXISTS;
    }
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (Environment) EnvironmentColors();
    InitCommonControlsEx(&Controls);
    Result = DialogBoxParamW(Instance, MAKEINTRESOURCEW(IDD_RECOVERY), NULL, DialogProc, 0);
    CoUninitialize();
    CloseHandle(Mutex);
    return Result == -1 ? GetLastError() : 0;
}
