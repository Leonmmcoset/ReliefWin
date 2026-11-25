/*
 * PROJECT:     ReliefOffice
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     UTF-8 file and recovery operations on Windows
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#ifndef RELIEFOFFICE_PLATFORM_POSIX_H
#define RELIEFOFFICE_PLATFORM_POSIX_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
#include <sys/stat.h>
#if !defined(_SSIZE_T_DEFINED) && !defined(_SSIZET_DEFINED)
typedef ptrdiff_t ssize_t;
#define _SSIZE_T_DEFINED
#define _SSIZET_DEFINED
#endif
#ifndef O_ACCMODE
#define O_ACCMODE (_O_WRONLY | _O_RDWR)
#endif
#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#define O_WRONLY _O_WRONLY
#define O_RDWR _O_RDWR
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define O_EXCL _O_EXCL
#define O_APPEND _O_APPEND
#endif
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 0x08000000
#define O_DIRECTORY 0x10000000
#define O_NOFOLLOW 0x20000000
#define O_NONBLOCK 0x40000000
#endif
#define AT_SYMLINK_NOFOLLOW 0x100
#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8
#define FD_CLOEXEC 1
#define F_GETFD 1
#define F_SETFD 2
#ifndef S_IFREG
#define S_IFREG 0100000
#define S_IFDIR 0040000
#define S_IFMT 0170000
#endif
#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif
#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISLNK
#define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
#endif
namespace ro {
using Mode = unsigned;
struct Time { int64_t tv_sec = 0; long tv_nsec = 0; };
struct Stat {
    uint64_t st_dev = 0, st_ino = 0, st_uid = 0, st_nlink = 0;
    int64_t st_size = 0, st_mtime = 0;
    Mode st_mode = 0;
    Time st_mtim, st_ctim;
};
struct Dir;
struct Dirent { char d_name[4096]; };
int open(const char *, int, Mode = 0600);
int openat(int, const char *, int, Mode = 0600);
int close(int);
int dup(int);
ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
int fstat(int, Stat *);
int stat(const char *, Stat *);
int lstat(const char *, Stat *);
int fstatat(int, const char *, Stat *, int);
int mkdir(const char *, Mode);
int mkdirat(int, const char *, Mode);
int chmod(const char *, Mode);
int fchmod(int, Mode);
int fsync(int);
int unlink(const char *);
int unlinkat(int, const char *, int);
int rename(const char *, const char *);
int renameat(int, const char *, int, const char *);
int rename_noreplace(const char *, const char *);
int renameat_noreplace(int, const char *, int, const char *);
int mkstemp(char *);
int flock(int, int);
int fcntl(int, int, int = 0);
uint64_t getuid();
uint64_t geteuid();
unsigned long getpid();
const char *getenv(const char *);
FILE *fopen(const char *, const char *);
Dir *opendir(const char *);
Dir *fdopendir(int);
Dirent *readdir(Dir *);
int closedir(Dir *);
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);
}
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <strings.h>
namespace ro {
using Mode = mode_t;
using Stat = struct ::stat;
using Dir = DIR;
using Dirent = struct ::dirent;
using ::open; using ::openat; using ::close; using ::dup; using ::read; using ::write;
using ::fstat; using ::stat; using ::lstat; using ::fstatat;
using ::mkdir; using ::mkdirat; using ::chmod; using ::fchmod; using ::fsync;
using ::unlink; using ::unlinkat; using ::rename; using ::renameat;
using ::mkstemp; using ::flock; using ::fcntl; using ::getuid; using ::geteuid;
using ::getpid; using ::getenv; using ::fopen; using ::opendir; using ::fdopendir; using ::readdir;
using ::closedir; using ::strcasecmp; using ::strncasecmp;
int rename_noreplace(const char *, const char *);
int renameat_noreplace(int, const char *, int, const char *);
}
#endif
namespace ro {
int set_document_mode(int, Mode);
const char *last_separator(const char *);
bool absolute_path(const char *);
int root_directory(const char *, size_t &);
double monotonic();
struct tm *localtime(const time_t *,struct tm *);
class SaveLock {
    int path_=-1,identity_=-1;
#ifdef _WIN32
    void *identity_mutex_=nullptr;
#endif
public:
    SaveLock()=default;
    SaveLock(const SaveLock &)=delete;
    SaveLock &operator=(const SaveLock &)=delete;
    ~SaveLock();
    bool acquire(const char *);
};
}
#endif
