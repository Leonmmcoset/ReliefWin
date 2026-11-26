
list(APPEND SOURCE
    CaptionGlyphs.c
    CloseThemeData.c
    ColorPreference.c
    DrawThemeParentBackground.c
    GetThemeParseErrorInfo.c
    MenuTheme.c
    ModernMenu.c
    NativeScrollbarTheme.c
    NonClientBorder.c
    NonClientCaption.c
    NonClientHitTest.c
    PreferredAppMode.c
    RoundedControls.c
    SetWindowTheme.c
    TextScaling.c
    ThemeChangeRedraw.c
    ThemeDpi.c
    SetThemeAppProperties.c
    ThemeRendering.c
    ThemeShellTransition.c
    ThemeSysSize.c
    TrackbarCenter.c
    TrackbarPaintLifetime.c
    ../include/msgtrace.c
    testlist.c)

add_executable(uxtheme_apitest ${SOURCE} RoundedControls.rc)
target_link_libraries(uxtheme_apitest wine ${PSEH_LIB})
set_module_type(uxtheme_apitest win32cui)
add_importlibs(uxtheme_apitest uxtheme dwmapi comctl32 user32 gdi32 msvcrt kernel32)
add_rostests_file(TARGET uxtheme_apitest)
