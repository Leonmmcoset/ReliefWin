/*
 * PROJECT:     ReliefOffice
 * LICENSE:     Apache-2.0
 * PURPOSE:     Embedded UTF-8 translation interface
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__GNUC__)
__attribute__((format_arg(1)))
#endif
const char *reliefoffice_gettext(const char *message);
void reliefoffice_init_locale(const char *application);
#ifdef __cplusplus
}
#endif
#define gettext reliefoffice_gettext
