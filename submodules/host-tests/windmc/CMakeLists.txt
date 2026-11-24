# PROJECT:     ReliefWin host-native tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Message-header comment regression test build configuration
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME windmc.comment_headers COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/comment-headers-test.py --root ${REACTOS_HOST_SOURCE_ROOT}
        --cc ${CMAKE_C_COMPILER} --verify-regression)
    set_tests_properties(windmc.comment_headers PROPERTIES TIMEOUT 60)
endif()
