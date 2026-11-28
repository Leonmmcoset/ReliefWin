# 全新设备构建 ReliefWin

## Linux 主机

构建主机需要 CMake 3.24 及以上、Python 3.9 及以上、本机 C/C++ 编译器和 RosBE 交叉工具链。目标架构通过 `-a` 选择，与构建主机的架构无关。Windows 主机可以在 WSL2 中采用同一流程。

Arch Linux：

```sh
sudo pacman -S --needed base-devel git cmake ninja flex bison m4 ccache curl wget xz tar patch pkgconf python python-mako python-yaml python-packaging glslang msitools
```

Debian 12 / Ubuntu 24.04 及以上：

```sh
sudo apt-get update
sudo apt-get install -y build-essential git cmake ninja-build flex bison m4 ccache curl wget xz-utils tar patch pkg-config python3 python3-mako python3-yaml python3-packaging glslang-tools msitools
```

`configure.sh` 会安装上述两类发行版缺少的依赖；其他主机需要手动安装。它在构建菜单工具、获取源码之前检查依赖与工具版本。旧发行版还需自行升级 CMake 和 Python，依赖检查不会把过旧工具视为可用版本。Python 使用虚拟环境时，必须把三个 Python 模块安装到当前 `python3` 对应的环境；发行版的 Python 包未必对虚拟环境可见。

## 工具链、源码与安装 ISO

```sh
curl -fsSL https://raw.githubusercontent.com/LiberNT/libernt-build-toolchain/main/rosbe-unix-bootstrap.sh | sh
git clone https://github.com/Leonmmcoset/ReliefWin.git
cd ReliefWin
CCACHE_DISABLE=1 ./configure.sh --clang -a amd64 --release
CCACHE_DISABLE=1 ninja -C output-Clang-amd64-release -j4 bootcd
```

结果为 `output-Clang-amd64-release/bootcd.iso`。需要 Debug 时去掉 `--release`，输出目录改为 `output-Clang-amd64-debug`。i386、arm64 分别使用 `-a i386`、`-a arm64`，输出目录的架构也相应变化。`-j4` 可按主机资源调整。

配置脚本还会根据 SDK 头文件构建匹配的 LLVM C++ 运行库。不要只执行裸 `cmake` 来代替首次 `configure.sh`；裸 CMake 不执行源码获取和运行库准备。

已有源码更新后，使用原来的架构、编译器和构建类型重新运行配置脚本，再执行 `ninja bootcd`。只重新生成现有规则可以执行 `cmake -S . -B <已有构建目录>`，这不会获取缺失的外部源码。

## 外部组件

源码清单位于 `feeds.conf.default`；`feeds.conf` 可覆盖本机配置。`scripts/feeds status` 查看当前版本。

| 组件 | 获取条件 |
| --- | --- |
| Mesa | 支持的 Clang 目标启用 `MESA_GALLIUM_FROM_SOURCE` |
| DirectXShaderCompiler | 支持的 Clang 目标启用 `ENABLE_DXILCONV` |
| FEX | arm64 启用 FEX，或 x86 内核调试器需要其中的 Zydis / Zycore |
| FFmpeg | 启用 `ENABLE_FFMPEG` 和 `FFMPEG_FROM_SOURCE` |
| felix86 | riscv64 启用 `ENABLE_FELIX86` |

首次获取需要连接上游 GitHub 仓库。其他构建组件也可能下载固定版本、校验 SHA256 的发布载荷。配置缺少网络访问时不要把省略可选组件的结果当作完整功能构建；检查配置输出中的 `skipping`、`without` 和下载错误。

`--no-feeds-update` 只跳过源码 feed 更新，不保证整个构建离线。离线构建还需要事先准备工具链、所有选择的 feed 和使用到的下载载荷。

## Windows 原生入口

推荐使用 WSL2。原生 `configure.cmd` 需要事先安装构建工具和 Python 模块，并手动准备 feed；它不会执行 Unix 配置脚本的包安装与源码获取步骤。安装 Git for Windows 后，在 Git Bash 的源码目录执行：

```sh
scripts/feeds update mesa dxc
scripts/feeds update --paths External/zydis,External/zydis/dependencies/zycore fex-arm64ec
```

arm64 使用完整的 `scripts/feeds update fex-arm64ec`。随后从相应工具链的 Windows 开发者命令行运行 `configure.cmd`。MSVC 不构建 Clang 专用的 Mesa 和 DXC，但 x86 集成内核调试器仍需要 Zydis / Zycore。

## Gitpod

`.gitpod.Dockerfile` 安装当前 RosBE 和主机依赖，并使用独立 Python 环境中的 CMake 3.31.10，以免旧基础镜像的 CMake 低于要求。`.gitpod.yml` 在实际工作区中执行同一份 `configure.sh` 和 `bootcd` 目标。该入口不再依赖旧版 i386 RosBE 或固定的 `/workspace/reactos/build` 目录。

## 构建回归检查

```sh
python3 scripts/test_build_bootstrap.py
python3 scripts/test_office_build_order.py --build output-Clang-amd64-debug
python3 sdk/tools/depcheck.py output-Clang-amd64-debug --mode depfiles --nested --fail-on-gaps
```

依赖安装测试使用临时模拟包管理器，不会修改主机软件包。Office 检查读取真实 Ninja 构建图，确认编译前已经依赖 SDK 头文件生成器；最后一项读取实际编译记录来检查生成文件的依赖关系。

编译成功只证明相应主机、工具链、配置能生成产物，不代表所有硬件都能启动或所有可选配置均经过验证。RISC-V、PowerPC 等仍在开发的移植不能由其他架构的成功构建代替验证。

本次主机、配置、构建产物与未验证范围见[构建系统审查记录](RELIEFWIN_VALIDATION.md#构建系统审查2026-11-28)。
