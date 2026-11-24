# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     PCI synchronous I/O ownership regression registration
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME pcix.synchronous_irp_ownership COMMAND ${Python3_EXECUTABLE}
        ${REACTOS_HOST_SOURCE_ROOT}/scripts/test_pci_irp.py)
    set_tests_properties(pcix.synchronous_irp_ownership PROPERTIES TIMEOUT 60)
endif()
