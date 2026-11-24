> ReliefWin has no public support endpoint configured. Contact the maintainer of this checkout.

# Installing ReliefWin

> **English** | [中文](translations/zh-CN/INSTALL.md) | [Français](translations/fr-FR/INSTALL.md) | [Español](translations/es-ES/INSTALL.md) | [日本語](translations/ja-JP/INSTALL.md) | [العربية](translations/ar-MA/INSTALL.md) | [Українська](translations/uk-UA/INSTALL.md)

## Building the images

Set up the build environment and configure a build as described in [Building](README.md#building). Then build the image you need from the output folder, for example `output-Clang-amd64-debug`:

| Command | Creates | Use it to |
|---|---|---|
| `ninja bootcd` | `bootcd.iso` | Run ReliefWin from a CD image, or start ReliefWin Setup from it and install ReliefWin on a disk. |
| `ninja reliefwinimg` | `ReliefWin.img` | Boot a disk with ReliefWin already installed. It boots with UEFI or BIOS. |
| `ninja reliefwinvhd` | `ReliefWin.vhd`, `ReliefWin.vhdx` | Use the same preinstalled disk in virtual machines: the VHD in VirtualBox and Hyper-V Generation 1, the VHDX in Hyper-V Generation 2, which does not take VHD disks. |

Each target builds everything it needs first, and the files are written to the output folder.

## Installing from the setup CD

Boot `bootcd.iso` in a virtual machine or on a PC, and follow the instructions of ReliefWin Setup. Setup lists the drives the way Windows Setup does, and can create, delete, format, shrink and extend partitions. It installs ReliefWin on NTFS and creates the EFI system partition it needs.

## Using the preinstalled disk

`ReliefWin.img` needs no setup. Start it in a virtual machine such as QEMU, or write it to a USB drive or an SD card and boot the machine from it. Use `ReliefWin.vhd` for virtual machines that take VHD disks, and `ReliefWin.vhdx` for Hyper-V Generation 2 virtual machines, with Secure Boot turned off.

The preinstalled disk also carries ReliefWin Setup. Run `C:\<architecture>\reliefwin.exe`, for example `C:\arm64\reliefwin.exe`, to install ReliefWin on another disk from the running system.

## Help

If something goes wrong, report it in Issues with the architecture, the build type, the commit you built and the steps that lead to the problem. Guides and notes are in the Wiki, and you can ask the team on Discord or Matrix.
