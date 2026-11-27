# PROJECT:     ReactOS CSPRNG Library
# LICENSE:     MIT (https://spdx.org/licenses/MIT)
# PURPOSE:     User-mode interface to the kernel random number generator
# COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif193@gmail.com>

include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reliefwin/drivers/ksecdd)

list(APPEND SOURCE
    csprng.c
    ${REACTOS_SOURCE_DIR}/dll/3rdparty/mbedtls/chacha20.c)

add_library(csprng ${SOURCE})
target_include_directories(csprng PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${REACTOS_SOURCE_DIR}/sdk/include/reliefwin/libs)
target_compile_definitions(csprng PRIVATE MBEDTLS_CONFIG_FILE="csprng_config.h")
add_dependencies(csprng psdk xdk)

set(_bearssl ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/bearssl)
add_library(entropy_drbg STATIC drbg.c
    ${_bearssl}/src/rand/hmac_drbg.c
    ${_bearssl}/src/mac/hmac.c
    ${_bearssl}/src/hash/sha2small.c
    ${_bearssl}/src/hash/sha2big.c
    ${_bearssl}/src/codec/enc32be.c
    ${_bearssl}/src/codec/dec32be.c
    ${_bearssl}/src/codec/enc64be.c
    ${_bearssl}/src/codec/dec64be.c)
target_include_directories(entropy_drbg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${_bearssl}/inc)
target_include_directories(entropy_drbg PRIVATE ${_bearssl}/src)
add_dependencies(entropy_drbg psdk)

add_executable(entropy_test tests.c)
set_module_type(entropy_test win32cui)
target_link_libraries(entropy_test entropy_drbg)
add_importlibs(entropy_test msvcrt kernel32)
