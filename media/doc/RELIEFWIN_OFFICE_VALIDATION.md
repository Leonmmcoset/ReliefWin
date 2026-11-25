# ReliefOffice 与启动进度验证记录

本记录覆盖基于 `e01ee711` 的启动进度界面、ReliefOffice 安装维护集成及安装器目标路径修复。品牌改名既有结果见 [RELIEFWIN_VALIDATION.md](RELIEFWIN_VALIDATION.md)。下文分别列明 Linux x86-64 主机回归、三个架构构建，以及 AMD64 ReliefWin 客体的实际结果；未执行的检查单独注明。

最终 AMD64 ISO 的 SHA-256 为 `f425257ce0f5afcba84b4d34e4e331d30dcac2a62ad148aafb7c0abf4c5e179c`。该介质已在无 NIC 的 QEMU 中清除旧 GPT、重新分区并格式化 NTFS，完成全新离线安装及简体中文首次设置，使用新建账号 `ReliefUser` 运行下表测试；没有预先运行手工注册助手。安装器日志确认内嵌权限策略成功应用。独立测试程序由固定 FAT 载荷提供，不进入 ISO。

| 最终 ISO 新装系统的实际检查 | 直接读回的结果 |
| --- | --- |
| Office 安装及七个 GUI 应用 | **98 项、0 失败、退出 0**；14 个安装文件、七种原生关联、七个公共开始菜单链接及七个可见应用窗口和正常关闭。 |
| 七种原始文档格式 | **3,436 项通过、退出 0**。 |
| 新版 IO 参数入口及 NTFS 完整套件 | 四组缺失／普通文件 fixtures 和 work 参数均仅输出一行相应错误，**退出 2**，没有执行用例；有效既有目录 **1,567 项、0 失败、退出 0**。 |
| `ReOpenFile` | **90 项、0 失败、0 跳过、退出 0**。 |
| ReliefMath 原生 worker／文件任务／超时 | **41 项通过、退出 0**。 |
| ReliefMath FAT 公共卷保存及私有恢复拒绝 | **24 项通过、退出 0**。 |
| ReliefMath++ 计算子进程自检 | `--calculation-selftest` **退出 0**。 |
| Office 实际卸载 | 准备 **52 项、0 失败**；真实维护程序 `/uninstall /quiet` **退出 0**；卸载后检查 **51 项、0 失败、退出 0**。13 个可立即移除的清单文件、七个自有 ProgID、七个开始菜单链接及卸载项已移除，维护程序已登记重启删除；中文文档与十种外来扩展默认值保留。 |
| 正常重启后的删除与保留 | **50 项、0 失败、退出 0**；维护程序已删除，中文文档原始字节及外来扩展默认值继续保留。 |
| 固定离线载荷恢复 | 真实注册代码记录返回 **0**；恢复安装检查 **98 项、0 失败、退出 0**，七个 GUI 应用均正常启动与关闭。 |

上述日志与退出码已从 `/tmp/reliefwin-branding-fat.img@@1048576` 直接只读取得：`fresh-install`、`fresh-formats`、`fresh-reopen`、`fresh-math`、`fresh-math-fat` 的 `.log`／`.exit`，`fresh-mathpp.exit`，以及 `io-args-*` 的 `.log`／`.exit`。维护流程另读取 `uninstall-prepare.log`、`uninstall.exit`、`uninstall-verify`／`uninstall-reboot`／`restored-install` 的 `.log`／`.exit`、`restore.log` 和 `restored.txt`。`fresh-done.txt` 与 `io-args-done.txt` 均为 `done`，`restored.txt` 为 `restored`；这些标志不替代检查结果。本轮新装系统由安装器直接应用 2,685 个对象的权限策略，没有离线修补 ACL；上述卸载、重启删除与恢复结果均来自该新装系统。

## 当前实现与来源

引导程序与内核按实际工作阶段更新顶部全宽白色进度条，在原下方进度条位置显示 `Loading <实际组件名>…`。RAM 盘使用已复制字节数，引导驱动和系统驱动使用实际条目数；其余百分比表示已进入的初始化阶段，不表示预计时间。进度保持单调，不添加等待。通用状态显示保留独立接口，以继续显示网络调试密钥等非加载文字。状态字体使用 U+2026；VGA 无该字形时回退为单个点。

安装维护代码使用目标注册表中的 `ProgramFilesDir` 定位 ReliefOffice，通过自定义 DIRID 32768 和 `SPINST_REGISTRY | SPINST_PROFILEITEMS` 注册七个公共开始菜单链接、七种原生格式及一个卸载入口。INF 使用 `FLG_ADDREG_NOCLOBBER` 保留已有的扩展名默认值。固定清单含 14 个文件，包括渲染进程、STIX 字体、维护程序和许可证。卸载检查安装位置、链接目标及打开命令的所有权，保留用户文档；失败保留可重试入口。移除运行中的维护程序通过重启删除队列完成，队列登记失败时恢复卸载项。

最终介质审查发现，原 bootcd 把 Office 文件直接放在介质根的 `Program Files/ReliefOffice`，而当前空路径 RAM 启动会复制整个 ISO；即使没有注册和快捷方式，Live 运行环境仍含已安装位置的应用文件。因此 bootcd 改为 `deployment/ReliefOffice` 离线载荷，专用 `txtsetup.sif` 的 `SystemTrees` 将它映射到目标卷的 `\Program Files\ReliefOffice`，沿用实际安装复制队列和随后注册。preinstall 仍在原安装目录携带文件，并沿用原 SIF。Live RAM 可以携带明确的离线部署载荷；它不等于 Live 已安装应用。此变更不修改 RAM 拷贝器，不声称 RAM 中完全没有 Office 可执行文件字节。

接口契约来源为 Microsoft 文档：[SetupInstallFromInfSectionW](https://learn.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupinstallfrominfsectionw)、[SetupSetDirectoryIdW](https://learn.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupsetdirectoryidw)、[INF ProfileItems](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/inf-profileitems-directive)、[INF AddReg](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/inf-addreg-directive)、[MoveFileExW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw) 和 [IShellLinkW::GetPath](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishelllinkw-getpath)。细节见 [维护说明](../../base/applications/reliefoffice/maintenance/README.md)。

目标注册表路径超过固定缓冲区时，`OpenTargetKey` 清空输出句柄并返回 `STATUS_NAME_TOO_LONG`，避免打开截断后的路径。该状态值 `0xC0000106` 及名字过长的语义已核对 Microsoft [MS-ERREF](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/596a1078-e883-4972-9bbc-49e60bebca55)。Live 安装的 SAM、SECURITY 临时备份使用 `.reliefwinsave`；创建备份与安排恢复均消费同一后缀，没有其他协议消费者。

## 本次主机回归

所有临时测试源码与可执行文件复用 `/tmp/reliefwin-brand-host`。C 测试由实际实现函数抽取后编译，使用模拟的绘图、分配、注册表、文件或 SetupAPI 依赖。

| 命令 | 实际结果与覆盖 |
| --- | --- |
| `python3 scripts/test_boot_brand.py` | 内核 15,091,427 项、引导程序 2,105 项、字体映射 768 项通过。覆盖边界、进度钳位及单调性、长名称与覆盖、显示所有权、物理缩放、分配失败、状态清理及 U+2026。 |
| `python3 scripts/test_boot_brand.py --baseline e01ee711` | 检查确认该基线仍使用旧下方动画且没有顶部进度绘制，实现不符合新行为。 |
| `python3 base/applications/reliefoffice/maintenance/test_maintenance.py` | 使用 `DIRID_USER`／32768 及显式 `%ls` 的当前实现重新运行：维护函数 649 项、实际快捷方式所有权函数 72 项、目标目录与 SetupAPI 注册函数 52 项通过；INF 的七个公共链接和不覆盖默认值标志通过。包括占用文件、缺失文件、外来关联、失败重试、卸载项删除失败、自身删除排队失败及中文目标目录。COM 与 SetupAPI 依赖为模拟；这些通过项没有验证实际目录 ID 展开或注册表写入。 |
| `python3 scripts/test_syssetup_target_path.py` | 实际备份/恢复函数 72 项、实际 `OpenTargetKey` 17,483 项通过。覆盖两种 hive、日志存在/缺失、保存失败、路径长度 0–261、空或缺省后缀及打开失败传播。 |
| 目标路径旧实现回归 | 增加边界测试后、修改函数前，旧实现确实在超长路径断言失败。新增备份路径检查在修改后缀前也失败；`--baseline e01ee711` 再次在旧备份路径断言失败。 |
| `python3 scripts/test_iso_unicode_lists.py` | 实际 CMake 清单生成及现有 mkisofs 在内存生成 ISO 后，七条包含中文、空格、希腊字母及 emoji 的路径和文件内容逐字节读回通过。覆盖可选文件存在/缺失、中文目的地优先项、Live/preinstall/Mono 清单读入、服务清单、介质重打包、符号预算及未变更文件的写入时间稳定性。RAM 分区容量脚本使用记录格式化请求的模拟依赖，实际请求的容量参数已核对；未生成另一份磁盘镜像。`--baseline e01ee711` 由实际 mkisofs 复现 `Invalid node - 'Users/Public/Desktop/'`，当前版通过；根因是默认 ASCII 的 `file(STRINGS)` 拆断 UTF-8 名称，同一清单的八处读入已明确指定 UTF-8。 |
| `PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_boot_fat_utf8.py --verify-regression --sanitize` | 实际 RAM FAT formatter／writer、RTL UTF-8 转换及启动期大小写函数编入 C harness。旧 writer 首先在“安装 ReliefWin.lnk”的独立 UTF-16 LFN 解码检查失败；当前版 729 项、0 失败，ASan／UBSan 无诊断。覆盖中文、emoji、完整 16 位名称匹配、13 单元边界、代理对跨槽、LFN 数量／终止／填充、短名碰撞、中文目录重复创建和现有路径字节上限。无效 UTF-8 及 U+FFFF 拒绝后，整卷与分配状态逐字节不变。没有用 writer 自身的错误匹配充当文件名正确的证据。 |
| `PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_wim_unicode.py --verify-regression` | 实际 WIM 转换、dentry 与 tree 函数编入 C harness，以 Python Unicode codec 独立生成 3,109 组向量，32,316 项、0 失败，ASan／UBSan／泄漏检查通过。原始 helper 的 U+1F600 输出确为 CESU-8 `eda0bdedb880`，与正确 `f09f9880` 不符；原实现及奇数字节、清理、错误传播、记录边界四个定向变异均按预期失败。覆盖代理对、全部孤立代理项、截断、容量算术、分配失败及真实消费者清理。没有执行完整 WIM 引导。 |
| `python3 scripts/test_office_deployment.py --verify-regression` | 实际执行 Office/math 的 CMake 与两个 SIF 的生成及介质选择；14 项 bootcd 离线载荷和 preinstall 原目录/原生成内容通过。抽取实际 loader 的 SIF 解析器与 loader/setuplib 路径映射，626 项通过，覆盖字体子目录、全部旧 inventory 路径、误前缀、排除目录、不同目标根及缓冲区不足。旧 14 文件已安装目录/单 SIF 布局、bootcd 误用原 SIF、缺少 deployment 映射三个变异分别因预期布局或实际映射行为失败。同一最终 C harness 的 ASan/UBSan 和泄漏检查通过。这些主机检查没有执行实际磁盘复制。 |
| `PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_office_security.py` | 215 项、0 失败。执行抽取的真实 Setup 策略解析／路径匹配，以及既有 native-ntfsimg 的实际格式化、策略应用和二进制 ACL 读回。旧策略确实缺少 BA/SY 文件 DELETE 与目录 DELETE_CHILD；当前策略给予 3 个 Office 目录、14 个载荷文件和 1 个中文用户文档 BA/SY 完全控制，BU 仍为 `0x1200a9`。Program Files 根、相似前缀目录和普通组件共 10 个描述符逐字节不变，全部文件摘要不变，重复应用幂等。Setup harness 的 SDDL 转换使用模拟，真实 SDDL 解析和磁盘写入由 native 工具执行；同一 matcher harness 的 ASan/UBSan 与泄漏检查通过。该测试不执行客体权限检查。 |
| 作者代码的空白／冲突标记检查 | 已跟踪改动的 `git diff --check` 和新增 Office 作者代码的逐文件检查均无诊断。全部新增 Office 文件另做 no-index 检查产生 617 项格式／标记诊断，全部限于原样上游 thirdparty 和汇总许可证原文，作者代码范围为 0。保留这些原始上游内容，没有将整个新增 vendor 树说成零诊断；完整日志为 `/tmp/reliefoffice-whitespace.log`。 |

客体测试 `reliefoffice_install_smoke` 的代码检查了 14 个安装文件、七种原生关联、七个公共链接目标、无桌面链接，以及七个应用的可见窗口和正常关闭；外来关联检查包含 `.doc`、`.docx`、`.xls`、`.xlsx`、`.ppt`、`.pptx`、`.pdf`、`.txt`、`.csv`、`.html`。本节未把该测试的源码覆盖计作客体运行通过。

## 维护目标三架构构建

完成显式 `%ls` 修复、撤回没有合同依据的 `S_FALSE` 产品改动后，在既有 `output-Clang-amd64-debug`、`output-Clang-i386-debug`、`output-Clang-arm64-debug` 中，以 `CCACHE_DISABLE=1` 和每架构 `ninja -j 2` 构建 `reliefoffice-maintenance`、`reliefoffice_install_smoke`、`reliefoffice_uninstall_smoke`、`syssetup`。三架构全部退出 0；沿用最新依赖，只重新编译有变更的维护程序，没有清空构建缓存或重新封装 ISO。

12 个 EXE／DLL 的 PE 机器类型逐个核对为对应的 x86-64、80386 和 AArch64。四目标的构建结果、源文件及产物 SHA-256 冻结清单为 `/tmp/reliefwin-maintenance-build.json`，唯一工作日志为 `/tmp/reliefwin-maintenance-build.log`。临时 `tests/platform-probe.cpp` 及其 CMake target 已移除，卸载 smoke 和 ReOpenFile 测试 target 保留。上述结果证明这些目标可在三架构构建，不代表 i386／ARM64 的客体运行已通过。

Office 权限策略修正后，主执行者再次在三个既有构建目录重建 `reliefwin` 安装器目标，三架构均退出 0。另直接解析当前三份 PE，机器类型分别为 `0x8664`、`0x14c`、`0xaa64`；三份 RCDATA／120 均逐字节等于当前 `filesecurity.txt`。实际文件与策略摘要记录在 `/tmp/reliefwin-installer-policy-artifacts.json`。这是资源构建和产物证据，没有将其计为 i386／ARM64 客体安装运行。

## Live RAM 中文文件名修复

最终介质的实际 Live 桌面出现“安装 ReliefWin”和“使用说明”两个文件型快捷方式名称乱码，而此电脑、网络和回收站的中文资源显示正常。逐字节读取真实 ISO，名称本身仍为正确 UTF-8。实际链为 CMake 中文输出名、ISO 主目录字节名、`RdGetIsoName`、RAM FAT writer；原 writer 用 `strlen` 作为 UTF-16 名称长度，并将每个 UTF-8 字节直接扩为 WCHAR。例如“安”的 `e5 ae 89` 被写成 `U+00E5 U+00AE U+0089`。原查找函数再截取每个 WCHAR 的低八位，能碰巧找到同样损坏的名称，因此原先的清单／ISO 字节回归没有验证 RAM 目标的 Unicode 文件名。

修复复用既有 `RtlUTF8ToUnicodeN` 并加入启动 RTL 的源清单，在路径解析阶段逐组件严格验证，随后按 UTF-16 单元计算和写入 LFN，查找比较完整 16 位名称。仅接受 `STATUS_SUCCESS`；`STATUS_SOME_NOT_MAPPED` 虽属于非负状态，但表示替换字符，必须拒绝。U+FFFF 与 FAT LFN 填充标记冲突，同样在分配前拒绝。来源为 Microsoft [RtlUTF8ToUnicodeN](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlutf8tounicoden) 和 [FAT 规范](https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc)。本次保持既有组件 255 个 UTF-8 字节、`Fat32FindFile` 总路径 259 字节上限，以及启动期仅 ASCII 大小写折叠。

同一 writer 的 WIM 消费者明确传入 `name_utf8`。实际 WIM 转换 helper 原先逐 UTF-16 单元输出三字节编码，未合并代理对；现按 [Unicode 第 3 章](https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/) 和 [RFC 3629](https://www.rfc-editor.org/rfc/rfc3629) 合并合法代理对、拒绝孤立代理项并检查容量算术。沿真实消费者修复奇数 UTF-16 字节长度的越界、记录范围加法溢出以及子项错误被吞掉时的清理和失败传播。

独立审查追踪了 ISO／WIM 输入、短名回退、分配／回滚和同步数据写入：新增名称状态只在调用栈上使用，写文件和进度回调不保留这些指针；writer 操作的是新格式化 RAM 卷。两组 C harness 的通过结果见前表。保持各源冻结后，以 `CCACHE_DISABLE=1`、每架构 `ninja -j 2` 重建 amd64／i386 的 `winload`、`winloadpcat`、`bootmgfw` 及 arm64 的 `winload`、`bootmgfw`，三架构均退出 0；8 个 PE 的机器类型逐项正确，源码和产物摘要见 `/tmp/reliefwin-boot-utf8-build.json`，单份构建日志为 `/tmp/reliefwin-boot-utf8-build.log`。

Cppcheck 使用构建目录外的 `/tmp/reliefwin-boot-utf8-compdb.json`，保留真实编译命令、include 路径和 AMD64 defines，逐文件过滤。`wimcore.c` 退出 0 且无诊断；`fatwrite.c` 虽退出 0，但在现有 CRT `stdio.h:90` 的 `_CRTDATA` 宏处解析中止，分析未完成，没有计作通过。输出分别为 `/tmp/reliefwin-wim-unicode-cppcheck.log` 与 `/tmp/reliefwin-boot-utf8-cppcheck.log`。修复后的 `f425257c…` ISO 已实际启动到 Live 桌面，“安装 ReliefWin”和“使用说明”两个中文文件名显示正常，从安装图标启动安装器成功；[实际截图](../graphics/screenshots/live-installer.png)已保存。这项真实运行确认了所测 ISO／RAM 路径的修复，没有将其扩展为 Windows 11 对照、完整 WIM 引导或未运行架构的客体通过。

## 实际安装注册问题

初次 AMD64 客体运行的 `install.log` 实际报告 77 项检查、50 项失败。直接读回的卸载项 `InstallLocation` 错误地指向 `C:\Windows\system32\spool\drivers\x64`。其余失败涉及目录、安装文件、原生关联、开始菜单目标及应用创建；随后实际诊断发现，测试自身的宽字符串格式错误会生成错误的路径与注册表键，因此该失败数不能用于断言这些产品行为均有缺陷。这次运行没有验证应用窗口与正常关闭。

根因之一已从 `dll/win32/setupapi/dirid.c` 确认：目录解析在查找用户映射之前，将 66000 解释为打印驱动目录。安装函数和 INF 已统一改用公开 SDK 的 `DIRID_USER`／32768，并在客体 smoke 中增加 `InstallLocation` 必须等于实际 `ProgramFilesDir\ReliefOffice` 的检查。32768 的定义来自 Windows SDK `setupapi.h` 和 Microsoft [Using Dirids](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/using-dirids)。

关联检查直接读取 `HKLM\SOFTWARE\Classes`，不经过 HKCR 的用户合并视图。打开命令按注册的绝对安装路径与字面的 `%1` 比较；INF 不要求环境变量展开。`FLG_ADDREG_NOCLOBBER` 会保留原生扩展已有的外来默认值，因此原生默认值必须属于 Office 的断言适用于干净安装；已有值须通过实际注册表读回判断。

实际 `setup.log` 显示了第二个独立问题：UCRT 下 `StringCchPrintfW` 的 `%s` 按窄字符串解释，使 `.rword` 键变成 `.`、ProgID 变成 `R`，诊断 INF 路径变成 `C\reliefoffice.inf`。先前的临时诊断没有成功执行真实注册。维护程序、安装 smoke、syssetup 路径和临时诊断均已使用显式 `%ls`。维护及目标路径主机测试原先会把 `%s` 自动翻译为 `%ls`，掩盖了这个问题；已移除翻译，让实际格式直接传入 `vswprintf`。保留旧产品格式时，维护回归在 `OfficeRemoveAssociation` 的所有权检查实际失败；修复格式后，维护 649 项、安装 52 项及备份/路径 72／17,483 项重新通过。

修复真实注册与大部分 smoke 格式后的 AMD64 客体运行曾报告 98 项检查、7 项失败，剩余七项来自测试链接路径一行仍使用旧 `%s`。更正后再次运行当前 PE，安装 smoke 为 **98 项、0 失败**，包括实际安装目录、14 个清单文件、七种原生关联、七条打开命令、七个公共开始菜单链接，以及七个 GUI 应用创建进程、进入输入空闲、可见窗口和正常关闭。

新增针对实际 `OfficeRemoveShortcut` 的 72 项主机检查，覆盖外来与自有目标、COM 创建／查询／载入／取路径失败、对象释放、删除占用、缺失与拒绝访问，以及中文目录。Microsoft [IShellLinkW::GetPath](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishelllinkw-getpath) 明确规定 `S_FALSE` 时输出为空串，模拟遵守该合同；对应保留链接的检查通过。原函数在该合同内没有确认缺陷，因此未为 `S_FALSE` 增加产品改动。客体测试显式以 `S_OK` 判定已取得有效目标；这部分主机模拟尚无 Windows 11 对照运行。

持久客体测试 `reliefoffice_uninstall_smoke` 不进入 ISO。`--prepare` 验证 14 项实际安装清单、七条打开命令和七个链接的实际目标，记录七个原生及十个外来扩展的原始默认值，再以 `CREATE_NEW` 创建中文用户文档和固定状态文件，避免覆盖已有文件。实际卸载后，`--verify` 检查已安装文件、七个 ProgID、所属默认值及公共链接移除，外来默认值逐字节不变，中文文档逐字节保留，以及维护程序尚在并已登记重启删除。正常重启后，`--after-reboot` 再检查维护程序消失及其余状态。准备、实际卸载和正常重启三个阶段的实际结果见下文。

首次真实卸载前 `--prepare` 为 **52 项、0 失败**，实际维护程序返回 **5／拒绝访问**，随后 `--verify` 为 **51 项、36 失败**，文件、链接和注册均保留。实际只读权限 probe 确认 `IsUserAnAdmin=1`、令牌已提升、管理员组成员检查为真；注册表写入和链接 DELETE 打开成功，Office 目录 DELETE_CHILD 与应用／维护程序 DELETE 打开返回 5。根因是镜像策略把整个 Program Files 的文件递归设为受保护组件：COMPONENTFILE 对 BA/SY 仅授予读取执行，阻止正常卸载。

最小修复在 Program Files 三条通用规则之后添加 ReliefOffice 的 `dir`、`dirs`、`files` 三条专用规则，使用既有 CREATEDDIR／CREATEDFILE。Setup 与 native 工具均优先精确路径，其次最长递归前缀，要求 `/` 路径边界；相似名字的相邻目录不受影响。权限定义及删除访问位参照 Microsoft [File Security and Access Rights](https://learn.microsoft.com/en-us/windows/win32/fileio/file-security-and-access-rights)。未修改管理员检查、提权或安全 API，也未让卸载器接管所有权。

预安装、RAM、VHD 镜像与格式化卷的新安装从卷根应用完整策略。升级只遍历新 inventory 的已安装对象；Office 的 14 个文件齐全，目录没有单独条目，所以文件可得到删除权限，不能宣称目录 ACL 也必定改为 CREATEDDIR。未格式化卷的新安装当前只对 Windows 子树应用显式策略，Office 依赖继承；本次没有为尚未证实失败的这两条路径扩大产品改动。

在主虚拟机完全退出后，对同一 `ReliefWin.img` 的第二 NTFS 分区（偏移 68,157,440 字节、长度 4,226,809,856 字节）用实际 native-ntfsimg 精准应用仅含定义和三条 Office 规则的临时策略。应用前确认 14 个清单文件的 BA ACL 均缺 DELETE；应用后 Office 根、字体目录和全部清单文件均为 SY owner/group、BA/SY 完全控制、BU 读取执行。14 个清单文件、中文用户文档和卸载 smoke 状态文件的 SHA-256 前后完全相同，Program Files、Windows、system32 与内核文件四处非 Office 描述符逐字节不变。loop 设备已正常释放；没有替换任何应用或维护 PE，也没有修改 FAT 测试载荷。证据为 `/tmp/reliefwin-brand-host/office-security/main-repair.json`。

随后在同一 AMD64 主镜像运行 `F:\retry-uninstall.cmd`，实际维护程序返回 **0**。从固定 FAT 分区（偏移 1,048,576 字节）读回的 `uninstall.exit` 为 0，`uninstall-verify.log` 完整报告 **51 项、0 失败**。13 个可立即移除的清单文件、七个自有 ProgID／关联、七个开始菜单链接和卸载项均已移除，维护程序保留且登记了重启删除。中文用户文档逐字节保留，全部外来扩展默认值与卸载前快照一致。这个结果验证了同一应用和维护二进制在修正 ACL 后的实际卸载行为。

正常重启同一主镜像后，`restore.cmd` 首先运行 `--after-reboot`；从固定 FAT 分区读回的 `uninstall-reboot.log` 完整报告 **50 项、0 失败**，对应退出状态为 **0**。维护程序已由重启删除队列移除，中文文档原始字节及全部外来扩展默认值继续保留。随后脚本从固定离线载荷复制套件并调用真实注册代码，注册返回 **0**；`restored-install.log` 为 **98 项、0 失败**，`restored.txt` 已写入成功标志。此恢复步骤验证了重新部署后的文件、关联、链接和七个 GUI 应用；最终 ISO 全新安装的独立结果另列下文。

RAM Unicode 修复前的 `a5aa1480…` ISO 另已在无网络 VM 中完成真实 GPT／NTFS 格式化和全新安装，安装 smoke **98 项、0 失败**，格式兼容 **3,436 项、0 失败**，NTFS IO **1,567 项、0 失败**，ReOpenFile **90 项、0 失败**。首次 IO 调用缺少要求的工作目录，补上 `md` 测试前置后才取得上述完整 IO 结果。该轮证据覆盖前一介质部署树的实际复制和注册。

最终 `f425257c…` ISO 已在无网络 VM 中再次清除 GPT、重新分区并格式化 NTFS，完成全新安装，期间没有预先运行手工注册助手。[安装完成截图](../graphics/screenshots/offline-install-complete.png)已保存。直接核对 `/tmp/reliefwin-brand-host/fresh-install.log`，安装器记录 `File security applied to 2685 items under \Device\HarddiskVolume7\ (Status 0x00000000)`，证明本轮安装流程实际成功应用了内嵌权限策略。新装系统已完成简体中文 OOBE，以 `ReliefUser` 完成本文开头表格中的安装、格式、IO、ReOpenFile、Math 及 Math++ 原生检查。随后 `D:\uninstall.cmd` 的准备检查为 52 项、0 失败，真实卸载退出 0，卸载后检查为 51 项、0 失败、退出 0；正常关机并重启后，`D:\restore.cmd` 的重启检查为 50 项、0 失败、退出 0，恢复注册记录返回 0，恢复安装检查为 98 项、0 失败、退出 0。重启检查确认中文用户文档和十种外来扩展默认值均保留，维护程序已删除。本轮没有离线修补 ACL，与前述历史主镜像诊断独立。

正常关机后，对最终安装镜像只读映射并用 `ntfscat` 读取已恢复的 Office 固定清单，14 个文件的 SHA-256 全部匹配最终 `f425257c…` ISO 的部署载荷；结果见 `/tmp/reliefwin-brand-host/final-installed-office.json`。只读映射已在再次启动前释放，没有热修改磁盘。[最终开机到桌面录屏](../graphics/screenshots/boot-to-desktop.mp4)来自该新装系统恢复套件后的正常冷启动，使用普通启动项；从虚拟 CPU 开始执行前连续采集 55 秒，实际 MP4 时长 55.040 秒，桌面约在 29.6 秒出现，未裁剪等待或加速。当前[启动进度图](../graphics/screenshots/boot-progress.png)取自该录屏的即插即用阶段，[桌面图](../graphics/screenshots/desktop.png)由同一 `ReliefUser` 会话通过 QMP 重新取得。

## 当前品牌回归

| 检查 | 结果 |
| --- | --- |
| `python3 sdk/tools/check_branding.py` | Office 权限与 RAM 文件名修正后再次覆盖全树 40,189 个文件，未批准残留及默认值错误为 0。旧版权、真实依赖地址、测试向量和调试协议标签仍按既有逐项规则分类保留。完整结果为 `/tmp/reliefwin-source-brand-audit.json`。 |
| 当前 diff 路径及字节扫描 | 相对 `e01ee711` 的 2,188 个新增/修改文件、34,743,582 字节通过，其中 239 个文件含 NUL。大小写不敏感检查路径、ASCII 以及 UTF-16/UTF-32 的双端序旧产品字节串，含空格、连字符、下划线变体和旧缩写；命中为 0。扫描没有跳过二进制扩展名。统计为本轮检查快照。 |
| `python3 scripts/check_brand_assets.py` | 40 项生成资源通过；2,883 张源图未发现已登记旧图的精确副本。原图摘要、留白、双蓝色、透明度、预乘 RGBA、BGRA、RLE4 及 VGA 共用调色板检查通过；原始 JPEG 与用户附件逐字节一致，SHA-256 为 `124d19dbcafcbabfe3dfac88de9977a9c00411868e246f0c0913c4a4ae3dd5e5`。 |
| `python3 submodules/host-tests/branding/audit-test.py --source-only` | 源码和默认值审计回归 27 项通过；未请求 PE 测试。 |

Office 源码未出现旧产品名称。`UPSTREAM_NOTICE` 与 `UPSTREAM_LICENSE` 逐字节等于来源仓库 `ReliefOSProject/ReliefOS` 固定版本 `bb8682976cb491fde58d97644697a98893a2df3b` 的 `NOTICE`、`LICENSE`。历史 ReliefOS 归属文字保留。两文件 SHA-256 分别为 `a4bdc53acb1756cc1729f5d29b602e1906a508cdd468d14201094c38055e40b3`、`13e40c009094a65298d87c6890898f558b6069b71a7156a487be33d05ae17932`。

字节扫描不能识别任意重新绘制的图中文字；图像检查针对已知旧资源摘要与当前生成合同。以上源码快照与下面的最终 ISO 产物核对分别计数。

## AMD64 安装介质离线核对

部署隔离、Office 权限与 RAM Unicode 修复后，停机重新封装的既有 `output-Clang-amd64-debug/bootcd.iso` 为 **594,857,984 字节**，SHA-256 为 `f425257ce0f5afcba84b4d34e4e331d30dcac2a62ad148aafb7c0abf4c5e179c`。只读解析 ISO 主目录树得到 2,436 个文件、230 个目录；审计前后 inode、大小和修改时间一致。卷标为 `ReliefWin`，发布者和制作者均为 `ReliefWin Project`。完整逐项结果为 `/tmp/reliefoffice-iso-audit.json`。

| 实际 ISO 内容核对 | 结果 |
| --- | --- |
| `deployment/ReliefOffice` 固定清单 | 14/14 文件逐字节等于当前 AMD64 PE 或源文件，无额外 Office 文件；包含七个 GUI、Math worker、字体、INF、维护程序及三份许可/归属文件。 |
| Live 安装状态 | ISO 的 `Program Files/ReliefOffice` 无文件，实际 SOFTWARE hive 无 Office 卸载项、七个 ProgID 或原生扩展关联；介质无 Office 开始菜单/桌面快捷方式，也不含测试 EXE。RAM 可复制独立部署载荷的边界见当前实现段。 |
| 专用安装映射及核心产物 | 介质 `amd64/txtsetup.sif` 逐字节等于 `txtsetup.bootcd.Debug.sif`。`syssetup.dll`、`kernel32.dll`、`ntoskrnl.exe`、UEFI `bootx64.efi`、EFI／PCAT `winload`、两处原始 `bootmgr`、系统图标和 servicing inventory 均与对应当前输出或源文件一致。EFI／PCAT `winload` 也逐字节匹配 Unicode 修复后已冻结的三架构构建证据中的 AMD64 产物。 |
| 安装器内嵌权限策略 | `setup.exe`、`amd64/reliefwin.exe` 和 `Windows/reliefwin.exe` 三份 PE 逐字节等于当前安装器输出。三份实际 RCDATA／120 都逐字节等于修正后的 `image/data/filesecurity.txt`，SHA-256 为 `5405becade1ad1444c003c75ffe3d2f8981340a4095ede6b4328683ef0c0bd85`，包含三条 Office 专用权限规则。 |
| 所有 MZ 文件的版本身份和语言 | 不按扩展名排除文件，共解析 1,597 个 PE；1,079 个 AMD64、518 个 i386，321 个带简体中文 `0x0804` 资源。`ProductName`、`FileDescription`、`CompanyName` 未批准旧身份为 0；两条明确 Wine/ReactOS 原作者归属单列保留。 |
| 已知旧图的嵌入残留 | 从原始 `a676708a` 逐字节取回全部 41 种已登记旧图，额外导出 BMP 的 DIB 和 ICO 单帧摘要。扫描介质文件及 87,849 个 PE 资源 payload，无已知旧图、DIB 或图标帧精确副本。此项不能识别任意重绘的旧图。 |
| 旧名字节与真实 hive 默认值 | ASCII、UTF-16/32 双端序旧名和变体逐文件检查，命中仅为已核对的版权文字、真实上游包名称/网址、KDNET 协议标签及来源说明；未分类命中为 0。实际 hive 的产品名 `ReliefWin`、机器名 `RELIEFWIN`、系统语言 `0804`、ACP/OEMCP `936`、用户区域 `00000804` 和 `zh-CN` 均正确。 |

独立复核另行直接读取同一新 ISO 的目录和三个安装器 PE，没有执行共享审计脚本：三份 RCDATA／120 均为 7,261 字节且逐字节匹配源策略，14 个部署文件匹配当前源产物，Live Office 文件、目录和快捷方式均为空。“安装 ReliefWin”和“使用说明”的 ISO 文件名为正确 UTF-8；独立读取前后两次整盘摘要一致，inode、大小和修改时间稳定。这项离线核对证明新介质实际携带的内容及 Live 初始注册状态；全新安装与原生测试的独立运行结果见本文开头表格。

## 本轮静态分析范围

cppcheck 使用 AMD64 Ninja 规则取得的 include 路径和 defines，外部编译数据库、每文件过滤及单个低优先级工作进程。早期以下 15 个 TU 的分析中，4 个退出 0，3 个遇到包含头解析限制，8 个带有已读源复核的诊断；后两类没有计作零诊断通过。维护格式修复后的当前结果另列在下表之后。

| 文件 | 结果 |
| --- | --- |
| `bootloader/winload/main.c`、`bootloader/lib/pe.c`、`sdk/lib/bootfont/bootfont.c`、`dll/win32/syssetup/target.c` | 退出 0，无诊断。 |
| `maintenance/uninstall.c`、`tests/install-smoke.c`（均在 `base/applications/reliefoffice`） | 在现有 `sdk/include/psdk/shellapi.h:895` 枚举处解析中止。 |
| `dll/win32/syssetup/install.c` | 在构建生成的 `wtypes.h:317` 枚举处解析中止。 |
| `ntos/inbv/inbvgop.c`、`ntos/inbv/bootlogo.c` | 包含头中的优先级索引及原生 SEH 空指针路径报告；按调用前提和异常退出路径复核，未确认本次新增缺陷。 |
| `ntos/ex/exinit.c`、`ntos/io/iomgr/drvinit.c`、`ntos/io/pnpmgr/pnpstate.c` | 上述包含头报告与 `RTL_CONSTANT_STRING` 宏的 `sizeofFunctionCall` 报告；已读源复核。 |
| `bootloader/gui/bmui.c`、`bootloader/winload/ramdisk.c` | 既有字符串遍历的 `arrayIndexThenCheck` 报告；字符串终止与显示宽度含义已读源复核。 |
| `bootloader/winload/registry.c` | 既有 `RTL_CONSTANT_STRING` 的 `sizeofFunctionCall` 报告；已读源复核。 |

修复 `%ls` 并添加卸载后验测试后，重新取得当前 Ninja 的五个实际编译命令，放入构建目录外的 `/tmp/reliefwin-maintenance-compdb.json`，逐文件过滤、以单个低优先级进程重新执行。`maintenance/uninstall.c`、`tests/install-smoke.c` 和 `tests/uninstall-smoke.c` 均在既有 `shellapi.h:895` 枚举解析中止；`syssetup/install.c` 在生成的 `wtypes.h:317` 中止；`syssetup/target.c` 在既有 CRT `stdio.h:90` 的 `_CRTDATA` 宏中止。这五项当前分析均未完成，不沿用先前 `target.c` 的退出 0 结果。逐文件源码摘要、输出与退出状态记录在 `/tmp/reliefwin-maintenance-cppcheck.json`。

本节静态分析范围不包含 Office 全部应用和第三方库；Office 应用、Math 和三架构构建结果另见后续专节。主机模拟不能证明内核 IRQL、实际显存、SetupAPI、注册表重定向、重启队列或 Windows 文件系统锁的客体行为。AMD64 的实际卸载、重启和恢复历史结果见前述安装注册节，实际截图和启动录屏见[截图目录](../graphics/screenshots/README.md)。本轮没有 Windows 11 对照、i386／ARM64 客体运行或实体硬件结果；最终 ISO 全新离线安装后的原生回归结果见本文开头表格。

## Office 文件平台与保存回归补充

Office 移植沿用固定上游版本的模型、编码器和解析器。上游 Unix 实现是历史应用与文档格式的兼容依据；没有将旧 ReactOS 代码作为 Win32 行为设计依据。Windows 接口使用公开 SDK/WDK 声明，具体契约及已读取的 Microsoft 文档链接见 [平台说明](../../base/applications/reliefoffice/platform/README.md)。文件路径从 UTF-8 转换为 UTF-16；句柄相对打开使用 `NtCreateFile` 的 `RootDirectory`，替换使用 `FILE_RENAME_INFO`，恢复文件使用当前用户与 SYSTEM 的受保护 DACL。

| 实际执行 | 结果与覆盖 |
| --- | --- |
| `tests/office-io.cpp`，Linux Clang、ASan/UBSan、泄漏检查，实际原生文档 fixtures 与当前实现 | 最新一次 1,553 项通过，失败 0。覆盖六个非 Math 应用的原始 Unix 文档解码及逐字节重新编码、中文和补充平面字符路径、真实保存/重读、后台任务、截断和恶意头、失败时原模型保留、外部修改冲突、进度取消、恢复文件私密权限与锁、独立描述符锁、两个同步启动的保存任务，以及临时文件清理。 |
| `tests/save-race.cpp`，真实最终 `rename` / `renameat` 的测试链接器包装，ASan/UBSan、泄漏检查 | 初始 53 项在修复前确实有 6 项失败：Word、Excel、PPT 各自同名目标及硬链接别名的两个写者均成功，形成覆盖。加入共同保存事务锁后，同一测试通过。扩展后的 72 项全部通过，覆盖不同目标不互锁、Word 与共享 Todo IO 双向竞争、不同已授权 Unix 用户，以及 FIFO 目标和锁记录必须在 2 秒内拒绝。产品实现没有测试钩子。 |
| 原始上游 `test_reliefdiagram.cpp` 与 `test_reliefmathpp.cpp`，调整 include 路径后直接编译当前模型/解析器/几何实现 | 两项独立 ASan/UBSan 和泄漏检查均通过；未把仅检查源码或重写预期的测试计作原上游通过。 |

保存事务锁采用非阻塞获取，在预期文件戳检查之前取得，覆盖最终替换和清理；同一目录的不同文件保持独立。已有目标同时按实际文件身份锁定，因此硬链接与短名称别名不能绕过合作写者之间的排斥。锁记录持久保留，以免删除重建使两个写者持有不同锁对象。Windows 的身份锁采用实际卷号与 64 位文件号命名的 `Global` mutex；进程内 SRW 保护的身份表拒绝同线程递归别名，跨进程非阻塞获取接受正常或 abandoned 所有权。`FindFirstFileW` 取得文件系统实际匹配的长名，已打开父目录的 `FILE_ID_BOTH_DIR_INFO` 枚举提供文件号；获取身份锁不打开目标文件。POSIX 仍按原文件 `flock`。主机测试没有验证 Windows 文件系统的这些具体语义。

测试期间真实旧 host 进程曾阻塞在 FIFO 的只读 `openat`；现场通过 gdb 栈确认并保留，随后只给该 FIFO 打开/关闭一次写端，使诊断进程退出。修复对目标与既有锁记录使用非阻塞打开并检查普通文件类型，独立的限时回归通过。释放旧诊断进程没有计作修复验证。

Win32 专用测试另包含原生 `C:\\` 与 `/C:/` 根路径锁别名、真实 DACL、句柄重命名及恢复锁；Math++ 提供 `--calculation-selftest` 运行真实计算子进程、无效表达式拒绝和取消回收。Office IO 的这些 Win32 文件分支已有下述最终客体结果；Math++ 独立计算子进程测试不能用 Office IO 的表达式检查代替。文件写入调用 `FlushFileBuffers`；Windows 没有对应 POSIX 目录 `fsync` 的受支持公开操作，因此最终目录项更新的断电持久性未作已验证承诺。未遵守保存锁协议的外部写者仍可能在文件戳检查之后修改目标。

### AMD64 客体文件平台诊断与 ReOpenFile 失败基线

以下结果来自实际 ReliefWin 客体的 Win32 调用与旧 `kernel32.dll`，没有使用模拟文件系统或 Windows 二进制分析。临时诊断程序已在记录结果后移除；回归源 `modules/rostests/apitests/kernel32/ReOpenFile.c` 与独立目标 `reliefoffice_reopen_file` 保留。

| 实际调用或运行 | 已观察结果与含义 |
| --- | --- |
| FAT 上的原始 `fixture.rword` | 普通属性 `0x20`；`GetFileInformationByHandle` 与 `FileBasicInfo` 查询成功；`GetSecurityInfo` 返回 `ERROR_INVALID_FUNCTION`／1。旧平台无条件要求安全信息，使普通 `lstat`／`fstat` 返回 UCRT `ENOSYS`／40。该错误不是 `ELOOP`，UCRT 的 `ELOOP` 为 114。 |
| 同次运行的 NTFS 文件及目录 | 实际 owner／DACL 查询成功；卷 flags 为 `0x00a42087`，没有 `FS_PERSISTENT_ACLS` 位。因此不能只根据当前卷 flags 拒绝安全操作，也不能把缺位当成安全信息必然不可用。 |
| NTFS 目录 `ReOpenFile` | 带 `FILE_FLAG_BACKUP_SEMANTICS` 的属性读取和 `WRITE_DAC` 两种重开均返回无效句柄及错误 5，严格目录 `fchmod` 因而返回 `EACCES`／13。源码无条件设置 `FILE_NON_DIRECTORY_FILE`，与 Microsoft [ReOpenFile](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-reopenfile) 明确允许该标志取得目录句柄的合同矛盾。 |
| `reliefoffice_reopen_file.exe ReOpenFile`，实际旧客体 DLL | **72 项，2 失败，0 跳过**。失败恰为目录属性重开与目录 `WRITE_DAC` 重开，均为错误 5；其余普通文件、身份和数据、同步与重叠读、标志及错误参数检查通过。失败日志来自固定 FAT16 分区的 `reopen-old.log`。已修正测试宽字符串为显式 `%ls`，该基线实际进入了目录检查。 |
| 同一 API 回归，修正后的实际客体 DLL | **90 项，0 失败，0 跳过**；目录句柄成功后执行了额外的身份／属性检查。 |
| NTFS 目录 DACL 权限对照 | `WRITE_DAC | FILE_READ_ATTRIBUTES`（`0x40080`）重开成功，但设置 DACL 返回 5；保留 `READ_CONTROL`（`0x60080`）后返回 0；改加 `WRITE_OWNER`（`0xc0080`）仍返回 5。平台已保留原元数据句柄所需的 `READ_CONTROL`，没有扩大到 `WRITE_OWNER` 或改动 ntmarta。 |
| Excel 拆分断言与直接替换回归，旧身份锁 | 102 次 Excel 失败均明确为 `Could not replace saved file: Permission denied`；直接持有 `SaveLock` 后 `ro::rename` 替换已有目标也返回 `EACCES`／13。不是依据复合断言推断模型被破坏。 |

产品修复使 `ReOpenFile` 只在没有 `FILE_FLAG_BACKUP_SEMANTICS` 时设置 `FILE_NON_DIRECTORY_FILE`。平台仅在实际安全查询返回 1 或 50 时报告未知 owner `UINT64_MAX` 及明确公共的 0666／0444 文件权限或 0777 目录权限，保留普通文件元数据读取；其他错误仍然失败。普通文档的 `set_document_mode` 先执行严格 `fchmod`，只在操作不支持且后续 `fstat` 证实普通文件、未知 owner 与公共权限时允许继续。七个应用的普通保存使用该规则；恢复路径保持严格的权限和 owner 检查，不能把 FAT 伪报为私有 0600／0700。

`reliefoffice_io <fixtures> <work> --public-volume` 执行六个非 Math 应用的实际原生保存、重新打开、外部修改冲突与并发保存，并验证 FAT 严格文件／目录私有化失败。Word 与 Excel 的恢复锁必须在清零 `errno` 后得到实际 `ENOSYS`；PPT 的固定恢复根检查先拒绝根外文件，该项只验证根限制，没有计为执行到 FAT DACL 检查。拆开 Excel 保存／内容断言并增加持锁替换检查后，Linux Clang ASan/UBSan 与泄漏检查运行 1,553 项，失败 0。

`kernel32`、该 API 回归及六个非 Math 应用已在既有 amd64、i386、arm64 构建目录中以 `CCACHE_DISABLE=1` 构建通过，IO／格式测试也已更新；这不是 i386 或 ARM64 运行通过的证据。新客体 DLL 的 API 复测结果见上表；最终目录身份 mutex 与权限修复已在三架构的七个应用、IO／格式、Math worker／测试及 ReOpenFile 中构建退出 0。最终 AMD64 ReliefWin 使用当前源码对应的实际 EXE 完成 NTFS **1,567 项、0 失败**与 FAT **1,552 项、0 失败**；固定 FAT16 payload 起始偏移 1,048,576 字节中的 `io.log`、`fat.log` 已直接读回，均包含完整最终统计。没有 Windows 11 基线。

Microsoft [FILE_RENAME_INFORMATION](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_rename_information) 公开合同说明，普通替换重命名不能覆盖有打开句柄的目标。旧 Windows 身份字节锁恰好保留了该句柄。新实现从父目录取得身份并持有 mutex，既释放了永久目标句柄，也不再为竞争者的身份探测打开临时目标句柄。独立审查追踪了 SRW 发布／撤销、递归、abandoned、跨用户最小 `SYNCHRONIZE | MUTEX_MODIFY_STATE` 权限以及失败清理。真实消费者均在取得锁后检查当前文件戳，并由同一线程退出栈作用域释放锁。

最终 NTFS 的 1,567 项包含六个非 Math 应用的原生文档、中文／补充平面字符路径、重复保存与逐字节重读、截断及恶意输入、取消、过期文件戳冲突、后台任务和清理；同时通过 `READ_CONTROL` 修复后的真实恢复目录／文件 owner、私有 DACL／模式、独占恢复锁及释放检查。身份锁检查通过了持锁目标替换、同线程硬链接别名排斥、跨进程别名排斥、不同目标独立、正常退出释放，以及 owner worker 退出但进程仍存活时的 abandoned 接管。

最终 FAT `--public-volume` 的 1,552 项通过六个应用的普通保存、替换与重读；严格文件和目录私有化继续失败，并报告未知 owner 和明确公共权限。Word／Excel 恢复锁的拒绝来自实际不支持的安全操作；PPT 在固定私有恢复根之外拒绝文件，未把该项说成执行了 FAT DACL。FAT 不执行硬链接成功或私有恢复成功套件。ReliefMath 结果由独立套件记录。

Windows 原生回归另外启动真实子进程验证硬链接排斥、不同目标独立与正常释放；abandoned 检查让子 worker 退出而其进程保留 mutex 句柄，再由另一进程接管。路径通过 ASCII 十六进制传输后还原 UTF-8，避免 CRT 窄参数代码页影响中文。测试没有确定性暂停旧身份探测中的失败竞争者；消除该目标打开的证据来自源码分析，不能把一次通过说成证明所有竞态。外部普通读取仍遵循文件系统的公开共享／重命名规则。NTFS 子进程回归使用同一客体用户，Windows 跨用户会话没有实际运行；跨用户最小 mutex ACL 来自公开合同和源码审查。Linux 的不同已授权用户回归另有记录。上述结果不证明所有竞态或断电持久性，也不扩展为 Windows 11、i386／ARM64 运行或实体硬件通过。

固定 payload 的早期 8 MiB FAT12 空间耗尽，导致后续客体日志截断，不能据此给出完整测试结论。重建为标准 MBR、起始扇区 2048、129024 扇区 FAT16 分区后，实际 `mountvol` 确认它分配为 **F:**；旧脚本仍硬编码 D:，构成介质找不到的直接原因。主机只读 MBR／BPB 与 mtools 目录检查成功，当前 fastfat／fs_rec 的可见识别条件均满足；没有为这个盘符变化修改文件系统驱动。

### IO 测试目录参数检查

全新安装后的首次 IO 调用误传尚未创建的 `C:/OfficeTest`，测试继续执行失败的文件操作，最终触发 UCRT 无效描述符终止；这是测试入口缺少前置检查。创建目录后，同一旧二进制实际运行 **1,567 项、0 失败、退出 0**。现在 `tests/office-io.cpp` 在执行任何用例前，通过 `lstat` 和目录类型检查验证 fixtures 与 work 参数；不存在、普通文件或链接均输出带参数名称与路径的错误，并退出 2。内部锁测试子进程入口保持独立。

修复前的 Linux 主机实测中，缺失或普通文件 fixtures 仍执行 54 项并失败 7 项；缺失或普通文件 work 仍执行 1,552 项并失败 372／375 项，四次均退出 1。修复后的 Clang ASan/UBSan／泄漏检查覆盖 fixtures 与 work 的缺失、普通文件、空路径，以及普通和 `--public-volume` 两种模式：**12 次均退出 2**，标准输出为空，标准错误各一行，工作目录树未被修改。有效既有目录的完整 IO **1,553 项、0 失败、退出 0**。前后命令、输出、源码摘要和产物摘要保存在 `/tmp/reliefoffice-io-arguments.json`。

仅更新 IO 测试目标，使用 `CCACHE_DISABLE=1` 在既有 amd64、i386、arm64 构建目录全部构建退出 0；未修改生产实现或重新打包 ISO。新版 AMD64 测试程序已在最终 `f425257c…` ISO 全新安装的 `ReliefUser` 会话中实际运行：fixtures 与 work 的缺失、普通文件四组输入均各输出一行相应 `Invalid … directory` 错误，退出 2，没有 checks 统计；`D:\fixtures` 与既有 `C:\OfficeTest` 的完整 IO 为 **1,567 项、0 失败、退出 0**。固定 FAT 载荷中的 `io-args-{fixtures-missing,fixtures-file,work-missing,work-file}.{log,exit}`、`io-args-valid.{log,exit}` 与 `io-args-done.txt` 已直接只读核对，完成标志为 `done`。

## Office 全部构建 TU 静态分析补充

使用当前真正优先选择 UCRT 头的 AMD64 Ninja 编译数据库 `/tmp/reliefoffice-compdb.json`。筛选 Office 自有 C/C++ 文件、排除第三方和资源编译命令；相同 TU 只保留第一条真实编译命令，全部原始 include 路径和 defines 原样保留。最终数据库置于构建目录外的 `/tmp/reliefoffice-final-compdb.json`。cppcheck 2.7 对每个文件分别传入精确 `--file-filter`，使用 AMD64 目标 defines、`--enable=warning,performance,portability --platform=win64 --inline-suppr --quiet` 及工具兼容的 `__has_*` 宏。

添加卸载后验测试前，具有构建条目的 90 个 C/C++ TU 均尝试执行：覆盖 `platform`、七个应用及 Math worker、`entry.c`、`localization`、维护程序和 Windows 测试。结果如下，不能将解析中止视作检查通过；新增的 `tests/uninstall-smoke.c` 及维护修改后的复查结果见前面的五文件记录。

| TU 数量 | 实际结果 |
| --- | --- |
| 85 | 在构建的 libc++ `__configuration/platform.h:38` 中，cppcheck 无法计算含 `__has_include(<features.h>)` 的条件，报 `preprocessorErrorDirective` 并退出 2。对平台文件追加工具兼容 `__has_include(x)=0` 后仍为同一限制。 |
| 3 | `entry.c`、`maintenance/uninstall.c`、`tests/install-smoke.c` 在既有 `sdk/include/psdk/shellapi.h:895` 的 `SIID_WORLD` 枚举处报 `syntaxError`，退出 2。 |
| 1 | `reliefmath/worker/freetype_debug.c` 在既有 CRT 头的 `_CRT_INSECURE_DEPRECATE_CORE` 宏处中止；补充工具兼容定义后继续在 `_CRTDATA` 宏处中止，未完成分析。 |
| 1 | `reliefmath/ui_layout.cpp` 退出 0，无诊断。 |

未新增抑制，也未修改系统头以迎合解析器；这 89 个 TU 的静态分析仍未完成。逐文件完整结果为 `/tmp/reliefoffice-final-cppcheck.json` 和 `/tmp/reliefoffice-final-cppcheck.log`。另外 19 个源文件没有 Windows 构建条目，未伪造目标编译参数：历史未构建的 Unix `reliefmath/process.cpp`、5 个 `tests/math/{document,jobs,model,protocol,render}.cpp` 主机测试、`tests/save-race.cpp` 和 12 个 `tests/upstream-*.cpp` 主机回归。它们的运行证据与 Windows 静态覆盖范围分别记录，不能相互替代。

文件平台与 `ReOpenFile` 修复后，另按真实 AMD64 编译数据库逐文件复查 9 个相关 TU：平台、Word／Excel／PPT 文档实现、共享 Todo IO、Office IO 测试，以及独立测试入口、`ReOpenFile.c`、`kernel32/client/file/create.c`。最初 cppcheck 2.7 全部在既有 libc++／CRT 头中止。随后在 `/tmp/reliefoffice-cppcheck-tool/cppcheck-2.22.0/cppcheck` 隔离构建 2.22.0，重新使用每个 TU 的真实编译器 `-dM -E` 输出提供精确目标预定义宏，保留原始 include 路径和 defines、逐文件 `--file-filter` 及规定的 AMD64 参数。新版越过旧版的 `__has_include` 限制，但 6 个 C++ TU 仍在 libc++ `stdio.h:103` 的 `__has_include_next` 条件处中止；3 个 C TU 在既有 UCRT `corecrt.h:615` 或 VCRuntime `crtdefs.h:232` 处语法中止。九项仍均未完成，未计为通过。最新命令、源码 hash、实际编译器预定义宏、输出和退出状态覆盖保存在 `/tmp/reliefoffice-reopen-cppcheck.json` 与 `.log`。

平台 `posix.cpp` 和 `tests/office-io.cpp` 另作补充检查：用真实目标 Clang 命令预处理，保留原始 `#line`，再交给 2.22.0；两项仍在既有 `vcruntime.h:271` 的 extern-C 闭括号处报 `Unmatched }`。这次补充没有伪造 `__has_include_next`，也未改 SDK 头或添加抑制；它独立于原编译数据库尝试，同样不是分析通过。记录在 `/tmp/reliefoffice-preprocessed-cppcheck.json` 与 `.log`。

本次 IO 参数入口修改重新提取了当前 AMD64 Ninja 的实际编译条目，保存为构建目录外的 `/tmp/reliefoffice-io-argument-compdb.json`，以精确文件过滤运行 cppcheck 2.22.0。直接运行在 libc++ 的目标字节序预定义宏检查处中止；加入同一真实编译器 `-dM -E` 产生的预定义宏后，仍在 `stdio.h:103` 的 `__has_include_next` 条件处中止，目标分析未完成。补充按实际 Linux 主机构建参数检查该测试文件，得到四项既有 `ConcurrentSave` 指针成员无构造函数初始化的报告；已逐处核实该类型只有两个聚合实例，均显式初始化这四个指针，worker 启动前已完成初始化，因此未修改或抑制。完整命令、源码摘要与诊断记录在 `/tmp/reliefoffice-io-argument-cppcheck.json` 和 `.log`。

## ReliefMath 原生运行与产物核对补充

最终 `f425257c…` ISO 全新安装的 AMD64 ReliefWin 已在 `ReliefUser` 会话中重新完成两组 Math 原生测试：`reliefmath_tests --timeout` **41 项全部通过**，`reliefmath_tests --public-volume D:/` **24 项全部通过**，对应退出码均为 0。已直接只读核对 `/tmp/reliefwin-branding-fat.img@@1048576` 中的 `::fresh-math.log`、`::fresh-math-fat.log`、`::fresh-math.exit`、`::fresh-math-fat.exit` 和完成标志 `::fresh-done.txt`。

此前同一固定 FAT 载荷分配为 F: 时，实际安装系统也完成 NTFS 41 项与 FAT 24 项，两组退出码均为 0；当时直接核对的日志为 `::math.log`、`::math-fat.log`、`::math.exit`、`::math-fat.exit` 与 `::done.txt`。历史失败基线和修复过程保留在下文。

| 当前原生实际运行 | 结果与范围 |
| --- | --- |
| 真实公式 worker、请求/响应身份、SVG／PNG／RGBA、取消与重启 | 通过；含中文公式，实际使用 Windows 目标静态 Cairo／Pixman 与 FreeType 字形路径。 |
| NTFS 中文文档后台保存/加载、不可变快照、旧文件戳冲突、已有文档成功覆盖及再次加载/final stamp | 通过；当前完整测试为 41 项，包含早期 36 项没有覆盖的正向已有目标替换。 |
| 向不读取输入的 worker 提交 60 KB 请求后取消、复用 client、十秒请求超时 | 通过；日志明确含 `ReliefMath native worker timeout passed`。 |
| FAT 65 字符文档名首次保存、凭有效 stamp 覆盖、重新读取及最终 stamp、旧 stamp 冲突 | 通过；完整 FAT 测试为 24 项，首次长名安装与第二次覆盖均已实际执行。 |
| FAT 文件/目录公共 metadata、严格权限失败、私有恢复存储拒绝与清理 | 通过；普通文档支持没有降低恢复目录的 owner 与私有权限要求。 |

当前日志中的实际 worker 是 `C:\Program Files\ReliefOffice/reliefmath-render.exe`，资源是同级 `ReliefMath`，中文字体是 `C:\Windows/Fonts/NotoSansSC-Regular.otf`。这次运行验证了真正的 Windows 目标依赖构建；Linux 主机 Cairo／FreeType 测试不能代替它。

Math 应用、worker、原生测试与最终共享目录枚举身份 mutex、目录 `READ_CONTROL` 修复一同在 AMD64、i386、ARM64 重新构建，三架构均退出 0。最新构建命令记录于工具会话；当前九个 Math 产物摘要和 worker 导入检查为 `/tmp/reliefoffice-math-artifacts.json`，三架构均无 MSVCRT DLL 导入。此前整组七个应用、worker、Math／Office IO／格式／ReOpenFile／安装／卸载测试、维护程序、syssetup、kernel32、内核与 EFI loaders 的构建快照保留在 `/tmp/reliefoffice-build.log`、`/tmp/reliefoffice-i386-build.log` 和 `/tmp/reliefoffice-arm64-build.log`，不把旧日志当作最新共享平台编译记录。

文档模式与 FAT 身份修复后的 Linux ASan／UBSan 模型、协议、文档、任务套件及 68 项渲染检查通过。新增的 96 次 post-rename 确认打开失败在 64 个 fd 的预算内反复运行，逐次检查已保存的完整文档、输出 stamp 保持不变及无临时名字，覆盖句柄回收。

初次原生渲染实际返回 `Code::Protocol`（9）、`Invalid frame magic`。旧 worker 同时从 MSVCRT 导入 `read`／`write` 的旧名桥接，而 `_fileno`／`_setmode` 来自 UCRT；SVG 正文末尾的 LF 经文本输出扩为 CRLF，物理字节数超过帧内声明长度，下一帧的魔数错位。显式调用同一 UCRT 的 `_read`／`_write` 修复了传输。

第一次名义上的新旧对照仍执行旧安装文件；离线读回 SHA-256 为 `3e4ae8898599ea7176840025d671b83e3efded148fcead9fcd0d19cf629ca057`，证明当时覆盖没有成功，不能把那两份相同失败日志作为新 worker 的验证。随后真正替换产物后，早期 36 项原生测试通过，日志保留在 `/tmp/reliefwin-brand-host/previous-guest-logs/math.log`；当前 41 项结果进一步覆盖已有目标成功替换。

原始 65 字符文件名 FAT 回归的首次 `atomic_save` 曾失败：`Code::Io`（5），`document was replaced, but its installed stamp could not be confirmed`。旧实现错误地把重命名前临时文件 ID 当作不变值。[Microsoft 的 BY_HANDLE_FILE_INFORMATION 文档](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/ns-fileapi-by_handle_file_information) 允许 FAT 重命名到更长名称时改变 file ID，并说明用同时打开句柄的 volume／file ID 比较文件身份。

Math 保存现在保持 writer 跨过 rename，以其重命名后的当前 ID 比对安装文件的只读确认句柄；该只读句柄继续保有身份，writer 关闭后才查询最终 metadata，再检查当前路径仍指向该文件。关闭后的时间查询依据 [WriteFile 的最后写入时间契约](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile)。失败路径回收持有句柄，已 rename 的目标不会作为临时文件删除，关闭失败明确报告文件已经替换。此修复先通过原生 FAT 首次长名保存，再暴露旧共享保存锁保留目标句柄造成的第二次覆盖拒绝访问；最终不打开目标的目录枚举身份锁已通过完整 FAT 24 项及 NTFS 41 项。

静态分析仍未完成。worker、storage、原生测试三个 TU 均用真实 AMD64 编译条目尝试 Cppcheck 2.7，全部在 libc++ 的 `__has_include` 条件处中止；即使工具退出 0，也没有计作分析通过。记录为 `/tmp/reliefoffice-math-cppcheck.json` 和固定工作日志 `/tmp/reliefoffice-math-cppcheck.log`。

另从官方发布源码构建 Cppcheck 2.22.0，临时工具位于 `/tmp/reliefoffice-cppcheck-tool`，发布包 SHA-256 为 `d74945deb2d50393430e07596b766f8a779512c7f60dac2a30ea64e059ece57b`。按三个真实编译条目，用相同目标编译参数执行 `-dM -E` 取得实际预定义宏再查；新版越过旧障碍，但三个 TU 仍在 libc++ 的 `__has_include_next` 条件中止。未删改头文件或用假宏跳过其内容；实际命令、源码及预定义宏摘要、输出记录于同一 Math cppcheck JSON 和工作日志。

另以实际目标编译命令预处理 worker，保留原源码行记录，再做独立补充检查。Cppcheck 在现有 `vcruntime.h` 的 extern-C 结束处报未匹配的右花括号，实际编译器接受同一输入。本项仍未完成，单独记录于 `/tmp/reliefoffice-math-preprocessed.json` 和 `/tmp/reliefoffice-math-preprocessed.log`，没有替代原始编译数据库分析。

本节原生结果覆盖先前所测安装产物及最终 `f425257c…` ISO 全新安装的 AMD64 ReliefWin，没有 Windows 11 对照、i386／ARM64 实际运行或实体硬件结果。主机与客体测试不能证明所有竞争均不存在，也不能扩展为未运行错误路径已通过。
