
#define STANDALONE
#include <apitest.h>

extern void func_CaptionGlyphs(void);
extern void func_CloseThemeData(void);
extern void func_ColorPreference(void);
extern void func_DrawThemeParentBackground(void);
extern void func_DwmCaptionButton(void);
extern void func_GetThemeParseErrorInfo(void);
extern void func_CustomCaptionInset(void);
extern void func_MenuTheme(void);
extern void func_MenuAppearance(void);
extern void func_MenuInteraction(void);
extern void func_MenuShowcase(void);
extern void func_NativeMenuAppearance(void);
extern void func_NativeMenuScroll(void);
extern void func_NativeScrollbarTheme(void);
extern void func_NativeScrollbarRendering(void);
extern void func_NativeScrollbarThemeShutdown(void);
extern void func_NonClientActivate(void);
extern void func_NonClientBorder(void);
extern void func_NonClientCaption(void);
extern void func_NonClientHitTest(void);
extern void func_ParentBackgroundClip(void);
extern void func_ParentBackgroundSiblings(void);
extern void func_PreferredAppMode(void);
extern void func_RoundedControls(void);
extern void func_RoundedControlsShowcase(void);
extern void func_RoundedControlLifetime(void);
extern void func_RoundedControlDpi(void);
extern void func_SetThemeAppProperties(void);
extern void func_SetWindowTheme(void);
extern void func_TextScaling(void);
extern void func_ThemeChangeRedraw(void);
extern void func_ThemeDpi(void);
extern void func_ThemeRendering(void);
extern void func_ThemeShellTransition(void);
extern void func_ThemeSysSize(void);
extern void func_TrackbarCenter(void);
extern void func_TrackbarBuddyLifetime(void);
extern void func_TrackbarPaintLifetime(void);

const struct test winetest_testlist[] =
{
    { "CaptionGlyphs", func_CaptionGlyphs },
    { "CloseThemeData", func_CloseThemeData },
    { "ColorPreference", func_ColorPreference },
    { "DrawThemeParentBackground", func_DrawThemeParentBackground },
    { "DwmCaptionButton", func_DwmCaptionButton },
    { "GetThemeParseErrorInfo", func_GetThemeParseErrorInfo },
    { "CustomCaptionInset", func_CustomCaptionInset },
    { "MenuTheme", func_MenuTheme },
    { "MenuAppearance", func_MenuAppearance },
    { "MenuInteraction", func_MenuInteraction },
    { "MenuShowcase", func_MenuShowcase },
    { "NativeMenuAppearance", func_NativeMenuAppearance },
    { "NativeMenuScroll", func_NativeMenuScroll },
    { "NativeScrollbarTheme", func_NativeScrollbarTheme },
    { "NativeScrollbarRendering", func_NativeScrollbarRendering },
    { "NativeScrollbarThemeShutdown", func_NativeScrollbarThemeShutdown },
    { "NonClientActivate", func_NonClientActivate },
    { "NonClientBorder", func_NonClientBorder },
    { "NonClientCaption", func_NonClientCaption },
    { "NonClientHitTest", func_NonClientHitTest },
    { "ParentBackgroundClip", func_ParentBackgroundClip },
    { "ParentBackgroundSiblings", func_ParentBackgroundSiblings },
    { "PreferredAppMode", func_PreferredAppMode },
    { "RoundedControls", func_RoundedControls },
    { "RoundedControlsShowcase", func_RoundedControlsShowcase },
    { "RoundedControlLifetime", func_RoundedControlLifetime },
    { "RoundedControlDpi", func_RoundedControlDpi },
    { "SetWindowTheme", func_SetWindowTheme },
    { "TextScaling",    func_TextScaling },
    { "ThemeChangeRedraw", func_ThemeChangeRedraw },
    { "SetThemeAppProperties", func_SetThemeAppProperties },
    { "ThemeDpi", func_ThemeDpi },
    { "ThemeRendering", func_ThemeRendering },
    { "ThemeShellTransition", func_ThemeShellTransition },
    { "ThemeSysSize", func_ThemeSysSize },
    { "TrackbarCenter", func_TrackbarCenter },
    { "TrackbarBuddyLifetime", func_TrackbarBuddyLifetime },
    { "TrackbarPaintLifetime", func_TrackbarPaintLifetime },
    { 0, 0 }
};
