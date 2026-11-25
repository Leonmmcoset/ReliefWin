/*
 * PROJECT:     ReliefOffice
 * LICENSE:     Apache-2.0
 * PURPOSE:     Pass Unicode shell command lines to UTF-8 application entry points
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

extern int main(int, char **);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command, int show)
{
    int argc = 0, index, result = 1;
    WCHAR **wide = CommandLineToArgvW(GetCommandLineW(), &argc);
    char **argv;
    if (!wide) return result;
    argv = calloc((size_t)argc + 1, sizeof(*argv));
    if (!argv) { LocalFree(wide); return result; }
    for (index = 0; index < argc; ++index) {
        int length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[index], -1, NULL, 0, NULL, NULL);
        if (!length || !(argv[index] = malloc(length)) ||
            !WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[index], -1, argv[index], length, NULL, NULL))
            break;
    }
    if (index == argc) result = main(argc, argv);
    for (index = 0; index < argc; ++index) free(argv[index]);
    free(argv);
    LocalFree(wide);
    return result;
}
