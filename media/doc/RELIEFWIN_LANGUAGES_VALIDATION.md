# 三种界面语言的清理与验证

ReliefWin 的界面语言范围固定为简体中文 `zh-CN`、美式英语 `en-US` 和日语 `ja-JP`，默认简体中文。此次保留已有翻译；缺失的翻译继续使用原有英语回退，不代表三个语言版本均已翻译完整。

## 删除范围与兼容数据

删除 3,558 个独立 RC 翻译文件、4 个独立 MC 文件、24 个其他语言的文档翻译及罗马尼亚语翻译说明。混合资源中的其他语言消息、INF 字符串及对应构建入口同时清理。语言构建选项只接受上述三个语言和 `all`，其中 `all` 也只启用这三种语言。安装器和首次配置的语言列表使用相同范围。

三个架构最终构建完成后，清理现有构建目录中 96 个不再被生成 RC 或 Ninja 图引用的其他语言消息输出；仍被引用的英语消息保持原样。所有生成 RC 也检查了语言标记，未发现其他界面语言。

区域格式、键盘布局、字体、字符编码和 NLS 数据按用户确认保留。136 个安装键盘布局头保持原样；SIF 除界面语言节外保持原样。API 及兼容性测试中用于验证国际化合同的语言样例不作为产品翻译删除。IMM32 的繁体输入模式图像与内核区域数据按实际资源编号分类，不能将其误认为可选界面语言。

用户明确批准保留原厂 MediaTek／Ralink 网卡驱动包。闭源 `drivers/network/dd/netr28ux/RaCoInstx.dll` 原样保留，其 49 个繁体中文编号资源是唯一的厂商界面例外。审计同时要求文件名、完整 SHA-256 和资源三元组匹配；没有哈希、内容变化、其他文件名和额外资源均不豁免。

```text
6b342af3086911b07e60aecc10700d7d10203d7ff5674b70b664a74bf2ef1ca6
```

## 安装器相关修正

初始界面语言来自实际 UI 语言而非区域 LCID，键盘可独立选择；OOBE 仍使用一个“语言和区域”选项。不支持的区域或原系统语言不会生成已删除的界面选项；保留原有受支持默认值。键盘列表只枚举一次，避免中文配置重复整份列表。选中的键盘配置置于首位，保留已有的语言／布局配对，允许与界面语言不同的键盘；格式无效的 KLID、容量不足和注册表错误会返回失败。句柄清理只关闭已取得的句柄，热键设置失败向上传递。

KLID 的八位十六进制形式及低字语言标识来自 Microsoft [LoadKeyboardLayoutW 文档](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadkeyboardlayoutw)。既有布局表中的配对优先于低字推导，保留已有 IME 布局配对。没有增加内核 ABI 字段或架构分支。

完整 ARM64 构建发现 WoW64 子构建忽略外部 `HOST_TOOLS_DIR`、硬编码尚不存在的本地目录。子构建现继承实际工具路径；未指定时沿用既有默认路径，原工具构建依赖保留。

## 验证方法与限制

资源检查覆盖受版本控制的源码与预编译 PE、现有构建目录的运行时 PE、最终 AMD64 安装 ISO 和已安装系统。RC、MC、INF 对照改前提交，确认保留语言文本没有变化；PE 检查资源目录的语言编号及版本资源内部的语言标记。中性资源、区域数据、输入数据和批准的厂商例外分别计数，不能把结果表述为整个仓库没有其他语言的字节。

版本资源的布局依据 Microsoft 的 [VS_VERSIONINFO](https://learn.microsoft.com/en-us/windows/win32/menurc/vs-versioninfo)、[StringTable](https://learn.microsoft.com/en-us/windows/win32/menurc/stringtable) 和 [Var](https://learn.microsoft.com/en-us/windows/win32/menurc/var-str) 文档。真实记事本 PE 的内存变异测试证明：单边为零的资源目录、外层语言编号不变而内部俄语标记出现、畸形版本结构均会失败。此检查识别语言编号与元数据，不能自动识别错误标为英语的异语文字。

主机测试提取实际产品函数，使用 AddressSanitizer 和 UndefinedBehaviorSanitizer，覆盖全部 65,536 个 LANGID、UI 与区域独立性、全部 88 个 SIF 键盘布局、重复配置、容量边界、注册表失败及句柄所有权。改前代码分别在目标断言失败；它们验证真实函数的逻辑，不能代替 Windows 11 注册表行为或客体系统调用测试。WoW64 测试执行实际 CMake 参数生成，覆盖默认、自定义、空值及含空格路径。

| 检查 | 当前结果 | 回归证据 |
| --- | --- | --- |
| 保留翻译内容 | 685 RC、22 MC、210 INF 一致 | 改前源码被语言范围审计检出 33,489 项 |
| 安装语言、OOBE 和初始化 | 1,118,353 checks，0 失败 | 原优先级、旧语言列表与旧初始化分别失败 |
| 键盘选择及配置配对 | 1,475 checks，0 失败 | 改前实现 169 失败；旧配对丢失实现 20 失败 |
| 注册表错误与所有权 | 94 cases／4,140 checks，0 失败 | 改前 48 失败，其中 8 例关闭未取得的句柄 |
| WoW64 工具路径 | 4 cases，0 失败 | 改前两个自定义路径失败 |
| PE 目录、内部版本语言及精确厂商例外 | 218 checks，0 失败 | 原目录检查接受两种畸形变异；隐藏俄语与错误厂商哈希均被拒绝 |

复现入口如下，改前提交固定为 `fa2a7ff1c0472dbfd6beab71c332db3f10416c9e`，避免提交后 `HEAD` 的含义变化：

```sh
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_ui_languages.py --baseline fa2a7ff1c0472dbfd6beab71c332db3f10416c9e --verify-regression fa2a7ff1c0472dbfd6beab71c332db3f10416c9e
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_setup_language.py --sanitize --verify-regression --verify-ui-regression fa2a7ff1c0472dbfd6beab71c332db3f10416c9e --verify-initialization-regression fa2a7ff1c0472dbfd6beab71c332db3f10416c9e
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_setup_keyboard.py --sanitize --verify-regression fa2a7ff1c0472dbfd6beab71c332db3f10416c9e --verify-profile-regression
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_setup_registry.py --sanitize --verify-regression fa2a7ff1c0472dbfd6beab71c332db3f10416c9e
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_wow64_host_tools.py --verify-regression fa2a7ff1c0472dbfd6beab71c332db3f10416c9e
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_pe_resource_audit.py --verify-regression
PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_ui_languages.py --no-source --pe output-Clang-amd64-debug --pe output-Clang-i386-debug --pe output-Clang-arm64-debug
```

实际运行使用现有 `output-Clang-amd64-debug/ReliefWin.img`，离线更新产品文件后正常冷启动。QEMU 为 q35／TCG、2 vCPU、2048 MiB，无网卡，1280×900、96 DPI；通过 `ROS_QEMU_DISPLAY=none --timeout 30 --stall 6` 运行监视器。已进入桌面并启动真实首次配置程序，语言下拉框只列出三个批准选项。[截图](../graphics/screenshots/supported-languages.png) 是 QMP 的原始无损采集，没有合成或修饰。取消向导后通过系统关机界面正常关机，随后只读审计安装磁盘；未在 VM 使用时修改镜像。

本环境没有 Windows 11 参考镜像，相关行为的 Windows 11 对照为 `INFO_NEEDED`；不声称已通过 Windows 11 行为基线。i386 与 ARM64 的运行时和实际网卡硬件未经本次验证。完整构建与主机测试均不能证明这些平台已经正常启动。

已用实际 AMD64 编译数据库、构建定义和逐文件过滤器对四个更改的 C 文件尝试 cppcheck 2.7。解析被已有 `stdio.h:90` 的 `_CRTDATA` 和 `shellapi.h:895` 的 `SIID_WORLD` 阻断，退出码 0 不作为静态分析通过；没有添加压制或伪造 SDK 宏。

`git diff --check` 指出的 DiskPart 英文消息四处尾空格原已存在，本次因相邻翻译删除而显示为新增行；为保留消息原文未修改这些字节。

最终构建、审计和测试计数见[机器可读结果](RELIEFWIN_LANGUAGES_RESULTS.json)。

最终构建使用现有 Clang Debug 目录，`CCACHE_DISABLE=1`：ARM64 和 i386 的 `all` 目标、AMD64 的 `bootcd` 目标均成功，包含对应 WoW64 子构建。AMD64 的这次启动验证使用部署后的已安装系统；最终 ISO 另做完整内容审计，本次没有重新进行一次全新安装。

| 最终产物审计 | PE 数量 | 未批准语言或解析失败 |
| --- | ---: | ---: |
| AMD64 构建目录 | 1,555 | 0 |
| i386 构建目录 | 1,063 | 0 |
| ARM64 构建目录 | 1,535 | 0 |
| AMD64 安装 ISO | 1,527 | 0 |
| AMD64 已安装系统 | 1,552 | 0 |

构建目录的数量包括对应 WoW64 子构建及既有暂存副本，不表示独立组件数。源码中的 17 个预编译 PE 另行检查通过。ISO 的 49 项厂商例外与安装系统相同；构建输出目录不复制该原厂文件，不能只检查构建目录来代替 ISO 审计。

最终关机后，安装磁盘中的全部 1,552 个 PE 均与最终构建或批准的源预编译文件逐字一致；没有未匹配文件。构建末尾更新的代码完整性相关文件也已部署，重新启动并取得截图后才完成此比对。

最终 `bootcd.iso` 的 SHA-256 为：

```text
87161922d369dfa4d101bec9efd9bbb542bd389caf8c9dd37fbce292bd1d1090
```
