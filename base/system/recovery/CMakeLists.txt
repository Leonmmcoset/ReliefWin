add_rc_deps(recovery.rc ${CMAKE_CURRENT_SOURCE_DIR}/resource.h)
add_executable(recoverycenter main.c operations.c recovery.rc)
set_module_type(recoverycenter win32gui UNICODE)
add_importlibs(recoverycenter advapi32 comctl32 comdlg32 shell32 ole32 user32 gdi32 msvcrt kernel32)
add_cd_file(TARGET recoverycenter DESTINATION reactos/system32 FOR all)

add_executable(recovery_test tests.c operations.c)
set_module_type(recovery_test win32cui UNICODE)
add_importlibs(recovery_test advapi32 msvcrt kernel32)
