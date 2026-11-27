add_library(volumecrypt STATIC luks2.c json.c journal.c descriptor.c)
add_dependencies(volumecrypt psdk)

set(_symcrypt ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/symcrypt)
set(_crypto_sources
    equal.c hash.c hmac.c hmacsha256.c marvin32.c
    libmain.c pbkdf2.c pbkdf2_hmacsha256.c selftest.c sha256.c)
list(TRANSFORM _crypto_sources PREPEND ${_symcrypt}/lib/)
set(_bearssl ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/bearssl)
set(_aes_sources
    ${_bearssl}/src/codec/dec32le.c
    ${_bearssl}/src/symcipher/aes_ct64.c
    ${_bearssl}/src/symcipher/aes_ct64_enc.c
    ${_bearssl}/src/symcipher/aes_ct64_dec.c)
add_library(volumecrypt_crypto STATIC crypto.c xts.c ${_crypto_sources} ${_aes_sources})
target_include_directories(volumecrypt_crypto PRIVATE ${_symcrypt}/inc ${_symcrypt}/lib ${_bearssl}/inc ${_bearssl}/src)
target_compile_definitions(volumecrypt_crypto PRIVATE
    __WINE_PE_BUILD SYMCRYPT_PLATFORM_WINE=1 SYMCRYPT_IGNORE_PLATFORM=1)
add_dependencies(volumecrypt_crypto psdk)

add_executable(volumecrypt_test tests/core.c tests/windows.c)
set_module_type(volumecrypt_test win32cui)
target_link_libraries(volumecrypt_test volumecrypt_crypto volumecrypt)
add_importlibs(volumecrypt_test bcrypt msvcrt kernel32)
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(volumecrypt_crypto PRIVATE -O2)
    set_source_files_properties(${_crypto_sources} PROPERTIES
        COMPILE_OPTIONS "-Wno-missing-braces;-Wno-incompatible-pointer-types;-Wno-unused-function;-Wno-unused-variable")
endif()
