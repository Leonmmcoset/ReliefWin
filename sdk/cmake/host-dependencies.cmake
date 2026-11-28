# PROJECT:     ReliefWin build tools
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Validate the common host tool versions
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

cmake_minimum_required(VERSION 3.24)

find_program(RELIEFWIN_PYTHON_EXECUTABLE NAMES python3 python REQUIRED NO_CMAKE_FIND_ROOT_PATH)
execute_process(
    COMMAND "${RELIEFWIN_PYTHON_EXECUTABLE}" -c "import sys; print(sys.version.split()[0]); sys.exit(sys.version_info < (3, 9))"
    RESULT_VARIABLE _reliefwin_python_status
    OUTPUT_VARIABLE _reliefwin_python_version
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT _reliefwin_python_status STREQUAL "0")
    message(FATAL_ERROR "ReliefWin needs Python 3.9 or newer; ${RELIEFWIN_PYTHON_EXECUTABLE} reports ${_reliefwin_python_version}")
endif()
