/*
 * PROJECT:     ReliefOffice
 * LICENSE:     Apache-2.0
 * PURPOSE:     Select embedded Simplified Chinese catalogs
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <windows.h>
#include <string.h>
#include "libintl.h"
#include "catalogs.inc"

static const OfficeCatalog *catalog;

void reliefoffice_init_locale(const char *application)
{
    catalog = nullptr;
    if (PRIMARYLANGID(GetUserDefaultUILanguage()) != LANG_CHINESE || !application)
        return;
    for (const auto &candidate : office_catalogs)
        if (!strcmp(candidate.name, application)) {
            catalog = &candidate;
            return;
        }
}

const char *reliefoffice_gettext(const char *message)
{
    if (!message) return "";
    if (!catalog) return message;
    unsigned first = 0, last = catalog->count;
    while (first < last) {
        unsigned middle = first + (last - first) / 2;
        int order = strcmp(message, catalog->entries[middle].id);
        if (!order) return catalog->entries[middle].text;
        if (order < 0) last = middle;
        else first = middle + 1;
    }
    return message;
}
