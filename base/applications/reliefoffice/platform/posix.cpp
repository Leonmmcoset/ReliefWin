/*
 * PROJECT:     ReliefOffice
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Preserve document IO and recovery semantics with Windows handles
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include "posix.h"
#include <limits.h>
#include <time.h>
#include <new>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winternl.h>
#include <aclapi.h>
#include <shlobj.h>
#include <io.h>

namespace ro {
namespace {
int failure(DWORD code = GetLastError()) {
    switch (code) {
    case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND: errno = ENOENT; break;
    case ERROR_FILE_EXISTS: case ERROR_ALREADY_EXISTS: errno = EEXIST; break;
    case ERROR_ACCESS_DENIED: case ERROR_PRIVILEGE_NOT_HELD: errno = EACCES; break;
    case ERROR_SHARING_VIOLATION: case ERROR_LOCK_VIOLATION: errno = EAGAIN; break;
    case ERROR_NOT_ENOUGH_MEMORY: case ERROR_OUTOFMEMORY: errno = ENOMEM; break;
    case ERROR_DISK_FULL: case ERROR_HANDLE_DISK_FULL: errno = ENOSPC; break;
    case ERROR_INVALID_HANDLE: errno = EBADF; break;
    case ERROR_DIRECTORY: errno = ENOTDIR; break;
    case ERROR_NOT_SAME_DEVICE: errno = EXDEV; break;
    case ERROR_FILENAME_EXCED_RANGE: errno = ENAMETOOLONG; break;
    case ERROR_NOT_SUPPORTED: case ERROR_INVALID_FUNCTION: errno = ENOSYS; break;
    case ERROR_NO_UNICODE_TRANSLATION: errno = EILSEQ; break;
    case ERROR_INVALID_PARAMETER: case ERROR_INVALID_NAME: errno = EINVAL; break;
    default: errno = EIO; break;
    }
    return -1;
}
struct Wide {
    wchar_t *data = nullptr;
    size_t length = 0;
    explicit Wide(const char *input, bool path = true) {
        if (!input) { errno = EINVAL; return; }
        if (path && input[0] == '/' && input[1] && input[2] == ':') ++input;
        int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, nullptr, 0);
        if (!count) { failure(); return; }
        data = static_cast<wchar_t *>(malloc((size_t(count) + 2) * sizeof(wchar_t)));
        if (!data) { errno = ENOMEM; return; }
        if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, data, count)) {
            failure(); free(data); data = nullptr; return;
        }
        length = size_t(count - 1);
        if (path) {
            for (size_t i = 0; i < length; ++i) if (data[i] == L'/') data[i] = L'\\';
            if (length == 2 && data[1] == L':') { data[length++] = L'\\'; data[length] = 0; }
        }
    }
    ~Wide() { free(data); }
    Wide(const Wide &) = delete;
    Wide &operator=(const Wide &) = delete;
};
HANDLE handle(int fd) {
    intptr_t value = _get_osfhandle(fd);
    return value == -1 ? INVALID_HANDLE_VALUE : reinterpret_cast<HANDLE>(value);
}
uint64_t sid_id(PSID sid) {
    uint64_t hash = UINT64_C(14695981039346656037);
    const unsigned char *bytes = static_cast<const unsigned char *>(sid);
    for (DWORD i = 0; i < GetLengthSid(sid); ++i) { hash ^= bytes[i]; hash *= UINT64_C(1099511628211); }
    return hash;
}
struct Identity {
    unsigned char user[SECURITY_MAX_SID_SIZE];
    unsigned char system[SECURITY_MAX_SID_SIZE];
    uint64_t id = 0;
    bool valid = false;
};
INIT_ONCE identity_once = INIT_ONCE_STATIC_INIT;
Identity identity;
BOOL CALLBACK load_identity(PINIT_ONCE, PVOID, PVOID *) {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return TRUE;
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    TOKEN_USER *user = static_cast<TOKEN_USER *>(malloc(size));
    if (user && GetTokenInformation(token, TokenUser, user, size, &size) &&
        CopySid(sizeof(identity.user), identity.user, user->User.Sid)) {
        DWORD system_size = sizeof(identity.system);
        if (CreateWellKnownSid(WinLocalSystemSid, nullptr, identity.system, &system_size)) {
            identity.id = sid_id(identity.user);
            identity.valid = true;
        }
    }
    free(user); CloseHandle(token); return TRUE;
}
bool current_identity() {
    InitOnceExecuteOnce(&identity_once, load_identity, nullptr, nullptr);
    if (!identity.valid) { errno = EACCES; return false; }
    return true;
}
struct PrivateSecurity {
    SECURITY_DESCRIPTOR descriptor;
    unsigned char storage[sizeof(ACL) + 3 * (sizeof(ACCESS_ALLOWED_ACE) + SECURITY_MAX_SID_SIZE)];
    bool init(bool directory) {
        if (!current_identity()) return false;
        PACL acl = reinterpret_cast<PACL>(storage);
        DWORD flags = directory ? OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE : 0;
        if (!InitializeAcl(acl, sizeof(storage), ACL_REVISION) ||
            !AddAccessAllowedAceEx(acl, ACL_REVISION, flags, FILE_ALL_ACCESS, identity.user) ||
            (!EqualSid(identity.user, identity.system) &&
             !AddAccessAllowedAceEx(acl, ACL_REVISION, flags, FILE_ALL_ACCESS, identity.system)) ||
            !InitializeSecurityDescriptor(&descriptor, SECURITY_DESCRIPTOR_REVISION) ||
            !SetSecurityDescriptorOwner(&descriptor, identity.user, FALSE) ||
            !SetSecurityDescriptorDacl(&descriptor, TRUE, acl, FALSE) ||
            !SetSecurityDescriptorControl(&descriptor, SE_DACL_PROTECTED, SE_DACL_PROTECTED)) {
            failure(); return false;
        }
        return true;
    }
};
int adopt(HANDLE value, int flags) {
    if (value == INVALID_HANDLE_VALUE) return failure();
    if (flags & O_NOFOLLOW) {
        BY_HANDLE_FILE_INFORMATION info;
        if (!GetFileInformationByHandle(value, &info)) {
            DWORD error = GetLastError(); CloseHandle(value); return failure(error);
        }
        if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            CloseHandle(value); errno = ELOOP; return -1;
        }
    }
    int fd = _open_osfhandle(reinterpret_cast<intptr_t>(value), _O_BINARY | _O_NOINHERIT | (flags & O_ACCMODE));
    if (fd < 0) CloseHandle(value);
    return fd;
}
HANDLE relative_open(int directory, const wchar_t *name, size_t length,
                     DWORD access, DWORD disposition, DWORD options, PVOID security = nullptr) {
    if (length > USHRT_MAX / sizeof(wchar_t)) { errno = ENAMETOOLONG; return INVALID_HANDLE_VALUE; }
    HANDLE root = handle(directory);
    if (root == INVALID_HANDLE_VALUE) { errno = EBADF; return INVALID_HANDLE_VALUE; }
    UNICODE_STRING string;
    string.Buffer = const_cast<wchar_t *>(name);
    string.Length = static_cast<USHORT>(length * sizeof(wchar_t));
    string.MaximumLength = string.Length;
    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(&attributes, &string, OBJ_CASE_INSENSITIVE, root, security);
    IO_STATUS_BLOCK status;
    HANDLE file = INVALID_HANDLE_VALUE;
    NTSTATUS result = NtCreateFile(&file, access | SYNCHRONIZE, &attributes, &status, nullptr,
        FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        disposition, options | FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0);
    if (result < 0) { failure(RtlNtStatusToDosError(result)); return INVALID_HANDLE_VALUE; }
    return file;
}
DWORD desired_access(int flags) {
    DWORD access = FILE_READ_ATTRIBUTES | READ_CONTROL;
    if (flags & O_DIRECTORY) return access | FILE_LIST_DIRECTORY;
    if ((flags & O_ACCMODE) != O_WRONLY) access |= GENERIC_READ;
    if ((flags & O_ACCMODE) != O_RDONLY) access |= GENERIC_WRITE;
    return access;
}
bool security_mode(HANDLE file, DWORD attributes, Mode &mode, uint64_t &owner) {
    PSID sid = nullptr;
    PACL acl = nullptr;
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    DWORD result = GetSecurityInfo(file, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
        &sid, nullptr, &acl, nullptr, &descriptor);
    if (result == ERROR_NOT_SUPPORTED || result == ERROR_INVALID_FUNCTION) {
        owner = UINT64_MAX;
        mode |= S_ISDIR(mode) ? 0777 : (attributes & FILE_ATTRIBUTE_READONLY ? 0444 : 0666);
        return true;
    }
    if (result != ERROR_SUCCESS) { failure(result); return false; }
    if (!sid || !IsValidSid(sid)) { LocalFree(descriptor); errno = EACCES; return false; }
    owner = sid_id(sid);
    bool private_acl = acl != nullptr;
    if (!current_identity()) { LocalFree(descriptor); return false; }
    if (acl) for (DWORD i = 0; i < acl->AceCount; ++i) {
        void *raw;
        if (!GetAce(acl, i, &raw)) { LocalFree(descriptor); failure(); return false; }
        ACE_HEADER *header = static_cast<ACE_HEADER *>(raw);
        if (header->AceFlags & INHERIT_ONLY_ACE) continue;
        if (header->AceType == ACCESS_ALLOWED_ACE_TYPE) {
            ACCESS_ALLOWED_ACE *ace = static_cast<ACCESS_ALLOWED_ACE *>(raw);
            if (ace->Mask && !EqualSid(&ace->SidStart, sid) && !EqualSid(&ace->SidStart, identity.system))
                private_acl = false;
        } else if (header->AceType != ACCESS_DENIED_ACE_TYPE && header->AceType != SYSTEM_AUDIT_ACE_TYPE) {
            private_acl = false;
        }
    }
    mode |= S_ISDIR(mode) ? 0700 : 0600;
    if (!private_acl) mode |= 0044;
    LocalFree(descriptor);
    return true;
}
Time timestamp(LONGLONG ticks) {
    const int64_t epoch = INT64_C(116444736000000000);
    int64_t value = ticks - epoch;
    Time result;
    result.tv_sec = value / 10000000;
    result.tv_nsec = long(value % 10000000) * 100;
    if (result.tv_nsec < 0) { --result.tv_sec; result.tv_nsec += 1000000000; }
    return result;
}
int move_at(int from_directory, const char *from, int to_directory, const char *to, bool replace) {
    Wide source(from), destination(to);
    if (!source.data || !destination.data) return -1;
    HANDLE file = relative_open(from_directory, source.data, source.length,
        DELETE | FILE_READ_ATTRIBUTES, FILE_OPEN, FILE_OPEN_REPARSE_POINT | FILE_WRITE_THROUGH);
    if (file == INVALID_HANDLE_VALUE) return -1;
    size_t bytes = offsetof(FILE_RENAME_INFO, FileName) + (destination.length + 1) * sizeof(wchar_t);
    FILE_RENAME_INFO *info = static_cast<FILE_RENAME_INFO *>(calloc(1, bytes));
    if (!info) { CloseHandle(file); errno = ENOMEM; return -1; }
    info->ReplaceIfExists = replace;
    info->RootDirectory = handle(to_directory);
    info->FileNameLength = DWORD(destination.length * sizeof(wchar_t));
    memcpy(info->FileName, destination.data, info->FileNameLength);
    BOOL ok = SetFileInformationByHandle(file, FileRenameInfo, info, DWORD(bytes));
    DWORD error = GetLastError();
    free(info); CloseHandle(file);
    return ok ? 0 : failure(error);
}
int parent(const char *path, const char *&base) {
    const char *separator = last_separator(path);
    base = separator ? separator + 1 : path;
    if (!*base) { errno = EINVAL; return -1; }
    if (!separator) return open(".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    size_t length = size_t(separator - path);
    if (!length || (length==2 && path[1]==':') ||
        (length==3 && path[0]=='/' && path[2]==':')) ++length;
    char *directory = static_cast<char *>(malloc(length + 1));
    if (!directory) { errno = ENOMEM; return -1; }
    memcpy(directory, path, length); directory[length] = 0;
    int result = open(directory, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    free(directory);
    return result;
}
INIT_ONCE folders_once = INIT_ONCE_STATIC_INIT;
char home_folder[4096], state_folder[4096];
bool folder_utf8(int folder, char *output, size_t capacity, const char *suffix = "") {
    wchar_t path[MAX_PATH];
    if (FAILED(SHGetFolderPathW(nullptr, folder, nullptr, SHGFP_TYPE_CURRENT, path))) return false;
    char converted[4096];
    if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, converted, sizeof(converted), nullptr, nullptr)) return false;
    for (char *at = converted; *at; ++at) if (*at == '\\') *at = '/';
    const char *prefix = converted[0] && converted[1] == ':' ? "/" : "";
    int n = snprintf(output, capacity, "%s%s%s", prefix, converted, suffix);
    return n > 0 && size_t(n) < capacity;
}
BOOL CALLBACK load_folders(PINIT_ONCE, PVOID, PVOID *) {
    folder_utf8(CSIDL_PROFILE, home_folder, sizeof(home_folder));
    folder_utf8(CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, state_folder, sizeof(state_folder), "/ReliefOffice");
    return TRUE;
}
}
int open(const char *path, int flags, Mode) {
    Wide wide(path);
    if (!wide.data) return -1;
    DWORD disposition = flags & O_CREAT ? (flags & O_EXCL ? CREATE_NEW : (flags & O_TRUNC ? CREATE_ALWAYS : OPEN_ALWAYS)) :
        (flags & O_TRUNC ? TRUNCATE_EXISTING : OPEN_EXISTING);
    DWORD attributes = FILE_FLAG_BACKUP_SEMANTICS;
    if (flags & O_NOFOLLOW) attributes |= FILE_FLAG_OPEN_REPARSE_POINT;
    if ((flags & O_ACCMODE) != O_RDONLY) attributes |= FILE_FLAG_WRITE_THROUGH;
    PrivateSecurity security;
    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, FALSE};
    if (flags & O_CREAT) { if (!security.init(false)) return -1; sa.lpSecurityDescriptor = &security.descriptor; }
    HANDLE file = CreateFileW(wide.data, desired_access(flags), FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        &sa, disposition, attributes, nullptr);
    int fd = adopt(file, flags);
    if (fd >= 0 && (flags & O_DIRECTORY)) {
        BY_HANDLE_FILE_INFORMATION info;
        if (!GetFileInformationByHandle(file, &info) || !(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            close(fd); errno = ENOTDIR; return -1;
        }
    }
    return fd;
}
int openat(int directory, const char *path, int flags, Mode mode) {
    if (absolute_path(path) || (path && strlen(path) == 2 && path[1] == ':')) return open(path, flags, mode);
    Wide name(path);
    if (!name.data) return -1;
    if (wcschr(name.data, L':')) { errno = EINVAL; return -1; }
    DWORD disposition = flags & O_CREAT ? (flags & O_EXCL ? FILE_CREATE : (flags & O_TRUNC ? FILE_OVERWRITE_IF : FILE_OPEN_IF)) :
        (flags & O_TRUNC ? FILE_OVERWRITE : FILE_OPEN);
    DWORD options = flags & O_DIRECTORY ? FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
    if (flags & O_NOFOLLOW) options |= FILE_OPEN_REPARSE_POINT;
    if ((flags & O_ACCMODE) != O_RDONLY) options |= FILE_WRITE_THROUGH;
    PrivateSecurity security;
    if ((flags & O_CREAT) && !security.init(false)) return -1;
    HANDLE file=relative_open(directory, name.data, name.length, desired_access(flags), disposition, options,
        flags & O_CREAT ? &security.descriptor : nullptr);
    return file==INVALID_HANDLE_VALUE?-1:adopt(file,flags);
}
int close(int fd) { return _close(fd); }
int dup(int fd) { return _dup(fd); }
ssize_t read(int fd, void *bytes, size_t count) { return _read(fd, bytes, unsigned(count > INT_MAX ? INT_MAX : count)); }
ssize_t write(int fd, const void *bytes, size_t count) { return _write(fd, bytes, unsigned(count > INT_MAX ? INT_MAX : count)); }
int fstat(int fd, Stat *output) {
    if (!output) { errno = EINVAL; return -1; }
    HANDLE file = handle(fd);
    if (file == INVALID_HANDLE_VALUE) { errno = EBADF; return -1; }
    BY_HANDLE_FILE_INFORMATION info;
    FILE_BASIC_INFO basic;
    if (!GetFileInformationByHandle(file, &info) ||
        !GetFileInformationByHandleEx(file, FileBasicInfo, &basic, sizeof(basic))) return failure();
    Stat st;
    st.st_dev = info.dwVolumeSerialNumber;
    st.st_ino = (uint64_t(info.nFileIndexHigh) << 32) | info.nFileIndexLow;
    st.st_size = int64_t((uint64_t(info.nFileSizeHigh) << 32) | info.nFileSizeLow);
    st.st_nlink = info.nNumberOfLinks;
    st.st_mode = info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT ? S_IFLNK :
        (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? S_IFDIR : S_IFREG);
    st.st_mtim = timestamp(basic.LastWriteTime.QuadPart);
    st.st_ctim = timestamp(basic.ChangeTime.QuadPart);
    st.st_mtime = st.st_mtim.tv_sec;
    if (!security_mode(file, info.dwFileAttributes, st.st_mode, st.st_uid)) return -1;
    *output = st;
    return 0;
}
int stat(const char *path, Stat *output) {
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) return -1;
    int result = fstat(fd, output), saved = errno; close(fd); errno = saved; return result;
}
int lstat(const char *path, Stat *output) {
    Wide wide(path);
    if (!wide.data) return -1;
    HANDLE file = CreateFileW(wide.data, FILE_READ_ATTRIBUTES | READ_CONTROL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    int fd = adopt(file, O_RDONLY);
    if (fd < 0) return -1;
    int result = fstat(fd, output), saved = errno; close(fd); errno = saved; return result;
}
int fstatat(int directory, const char *path, Stat *output, int flags) {
    if (absolute_path(path)) return flags & AT_SYMLINK_NOFOLLOW ? lstat(path, output) : stat(path, output);
    Wide name(path);
    if (!name.data) return -1;
    HANDLE file = relative_open(directory, name.data, name.length, FILE_READ_ATTRIBUTES | READ_CONTROL,
        FILE_OPEN, FILE_OPEN_FOR_BACKUP_INTENT | (flags & AT_SYMLINK_NOFOLLOW ? FILE_OPEN_REPARSE_POINT : 0));
    if (file == INVALID_HANDLE_VALUE) return -1;
    int fd = adopt(file, O_RDONLY);
    if (fd < 0) return -1;
    int result = fstat(fd, output), saved = errno; close(fd); errno = saved; return result;
}
int mkdir(const char *path, Mode) {
    Wide wide(path);
    PrivateSecurity security;
    if (!wide.data || !security.init(true)) return -1;
    SECURITY_ATTRIBUTES sa = {sizeof(sa), &security.descriptor, FALSE};
    return CreateDirectoryW(wide.data, &sa) ? 0 : failure();
}
int mkdirat(int directory, const char *path, Mode mode) {
    if (absolute_path(path)) return mkdir(path, mode);
    Wide name(path);
    PrivateSecurity security;
    if (!name.data || !security.init(true)) return -1;
    HANDLE file = relative_open(directory, name.data, name.length, FILE_LIST_DIRECTORY | READ_CONTROL,
        FILE_CREATE, FILE_DIRECTORY_FILE | FILE_OPEN_REPARSE_POINT, &security.descriptor);
    if (file == INVALID_HANDLE_VALUE) return -1;
    CloseHandle(file); return 0;
}
int fchmod(int fd, Mode mode) {
    if (mode & ~0777U) { errno = EINVAL; return -1; }
    PrivateSecurity security;
    if (!security.init((mode & 0100) != 0)) return -1;
    if (mode & 0077) {
        unsigned char everyone[SECURITY_MAX_SID_SIZE]; DWORD size=sizeof(everyone);
        if (!CreateWellKnownSid(WinWorldSid,nullptr,everyone,&size)) return failure();
        DWORD access=0;
        if (mode & 0044) access|=FILE_GENERIC_READ;
        if (mode & 0022) access|=FILE_GENERIC_WRITE;
        if (mode & 0011) access|=FILE_GENERIC_EXECUTE;
        if (!AddAccessAllowedAceEx(reinterpret_cast<PACL>(security.storage),ACL_REVISION,
            mode & 0100 ? OBJECT_INHERIT_ACE|CONTAINER_INHERIT_ACE : 0,access,everyone)) return failure();
    }
    HANDLE file = ReOpenFile(handle(fd), WRITE_DAC | READ_CONTROL | FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
    if (file == INVALID_HANDLE_VALUE) return failure();
    BOOL present, defaulted;
    PACL acl = nullptr;
    GetSecurityDescriptorDacl(&security.descriptor, &present, &acl, &defaulted);
    DWORD result = SetSecurityInfo(file, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
        nullptr, nullptr, acl, nullptr);
    CloseHandle(file);
    return result == ERROR_SUCCESS ? 0 : failure(result);
}
int chmod(const char *path, Mode mode) {
    int fd = open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) return -1;
    int result = fchmod(fd, mode), saved = errno; close(fd); errno = saved; return result;
}
int fsync(int fd) {
    HANDLE file = handle(fd);
    if (file == INVALID_HANDLE_VALUE) { errno = EBADF; return -1; }
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(file, &info)) return failure();
    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return 0;
    return FlushFileBuffers(file) ? 0 : failure();
}
int unlinkat(int directory, const char *path, int flags) {
    if (flags) { errno = EINVAL; return -1; }
    Wide name(path);
    if (!name.data) return -1;
    HANDLE file = relative_open(directory, name.data, name.length, DELETE | FILE_READ_ATTRIBUTES,
        FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_OPEN_REPARSE_POINT);
    if (file == INVALID_HANDLE_VALUE) return -1;
    FILE_DISPOSITION_INFO info = {TRUE};
    BOOL ok = SetFileInformationByHandle(file, FileDispositionInfo, &info, sizeof(info));
    DWORD error = GetLastError(); CloseHandle(file);
    return ok ? 0 : failure(error);
}
int unlink(const char *path) {
    const char *base;
    int directory = parent(path, base);
    if (directory < 0) return -1;
    int result = unlinkat(directory, base, 0), saved = errno; close(directory); errno = saved; return result;
}
int renameat(int from_directory, const char *from, int to_directory, const char *to) {
    return move_at(from_directory, from, to_directory, to, true);
}
int renameat_noreplace(int from_directory, const char *from, int to_directory, const char *to) {
    return move_at(from_directory, from, to_directory, to, false);
}
int move_path(const char *from, const char *to, bool replace) {
    const char *from_base, *to_base;
    int source = parent(from, from_base);
    if (source < 0) return -1;
    int destination = parent(to, to_base);
    if (destination < 0) { int saved = errno; close(source); errno = saved; return -1; }
    int result = move_at(source, from_base, destination, to_base, replace), saved = errno;
    close(destination); close(source); errno = saved; return result;
}
int rename(const char *from, const char *to) { return move_path(from, to, true); }
int rename_noreplace(const char *from, const char *to) { return move_path(from, to, false); }
int mkstemp(char *path) {
    if (!path) { errno = EINVAL; return -1; }
    size_t length = strlen(path);
    if (length < 6 || strcmp(path + length - 6, "XXXXXX")) { errno = EINVAL; return -1; }
    static volatile LONG sequence = 0;
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (unsigned attempt = 0; attempt < 256; ++attempt) {
        uint64_t value = (uint64_t(GetCurrentProcessId()) << 32) ^ GetTickCount64() ^ uint32_t(InterlockedIncrement(&sequence));
        for (unsigned i = 0; i < 6; ++i) { path[length - 6 + i] = digits[value % 36]; value /= 36; }
        int fd = open(path, O_RDWR | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, 0600);
        if (fd >= 0 || errno != EEXIST) return fd;
    }
    errno = EEXIST; return -1;
}
int flock(int fd, int operation) {
    OVERLAPPED offset = {};
    HANDLE file = handle(fd);
    if (file == INVALID_HANDLE_VALUE) { errno = EBADF; return -1; }
    BOOL ok = operation & LOCK_UN ? UnlockFileEx(file, 0, MAXDWORD, MAXDWORD, &offset) :
        LockFileEx(file, (operation & LOCK_EX ? LOCKFILE_EXCLUSIVE_LOCK : 0) |
            (operation & LOCK_NB ? LOCKFILE_FAIL_IMMEDIATELY : 0), 0, MAXDWORD, MAXDWORD, &offset);
    return ok ? 0 : failure();
}
int fcntl(int fd, int command, int value) {
    if (command == F_GETFD) {
        DWORD flags;
        if (!GetHandleInformation(handle(fd), &flags)) return failure();
        return flags & HANDLE_FLAG_INHERIT ? 0 : FD_CLOEXEC;
    }
    if (command == F_SETFD)
        return SetHandleInformation(handle(fd), HANDLE_FLAG_INHERIT, value & FD_CLOEXEC ? 0 : HANDLE_FLAG_INHERIT) ? 0 : failure();
    errno = EINVAL; return -1;
}
uint64_t getuid() { return current_identity() ? identity.id : 0; }
uint64_t geteuid() { return getuid(); }
unsigned long getpid() { return GetCurrentProcessId(); }
const char *getenv(const char *name) {
    if (!strcmp(name, "HOME") || !strcmp(name, "XDG_STATE_HOME")) {
        InitOnceExecuteOnce(&folders_once, load_folders, nullptr, nullptr);
        const char *value = !strcmp(name, "HOME") ? home_folder : state_folder;
        return *value ? value : nullptr;
    }
    return ::getenv(name);
}
FILE *fopen(const char *path, const char *mode) {
    Wide wide(path), wide_mode(mode, false);
    return wide.data && wide_mode.data ? _wfopen(wide.data, wide_mode.data) : nullptr;
}
struct Dir {
    int fd = -1;
    HANDLE search = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW found;
    Dirent entry;
    bool first = true;
};
Dir *fdopendir(int fd) {
    wchar_t path[32768];
    DWORD length = GetFinalPathNameByHandleW(handle(fd), path, 32766, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    if (!length || length >= 32766) { if (length) errno = ENAMETOOLONG; else failure(); return nullptr; }
    path[length++] = L'\\'; path[length++] = L'*'; path[length] = 0;
    Dir *dir = new (std::nothrow) Dir;
    if (!dir) { errno=ENOMEM; return nullptr; }
    dir->search = FindFirstFileW(path, &dir->found);
    if (dir->search == INVALID_HANDLE_VALUE) { DWORD saved = GetLastError(); delete dir; failure(saved); return nullptr; }
    dir->fd = fd;
    return dir;
}
Dir *opendir(const char *path) {
    int fd = open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) return nullptr;
    Dir *dir = fdopendir(fd);
    if (!dir) { int saved = errno; close(fd); errno = saved; }
    return dir;
}
Dirent *readdir(Dir *dir) {
    if (!dir) { errno = EBADF; return nullptr; }
    if (!dir->first && !FindNextFileW(dir->search, &dir->found)) {
        DWORD error = GetLastError();
        if (error == ERROR_NO_MORE_FILES) errno = 0; else failure(error);
        return nullptr;
    }
    dir->first = false;
    if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, dir->found.cFileName, -1,
        dir->entry.d_name, sizeof(dir->entry.d_name), nullptr, nullptr)) { failure(); return nullptr; }
    return &dir->entry;
}
int closedir(Dir *dir) {
    if (!dir) { errno = EBADF; return -1; }
    BOOL ok = FindClose(dir->search);
    DWORD error = GetLastError();
    int result = close(dir->fd);
    delete dir;
    return ok ? result : failure(error);
}
int strcasecmp(const char *a, const char *b) { return _stricmp(a, b); }
int strncasecmp(const char *a, const char *b, size_t count) { return _strnicmp(a, b, count); }
double monotonic() {
    LARGE_INTEGER value, frequency;
    if (!QueryPerformanceCounter(&value) || !QueryPerformanceFrequency(&frequency) || !frequency.QuadPart) return 0;
    return double(value.QuadPart) / double(frequency.QuadPart);
}
}
#else
namespace ro {
int renameat_noreplace(int from_directory, const char *from, int to_directory, const char *to) {
    if (::linkat(from_directory, from, to_directory, to, 0) < 0) return -1;
    if (::unlinkat(from_directory, from, 0) == 0) return 0;
    int saved = errno; ::unlinkat(to_directory, to, 0); errno = saved; return -1;
}
int rename_noreplace(const char *from, const char *to) { return renameat_noreplace(AT_FDCWD, from, AT_FDCWD, to); }
double monotonic() {
    struct timespec value;
    return clock_gettime(CLOCK_MONOTONIC, &value) == 0 ? value.tv_sec + value.tv_nsec / 1e9 : 0;
}
}
#endif
namespace ro {
int set_document_mode(int fd, Mode mode) {
    int saved=errno;
    if (fchmod(fd,mode)==0) return 0;
#ifdef _WIN32
    if (errno!=ENOSYS) return -1;
    int permission_error=errno;
    Stat st;
    if (fstat(fd,&st)<0) return -1;
    Mode permissions=st.st_mode&0777;
    if (S_ISREG(st.st_mode) && st.st_uid==UINT64_MAX &&
        (permissions==0666 || permissions==0444)) {
        errno=saved;
        return 0;
    }
    errno=permission_error;
#else
    (void)saved;
#endif
    return -1;
}
struct tm *localtime(const time_t *value,struct tm *output) {
#ifdef _WIN32
    return localtime_s(output,value)==0?output:nullptr;
#else
    return ::localtime_r(value,output);
#endif
}
namespace {
#ifdef _WIN32
struct IdentityMutex {
    IdentityMutex *next=nullptr;
    uint64_t volume=0,file=0;
    HANDLE handle=nullptr;
    bool owned=false;
};
SRWLOCK identity_mutex_guard=SRWLOCK_INIT;
IdentityMutex *identity_mutexes=nullptr;
void unlock_identity(IdentityMutex *entry) {
    if (entry->owned) ReleaseMutex(entry->handle);
    if (entry->handle) CloseHandle(entry->handle);
    AcquireSRWLockExclusive(&identity_mutex_guard);
    IdentityMutex **at=&identity_mutexes;
    while (*at && *at!=entry) at=&(*at)->next;
    if (*at) *at=entry->next;
    ReleaseSRWLockExclusive(&identity_mutex_guard);
    delete entry;
}
IdentityMutex *lock_identity(const Stat &metadata) {
    IdentityMutex *entry=new(std::nothrow) IdentityMutex;
    if (!entry) { errno=ENOMEM; return nullptr; }
    entry->volume=metadata.st_dev;
    entry->file=metadata.st_ino;
    AcquireSRWLockExclusive(&identity_mutex_guard);
    for (IdentityMutex *at=identity_mutexes;at;at=at->next) {
        if (at->volume==entry->volume && at->file==entry->file) {
            ReleaseSRWLockExclusive(&identity_mutex_guard);
            delete entry;
            errno=EAGAIN;
            return nullptr;
        }
    }
    entry->next=identity_mutexes;
    identity_mutexes=entry;
    ReleaseSRWLockExclusive(&identity_mutex_guard);
    wchar_t name[96];
    int length=swprintf(name,sizeof(name)/sizeof(name[0]),L"Global\\ReliefOffice-save-%016llx-%016llx",
        static_cast<unsigned long long>(metadata.st_dev),static_cast<unsigned long long>(metadata.st_ino));
    unsigned char everyone[SECURITY_MAX_SID_SIZE];
    alignas(ACL) unsigned char storage[sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)+SECURITY_MAX_SID_SIZE];
    PACL acl=reinterpret_cast<PACL>(storage);
    DWORD size=sizeof(everyone),error=ERROR_SUCCESS;
    SECURITY_DESCRIPTOR descriptor;
    if (length<=0 || size_t(length)>=sizeof(name)/sizeof(name[0])) error=ERROR_INVALID_NAME;
    else if (!CreateWellKnownSid(WinWorldSid,nullptr,everyone,&size) ||
        !InitializeAcl(acl,sizeof(storage),ACL_REVISION) ||
        !AddAccessAllowedAce(acl,ACL_REVISION,SYNCHRONIZE|MUTEX_MODIFY_STATE,everyone) ||
        !InitializeSecurityDescriptor(&descriptor,SECURITY_DESCRIPTOR_REVISION) ||
        !SetSecurityDescriptorDacl(&descriptor,TRUE,acl,FALSE)) error=GetLastError();
    if (error==ERROR_SUCCESS) {
        SECURITY_ATTRIBUTES attributes={sizeof(attributes),&descriptor,FALSE};
        entry->handle=CreateMutexExW(&attributes,name,0,SYNCHRONIZE|MUTEX_MODIFY_STATE);
        if (!entry->handle) error=GetLastError();
    }
    if (error==ERROR_SUCCESS) {
        DWORD result=WaitForSingleObject(entry->handle,0);
        if (result==WAIT_OBJECT_0 || result==WAIT_ABANDONED) entry->owned=true;
        else error=result==WAIT_TIMEOUT?ERROR_LOCK_VIOLATION:GetLastError();
    }
    if (error!=ERROR_SUCCESS) {
        unlock_identity(entry);
        failure(error);
        return nullptr;
    }
    return entry;
}
bool identity_metadata(int directory,const char *path,const char *base,Stat &metadata,bool &exists) {
    exists=false;
    Wide full(path),name(base);
    if (!full.data || !name.data) return false;
    if (wcspbrk(name.data,L"<>:\"/\\|?*")) { errno=EINVAL; return false; }
    WIN32_FIND_DATAW found;
    HANDLE search=FindFirstFileW(full.data,&found);
    if (search==INVALID_HANDLE_VALUE) {
        DWORD error=GetLastError();
        if (error==ERROR_FILE_NOT_FOUND || error==ERROR_PATH_NOT_FOUND) return true;
        failure(error); return false;
    }
    if (!FindClose(search)) { failure(); return false; }
    size_t length=wcslen(found.cFileName);
    if (!length) { errno=EIO; return false; }
    alignas(8) unsigned char storage[8192];
    FILE_INFO_BY_HANDLE_CLASS query=FileIdBothDirectoryRestartInfo;
    for (;;) {
        memset(storage,0,sizeof(storage));
        if (!GetFileInformationByHandleEx(handle(directory),query,storage,sizeof(storage))) {
            DWORD error=GetLastError();
            if (error==ERROR_NO_MORE_FILES) return true;
            failure(error); return false;
        }
        query=FileIdBothDirectoryInfo;
        size_t offset=0;
        for (;;) {
            size_t remaining=sizeof(storage)-offset;
            constexpr size_t header=offsetof(FILE_ID_BOTH_DIR_INFO,FileName);
            if (remaining<header) { errno=EIO; return false; }
            const FILE_ID_BOTH_DIR_INFO *entry=reinterpret_cast<const FILE_ID_BOTH_DIR_INFO *>(storage+offset);
            size_t bytes=entry->NextEntryOffset?entry->NextEntryOffset:remaining;
            if (bytes<header || bytes>remaining || (entry->NextEntryOffset%8) ||
                !entry->FileNameLength || entry->FileNameLength%sizeof(wchar_t) ||
                entry->FileNameLength>bytes-header) { errno=EIO; return false; }
            if (entry->FileNameLength==length*sizeof(wchar_t) &&
                !memcmp(entry->FileName,found.cFileName,entry->FileNameLength)) {
                if (entry->FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT) { errno=ELOOP; return false; }
                if (entry->FileAttributes&FILE_ATTRIBUTE_DIRECTORY) { errno=EINVAL; return false; }
                metadata.st_ino=uint64_t(entry->FileId.QuadPart);
                exists=true;
                return true;
            }
            if (!entry->NextEntryOffset) break;
            offset+=entry->NextEntryOffset;
        }
    }
}
#endif
int lock_directory(int parent) {
#ifdef _WIN32
    static const wchar_t name[]=L".reliefoffice-locks";
    HANDLE directory=relative_open(parent,name,sizeof(name)/sizeof(name[0])-1,
        FILE_LIST_DIRECTORY|FILE_READ_ATTRIBUTES|READ_CONTROL,FILE_OPEN_IF,
        FILE_DIRECTORY_FILE|FILE_OPEN_REPARSE_POINT);
    return directory==INVALID_HANDLE_VALUE?-1:adopt(directory,O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
#else
    bool created=::mkdirat(parent,".reliefoffice-locks",0777)==0;
    if (!created && errno!=EEXIST) return -1;
    int directory=::openat(parent,".reliefoffice-locks",O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
    if (directory<0) return -1;
    struct ::stat parent_info;
    if (created && (::fstat(parent,&parent_info)<0 || ::fchmod(directory,(parent_info.st_mode&0777)|S_ISVTX)<0)) {
        int saved=errno; ::close(directory); errno=saved; return -1;
    }
    return directory;
#endif
}
int lock_record(int directory,const char *path) {
#ifdef _WIN32
    Wide name(path);
    if (!name.data) return -1;
    HANDLE record=relative_open(directory,name.data,name.length,
        FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL,FILE_OPEN_IF,
        FILE_NON_DIRECTORY_FILE|FILE_OPEN_REPARSE_POINT);
    return record==INVALID_HANDLE_VALUE?-1:adopt(record,O_RDONLY|O_NOFOLLOW);
#else
    int record=::openat(directory,path,O_RDONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK,0444);
    if (record<0) return errno==EEXIST?::openat(directory,path,O_RDONLY|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK):-1;
    if (::fchmod(record,0444)<0) { int saved=errno; ::close(record); errno=saved; return -1; }
    return record;
#endif
}
}
SaveLock::~SaveLock() {
    int saved=errno;
#ifdef _WIN32
    if (identity_mutex_) unlock_identity(static_cast<IdentityMutex *>(identity_mutex_));
#endif
    if (identity_>=0) close(identity_);
    if (path_>=0) close(path_);
    errno=saved;
}
bool SaveLock::acquire(const char *path) {
    if (path_>=0 || identity_>=0 || !path || !*path) { errno=EINVAL; return false; }
    const char *separator=last_separator(path);
    const char *base=separator?separator+1:path;
    if (!*base || !strcmp(base,".") || !strcmp(base,"..")) { errno=EINVAL; return false; }
    size_t length=separator?size_t(separator-path):1;
    if (separator && (!length || (length==2 && path[1]==':') ||
        (length==3 && path[0]=='/' && path[2]==':'))) ++length;
    char *parent=static_cast<char *>(malloc(length+1));
    if (!parent) { errno=ENOMEM; return false; }
    if (separator) memcpy(parent,path,length); else parent[0]='.';
    parent[length]=0;
    int directory=open(parent,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    free(parent);
    if (directory<0) return false;
    int locks=lock_directory(directory);
    bool ok=locks>=0;
    Stat metadata;
    if (ok && fstat(locks,&metadata)<0) ok=false;
    if (ok && !S_ISDIR(metadata.st_mode)) { errno=ENOTDIR; ok=false; }
    if (ok) {
        path_=lock_record(locks,base);
        if (path_<0 || fstat(path_,&metadata)<0) ok=false;
    }
    if (ok && (!S_ISREG(metadata.st_mode) || metadata.st_nlink!=1)) { errno=EACCES; ok=false; }
    if (ok && flock(path_,LOCK_EX|LOCK_NB)<0) ok=false;
#ifdef _WIN32
    if (ok) {
        bool exists=false;
        ok=identity_metadata(directory,path,base,metadata,exists);
        if (ok && exists) {
            identity_mutex_=lock_identity(metadata);
            ok=identity_mutex_!=nullptr;
        }
    }
#else
    if (ok) {
        identity_=openat(directory,base,O_RDONLY|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK);
        if (identity_<0) ok=errno==ENOENT;
        else {
            if (fstat(identity_,&metadata)<0) ok=false;
            if (ok && !S_ISREG(metadata.st_mode)) { errno=EINVAL; ok=false; }
            if (ok) {
                if (::flock(identity_,LOCK_EX|LOCK_NB)<0) ok=false;
            }
        }
    }
#endif
    int saved=errno;
    if (locks>=0) close(locks);
    close(directory);
    errno=saved;
    return ok;
}
const char *last_separator(const char *path) {
    if (!path) return nullptr;
    const char *last = nullptr;
    for (; *path; ++path) if (*path == '/' || *path == '\\') last = path;
    return last;
}
bool absolute_path(const char *path) {
    if (!path || !*path) return false;
    if (*path == '/' || *path == '\\') return true;
    return path[1] == ':' && (path[2] == '/' || path[2] == '\\');
}
int root_directory(const char *path, size_t &consumed) {
    consumed = 0;
    if (!path) { errno = EINVAL; return -1; }
    size_t length = strlen(path), root = 0;
    if (length >= 3 && path[1] == ':' && (path[2] == '/' || path[2] == '\\')) root = 3;
    else if (length >= 4 && path[0] == '/' && path[2] == ':' && path[3] == '/') root = 4;
    else if (length >= 2 && ((path[0] == '/' && path[1] == '/') || (path[0] == '\\' && path[1] == '\\'))) {
        root = 2;
        for (unsigned component = 0; component < 2; ++component) {
            size_t start = root;
            while (root < length && path[root] != '/' && path[root] != '\\') ++root;
            if (root == start) { errno = EINVAL; return -1; }
            if (root < length) ++root;
        }
    } else if (*path == '/' || *path == '\\') root = 1;
    if (!root) return open(".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    char *base = static_cast<char *>(malloc(root + 1));
    if (!base) { errno = ENOMEM; return -1; }
    memcpy(base, path, root); base[root] = 0;
    int fd = open(base, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    free(base);
    if (fd >= 0) consumed = root;
    return fd;
}
}
