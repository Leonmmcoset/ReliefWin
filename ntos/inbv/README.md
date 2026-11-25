# Boot status and progress

The graphical boot screen uses the supplied ReliefWin mark, a white progress fill on a full-width dark track at the top edge, and a centered `Loading <component>…` line at the former lower progress indicator position. The text is white and the status band is cleared before a shorter name replaces a longer one. The proportional status font maps the status byte 0x85 to U+2026; the loader renders U+2026 directly. The VGA and text-console fallback uses a dot when Unicode glyph rendering is unavailable.

Progress follows work, with no animation thread, timer advancement or artificial delay. The bands represent completed startup stages, rather than an estimate of remaining time:

| Progress | Work |
|---|---|
| 0–10% | Actual bytes copied into a boot RAM disk, when applicable |
| 10–15% | SYSTEM hive |
| 15–25% | Kernel, HAL and imported modules |
| 25–30% | National language support data |
| 30–45% | Boot driver list entries attempted by the loader |
| 45–50% | Driver database, loader block and kernel handoff preparation |
| 50–70% | Kernel subsystem initialization |
| 70–80% | Boot driver initialization and initial device enumeration |
| 80–90% | System driver list entries attempted by the kernel |
| 90–100% | Remaining initialization, ntdll and creation of smss.exe |

Loader PE loading supplies the actual current module name, including dependencies. Driver initialization supplies the module's `BaseDllName`; subsystem initialization supplies the subsystem being entered. A failed or skipped driver still completes its list entry, so the known list denominator remains stable. Progress never moves backward. A phase with no items completes when the phase returns.

The loader reuses its existing bitmap and font rendering code on UEFI and BIOS. It frees its drawing pages without clearing the last frame before preparing the kernel address space or taking the final firmware memory map. The kernel continues from the 50% handoff milestone. `NOGUIBOOT` and screen debugging retain their existing display paths.

Both framebuffer and VGA kernel paths use the boot video driver's logical display coordinates for the top track. Antialiased framebuffer text uses the existing physical dimensions and transformation. The VGA and allocation-failure paths use the existing centered character renderer. Generic status messages such as an explicitly enabled network debug key keep their own text.

Kernel drawing, status buffers and ownership checks use the Inbv lock. Component strings are consumed synchronously and never retained. Teardown detaches the buffers under the lock, disables status drawing, then frees each detached allocation outside the lock. No writer can draw once ownership has been lost or status drawing has been disabled.

`python3 scripts/test_boot_brand.py` compiles and runs the actual C drawing, formatting, initialization and teardown functions with host video mocks. It checks progress boundaries and monotonicity, full-width coverage, long and short names, missing display ownership, framebuffer scale, allocation failures, text fallback and page cleanup. The loader test also uses the real checked-in RGBA mark. These tests validate drawing and resource lifetimes; an actual boot is required to validate the firmware-to-kernel transition on a target system.
