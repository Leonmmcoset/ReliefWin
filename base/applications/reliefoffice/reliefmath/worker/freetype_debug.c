/*
 * PROJECT:     ReliefMath
 * LICENSE:     Apache-2.0 (https://spdx.org/licenses/Apache-2.0)
 * PURPOSE:     User mode diagnostics for the shared FreeType engine
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/internal/ftdebug.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
FT_BASE_DEF(void) FT_Message(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
}
FT_BASE_DEF(void) FT_Panic(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fflush(stderr);
    abort();
}
