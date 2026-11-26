> ReliefWin has no public support endpoint configured. Contact the maintainer of this checkout.

# 安装 ReliefWin

> [English](../../INSTALL.md) | **中文** | [日本語](../ja-JP/INSTALL.md)
>
> 翻译说明：本译文由 AI 初译并排版；术语、代码块、链接与锚点已与英文原版核对，中文术语已经人工核对。如有纰漏，请提交 issue 指出；表述以英文版为准。

## 构建镜像

先按[构建](README.md#自行构建-reliefwin)一节搭好构建环境并完成配置，再在输出目录(例如 `output-Clang-amd64-debug`)里构建所需镜像：

| 命令 | 生成 | 用途 |
|---|---|---|
| `ninja bootcd` | `bootcd.iso` | 从 CD 镜像运行 ReliefWin，或从该镜像启动 ReliefWin 安装程序，把 ReliefWin 装到磁盘。 |
| `ninja reliefwinimg` | `ReliefWin.img` | 启动一块已装好 ReliefWin 的磁盘，UEFI 与 BIOS 均可。 |
| `ninja reliefwinvhd` | `ReliefWin.vhd`、`ReliefWin.vhdx` | 在虚拟机中复用同一份预装盘：VHD 用于 VirtualBox 和 Hyper-V 第一代，VHDX 用于 Hyper-V 第二代(该代不支持 VHD)。 |

每个目标都会先把它依赖的内容构建好，产物写入输出目录。

## 从安装光盘安装

在虚拟机或 PC 上启动 `bootcd.iso`，按 ReliefWin 安装程序的提示操作。安装程序的磁盘列表与 Windows 安装程序一致，可新建、删除、格式化、压缩和扩展分区。它会把 ReliefWin 装到 NTFS 分区，并创建所需的 EFI 系统分区。

## 使用预装盘

`ReliefWin.img` 无需安装，开箱即用。把它放进 QEMU 等虚拟机中启动，或写入 U 盘、SD 卡，再从该设备启动。使用 VHD 磁盘的虚拟机请选 `ReliefWin.vhd`；Hyper-V 第二代虚拟机请用 `ReliefWin.vhdx`，并关闭安全启动。

预装盘中也带有 ReliefWin 安装程序。在运行中的系统里执行 `C:\<architecture>\reliefwin.exe`，例如 `C:\arm64\reliefwin.exe`，即可把 ReliefWin 装到另一块磁盘。

## 求助

遇到问题请在 Issues 反馈，并附上架构、构建类型、所用提交，以及复现步骤。指南和笔记见 Wiki，也可以到 Discord 或 Matrix 找团队。
