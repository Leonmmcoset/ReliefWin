# PROJECT:     ReliefWin host-native tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Branding regression test build configuration
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

find_package(Python3 QUIET COMPONENTS Interpreter)
find_program(BRANDING_CXX NAMES c++ g++)
if(Python3_Interpreter_FOUND)
    add_test(NAME branding.source_audit_guards COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/audit-test.py --source-only)
    set_tests_properties(branding.source_audit_guards PROPERTIES TIMEOUT 60)
    set(branding_native_tools "${REACTOS_HOST_SOURCE_ROOT}/output-Clang-amd64-debug/host-tools/bin")
    if(EXISTS "${branding_native_tools}/mkhive" AND EXISTS "${branding_native_tools}/utf16le")
        execute_process(COMMAND ${Python3_EXECUTABLE} -c "import Registry" RESULT_VARIABLE branding_registry_module ERROR_QUIET)
        if(branding_registry_module EQUAL 0)
            add_test(NAME branding.start_shortcut_hive_encoding COMMAND ${Python3_EXECUTABLE}
                ${REACTOS_HOST_SOURCE_ROOT}/scripts/test_start_shortcuts.py --verify-regression)
            set_tests_properties(branding.start_shortcut_hive_encoding PROPERTIES TIMEOUT 60)
        endif()
    endif()
    set(branding_icu_module "${REACTOS_HOST_SOURCE_ROOT}/output-Clang-amd64-debug/submodules/icu-payload/72.1-1/system32/icu.dll")
    if(EXISTS "${branding_icu_module}")
        add_test(NAME branding.pe_audit_guards COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/audit-test.py --icu-module "${branding_icu_module}")
        set_tests_properties(branding.pe_audit_guards PROPERTIES TIMEOUT 60)
    endif()
endif()
if(Python3_Interpreter_FOUND AND BRANDING_CXX)
    add_test(NAME branding.setup_language_precedence COMMAND ${Python3_EXECUTABLE}
        ${REACTOS_HOST_SOURCE_ROOT}/scripts/test_setup_language.py
        --cxx ${BRANDING_CXX} --verify-regression)
    set_tests_properties(branding.setup_language_precedence PROPERTIES TIMEOUT 60)
    add_test(NAME branding.installed_release_and_update COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/branding-test.py --root ${REACTOS_HOST_SOURCE_ROOT}
        --cxx ${BRANDING_CXX} --verify-regression)
    set_tests_properties(branding.installed_release_and_update PROPERTIES TIMEOUT 60)
endif()
