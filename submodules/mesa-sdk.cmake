# PROJECT:     ReliefWin build integration
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Prepare and restore the Mesa feed's SDK include selection
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

find_program(_mesa_git NAMES git REQUIRED)
execute_process(COMMAND "${_mesa_git}" -C "${MESA_SOURCE_DIR}" rev-parse --git-path reliefwin-sdk.lock
    OUTPUT_VARIABLE _mesa_lock OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)
get_filename_component(_mesa_lock "${_mesa_lock}" ABSOLUTE BASE_DIR "${MESA_SOURCE_DIR}")
file(LOCK "${_mesa_lock}" GUARD PROCESS TIMEOUT 30)

set(_mesa_file "${MESA_SOURCE_DIR}/CMakeLists.txt")
file(READ "${_mesa_file}" _mesa_cmake)
set(_mesa_original [=[if(EXISTS "${MESA_LIBERNT_SOURCE_DIR}/sdk/include/libernt/libs/zlib/zlib.h")]=])
set(_mesa_updated [=[if(EXISTS "${MESA_LIBERNT_SOURCE_DIR}/sdk/include/reliefwin/libs/zlib/zlib.h")
    set(MESA_LIBERNT_INCLUDE_DIR "${MESA_LIBERNT_SOURCE_DIR}/sdk/include/reliefwin")
elseif(EXISTS "${MESA_LIBERNT_SOURCE_DIR}/sdk/include/libernt/libs/zlib/zlib.h")]=])
string(FIND "${_mesa_cmake}" "${_mesa_updated}" _mesa_applied)
string(FIND "${_mesa_cmake}" "${_mesa_original}" _mesa_unpatched)
if(RESTORE)
    if(NOT _mesa_applied EQUAL -1)
        string(REPLACE "${_mesa_updated}" "${_mesa_original}" _mesa_cmake "${_mesa_cmake}")
        file(WRITE "${_mesa_file}" "${_mesa_cmake}")
    endif()
elseif(NOT _mesa_applied EQUAL -1)
    return()
elseif(NOT _mesa_unpatched EQUAL -1)
    string(REPLACE "${_mesa_original}" "${_mesa_updated}" _mesa_cmake "${_mesa_cmake}")
    file(WRITE "${_mesa_file}" "${_mesa_cmake}")
else()
    message(FATAL_ERROR "The Mesa SDK include selection has changed; update submodules/mesa-sdk.cmake for this feed revision.")
endif()
