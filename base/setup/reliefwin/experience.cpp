/*
 * PROJECT:     ReliefWin Setup
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Fluent first-run experience with offline setup and safe preview
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#define WIN32_NO_STATUS
#include <winsock2.h>
extern "C"
{
#include "reliefwin.h"
#include <tzlib.h>
#include <winioctl.h>
#include <reliefwin/drivers/volumecrypt.h>
#include "oobe.h"
}
#include <shellframe.h>
#include <iphlpapi.h>
#include <lm.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <reliefwin/desktopready.h>
#include <string>
#include <vector>
#include "resource.h"

using namespace ShellFrame;

namespace
{
enum Page { Language, Region, Device, Network, Account, Security, Apps, Review, Applying, Done };
enum Control { Next = 100, Back, Close, FirstLanguage = 110, Layout = 120, Zone, Daylight,
               Computer, User, Password, Confirm, NetworkSettings, Wifi, Encryption, Acknowledge, AppList,
               Activity, Progress, Item };
const UINT CompletedMessage = WM_APP + 10;
const UINT RefreshTimer = 1;
const UINT PreviewTimer = 2;

struct TimeZone
{
    ULONG Index;
    LONG Bias;
    bool Daylight;
    std::wstring Name;
};

LONG ReadZone(HKEY key, PVOID context)
{
    std::vector<TimeZone> *zones = static_cast<std::vector<TimeZone> *>(context);
    WCHAR name[192];
    REG_TZI_FORMAT information;
    ULONG index, size = sizeof(name);
    if (QueryTimeZoneData(key, &index, &information, name, &size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        zones->push_back({index, information.Bias, information.StandardDate.wMonth != 0, name});
    return ERROR_SUCCESS;
}

class Experience
{
public:
    Experience(PSETUPDATA data, BOOL preview, BOOL components) :
        Data(data), Preview(preview), Components(components), Current(components ? Apps : Language)
    {
    }

    int Run(int show)
    {
        WNDCLASSEXW cls = {sizeof(cls)};
        cls.lpfnWndProc = WindowProc;
        cls.hInstance = Data->hInstance;
        cls.hCursor = LoadCursorW(NULL, IDC_ARROW);
        cls.hIcon = LoadIconW(Data->hInstance, MAKEINTRESOURCEW(IDI_MAIN));
        cls.lpszClassName = L"ReliefWin.FirstRun";
        if (!RegisterClassExW(&cls) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return GetLastError();
        HMONITOR monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO info = {sizeof(info)};
        if (!GetMonitorInfoW(monitor, &info))
            return GetLastError();
        if (!CreateWindowExW(WS_EX_APPWINDOW | WS_EX_CONTROLPARENT, cls.lpszClassName, L"ReliefWin",
                             WS_POPUP | WS_CLIPCHILDREN, info.rcMonitor.left, info.rcMonitor.top,
                             info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top,
                             NULL, NULL, Data->hInstance, this))
            return GetLastError();
        ShowWindow(Window, show == SW_HIDE ? SW_SHOW : show);
        UpdateWindow(Window);
        if (!Preview)
        {
            HANDLE ready = OpenEventW(EVENT_MODIFY_STATE, FALSE, REACTOS_USER_DESKTOP_READY_EVENT);
            if (ready)
            {
                SetEvent(ready);
                CloseHandle(ready);
            }
        }
        MSG msg;
        int status;
        while ((status = GetMessageW(&msg, NULL, 0, 0)) > 0)
        {
            if (!IsDialogMessageW(Window, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        return status < 0 ? GetLastError() : static_cast<int>(msg.wParam);
    }

private:
    std::wstring Text(UINT id) const
    {
        WCHAR text[2048];
        int length = LoadStringW(Data->hInstance, id, text, ARRAYSIZE(text));
        return std::wstring(text, length);
    }

    int S(int value) const { return Style.Scale(value); }

    HWND ControlWindow(int id) const { return GetDlgItem(Window, id); }

    std::wstring Input(int id) const
    {
        WCHAR text[512];
        GetWindowTextW(ControlWindow(id), text, ARRAYSIZE(text));
        return text;
    }

    HWND Add(PCWSTR cls, const std::wstring &text, DWORD style, int id, int x, int y, int width, int height,
             TextStyle font = TextStyle::Body)
    {
        HWND child = CreateWindowExW(0, cls, text.c_str(), WS_CHILD | WS_VISIBLE | style,
                                     Content.left + S(x), Content.top + S(y), S(width), S(height),
                                     Window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), Data->hInstance, NULL);
        if (child)
        {
            if (IsThemeActive() && !Style.IsHighContrast())
                SetWindowTheme(child, L"ContentLight", NULL);
            SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(Style.Font(font)), FALSE);
            Children.push_back(child);
        }
        return child;
    }

    HWND Label(UINT text, int y, int height = 28)
    {
        return Add(L"STATIC", Text(text), SS_NOPREFIX, -1, 0, y, ContentWidth, height);
    }

    HWND Edit(int id, PCWSTR value, int y, int limit, DWORD extra = 0)
    {
        HWND edit = Add(L"EDIT", value, WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | extra,
                         id, 0, y, ContentWidth, 36);
        SendMessageW(edit, EM_LIMITTEXT, limit, 0);
        return edit;
    }

    HWND Button(UINT text, int id, int x, int y, int width = 170, int height = 40)
    {
        return Add(L"BUTTON", Text(text), WS_TABSTOP | BS_OWNERDRAW, id, x, y, width, height);
    }

    void LayoutWindow()
    {
        RECT client;
        GetClientRect(Window, &client);
        HDC dc = GetDC(Window);
        UINT dpi = dc ? static_cast<UINT>(GetDeviceCaps(dc, LOGPIXELSX)) : 96;
        if (dc) ReleaseDC(Window, dc);
        dpi = min(dpi, static_cast<UINT>(max(64, min(client.right * 96 / 960, client.bottom * 96 / 772))));
        Style.SetDpi(dpi);
        int width = min(S(1120), client.right - S(32));
        int height = min(S(740), client.bottom - S(32));
        Card = {(client.right - width) / 2, (client.bottom - height) / 2,
                (client.right + width) / 2, (client.bottom + height) / 2};
        LeftWidth = min(S(320), width / 3);
        Content = {Card.left + LeftWidth + S(48), Card.top + S(44), Card.right - S(48), Card.bottom - S(42)};
        ContentWidth = MulDiv(Content.right - Content.left, 96, Style.Dpi());
    }

    void Clear()
    {
        for (HWND child : Children)
            DestroyWindow(child);
        Children.clear();
    }

    void Remember()
    {
        if (Current == Region)
        {
            LRESULT choice = SendMessageW(ControlWindow(Layout), CB_GETCURSEL, 0, 0);
            if (choice >= 0 && static_cast<ULONG>(choice) < Data->LayoutCount)
                StringCchCopyW(Data->DefaultKBLayout, ARRAYSIZE(Data->DefaultKBLayout), Data->Layouts[choice].Id);
            choice = SendMessageW(ControlWindow(Zone), CB_GETCURSEL, 0, 0);
            if (choice >= 0 && static_cast<size_t>(choice) < Zones.size())
            {
                Data->TimeZoneIndex = Zones[choice].Index;
                StringCchCopyW(Data->TimeZoneName, ARRAYSIZE(Data->TimeZoneName), Zones[choice].Name.c_str());
            }
            Data->AutoDaylight = SendMessageW(ControlWindow(Daylight), BM_GETCHECK, 0, 0) == BST_CHECKED;
        }
        if (Current == Device)
            StringCchCopyW(Data->ComputerName, ARRAYSIZE(Data->ComputerName), Input(Computer).c_str());
        if (Current == Account)
        {
            StringCchCopyW(Data->UserName, ARRAYSIZE(Data->UserName), Input(User).c_str());
            GetWindowTextW(ControlWindow(Password), Data->Password, ARRAYSIZE(Data->Password));
            GetWindowTextW(ControlWindow(Confirm), Confirmation, ARRAYSIZE(Confirmation));
        }
        if (Current == Security)
            SecurityAccepted = SendMessageW(ControlWindow(Acknowledge), BM_GETCHECK, 0, 0) == BST_CHECKED;
        if (Current == Apps)
            for (ULONG i = 0; i < OobeAppCount; ++i)
                OobeApps[i].Selected = ListView_GetCheckState(ControlWindow(AppList), i);
    }

    UINT Title() const
    {
        const UINT ids[] = {IDS_EXPERIENCE_LANGUAGE, IDS_EXPERIENCE_REGION, IDS_EXPERIENCE_DEVICE,
                            IDS_EXPERIENCE_NETWORK, IDS_EXPERIENCE_ACCOUNT, IDS_EXPERIENCE_SECURITY,
                            IDS_EXPERIENCE_APPS, IDS_EXPERIENCE_REVIEW, IDS_EXPERIENCE_APPLYING,
                            IDS_EXPERIENCE_DONE};
        return ids[Current];
    }

    UINT Description() const
    {
        const UINT ids[] = {IDS_EXPERIENCE_LANGUAGE_DESC, IDS_EXPERIENCE_REGION_DESC, IDS_EXPERIENCE_DEVICE_DESC,
                            IDS_EXPERIENCE_NETWORK_DESC, IDS_EXPERIENCE_ACCOUNT_DESC, IDS_EXPERIENCE_SECURITY_DESC,
                            IDS_EXPERIENCE_APPS_DESC, IDS_EXPERIENCE_REVIEW_DESC, IDS_EXPERIENCE_APPLYING_DESC,
                            IDS_EXPERIENCE_DONE_DESC};
        if (Current == Done && Preview) return IDS_EXPERIENCE_PREVIEW_DONE;
        if (Current == Done && Components) return IDS_EXPERIENCE_DONE_COMPONENTS;
        if (Current == Done && Data->bMustReboot) return IDS_EXPERIENCE_DONE_RESTART;
        return ids[Current];
    }

    void UpdateZone()
    {
        LRESULT choice = SendMessageW(ControlWindow(Zone), CB_GETCURSEL, 0, 0);
        BOOL daylight = choice >= 0 && static_cast<size_t>(choice) < Zones.size() && Zones[choice].Daylight;
        EnableWindow(ControlWindow(Daylight), daylight);
        SendMessageW(ControlWindow(Daylight), BM_SETCHECK, daylight && Data->AutoDaylight ? BST_CHECKED : BST_UNCHECKED, 0);
    }

    void Rebuild()
    {
        Clear();
        LayoutWindow();
        if (Current == Language)
        {
            static const PCWSTR names[] = {L"简体中文", L"English (United States)", L"日本語"};
            for (int i = 0; i < 3; ++i)
                Add(L"BUTTON", names[i], BS_OWNERDRAW | WS_TABSTOP, FirstLanguage + i,
                     0, 140 + i * 76, ContentWidth, 60);
        }
        else if (Current == Region)
        {
            bool found = false;
            for (const TimeZone &zone : Zones)
                if (zone.Index == Data->TimeZoneIndex) found = true;
            if (!found && !Zones.empty())
            {
                TIME_ZONE_INFORMATION current;
                Data->TimeZoneIndex = Zones.front().Index;
                if (GetTimeZoneInformation(&current) != TIME_ZONE_ID_INVALID)
                    for (const TimeZone &zone : Zones)
                        if (zone.Bias == current.Bias)
                        {
                            Data->TimeZoneIndex = zone.Index;
                            if (!zone.Daylight) break;
                        }
            }
            Label(IDS_EXPERIENCE_KEYBOARD, 130);
            HWND layout = Add(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                              Layout, 0, 164, ContentWidth, 240);
            for (ULONG i = 0; i < Data->LayoutCount; ++i)
            {
                SendMessageW(layout, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Data->Layouts[i].Name));
                if (!_wcsicmp(Data->DefaultKBLayout, Data->Layouts[i].Id))
                    SendMessageW(layout, CB_SETCURSEL, i, 0);
            }
            Label(IDS_EXPERIENCE_TIMEZONE, 238);
            HWND zone = Add(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                            Zone, 0, 272, ContentWidth, 240);
            for (size_t i = 0; i < Zones.size(); ++i)
            {
                SendMessageW(zone, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Zones[i].Name.c_str()));
                if (Data->TimeZoneIndex == Zones[i].Index)
                    SendMessageW(zone, CB_SETCURSEL, i, 0);
            }
            Add(L"BUTTON", Text(IDS_EXPERIENCE_DAYLIGHT), BS_AUTOCHECKBOX | WS_TABSTOP,
                Daylight, 0, 332, ContentWidth, 34);
            UpdateZone();
        }
        else if (Current == Device)
        {
            Label(IDS_EXPERIENCE_DEVICE_NAME, 150);
            Edit(Computer, Data->ComputerName, 188, MAX_COMPUTERNAME_LENGTH, ES_UPPERCASE);
            Label(IDS_EXPERIENCE_DEVICE_HINT, 248, 90);
        }
        else if (Current == Network)
        {
            Add(L"STATIC", NetworkText(), SS_NOPREFIX, Item, 0, 134, ContentWidth, 220);
            Button(IDS_EXPERIENCE_WIFI, Wifi, 0, 380, ContentWidth);
            Button(IDS_EXPERIENCE_NETWORK_SETTINGS, NetworkSettings, 0, 436, ContentWidth);
        }
        else if (Current == Account)
        {
            Label(IDS_EXPERIENCE_USER, 130);
            Edit(User, Data->UserName, 162, 20);
            if (!Preview && OobeOwnsAccount(Data->UserName)) EnableWindow(ControlWindow(User), FALSE);
            Label(IDS_EXPERIENCE_PASSWORD, 220);
            Edit(Password, Data->Password, 252, 127, ES_PASSWORD);
            Label(IDS_EXPERIENCE_CONFIRM, 310);
            Edit(Confirm, Confirmation, 342, 127, ES_PASSWORD);
            Label(IDS_EXPERIENCE_PASSWORD_HINT, 402, 52);
        }
        else if (Current == Security)
        {
            DWORD flags, error = OobeEncryptionState(&flags);
            UINT state = error != ERROR_SUCCESS ? IDS_EXPERIENCE_CRYPT_UNKNOWN :
                         (flags & RW_CRYPT_FLAG_CONFIGURED) ? IDS_EXPERIENCE_ENCRYPTED : IDS_EXPERIENCE_PLAIN;
            Label(state, 142, 40);
            Label(IDS_EXPERIENCE_CRYPT_NOTE, 196, 142);
            Button(IDS_EXPERIENCE_CRYPT_OPEN, Encryption, 0, 350, ContentWidth);
            HWND confirm = Add(L"BUTTON", Text(IDS_EXPERIENCE_CRYPT_ACK), BS_AUTOCHECKBOX | BS_MULTILINE | WS_TABSTOP,
                               Acknowledge, 0, 414, ContentWidth, 58);
            SendMessageW(confirm, BM_SETCHECK, SecurityAccepted ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        else if (Current == Apps)
        {
            HWND list = Add(WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP | WS_BORDER,
                            AppList, 0, 148, ContentWidth, 280);
            ListView_SetExtendedListViewStyle(list, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
            LVCOLUMNW column = {0};
            column.mask = LVCF_TEXT | LVCF_WIDTH;
            std::wstring name = Text(IDS_EXPERIENCE_APP_NAME);
            column.pszText = const_cast<PWSTR>(name.c_str());
            column.cx = S(ContentWidth);
            ListView_InsertColumn(list, 0, &column);
            for (ULONG i = 0; i < OobeAppCount; ++i)
            {
                LVITEMW row = {0};
                row.mask = LVIF_TEXT;
                row.iItem = i;
                row.pszText = OobeApps[i].Name;
                ListView_InsertItem(list, &row);
                ListView_SetCheckState(list, i, OobeApps[i].Selected);
            }
        }
        else if (Current == Review)
        {
            WCHAR summary[2048];
            PCWSTR language = L"简体中文", keyboard = Data->DefaultKBLayout;
            ULONG id = wcstoul(Data->DefaultLanguage, NULL, 16);
            if (id == 0x0409) language = L"English (United States)";
            if (id == 0x0411) language = L"日本語";
            for (ULONG i = 0; i < Data->LayoutCount; ++i)
                if (!_wcsicmp(Data->DefaultKBLayout, Data->Layouts[i].Id)) keyboard = Data->Layouts[i].Name;
            StringCchPrintfW(summary, ARRAYSIZE(summary), Text(IDS_EXPERIENCE_SUMMARY).c_str(),
                             language, keyboard, Data->TimeZoneName, Data->ComputerName, Data->UserName);
            Add(L"STATIC", summary, SS_NOPREFIX, -1, 0, 144, ContentWidth, 300);
        }
        else if (Current == Applying)
        {
            Label(IDS_EXPERIENCE_BUSY, 180, 52);
            Add(L"STATIC", L"", SS_NOPREFIX, Activity, 0, 252, ContentWidth, 56);
            Add(PROGRESS_CLASSW, L"", PBS_MARQUEE, Progress, 0, 330, ContentWidth, 6);
            Add(L"STATIC", L"", SS_NOPREFIX, Item, 0, 354, ContentWidth, 98);
            SendMessageW(ControlWindow(Progress), PBM_SETMARQUEE, TRUE, 40);
        }
        else if (Current == Done)
        {
            Label(Preview ? IDS_EXPERIENCE_PREVIEW_DONE : Components ? IDS_EXPERIENCE_DONE_COMPONENTS :
                  Data->bMustReboot ? IDS_EXPERIENCE_DONE_RESTART : IDS_EXPERIENCE_DONE_DESC, 230, 150);
        }
        int footer = MulDiv(Content.bottom - Content.top, 96, Style.Dpi()) - 40;
        if (Current != Applying)
        {
            if (Current != Language && Current != Done && !(Components && Current == Apps))
                Button(IDS_EXPERIENCE_BACK, Back, 0, footer, 112);
            UINT text = Current == Done ? (Preview || Components ? IDS_EXPERIENCE_CLOSE :
                        Data->bMustReboot ? IDS_EXPERIENCE_RESTART : IDS_EXPERIENCE_LOGIN) :
                        Current == Review || (Components && Current == Apps) ? IDS_EXPERIENCE_APPLY :
                        Current == Network && !HasNetwork ? IDS_EXPERIENCE_OFFLINE : IDS_EXPERIENCE_NEXT;
            Button(text, Next, ContentWidth - 188, footer, 188);
            if (Current != Done)
                Add(L"BUTTON", Text(IDS_EXPERIENCE_CLOSE), BS_OWNERDRAW | WS_TABSTOP,
                    Close, -LeftWidth * 96 / static_cast<int>(Style.Dpi()) - 16, footer, 128, 40);
        }
        HWND focus = ControlWindow(Current == Language ? FirstLanguage : Current == Device ? Computer :
                                    Current == Account ? User : Next);
        if (focus) SetFocus(focus);
        InvalidateRect(Window, NULL, TRUE);
    }

    std::wstring NetworkText()
    {
        ULONG bytes = 16 * 1024;
        std::vector<BYTE> buffer(bytes);
        DWORD result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                                              GAA_FLAG_SKIP_DNS_SERVER, NULL,
                                              reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &bytes);
        if (result == ERROR_BUFFER_OVERFLOW && bytes <= 1024 * 1024)
        {
            buffer.resize(bytes);
            result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                                          GAA_FLAG_SKIP_DNS_SERVER, NULL,
                                          reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &bytes);
        }
        HasNetwork = FALSE;
        std::wstring status;
        if (result == NO_ERROR)
        {
            for (PIP_ADAPTER_ADDRESSES adapter = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
                 adapter; adapter = adapter->Next)
            {
                if (adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK || adapter->IfType == IF_TYPE_TUNNEL)
                    continue;
                BOOL connected = adapter->OperStatus == IfOperStatusUp && adapter->FirstUnicastAddress;
                HasNetwork = HasNetwork || connected;
                if (status.size() < 1000)
                {
                    status += adapter->FriendlyName ? adapter->FriendlyName : L"";
                    status += L"\n" + Text(connected ? IDS_EXPERIENCE_NETWORK_LOCAL : IDS_EXPERIENCE_NETWORK_OFF) + L"\n\n";
                }
            }
        }
        if (status.empty()) status = Text(IDS_EXPERIENCE_NETWORK_NONE);
        return status;
    }

    void Launch(int id)
    {
        if (Preview)
        {
            Error = Text(IDS_EXPERIENCE_PREVIEW_NETWORK);
            InvalidateRect(Window, NULL, FALSE);
            return;
        }
        WCHAR root[MAX_PATH], path[MAX_PATH], line[2 * MAX_PATH];
        if (!GetWindowsDirectoryW(root, ARRAYSIZE(root))) return;
        PCWSTR relative = id == Encryption ? L"system32\\diskcrypt.exe" : L"ImmersiveControlPanel\\SystemSettings.exe";
        PCWSTR argument = id == Wifi ? L"ms-settings:network-wifi" : id == NetworkSettings ? L"ms-settings:network" : L"";
        if (FAILED(StringCchPrintfW(path, ARRAYSIZE(path), L"%s\\%s", root, relative)) ||
            FAILED(StringCchPrintfW(line, ARRAYSIZE(line), L"\"%s\" %s", path, argument))) return;
        STARTUPINFOW startup = {sizeof(startup)};
        PROCESS_INFORMATION process;
        if (CreateProcessW(path, line, NULL, NULL, FALSE, 0, NULL, NULL, &startup, &process))
        {
            CloseHandle(process.hThread);
            CloseHandle(process.hProcess);
        }
        else ShowError(GetLastError());
    }

    void ShowError(DWORD code)
    {
        WCHAR system[512] = L"", number[32];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                       code, 0, system, ARRAYSIZE(system), NULL);
        StringCchPrintfW(number, ARRAYSIZE(number), L" (%lu)", code);
        Error = Text(IDS_EXPERIENCE_ERROR_APPLY) + L" " + system + number;
        InvalidateRect(Window, NULL, FALSE);
    }

    bool Validate()
    {
        UINT error = 0;
        if (Current == Region && (Zones.empty() || !ControlWindow(Layout) ||
            SendMessageW(ControlWindow(Layout), CB_GETCURSEL, 0, 0) == CB_ERR ||
            SendMessageW(ControlWindow(Zone), CB_GETCURSEL, 0, 0) == CB_ERR))
        {
            ShowError(ERROR_NOT_FOUND);
            return false;
        }
        if (Current == Device && !IsValidSetupComputerName(Data->ComputerName)) error = IDS_EXPERIENCE_ERROR_DEVICE;
        if (Current == Account)
        {
            if (!IsValidSetupUserName(Data->UserName) || !_wcsicmp(Data->UserName, Data->ComputerName))
                error = IDS_EXPERIENCE_ERROR_USER;
            else if (!Preview && !OobeOwnsAccount(Data->UserName))
            {
                LPBYTE info = NULL;
                NET_API_STATUS status = NetUserGetInfo(NULL, Data->UserName, 0, &info);
                if (info) NetApiBufferFree(info);
                if (status == NERR_Success) error = IDS_EXPERIENCE_ERROR_EXISTS;
                else if (status != NERR_UserNotFound) { ShowError(status); return false; }
            }
            if (!error && (!Data->Password[0] || wcscmp(Confirmation, Data->Password))) error = IDS_EXPERIENCE_ERROR_PASSWORD;
        }
        if (Current == Security && !SecurityAccepted) error = IDS_EXPERIENCE_ERROR_CRYPTO;
        if (error)
        {
            Error = Text(error);
            InvalidateRect(Window, NULL, FALSE);
            return false;
        }
        Error.clear();
        return true;
    }

    static DWORD WINAPI Work(void *context)
    {
        Experience *self = static_cast<Experience *>(context);
        DWORD result = OobeApply(self->Data, &self->Status, self->Components);
        PostMessageW(self->Window, CompletedMessage, result, 0);
        return result;
    }

    void Apply()
    {
        Current = Applying;
        Rebuild();
        if (Preview)
        {
            SetTimer(Window, PreviewTimer, 1200, NULL);
            return;
        }
        Status = {ControlWindow(Activity), ControlWindow(Item), ControlWindow(Progress)};
        Worker = CreateThread(NULL, 0, Work, this, 0, NULL);
        if (!Worker)
        {
            DWORD error = GetLastError();
            Current = Components ? Apps : Account;
            Rebuild();
            ShowError(error);
        }
    }

    void Forward()
    {
        if (Current == Applying) return;
        if (Current == Done)
        {
            if (!Preview)
            {
                DWORD error = OobeComplete(Components);
                if (error != ERROR_SUCCESS) { ShowError(error); return; }
                if (!Components)
                {
                    error = OobeFinishSession(Data->bMustReboot);
                    if (error != ERROR_SUCCESS) { ShowError(error); return; }
                }
            }
            DestroyWindow(Window);
            return;
        }
        Remember();
        if (!Validate()) return;
        if (Current == Review || (Components && Current == Apps)) { Apply(); return; }
        Current = static_cast<Page>(Current + 1);
        Rebuild();
    }

    void Paint()
    {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(Window, &paint);
        {
        RECT client;
        GetClientRect(Window, &client);
        Canvas canvas(dc, client);
        const Palette &colors = Style.Colors();
        canvas.Fill(client, Style.IsHighContrast() ? colors.Window : RGB(232, 241, 253));
        canvas.FillRound(Card, static_cast<float>(S(16)), colors.Card);
        canvas.FrameRound(Card, static_cast<float>(S(16)), colors.CardBorder);
        RECT side = {Card.left + S(16), Card.top + S(16), Card.left + LeftWidth - S(8), Card.bottom - S(16)};
        canvas.FillRound(side, static_cast<float>(S(12)), Style.IsHighContrast() ? colors.Card : RGB(242, 247, 254));
        DrawIconEx(canvas.Dc(), side.left + (side.right - side.left - S(128)) / 2, side.top + S(62),
                   Logo, S(128), S(128), 0, NULL, DI_NORMAL);
        RECT brand = {side.left + S(16), side.top + S(212), side.right - S(16), side.top + S(252)};
        canvas.Text(L"ReliefWin", brand, Style.Font(TextStyle::Title), colors.Text, DT_CENTER | DT_SINGLELINE);
        RECT caption = {side.left + S(16), brand.bottom + S(12), side.right - S(16), brand.bottom + S(62)};
        canvas.Text(Text(IDS_EXPERIENCE_STEPS).c_str(), caption, Style.Font(TextStyle::Body), colors.TextSecondary,
                    DT_CENTER | DT_WORDBREAK);
        static const PCWSTR icons[] = {L"globe_20_regular", L"keyboard_20_regular", L"desktop_20_regular",
            L"globe_20_regular", L"person_20_regular", L"shield_checkmark_20_regular", L"apps_list_20_regular",
            L"checkmark_circle_20_regular", L"checkmark_circle_20_regular", L"checkmark_circle_20_regular"};
        RECT glyph = {side.left + (side.right - side.left - S(48)) / 2, caption.bottom + S(52), 0, 0};
        glyph.right = glyph.left + S(48); glyph.bottom = glyph.top + S(48);
        canvas.Icon(icons[Current], glyph, colors.Accent);
        RECT title = {Content.left, Content.top, Content.right, Content.top + S(48)};
        canvas.Text(Text(Title()).c_str(), title, Style.Font(TextStyle::Title), colors.Text, DT_WORDBREAK);
        RECT description = {Content.left, title.bottom + S(12), Content.right, Content.top + S(126)};
        canvas.Text(Text(Description()).c_str(), description, Style.Font(TextStyle::Body), colors.TextSecondary, DT_WORDBREAK);
        if (!Error.empty())
        {
            RECT error = {Content.left, Content.bottom - S(128), Content.right, Content.bottom - S(52)};
            canvas.Text(Error.c_str(), error, Style.Font(TextStyle::Caption), colors.Text, DT_WORDBREAK);
        }
        RECT note = {Card.left + S(20), Card.bottom + S(8), Card.right - S(20), client.bottom};
        if (Preview)
            canvas.Text(Text(IDS_EXPERIENCE_PREVIEW).c_str(), note, Style.Font(TextStyle::Caption), colors.TextSecondary, DT_CENTER);
        const int dotCount = 8;
        const int dotSize = S(8);
        const int dotSpacing = S(24);
        const int dotsWidth = (dotCount - 1) * dotSpacing + dotSize;
        const int dotsLeft = side.left + (side.right - side.left - dotsWidth) / 2;
        for (int i = 0; i < dotCount; ++i)
        {
            int left = dotsLeft + i * dotSpacing;
            RECT dot = {left, side.bottom - S(92), left + dotSize, side.bottom - S(92) + dotSize};
            canvas.FillEllipse(dot, i <= min(static_cast<int>(Current), dotCount - 1) ? colors.Accent : colors.Divider);
        }
        }
        EndPaint(Window, &paint);
    }

    void PaintButton(DRAWITEMSTRUCT *draw)
    {
        Canvas canvas(draw->hDC, draw->rcItem);
        const Palette &colors = Style.Colors();
        int id = static_cast<int>(draw->CtlID);
        BOOL primary = id == Next;
        BOOL selected = FALSE;
        if (id >= FirstLanguage && id < FirstLanguage + 3)
        {
            ULONG language = wcstoul(Data->DefaultLanguage, NULL, 16);
            const ULONG languages[] = {0x0804, 0x0409, 0x0411};
            selected = language == languages[id - FirstLanguage];
        }
        COLORREF fill = primary ? colors.Accent : selected ? colors.Selected : colors.ControlFill;
        if (draw->itemState & ODS_SELECTED) fill = colors.SubtlePressed;
        canvas.Fill(draw->rcItem, colors.Card);
        canvas.FillRound(draw->rcItem, static_cast<float>(S(6)), fill);
        canvas.FrameRound(draw->rcItem, static_cast<float>(S(6)), selected ? colors.Accent : colors.ControlStroke);
        WCHAR text[256];
        GetWindowTextW(draw->hwndItem, text, ARRAYSIZE(text));
        RECT box = draw->rcItem;
        InflateRect(&box, -S(14), 0);
        canvas.Text(text, box, Style.Font(TextStyle::Body), primary ? colors.AccentText : colors.Text,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        if (draw->itemState & ODS_FOCUS)
        {
            RECT focus = draw->rcItem;
            InflateRect(&focus, -S(3), -S(3));
            canvas.FrameRound(focus, static_cast<float>(S(4)), primary ? colors.AccentText : colors.Accent);
        }
    }

    LRESULT Message(UINT message, WPARAM wparam, LPARAM lparam)
    {
        switch (message)
        {
            case WM_CREATE:
                Style.SetDarkPreference(0);
                Style.Update(Window);
                Brush = CreateSolidBrush(Style.Colors().Card);
                Logo = static_cast<HICON>(LoadImageW(Data->hInstance, MAKEINTRESOURCEW(IDI_MAIN), IMAGE_ICON,
                                                    S(128), S(128), 0));
                EnumerateTimeZoneList(ReadZone, &Zones);
                Rebuild();
                SetTimer(Window, RefreshTimer, 3000, NULL);
                return 0;
            case WM_SIZE:
                if (!Children.empty() && Current != Applying) { Remember(); Rebuild(); }
                return 0;
            case WM_PAINT: Paint(); return 0;
            case WM_ERASEBKGND: return 1;
            case WM_DRAWITEM: PaintButton(reinterpret_cast<DRAWITEMSTRUCT *>(lparam)); return TRUE;
            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLORBTN:
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORLISTBOX:
                SetTextColor(reinterpret_cast<HDC>(wparam), Style.Colors().Text);
                SetBkColor(reinterpret_cast<HDC>(wparam), Style.Colors().Card);
                return reinterpret_cast<LRESULT>(Brush);
            case WM_COMMAND:
            {
                int id = LOWORD(wparam);
                if (id == Next || id == IDOK) { Forward(); return 0; }
                if (id == Back && Current > Language && Current < Applying && !(Components && Current == Apps))
                { Remember(); Current = static_cast<Page>(Current - 1); Error.clear(); Rebuild(); return 0; }
                if (id == Close || id == IDCANCEL) { SendMessageW(Window, WM_CLOSE, 0, 0); return 0; }
                if (id >= FirstLanguage && id < FirstLanguage + 3)
                {
                    PCWSTR languages[] = {L"00000804", L"00000409", L"00000411"};
                    StringCchCopyW(Data->DefaultLanguage, ARRAYSIZE(Data->DefaultLanguage), languages[id - FirstLanguage]);
                    SetSetupLanguage(Data->DefaultLanguage);
                    Rebuild();
                    return 0;
                }
                if (id == Zone && HIWORD(wparam) == CBN_SELCHANGE) { Data->AutoDaylight = TRUE; UpdateZone(); return 0; }
                if (id == Layout && HIWORD(wparam) == CBN_SELCHANGE)
                {
                    Remember();
                    LoadKeyboardLayoutW(Data->DefaultKBLayout, KLF_ACTIVATE | KLF_SETFORPROCESS);
                    return 0;
                }
                if (id == NetworkSettings || id == Wifi || id == Encryption) { Launch(id); return 0; }
                break;
            }
            case WM_TIMER:
                if (wparam == PreviewTimer)
                {
                    KillTimer(Window, PreviewTimer);
                    SecureZeroMemory(Data->Password, sizeof(Data->Password));
                    Current = Done; Rebuild();
                }
                else if (wparam == RefreshTimer && Current == Network)
                {
                    SetWindowTextW(ControlWindow(Item), NetworkText().c_str());
                    SetWindowTextW(ControlWindow(Next), Text(HasNetwork ? IDS_EXPERIENCE_NEXT : IDS_EXPERIENCE_OFFLINE).c_str());
                }
                return 0;
            case CompletedMessage:
                if (Worker) { WaitForSingleObject(Worker, INFINITE); CloseHandle(Worker); Worker = NULL; }
                SecureZeroMemory(Confirmation, sizeof(Confirmation));
                Current = wparam == ERROR_SUCCESS ? Done : Components ? Apps : Account;
                Rebuild();
                if (wparam != ERROR_SUCCESS) ShowError(static_cast<DWORD>(wparam));
                return 0;
            case DM_GETDEFID: return MAKELONG(Next, DC_HASDEFID);
            case WM_CLOSE:
                if (Current == Applying) return 0;
                if (!Preview && Current != Done && MessageBoxW(Window, Text(IDS_EXPERIENCE_QUIT_DESC).c_str(),
                    Text(IDS_EXPERIENCE_QUIT).c_str(), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES) return 0;
                DestroyWindow(Window);
                return 0;
            case WM_QUERYENDSESSION:
                return Current != Applying;
            case WM_DESTROY:
                KillTimer(Window, RefreshTimer);
                KillTimer(Window, PreviewTimer);
                if (Logo) DestroyIcon(Logo);
                if (Brush) DeleteObject(Brush);
                SecureZeroMemory(Data->Password, sizeof(Data->Password));
                SecureZeroMemory(Confirmation, sizeof(Confirmation));
                PostQuitMessage(Current == Done ? ERROR_SUCCESS : ERROR_CANCELLED);
                return 0;
        }
        return DefWindowProcW(Window, message, wparam, lparam);
    }

    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
    {
        Experience *self = reinterpret_cast<Experience *>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (message == WM_NCCREATE)
        {
            self = static_cast<Experience *>(reinterpret_cast<CREATESTRUCTW *>(lparam)->lpCreateParams);
            self->Window = window;
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        return self ? self->Message(message, wparam, lparam) : DefWindowProcW(window, message, wparam, lparam);
    }

    PSETUPDATA Data;
    BOOL Preview, Components;
    Page Current;
    HWND Window = NULL;
    HANDLE Worker = NULL;
    HBRUSH Brush = NULL;
    HICON Logo = NULL;
    Theme Style;
    RECT Card = {}, Content = {};
    int LeftWidth = 0, ContentWidth = 0;
    BOOL HasNetwork = FALSE, SecurityAccepted = FALSE;
    OOBE_PROGRESS Status = {};
    std::vector<HWND> Children;
    std::vector<TimeZone> Zones;
    std::wstring Error;
    WCHAR Confirmation[128] = {};
};
}

extern "C" int
RunOobeExperience(PSETUPDATA data, BOOL preview, BOOL components, int show)
{
    if (!Startup()) return ERROR_DLL_INIT_FAILED;
    int result;
    {
        Experience experience(data, preview, components);
        result = experience.Run(show);
    }
    Shutdown();
    return result;
}
