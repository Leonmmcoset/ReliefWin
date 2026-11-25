/*
 * PROJECT:     ReliefMath
 * LICENSE:     Apache-2.0 (https://spdx.org/licenses/Apache-2.0)
 * PURPOSE:     Resolve installed formula renderer and font resources
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include "runtime.h"
#include "render_types.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>
namespace rm {
namespace {
bool executable_directory(char *path, size_t capacity) {
    wchar_t wide[32768];
    DWORD length = GetModuleFileNameW(nullptr, wide, 32768);
    if (!length || length >= 32768) return false;
    while (length && wide[length - 1] != L'\\' && wide[length - 1] != L'/') --length;
    if (!length) return false;
    wide[length - 1] = 0;
    return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1,
        path, static_cast<int>(capacity), nullptr, nullptr) != 0;
}
}
const char *worker_path() {
    static char path[32768];
    if (!*path) {
        char directory[32768];
        if (!executable_directory(directory, sizeof(directory)) ||
            snprintf(path, sizeof(path), "%s/reliefmath-render.exe", directory) >= int(sizeof(path)))
            *path = 0;
    }
    return path;
}
const char *resource_path() {
    static char path[32768];
    if (!*path) {
        char directory[32768];
        if (!executable_directory(directory, sizeof(directory)) ||
            snprintf(path, sizeof(path), "%s/ReliefMath", directory) >= int(sizeof(path)))
            *path = 0;
    }
    return path;
}
const char *text_font_path() {
    static char path[32768];
    if (!*path) {
        wchar_t wide[32768];
        UINT length = GetWindowsDirectoryW(wide, 32768);
        char directory[32768];
        if (!length || length >= 32768 ||
            !WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1,
                directory, sizeof(directory), nullptr, nullptr) ||
            snprintf(path, sizeof(path), "%s/Fonts/NotoSansSC-Regular.otf", directory) >= int(sizeof(path)))
            *path = 0;
    }
    return path;
}
void open_directory(const char *path) {
    wchar_t wide[32768];
    if (path && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, 32768))
        ShellExecuteW(nullptr, L"open", wide, nullptr, nullptr, SW_SHOWNORMAL);
}
bool copy_raster(const Raster &raster, void *owner) {
    if (!owner || !raster.width || !raster.height ||
        raster.width > 16384 || raster.height > 16384 ||
        raster.stride != raster.width * 4 ||
        raster.rgba.size() != size_t(raster.stride) * raster.height) return false;
    SIZE_T pixels = raster.rgba.size();
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPV5HEADER) + pixels);
    if (!memory) return false;
    auto *header = static_cast<BITMAPV5HEADER *>(GlobalLock(memory));
    if (!header) { GlobalFree(memory); return false; }
    memset(header, 0, sizeof(*header));
    header->bV5Size = sizeof(*header);
    header->bV5Width = raster.width;
    header->bV5Height = -static_cast<LONG>(raster.height);
    header->bV5Planes = 1;
    header->bV5BitCount = 32;
    header->bV5Compression = BI_BITFIELDS;
    header->bV5SizeImage = static_cast<DWORD>(pixels);
    header->bV5RedMask = 0x00ff0000;
    header->bV5GreenMask = 0x0000ff00;
    header->bV5BlueMask = 0x000000ff;
    header->bV5AlphaMask = 0xff000000;
    header->bV5CSType = LCS_sRGB;
    auto *output = reinterpret_cast<unsigned char *>(header + 1);
    const unsigned char *input = raster.rgba.data();
    for (size_t index = 0; index < pixels; index += 4) {
        output[index] = input[index + 2];
        output[index + 1] = input[index + 1];
        output[index + 2] = input[index];
        output[index + 3] = input[index + 3];
    }
    GlobalUnlock(memory);
    if (!OpenClipboard(static_cast<HWND>(owner))) { GlobalFree(memory); return false; }
    bool okay = EmptyClipboard() && SetClipboardData(CF_DIBV5, memory);
    CloseClipboard();
    if (!okay) GlobalFree(memory);
    return okay;
}
}
