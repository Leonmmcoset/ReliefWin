#pragma once

#ifdef LANGUAGE_EN_US
    #include "lang/en-US.h"
#endif
#ifdef LANGUAGE_JA_JP
    #include "lang/ja-JP.h"
#endif
#ifdef LANGUAGE_ZH_CN
    #include "lang/zh-CN.h"
#endif

const MUI_LANGUAGE MUILanguageList[] =
{
  /* Lang ID,     ANSI CP, OEM CP, MAC CP, Language Name, GeoID, Fonts, KB Layouts */
#ifdef LANGUAGE_ZH_CN
    {L"00000804", 936, 936, 10008, L"Chinese (PRC)", 45, ChineseSimplifiedFonts, zhCNLayouts},
#endif
#ifdef LANGUAGE_EN_US
    {L"00000409", 1252, 437, 10000, L"English (United States)", 244, LatinFonts, enUSLayouts},
#endif
#ifdef LANGUAGE_JA_JP
    {L"00000411", 932, 932, 10001, L"Japanese", 122, JapaneseFonts, jaJPLayouts},
#endif
    {NULL, 0, 0, 0, NULL, 0, NULL, NULL}
};
