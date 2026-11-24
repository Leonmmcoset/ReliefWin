# PCI synchronous IRP ownership regression

Run `python3 scripts/test_pci_irp.py` from the repository root. The test extracts
`PciSendIoctl` and `PciGetDeviceCapabilities` from the actual driver source and
compiles them against host I/O mocks. The `HOST_TEST_PCIX` CMake option registers
the normal test with CTest. `--baseline REF` extracts the functions from an explicit
Git revision and requires the pending-completion and reference-release regressions
to fail. The original revision used to verify this regression is
`a676708adf7c492ecd32e35c2b35ec1d3edb0a28`.

The ownership contract comes from Microsoft's WDK documentation:

- [IoBuildDeviceIoControlRequest](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iobuilddeviceiocontrolrequest)
- [IoBuildSynchronousFsdRequest](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iobuildsynchronousfsdrequest)
- [IofCallDriver](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iofcalldriver)

The I/O manager owns and frees these synchronous IRPs on completion. The caller
waits on the supplied event and reads its own `IO_STATUS_BLOCK`. The mocks model
an already completed IRP as released and poison its status, while preserving the
caller's status block. They cover immediate success and failure, pending success
and failure completed both before the wait and during it, allocation failure,
capability metadata, output buffers, and exactly one device reference release.

The documented completion event synchronizes publication of the caller-owned
status block. The existing event wait remains the synchronization point; the fix
adds no shared state or lock. The early and late completion cases exercise both
sides of the wait without accessing the completed IRP.

These are host contract tests, not a Windows structure-layout test or a real
Windows 11 or PCI hardware run.
