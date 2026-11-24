# PROJECT:     ReliefWin host-native tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Printer ANSI conversion and failure propagation regression tests
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME winspool.ansi_conversion COMMAND ${Python3_EXECUTABLE}
        ${REACTOS_HOST_SOURCE_ROOT}/scripts/test_printer_ansi.py --cc ${CMAKE_C_COMPILER})
    set_tests_properties(winspool.ansi_conversion PROPERTIES TIMEOUT 60)
endif()
