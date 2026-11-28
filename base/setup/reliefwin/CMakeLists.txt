
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
    ${REACTOS_SOURCE_DIR}/sdk/include/reliefwin/libs/zlib
#    ${REACTOS_SOURCE_DIR}/base/setup/lib
    ${REACTOS_SOURCE_DIR}/sdk/include/reliefwin/drivers
    ${REACTOS_SOURCE_DIR}/sdk/lib/tzlib)

list(APPEND SOURCE
    spapisup/fileqsup.c
    spapisup/infsupp.c
    account.c
    identity.c
    alongside.c
    diskinfo.c
    diskpage.c
    filesec.c
    frame.c
    hostops.c
    hostpage.c
    langpage.c
    reliefwin.c
    selftest.c
    updbcd.c
    upgrade.c
    wizard.c
    reliefwin.h)

file(GLOB reliefwin_rc_deps res/*.*)
list(APPEND reliefwin_rc_deps
    ${REACTOS_SOURCE_DIR}/base/system/userinit/res/rosbitmap.bmp
    ${REACTOS_SOURCE_DIR}/base/system/userinit/res/rosbitmap_mask.bmp
    ${REACTOS_SOURCE_DIR}/image/data/filesecurity.txt)
add_rc_deps(reliefwin.rc ${reliefwin_rc_deps})
add_executable(reliefwin ${SOURCE} reliefwin.rc)

add_pch(reliefwin reliefwin.h SOURCE)
set_module_type(reliefwin win32gui UNICODE)
target_link_libraries(reliefwin uuid tzlib volresize)
add_delay_importlibs(reliefwin setuplib)
add_importlibs(reliefwin advapi32 gdi32 user32 comctl32 dwmapi shlwapi setupapi ole32 oleaut32 version bcrypt msvcrt kernel32 ntdll)
add_cd_file(TARGET reliefwin DESTINATION reactos NO_CAB FOR bootcd livecd)
add_cd_file(TARGET reliefwin DESTINATION ${ARCH} FOR preinstall)
add_cd_file(TARGET reliefwin DESTINATION root NO_CAB NAME_ON_CD setup.exe FOR bootcd)

list(APPEND OOBE_SOURCE
    identity.c
    apps.c
    oobe.c
    experience.cpp
    reliefwin.h)

add_rc_deps(oobe.rc ${reliefwin_rc_deps} ${CMAKE_CURRENT_SOURCE_DIR}/experience.rc)
add_executable(oobe ${OOBE_SOURCE} oobe.rc)
set_property(TARGET oobe PROPERTY CXX_STANDARD 17)
set_property(TARGET oobe PROPERTY CXX_STANDARD_REQUIRED ON)
set_module_type(oobe win32gui UNICODE)
target_link_libraries(oobe shellframe cppstl cpprt uuid tzlib)
add_importlibs(oobe advapi32 gdi32 gdiplus user32 comctl32 dwmapi shell32 uxtheme shlwapi iphlpapi ws2_32 setupapi ole32 oleaut32 msi wintrust crypt32 bcrypt netapi32 samlib userenv syssetup msvcrt kernel32 ntdll)
add_cd_file(TARGET oobe DESTINATION reactos/system32/oobe FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/oobeapps.inf DESTINATION reactos/system32/oobe FOR all)

add_executable(oobe_test tests/runtime.c tests/runtime.rc)
set_module_type(oobe_test win32cui UNICODE)
add_importlibs(oobe_test advapi32 netapi32 comctl32 uxtheme gdi32 user32 msvcrt kernel32)

list(APPEND UPDATE_SOURCE
    frame.c
    updagent.c
    updbcd.c
    updzip.c
    reliefwin.h)

add_rc_deps(lntupdate.rc ${reliefwin_rc_deps})
add_executable(lntupdate ${UPDATE_SOURCE} lntupdate.rc)
add_pch(lntupdate reliefwin.h UPDATE_SOURCE)
set_module_type(lntupdate win32gui UNICODE)
target_link_libraries(lntupdate uuid zlib)
add_importlibs(lntupdate advapi32 gdi32 user32 comctl32 dwmapi shell32 shlwapi setupapi ole32 oleaut32 bcrypt msvcrt kernel32 ntdll)
add_cd_file(TARGET lntupdate DESTINATION reactos/system32 FOR all)
add_registry_inf(lntupdate.inf)
