/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Disk encryption management with background conversion and safe pause
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"
#include "resource.h"
#include <commctrl.h>
#include <commdlg.h>

#define DC_DONE (WM_APP + 1)
#define DC_PROGRESS_MESSAGE (WM_APP + 2)

enum { ActionEncrypt, ActionDecrypt, ActionResume, ActionUnlock, ActionLock,
       ActionPassword, ActionRecovery, ActionBackup, ActionRestore };

static HINSTANCE Instance;
static DC_VOLUME Volumes[DC_MAX_VOLUMES];
static DC_STORE Store;
static DWORD VolumeCount;
static HANDLE Worker;
static volatile LONG Cancel;
static BOOL Ready;

typedef struct DC_JOB
{
    HWND Dialog;
    DWORD Action;
    DC_VOLUME Volume;
    WCHAR Path[DC_PATH];
    char Secret[513];
    char New[513];
    char Recovery[65];
} DC_JOB;

static PCWSTR Text(UINT Id)
{
    static WCHAR Values[8][1024];
    static unsigned Slot;
    PWSTR Value = Values[Slot++ % _countof(Values)];
    Value[0] = 0;
    LoadStringW(Instance, Id, Value, _countof(Values[0]));
    return Value;
}

static void Status(HWND Dialog, DWORD Error)
{
    WCHAR System[256] = L"", Message[768];
    if (!Error)
        SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_OK));
    else if (Error == ERROR_OPERATION_ABORTED)
        SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_PAUSED));
    else
    {
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, Error, 0,
                       System, _countof(System), NULL);
        StringCchPrintfW(Message, _countof(Message), Text(IDS_ERROR), Error, System);
        SetDlgItemTextW(Dialog, IDC_STATUS, Message);
    }
}

static void Fields(HWND Dialog)
{
    LRESULT Action = SendDlgItemMessageW(Dialog, IDC_ACTION, CB_GETCURSEL, 0, 0);
    BOOL New = Action == ActionEncrypt || Action == ActionPassword;
    BOOL Secret = Action != ActionEncrypt && Action != ActionLock;
    EnableWindow(GetDlgItem(Dialog, IDC_NEW), !Worker && New);
    EnableWindow(GetDlgItem(Dialog, IDC_CONFIRM), !Worker && New);
    EnableWindow(GetDlgItem(Dialog, IDC_SECRET), !Worker && Secret);
    EnableWindow(GetDlgItem(Dialog, IDC_GO), !Worker && Ready);
    EnableWindow(GetDlgItem(Dialog, IDC_PAUSE), Worker != NULL);
}

static void Busy(HWND Dialog, BOOL Busy)
{
    const UINT Controls[] = {IDC_BOOT, IDC_VOLUME, IDC_ACTION, IDC_GO, IDC_REFRESH, IDCANCEL,
                             IDC_SECRET, IDC_NEW, IDC_CONFIRM};
    unsigned i;
    for (i = 0; i != _countof(Controls); ++i)
        EnableWindow(GetDlgItem(Dialog, Controls[i]), !Busy);
    Fields(Dialog);
}

static void LoadStore(HWND Dialog)
{
    LRESULT Selection = SendDlgItemMessageW(Dialog, IDC_BOOT, CB_GETCURSEL, 0, 0);
    DWORD Error;
    Ready = FALSE;
    memset(&Store, 0, sizeof(Store));
    if (Selection == CB_ERR)
    {
        SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_NO_VOLUMES));
        Fields(Dialog);
        return;
    }
    Selection = SendDlgItemMessageW(Dialog, IDC_BOOT, CB_GETITEMDATA, Selection, 0);
    StringCchCopyW(Store.Root, DC_PATH, Volumes[Selection].Root);
    Error = DcReadStore(&Store);
    if (!Error) Error = DcBindVolumes(&Store, Volumes, VolumeCount);
    Ready = Error == ERROR_SUCCESS;
    Status(Dialog, Error);
    Fields(Dialog);
}

static void Selection(HWND Dialog)
{
    LRESULT Selected = SendDlgItemMessageW(Dialog, IDC_VOLUME, CB_GETCURSEL, 0, 0);
    UINT State;
    WCHAR Message[768];
    DC_VOLUME *Volume;
    if (Selected == CB_ERR || !Ready) return;
    Volume = Volumes + Selected;
    State = Volume->Flags & RW_CRYPT_FLAG_OFFLINE ? IDS_OFFLINE :
            !(Volume->Flags & RW_CRYPT_FLAG_CONFIGURED) ? IDS_PLAIN :
            Volume->Flags & RW_CRYPT_FLAG_UNLOCKED ? IDS_OPEN : IDS_LOCKED;
    StringCchPrintfW(Message, _countof(Message), L"%s%s%s", Text(State),
                     Volume->System || Volume->Running ? L" · " : L"",
                     Volume->Running ? Text(IDS_RUNNING_SYSTEM) : Volume->System ? Text(IDS_SYSTEM) : L"");
    SetDlgItemTextW(Dialog, IDC_STATUS, Message);
}

static void Refresh(HWND Dialog)
{
    WCHAR Label[768], Paths[DC_PATH];
    DWORD i, Required, RunningDisk = MAXDWORD, Error;
    LRESULT Item, Best = 0;
    SendDlgItemMessageW(Dialog, IDC_BOOT, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessageW(Dialog, IDC_VOLUME, CB_RESETCONTENT, 0, 0);
    Error = DcEnumerate(Volumes, &VolumeCount);
    if (Error)
    {
        Ready = FALSE;
        Status(Dialog, Error);
        Fields(Dialog);
        return;
    }
    for (i = 0; i < VolumeCount; ++i)
        if (Volumes[i].Running) RunningDisk = Volumes[i].Disk;
    for (i = 0; i < VolumeCount; ++i)
    {
        Paths[0] = 0;
        GetVolumePathNamesForVolumeNameW(Volumes[i].Root, Paths, DC_PATH, &Required);
        StringCchPrintfW(Label, _countof(Label), L"%s %s  |  Disk %lu / %lu  |  %I64u MiB",
                         Paths, Volumes[i].Label, Volumes[i].Disk, Volumes[i].Partition.PartitionNumber,
                         (ULONGLONG)Volumes[i].Partition.PartitionLength.QuadPart >> 20);
        SendDlgItemMessageW(Dialog, IDC_VOLUME, CB_ADDSTRING, 0, (LPARAM)Label);
        if (Volumes[i].Boot)
        {
            Item = SendDlgItemMessageW(Dialog, IDC_BOOT, CB_ADDSTRING, 0, (LPARAM)Label);
            SendDlgItemMessageW(Dialog, IDC_BOOT, CB_SETITEMDATA, Item, i);
            if (Volumes[i].Disk == RunningDisk) Best = Item;
        }
    }
    SendDlgItemMessageW(Dialog, IDC_BOOT, CB_SETCURSEL, Best, 0);
    SendDlgItemMessageW(Dialog, IDC_VOLUME, CB_SETCURSEL, 0, 0);
    LoadStore(Dialog);
    Selection(Dialog);
}

static void Notify(void *Context, const VC_PROGRESS *Progress)
{
    UINT Position = Progress->Total ? (UINT)((double)Progress->Completed / Progress->Total * 10000) : 0;
    PostMessageW(Context, DC_PROGRESS_MESSAGE, min(Position, 10000), 0);
}

static DWORD WINAPI Execute(void *Context)
{
    DC_JOB *Job = Context;
    BYTE *Header = NULL, Key[VC_KEY_SIZE];
    VC_META Meta;
    DWORD Error;
    switch (Job->Action)
    {
        case ActionEncrypt:
        case ActionDecrypt:
        case ActionResume:
            Error = DcConvert(&Store, &Job->Volume,
                               Job->Action == ActionEncrypt ? Job->New : Job->Secret,
                               Job->Recovery, Job->Path, Job->Action == ActionDecrypt,
                               &Cancel, Notify, Job->Dialog);
            break;
        case ActionUnlock:
        case ActionLock:
            Error = DcSetLock(&Store, &Job->Volume, Job->Secret, Job->Action == ActionUnlock);
            break;
        case ActionPassword:
        case ActionRecovery:
            Error = DcChange(&Store, &Job->Volume, Job->Secret,
                              Job->Action == ActionPassword ? Job->New : Job->Recovery,
                              Job->Action == ActionRecovery, Job->Path);
            break;
        case ActionBackup:
            Error = Job->Volume.Index < 0 ? ERROR_NOT_FOUND :
                    DcSecret(&Store, Store.Volumes + Job->Volume.Index, Job->Secret, &Header, &Meta, Key);
            if (!Error) Error = DcExport(&Store, &Job->Volume, Store.Volumes + Job->Volume.Index, Header, NULL, Job->Path);
            break;
        case ActionRestore:
            Error = DcImport(&Store, &Job->Volume, Job->Secret, Job->Path);
            break;
        default:
            Error = ERROR_INVALID_PARAMETER;
    }
    if (Header) DcOps.Free(NULL, Header, VC_METADATA_SIZE);
    VcWipe(Key, sizeof(Key));
    PostMessageW(Job->Dialog, DC_DONE, Error, (LPARAM)Job);
    return Error;
}

static BOOL ReadSecret(HWND Dialog, UINT Id, char *Secret, int Capacity)
{
    WCHAR Value[129];
    int Length;
    GetDlgItemTextW(Dialog, Id, Value, _countof(Value));
    Length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, Value, -1, Secret, Capacity, NULL, NULL);
    VcWipe(Value, sizeof(Value));
    SetDlgItemTextW(Dialog, Id, L"");
    return Length != 0;
}

static BOOL ChooseFile(HWND Dialog, WCHAR Path[DC_PATH], BOOL Save)
{
    OPENFILENAMEW File = {sizeof(File)};
    StringCchCopyW(Path, DC_PATH, Save ? L"ReliefWin-recovery.rwbackup" : L"");
    File.hwndOwner = Dialog;
    File.lpstrFile = Path;
    File.nMaxFile = DC_PATH;
    File.lpstrTitle = Text(Save ? IDS_SAVE : IDS_LOAD);
    File.lpstrFilter = L"ReliefWin recovery (*.rwbackup)\0*.rwbackup\0\0";
    File.lpstrDefExt = L"rwbackup";
    File.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | (Save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
    return Save ? GetSaveFileNameW(&File) : GetOpenFileNameW(&File);
}

static void Start(HWND Dialog)
{
    DC_JOB *Job;
    BYTE Random[32];
    char Confirm[513];
    WCHAR Message[1536];
    LRESULT Selected = SendDlgItemMessageW(Dialog, IDC_VOLUME, CB_GETCURSEL, 0, 0);
    DWORD i, Error = ERROR_SUCCESS;
    if (!Ready || Worker || Selected == CB_ERR) return;
    Job = VirtualAlloc(NULL, sizeof(*Job), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!Job)
    {
        Status(Dialog, GetLastError());
        return;
    }
    if (!VirtualLock(Job, sizeof(*Job)))
    {
        Error = GetLastError();
        goto Failure;
    }
    Job->Action = (DWORD)SendDlgItemMessageW(Dialog, IDC_ACTION, CB_GETCURSEL, 0, 0);
    Job->Dialog = Dialog;
    Job->Volume = Volumes[Selected];
    if (!ReadSecret(Dialog, IDC_SECRET, Job->Secret, sizeof(Job->Secret)) ||
        !ReadSecret(Dialog, IDC_NEW, Job->New, sizeof(Job->New)) ||
        !ReadSecret(Dialog, IDC_CONFIRM, Confirm, sizeof(Confirm)))
    {
        Error = ERROR_INVALID_PARAMETER;
        goto Failure;
    }
    if (Job->Action == ActionEncrypt || Job->Action == ActionPassword)
    {
        BOOL Valid = strlen(Job->New) >= 12 && strlen(Job->New) <= 128 && !strcmp(Job->New, Confirm);
        for (i = 0; Job->New[i]; ++i)
            Valid &= Job->New[i] >= 0x20 && Job->New[i] <= 0x7e;
        if (!Valid)
        {
            MessageBoxW(Dialog, Text(IDS_PASSWORD_RULE), Text(IDS_TITLE), MB_OK | MB_ICONINFORMATION);
            goto Cancelled;
        }
    }
    VcWipe(Confirm, sizeof(Confirm));
    if (Job->Action == ActionEncrypt || Job->Action == ActionRecovery)
    {
        if (!DcOps.Random(NULL, Random, sizeof(Random)))
        {
            Error = ERROR_GEN_FAILURE;
            goto Failure;
        }
        for (i = 0; i < sizeof(Random); ++i)
            sprintf(Job->Recovery + i * 2, "%02x", Random[i]);
        VcWipe(Random, sizeof(Random));
    }
    if (Job->Action == ActionEncrypt || Job->Action == ActionRecovery ||
        Job->Action == ActionBackup || Job->Action == ActionRestore)
        if (!ChooseFile(Dialog, Job->Path, Job->Action != ActionRestore)) goto Cancelled;
    StringCchPrintfW(Message, _countof(Message), Text(IDS_CONFIRM_ACTION),
                     Text(IDS_ENCRYPT + Job->Action), Job->Volume.Disk,
                     Job->Volume.Partition.PartitionNumber, Job->Volume.Root);
    if (MessageBoxW(Dialog, Message, Text(IDS_TITLE), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
        goto Cancelled;
    InterlockedExchange(&Cancel, 0);
    Worker = CreateThread(NULL, 0, Execute, Job, CREATE_SUSPENDED, NULL);
    if (!Worker)
    {
        Error = GetLastError();
        goto Failure;
    }
    Busy(Dialog, TRUE);
    SendDlgItemMessageW(Dialog, IDC_PROGRESS, PBM_SETPOS, 0, 0);
    SetDlgItemTextW(Dialog, IDC_STATUS, Text(IDS_BUSY));
    ResumeThread(Worker);
    return;
Failure:
    Status(Dialog, Error);
Cancelled:
    VcWipe(Confirm, sizeof(Confirm));
    VcWipe(Random, sizeof(Random));
    VcWipe(Job, sizeof(*Job));
    VirtualUnlock(Job, sizeof(*Job));
    VirtualFree(Job, 0, MEM_RELEASE);
}

static INT_PTR CALLBACK DialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam)
{
    unsigned i;
    const struct {UINT Control, String;} Labels[] = {
        {IDC_BOOT_LABEL, IDS_BOOT}, {IDC_VOLUME_LABEL, IDS_VOLUME}, {IDC_ACTION_LABEL, IDS_ACTION},
        {IDC_SECRET_LABEL, IDS_SECRET}, {IDC_NEW_LABEL, IDS_NEW}, {IDC_CONFIRM_LABEL, IDS_CONFIRM},
        {IDC_DESCRIPTION, IDS_HELP}, {IDC_GO, IDS_GO}, {IDC_PAUSE, IDS_PAUSE}, {IDC_REFRESH, IDS_REFRESH}, {IDCANCEL, IDS_CLOSE}};
    switch (Message)
    {
        case WM_INITDIALOG:
            SetWindowTextW(Dialog, Text(IDS_TITLE));
            for (i = 0; i < _countof(Labels); ++i)
                SetDlgItemTextW(Dialog, Labels[i].Control, Text(Labels[i].String));
            for (i = IDS_ENCRYPT; i <= IDS_RESTORE; ++i)
                SendDlgItemMessageW(Dialog, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)Text(i));
            SendDlgItemMessageW(Dialog, IDC_ACTION, CB_SETCURSEL, ActionUnlock, 0);
            SendDlgItemMessageW(Dialog, IDC_SECRET, EM_SETLIMITTEXT, 128, 0);
            SendDlgItemMessageW(Dialog, IDC_NEW, EM_SETLIMITTEXT, 128, 0);
            SendDlgItemMessageW(Dialog, IDC_CONFIRM, EM_SETLIMITTEXT, 128, 0);
            SendDlgItemMessageW(Dialog, IDC_PROGRESS, PBM_SETRANGE32, 0, 10000);
            Refresh(Dialog);
            return TRUE;
        case WM_COMMAND:
            if (LOWORD(WParam) == IDC_GO) Start(Dialog);
            else if (LOWORD(WParam) == IDC_REFRESH) Refresh(Dialog);
            else if (LOWORD(WParam) == IDC_BOOT && HIWORD(WParam) == CBN_SELCHANGE) { LoadStore(Dialog); Selection(Dialog); }
            else if (LOWORD(WParam) == IDC_VOLUME && HIWORD(WParam) == CBN_SELCHANGE) Selection(Dialog);
            else if (LOWORD(WParam) == IDC_ACTION && HIWORD(WParam) == CBN_SELCHANGE) Fields(Dialog);
            else if (LOWORD(WParam) == IDC_PAUSE) InterlockedExchange(&Cancel, 1);
            else if (LOWORD(WParam) == IDCANCEL) SendMessageW(Dialog, WM_CLOSE, 0, 0);
            return TRUE;
        case DC_PROGRESS_MESSAGE:
            SendDlgItemMessageW(Dialog, IDC_PROGRESS, PBM_SETPOS, WParam, 0);
            return TRUE;
        case DC_DONE:
        {
            DC_JOB *Job = (DC_JOB *)LParam;
            WaitForSingleObject(Worker, INFINITE);
            CloseHandle(Worker);
            Worker = NULL;
            if (Job->Recovery[0] && (WParam == ERROR_SUCCESS || WParam == ERROR_OPERATION_ABORTED))
                MessageBoxW(Dialog, Text(IDS_KEY_NOTE), Text(IDS_TITLE), MB_OK | MB_ICONINFORMATION);
            VcWipe(Job, sizeof(*Job));
            VirtualUnlock(Job, sizeof(*Job));
            VirtualFree(Job, 0, MEM_RELEASE);
            Refresh(Dialog);
            Busy(Dialog, FALSE);
            Status(Dialog, (DWORD)WParam);
            return TRUE;
        }
        case WM_QUERYENDSESSION:
            SetWindowLongPtrW(Dialog, DWLP_MSGRESULT, !Worker);
            return TRUE;
        case WM_CLOSE:
            if (Worker)
                MessageBoxW(Dialog, Text(IDS_BUSY_CLOSE), Text(IDS_TITLE), MB_OK | MB_ICONINFORMATION);
            else
                EndDialog(Dialog, 0);
            return TRUE;
    }
    return FALSE;
}

int WINAPI wWinMain(HINSTANCE Current, HINSTANCE Previous, PWSTR CommandLine, int Show)
{
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_PROGRESS_CLASS};
    HANDLE Mutex;
    INT_PTR Result;
    (void)Previous;
    (void)CommandLine;
    (void)Show;
    Instance = Current;
    Mutex = CreateMutexW(NULL, FALSE, L"Global\\ReliefWinDiskEncryption");
    if (!Mutex) return GetLastError();
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(Mutex);
        return ERROR_ALREADY_EXISTS;
    }
    VcCryptoInitialize();
    InitCommonControlsEx(&Controls);
    Result = DialogBoxParamW(Instance, MAKEINTRESOURCEW(IDD_DISKCRYPT), NULL, DialogProc, 0);
    CloseHandle(Mutex);
    return Result == -1 ? GetLastError() : (int)Result;
}
