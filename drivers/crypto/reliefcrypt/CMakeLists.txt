spec2def(reliefcrypt.sys reliefcrypt.spec)
add_library(reliefcrypt MODULE driver.c control.c io.c dump.c reliefcrypt.rc
    ${CMAKE_CURRENT_BINARY_DIR}/reliefcrypt.def)
set_module_type(reliefcrypt kernelmodedriver)
target_link_libraries(reliefcrypt volumecrypt_crypto volumecrypt wdmsec)
add_importlibs(reliefcrypt ntoskrnl hal)
add_registry_inf(reliefcrypt_reg.inf)
add_cd_file(TARGET reliefcrypt DESTINATION reactos/system32/drivers NO_CAB FOR all)

add_executable(reliefcrypt_test tests/io.c tests/data.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/volumecrypt/tests/windows.c)
set_module_type(reliefcrypt_test win32cui)
target_link_libraries(reliefcrypt_test volumecrypt_crypto volumecrypt)
add_importlibs(reliefcrypt_test bcrypt msvcrt kernel32)
