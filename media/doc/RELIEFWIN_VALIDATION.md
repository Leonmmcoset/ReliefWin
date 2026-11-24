# ReliefWin 改名验证记录

本次以 `a676708adf7c492ecd32e35c2b35ec1d3edb0a28` 为基线，将 LiberNT／LiberOS 的当前产品品牌统一为 ReliefWin。默认界面语言为简体中文，保留其他翻译与英文回退。操作系统更新默认关闭，没有配置官网、更新服务器或公开发布仓库。

## 原图与覆盖范围

原图：`media/graphics/branding/reliefwin-source.jpg`，与维护者提供的 JPEG 完全相同，SHA-256：`124d19dbcafcbabfe3dfac88de9977a9c00411868e246f0c0913c4a4ae3dd5e5`。

资源覆盖引导菜单、GOP 与 VGA 启动/关机画面、安装与首次配置、登录、开始按钮、设置预览、系统属性、关于对话框、系统图标及 dxdiag 图形测试。`assets.json` 记录 40 项生成资源的摘要；生成器保留原图中的双蓝色形状与留白。设置程序中的小型主题预览属于界面素材，不是系统运行截图。

## 保留的历史文字

全树扫描针对旧产品名、私有路径及中文和更新默认值，原始基线按预期未通过。以下文字按用途保留，并在扫描脚本中单独分类：

- 原作者版权、许可证、历史贡献者、上游商标和原始测试向量来源。
- ReactOS／Wine 上游来源及实际存在的 RosBE、ICU、FFmpeg、Mesa、FEX 下载地址。这些不作为 ReliefWin 的产品官网、发布服务或支持服务。
- Windows／Wine／ReactOS 的兼容宏、导出接口、控件类、签名、测试证书及需维持互操作的技术标识。
- KDNET 的四个原始密码协议标签。改变这些字节会破坏现有调试工具的握手，故与基线逐字保持一致。
- ECDH 测试的原始 `LiberNT ECDH A/B` 种子说明；它对应已验证的固定私钥向量，不能当作产品文案重命名。

详细的上游字符串清单位于 [UPSTREAM_IDENTIFIERS.json](UPSTREAM_IDENTIFIERS.json)，含 323 个文件中 853 处保留项及逐项理由。扫描器覆盖 C/C++ 常见后缀与 RC/RC2 资源，按出现次数逐字匹配，识别大小写变体及 C++ raw 字符串；新增旧产品标题的变异已确认被拒绝。

安装树最终使用 `Windows` 目录。CMake 的 `reactos/` 目的地是暂存命名，由镜像清单统一映射为 `Windows/`，不应单独改变而破坏映射。

实际设置页检查还发现了旧默认电脑名缩写，现将 ComputerName、TCP/IP Hostname 与 NV Hostname 统一设为 `RELIEFWIN`。自有引导程序生成的设备树 ACPI 表也同步改用六字节 OEMID `RWIN  ` 与四字节 CreatorID `RWIN`，HAL 按 `RWIN` 匹配并接受规定长度内的空格填充；生产者与消费者一起修改，保留表标识 `DTPLATFM`。

## 已执行的验证

| 检查 | 结果与边界 |
| --- | --- |
| `sdk/tools/check_branding.py` | 工作区旧产品名/默认值扫描通过；历史基线按预期失败。包含 UTF-8 与 UTF-16 文本。 |
| `scripts/check_brand_assets.py` | 40 项资源校验通过；2644 张源图（含交付截图）无旧图精确副本。旧图摘要基线含 41 个哈希、44 个原路径；把旧 PNG 改名放回时检查确认失败。还校验原图颜色/留白、尺寸、位深、透明度、预乘 RGBA、BGRA 位域、RLE4 解码及三张 VGA 图共享调色板（0 黑/15 白）。 |
| `scripts/test_boot_brand.py` | 提取内核实际进度绘制 C 函数，在 DPI 96–480 范围执行 146,669,828 项检查；旧几何按预期失败。宿主运行不等同于内核运行。 |
| dxdiag 真实 C 实现与单元测试 | 652 项检查、151 帧通过；旧形状有 5 项预期失败。ASan/UBSan 运行通过。 |
| `submodules/host-tests/branding/branding-test.py --verify-regression` | 实际版本清单解析 11 用例、实际更新检查函数 4 用例通过；旧偏移与移除空服务器保护的 3 个变异均确认失败。缺失、无效或空更新地址不发起网络请求。 |
| `submodules/host-tests/branding/audit-test.py` | 独立源码检查 27 项、包含实际 PE 的检查 34 项通过，覆盖旧标题及缩写的大小写、多种源文件后缀、同文件中兼容类名与产品标题的区分、默认主机名、ACPI 固定宽度标识、包管理器与中文桌面显示名、精确上游 PE 元数据白名单，以及无效/空目录和未稳定文件不能报成功。 |
| `submodules/host-tests/windmc/comment-headers-test.py --verify-regression` | 8 份实际 MC 源 × 3 种编码共 24 组头文件编译通过；原注释 24 组确认失败，消息编号及 `.bin` 内容不变；移除 Null/Dxgi 声明 6 组确认失败。 |
| 实际 FAT 工具 | 13 字节旧尝试按预期拒绝；11 字节 `RELIEFWINBT` 卷标生成成功，原始 FAT 引导扇区读回一致；`EFI/ReliefWin/Boot/BCD` 长文件名目录创建与文件逐字节往返读取通过。 |
| 实际注册表生成与读回 | 按预装覆盖顺序生成后 9 项通过：ProductName=ReliefWin、默认/安装语言 0804、ACP/OEMCP 936、用户 zh-CN 区域/中国地区、US 物理键盘。旧预装覆盖将 Locale 退回 00000409 的回归已确认；修复后为 00000804。当时尚不含未生成的 COM 注册部分，完整产物另验。 |
| 简体中文安装资源 | 101 条升级、更新、宿主安装文字翻译；全部格式参数顺序及类型保留，资源结构与原布局保持一致。 |
| 简体中文开始菜单 | 补齐 22 条 `IDS_SM2_*` 资源，并将 31 个生成快捷方式的显示名改为中文；同步预装/LiveCD 的 `.lnk` 引用和 `Strings.0804`，保留目标程序、参数与既有目录。修复实际截图中整块开始菜单回退英文的问题。 |
| `scripts/test_start_shortcuts.py` | 使用真实 native utf16le 与 mkhive，读回预装 9 条、LiveCD 8 条中文/英文快捷方式多字符串；17 条逐字节正确。原始 overlay 直接传 UTF8 的流程导致 15 条中文字符串乱码，回归确认失败。另校验实际 hive、镜像清单和生成 ShellLink 的对应关系，防止 Linux 终端的生产名称与引用不同。 |
| Explorer 本地帮助资源 | 原 C 风格引号被实际 llvm-windres 拒绝；使用 RC 双双引号后，最小资源的 UTF-16 读回与 ARM64 完整 Explorer 资源目标均通过，命令保留带引号的系统目录路径。 |
| `scripts/test_pci_irp.py` | 实际 PCI 驱动函数的 548 项宿主检查通过；指定原始基线运行确认 10 项预期失败，覆盖异步完成后的状态读取及请求分配失败的设备引用释放。 |
| 宿主 CTest | 复用 `/tmp/reliefwin-brand-host`，仅开启 BRANDING、WINDMC、PCIX、WINSPOOL，统一 8/8 通过，包括源码/PE 审计保护、安装器语言、中文快捷方式 hive 编码、版本解析与空更新地址、消息头编译、PCI 请求所有权及打印接口。 |
| 打印接口实际函数回归 | GNU 与 Clang ASan/UBSan 各 270,961 项通过；旧客户端、provider 及忽略转换失败/空指针的变异按预期失败。覆盖 CP936 中文、UTF8 槽位边界、实际 Add/Set/Enum/Get 入口、实际 provider 打包、DEVMODE、分配失败和错误传播。 |
| `scripts/test_setup_language.py` | 实际语言选择函数 11 场景、156 项检查通过，GNU 与 Clang ASan/UBSan 均通过；原固件覆盖变体 21 项预期失败。无旧 hive、默认 804 与 US 键盘时，英文固件不会覆盖当前默认语言。 |
| `git diff --check` | 通过。 |

新补的 `FACILITY_NULL=0` 与 `FACILITY_DXGI=2170 (0x87a)` 来自 Microsoft 公开 SDK 头：<https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/shared/winerror.h>。没有读取 Windows 私有实现。

静态分析还发现 PCI 驱动原有的同步 IRP 生命周期缺陷。本次修复只在两个等待完成分支读取调用方的 `IO_STATUS_BLOCK`，并补上分配失败的设备引用释放。所有权与完成通知契约来自 Microsoft WDK 的 `IoBuildDeviceIoControlRequest`、`IoBuildSynchronousFsdRequest` 和 `IofCallDriver` 文档，链接及测试边界见 [PCI 回归说明](../../submodules/host-tests/pcix/README.md)。

打印接口的已有缺陷会影响本次默认 CP936 中文环境，因此同时修复：实际 ANSI 字节容量、转换与分配失败传播、DEVMODE 空指针，以及 `FORM_INFO_2W.pKeyword` 始终为 `LPCSTR` 的公共契约。客户端与实际 provider 均按该字段的字节类型处理；详细来源、回归及限制见 [打印回归说明](../../submodules/host-tests/winspool/README.md)。

实际深色主题截图还验证了系统属性横幅的对比度。两组共享 BMP 横幅保留蓝色图形与深色文字，改用不透明浅色底，修复文字在深色面板上难以辨认的问题；仅四个资源改变，其余 36 项不变。新增检查在旧素材上确认失败，修复后通过，两次生成的字节完全相同。三架构 sysdm 的实际 PE 位图与最终两张 BMP 的 DIB 数据逐字节相同；userinit 的同名备用图目前未被 RC 引用，没有把它计作实际运行显示。

## 构建与实际启动

构建环境为 Linux x86-64、RosBE LLVM 24、CMake 3.31.10、Ninja，全部构建设置 `CCACHE_DISABLE=1`。复用 `output-Clang-{amd64,i386,arm64}-debug`；三个架构的 C++ 运行库已编译成功。原生 `genincdata.dll` 分别确认为 AMD64 (0x8664)、I386 (0x14c)、ARM64 (0xaa64)。

| 架构 | 已完成构建 |
| --- | --- |
| amd64 | 完整 `reliefwinimg` 成功，包含原生模块与全部 481 个 WoW64 增量目标。 |
| i386 | 23 个最终目标成功，包括内核、Explorer、Winver、系统属性、设置、安装器、shell32、pcix、winspool、localspl，以及最后新增的 HAL、bootmgfw、winload、bootmgrbios。未构建独立 i386 完整磁盘镜像。 |
| arm64 | 21 个最终目标成功，包括上述 19 个系统模块与 bootmgfw、winload；公共 ACPI 源由内核编译。实际 PE 头确认为 ARM64 / PE32+。未在 ARM64 虚拟机或实体板上运行。 |

最终 AMD64 原始镜像使用同一输出路径 `output-Clang-amd64-debug/ReliefWin.img`。本地测试配置设为 4096 MiB，以容纳 2 GiB 客体 RAM 自动生成的休眠文件与分页文件；未更改源码的镜像容量默认值。启动前按 MBR 实际分区表读取，NTFS 从字节 68,157,440 开始，长度 4,226,809,856，流式逐字节及 SHA-256 均与已审计的 canonical `partition.ntfs` 一致。FAT 分区也严格匹配 `mkdiskimg` 规定的分区位置/驱动器及保留 FAT 项修补结果。随后实际启动会正常修改该工作镜像中的注册表和用户数据。

启动前直接读取该分区中的 1,545 个已安装 PE（1,053 个 AMD64、492 个 I386），产品/文件描述/公司身份残留为 0，读取错误为 0，与真实构建模块的内容摘要差异为 0；310 个模块含 0x0804 中文资源。实际 hive 的 10 项读回通过：产品名 ReliefWin，默认/安装语言 0804，ACP/OEMCP 936，MACCP 10008，用户 Locale 00000804、LocaleName zh-CN，US 物理键盘 00000409，以及 ComputerName=RELIEFWIN。Tcpip 服务键由驱动安装期创建，未在该预装 hive 中生成；其 Hostname/NV Hostname 默认值通过实际 INF 和扫描器精确校验，没有把不存在的值计为读回通过。

卷名为 ReliefWin，EFI 路径为 `EFI/ReliefWin/Boot`，FAT 卷标为 `RELIEFWINBT`；根目录无旧产品名目录。上游完整 ICU 预编译包仅作代码完整性目录输入，镜像内 system32/SysWOW64 的 `icu.dll` 均确认来自自编 stub。原始上游包的 `ICU for LiberNT` 文件描述保留。7 个可选依赖包在首次配置列表中明确标为 `upstream package`，下载地址与摘要保持真实。

最终 amd64 镜像已在无网络的 headless QEMU 中实际启动到简体中文桌面，进入安装语言选择、账户/可选组件、开始菜单、关于、设置及系统属性页面。通过 QMP 屏幕转储保存[实际运行图集](../graphics/screenshots/README.md)，没有用资源预览代替截图。启动图保留了外部 OVMF 固件的 TianoCore 标志与灰色日志底图，新 ReliefWin 标识在其上方显示。

图集共 11 张。最终增量启动后重新确认了完整中文常用程序列表、默认电脑名及修复后的系统属性横幅；命令行实际查询 ProductName 返回 ReliefWin。其余已核验页面的截图来自本次改名构建期间的对应运行，未改变界面内容。最终预装/LiveCD 两类真实 hive、生成清单与 ShellLink 的 9/9、8/8 项映射核验通过。

验证中修正了两项宿主工具问题：监控工具退出时错误结束客体，以及 QMP 按键尚未释放时发送下一字符导致输入缺字。工具修改仅在临时宿主脚本中；没有将它们误报为操作系统崩溃或程序缺失。虚拟机运行遵守 30 秒监控窗口、6 秒停滞判定，复用同一个工作镜像，重包前先正常关机。

## 静态分析与验证限制

cppcheck 使用构建生成的外部编译数据库、目标 include 路径/defines 及每文件过滤。最终 18 个有逻辑变化或 ACPI 标识变化的 TU 均有当前源码摘要对应的实际分析记录；最后两个 ACPI 文件分别按 amd64 与 arm64 的实际构建参数分析，均退出 0。总计 56 个当前 TU 完成分析：23 个退出 0、25 个受工具解析限制、8 个含已逐项读源核查的诊断；没有将后两类算作通过。3,914 个变化的 C/C++ 源中，1,762 个取得相应配置命令，其中 1,706 个没有当前逐 TU 完成结果；2,152 个没有本次配置命令。早期中断批次不叠加虚增覆盖。

Ubuntu 的 cppcheck 2.7 无法完整解析部分现有 SDK 枚举宏与 libc++ 条件宏，因此不声称全树静态分析通过。确认的 PCI 和打印缺陷已修复并有回归；其他堆句柄 doubleFree、固定文本 StringCchCopyW 等报告经读源核查未确认缺陷，没有添加抑制。`inbvgop.c` 本体无诊断；两项包含头报告经调用契约和原生 SEH 退出路径核查未确认缺陷，不计作零警告通过。

`INFO_NEEDED`：ANSI 结果超过既有 UTF16 字符串槽时，目前安全返回缓冲不足；完整 UTF8 ACP 重排及精确所需大小需要 Windows 11 对照证据。DEVMODE 固定名称的转换失败已正确传播，但本次没有发明 Windows 11 的未公开截断规则。

本环境没有可用的 Windows 11 参考虚拟机，未执行新的 Windows 11 对照运行；品牌内容本身为本项目定义。宿主测试不能证明未运行架构或实体硬件的行为。实际系统截图必须来自新镜像的 QEMU 屏幕转储，不以素材预览代替。
