set(_recovery_dir ${REACTOS_BINARY_DIR}/boot/recovery)
set(_recovery_wim ${_recovery_dir}/Winre.wim)
get_property(_recovery_files GLOBAL PROPERTY LIVECD_FILE_LIST)
set(_recovery_inputs)
foreach(_entry IN LISTS _recovery_files)
    string(REGEX REPLACE "^[^=]+=" "" _source "${_entry}")
    list(APPEND _recovery_inputs "${_source}")
endforeach()
foreach(_hive system software default sam security)
    list(APPEND _recovery_inputs ${REACTOS_BINARY_DIR}/boot/bootdata/recovery/${_hive})
endforeach()
add_custom_command(OUTPUT ${_recovery_wim}
    COMMAND ${CMAKE_COMMAND} -DMODE=list
        -DBASE=${REACTOS_BINARY_DIR}/boot/livecd.$<CONFIG>.lst
        -DOPTIONAL=${REACTOS_BINARY_DIR}/boot/livecd.optional.$<CONFIG>.lst
        -DOUTPUT=${_recovery_dir}/files.lst -DSYSTEM_DIR=Windows
        -P ${REACTOS_SOURCE_DIR}/sdk/cmake/optional_files.cmake
    COMMAND ${CMAKE_COMMAND} -DLIST=${_recovery_dir}/files.lst
        -DSTAGE=${_recovery_dir}/stage -DWIM=${_recovery_wim}
        -DHIVES=${REACTOS_BINARY_DIR}/boot/bootdata/recovery
        -DWIMAGE=$<TARGET_FILE:native-wimage>
        -P ${REACTOS_SOURCE_DIR}/image/recovery_pack.cmake
    DEPENDS livecd_files recovery_hives native-wimage ${_recovery_inputs}
        ${REACTOS_BINARY_DIR}/boot/livecd.$<CONFIG>.lst
        ${REACTOS_BINARY_DIR}/boot/livecd.optional.$<CONFIG>.lst
        ${REACTOS_SOURCE_DIR}/image/recovery_pack.cmake
    VERBATIM)
add_custom_target(recovery_wim DEPENDS ${_recovery_wim})
foreach(_media bootloadercd bootloaderwimcd)
    if(TARGET ${_media})
        add_dependencies(${_media} recovery_wim)
    endif()
endforeach()
add_cd_file(TARGET recovery_wim FILE ${_recovery_wim}
    DESTINATION Recovery/ReliefWin FOR preinstall)
add_cd_file(TARGET recovery_wim FILE ${_recovery_wim}
    DESTINATION deployment/Recovery/ReliefWin FOR bootcd)
set_property(GLOBAL APPEND PROPERTY BOOTCD_SYSTEM_TREES
    "\"deployment\\Recovery\" = \"\\Recovery\"")

add_cd_file(TARGET bootloaderstandardbcd
    FILE ${REACTOS_BINARY_DIR}/bootloader/image/standard/BCD
    DESTINATION Recovery/ReliefWin NAME_ON_CD BCD-UEFI.initial FOR preinstall)
if(TARGET bootloaderstandardpcatbcd)
    add_cd_file(TARGET bootloaderstandardpcatbcd
        FILE ${REACTOS_BINARY_DIR}/bootloader/image/standardpcat/BCD
        DESTINATION Recovery/ReliefWin NAME_ON_CD BCD-BIOS.initial FOR preinstall)
endif()
