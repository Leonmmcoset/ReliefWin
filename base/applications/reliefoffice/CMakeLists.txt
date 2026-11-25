add_library(reliefoffice_cppstl INTERFACE)
target_link_libraries(reliefoffice_cppstl INTERFACE "$<LINK_ONLY:cppstl>")
include_directories(BEFORE ${REACTOS_SOURCE_DIR}/sdk/include/ucrt)
add_compile_definitions(_DLL _UCRT _CRT_DECLARE_NONSTDC_NAMES=1 _CRT_NONSTDC_NO_DEPRECATE)
include(dependencies.cmake)
find_program(RELIEFOFFICE_PYTHON NAMES python3 REQUIRED)
file(GLOB _office_catalogs ${CMAKE_CURRENT_SOURCE_DIR}/locale/*/zh_CN.po)
add_custom_command(OUTPUT ${_generated}/catalogs.inc
    COMMAND ${RELIEFOFFICE_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/localization/catalogs.py
        ${CMAKE_CURRENT_SOURCE_DIR}/locale ${_generated}/catalogs.inc
    DEPENDS ${_office_catalogs} localization/catalogs.py VERBATIM)
add_library(reliefoffice_locale STATIC localization/locale.cpp ${_generated}/catalogs.inc)
target_include_directories(reliefoffice_locale PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/localization PRIVATE ${_generated})
target_link_libraries(reliefoffice_locale PUBLIC libkernel32)
add_library(reliefoffice_platform STATIC platform/posix.cpp platform/worker.cpp)
set_target_properties(reliefoffice_platform PROPERTIES CXX_STANDARD 17)
target_include_directories(reliefoffice_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/platform)
target_link_libraries(reliefoffice_platform PUBLIC reliefoffice_cppstl libkernel32 libadvapi32 libshell32 libntdll)
set(_office_apps reliefdiagram reliefexcel reliefmathpp reliefppt relieftodo reliefword)
set_property(GLOBAL APPEND PROPERTY BOOTCD_SYSTEM_TREES
    "\"deployment\\ReliefOffice\" = \"\\Program Files\\ReliefOffice\"")
foreach(_app IN LISTS _office_apps)
    file(GLOB _sources CONFIGURE_DEPENDS ${_app}/*.cpp)
    if(_app STREQUAL reliefdiagram OR _app STREQUAL reliefmathpp)
        list(APPEND _sources relieftodo/file_io.cpp)
    endif()
    add_rc_deps(${_app}/app.rc ${CMAKE_CURRENT_SOURCE_DIR}/icons/${_app}.ico)
    add_executable(${_app} ${_sources} entry.c ${_app}/app.rc)
    set_target_properties(${_app} PROPERTIES CXX_STANDARD 17)
    set_module_type(${_app} win32gui)
    target_link_libraries(${_app} reliefoffice_fltk reliefoffice_locale reliefoffice_cppstl)
    target_link_libraries(${_app} reliefoffice_platform)
    add_importlibs(${_app} user32 gdi32 comdlg32 comctl32 shell32 advapi32 ole32 ucrtbase kernel32 ntdll)
    add_cd_file(TARGET ${_app} DESTINATION "Program Files/ReliefOffice" FOR preinstall)
    add_cd_file(TARGET ${_app} DESTINATION "deployment/ReliefOffice" FOR bootcd)
endforeach()
include(math.cmake)
set(_format_sources reliefword/model.cpp reliefword/document.cpp
    reliefexcel/model.cpp reliefexcel/document.cpp reliefexcel/formula.cpp
    reliefppt/model.cpp reliefppt/document.cpp reliefppt/geometry.cpp
    relieftodo/model.cpp relieftodo/document.cpp
    reliefdiagram/model.cpp reliefdiagram/document.cpp
    reliefmathpp/model.cpp reliefmathpp/document.cpp reliefmathpp/expression.cpp reliefmathpp/analysis.cpp
    reliefmath/model.cpp reliefmath/document.cpp)
add_executable(reliefoffice_formats tests/format-fixtures.cpp ${_format_sources})
set_target_properties(reliefoffice_formats PROPERTIES CXX_STANDARD 17)
set_module_type(reliefoffice_formats win32cui)
target_include_directories(reliefoffice_formats PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(reliefoffice_formats reliefoffice_platform reliefoffice_locale reliefoffice_cppstl)
add_importlibs(reliefoffice_formats advapi32 shell32 ucrtbase kernel32 ntdll)
set(_io_sources ${_format_sources})
list(FILTER _io_sources EXCLUDE REGEX "^reliefmath/")
add_executable(reliefoffice_io tests/office-io.cpp ${_io_sources} reliefexcel/edit.cpp relieftodo/file_io.cpp)
set_target_properties(reliefoffice_io PROPERTIES CXX_STANDARD 17)
set_module_type(reliefoffice_io win32cui)
target_include_directories(reliefoffice_io PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(reliefoffice_io reliefoffice_platform reliefoffice_locale reliefoffice_cppstl)
add_importlibs(reliefoffice_io advapi32 shell32 ucrtbase kernel32 ntdll)
add_executable(reliefoffice-maintenance maintenance/uninstall.c)
set_module_type(reliefoffice-maintenance win32gui UNICODE)
target_link_libraries(reliefoffice-maintenance uuid)
add_importlibs(reliefoffice-maintenance advapi32 shell32 shlwapi user32 ole32 ucrtbase kernel32)
add_cd_file(TARGET reliefoffice-maintenance DESTINATION "Program Files/ReliefOffice" FOR preinstall)
add_cd_file(TARGET reliefoffice-maintenance DESTINATION "deployment/ReliefOffice" FOR bootcd)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/maintenance/reliefoffice.inf
    DESTINATION "Program Files/ReliefOffice" FOR preinstall)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/maintenance/reliefoffice.inf
    DESTINATION "deployment/ReliefOffice" FOR bootcd)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/UPSTREAM_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/UPSTREAM_NOTICE
    ${CMAKE_CURRENT_SOURCE_DIR}/THIRD_PARTY_LICENSES.txt
    DESTINATION "Program Files/ReliefOffice" FOR preinstall)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/UPSTREAM_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/UPSTREAM_NOTICE
    ${CMAKE_CURRENT_SOURCE_DIR}/THIRD_PARTY_LICENSES.txt
    DESTINATION "deployment/ReliefOffice" FOR bootcd)
add_executable(reliefoffice_install_smoke tests/install-smoke.c)
set_module_type(reliefoffice_install_smoke win32cui)
target_link_libraries(reliefoffice_install_smoke uuid)
add_importlibs(reliefoffice_install_smoke advapi32 shell32 ole32 user32 ucrtbase kernel32)
add_executable(reliefoffice_uninstall_smoke tests/uninstall-smoke.c)
set_module_type(reliefoffice_uninstall_smoke win32cui)
target_link_libraries(reliefoffice_uninstall_smoke uuid)
add_importlibs(reliefoffice_uninstall_smoke advapi32 shell32 ole32 user32 ucrtbase kernel32)
add_executable(reliefoffice_reopen_file tests/reopen-testlist.c
    ${REACTOS_SOURCE_DIR}/modules/rostests/apitests/kernel32/ReOpenFile.c)
target_include_directories(reliefoffice_reopen_file PRIVATE ${REACTOS_SOURCE_DIR}/modules/rostests/apitests/include)
target_link_libraries(reliefoffice_reopen_file wine ${PSEH_LIB})
set_module_type(reliefoffice_reopen_file win32cui)
add_importlibs(reliefoffice_reopen_file ucrtbase kernel32 ntdll)
