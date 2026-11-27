# ReliefWin 系统恢复中心

第一版提供启动日志查看、离线驱动禁用与撤销、完整 BCD 文件备份与恢复。
启动菜单中选择 `Startup options → Repair your computer`。
同一个入口也在安装介质中提供，恢复环境直接启动恢复中心，不运行安装程序。
正常系统的“开始 → 管理工具”也提供恢复中心入口。

## 部署

- 系统盘：`\Recovery\ReliefWin\Winre.wim`，不创建额外分区。
- 安装介质：`\deployment\Recovery\ReliefWin\Winre.wim`。
- 恢复环境以 WIM 内存盘运行，系统分区可供离线修复。
- 安装时自动复制恢复环境，并将启动项的 WIM 父设备绑定到实际系统分区。
- 安装完成后保存 `BCD-UEFI.initial` 或 `BCD-BIOS.initial`。
  预安装磁盘镜像同时附带两种初始备份。
- 首次开机更换预安装磁盘签名时，同时更新恢复内存盘父设备和初始 BCD 备份。
- 恢复中心支持简体中文、美式英语、日语。

## 行为

恢复中心按文件标识拒绝把正在运行的 SYSTEM 注册表当作离线系统，包括文件别名。
驱动操作修改 `Select\Default` 所选控制集，保留其他控制集。
所有修改都需要用户选择目标并确认。

驱动禁用前先保存并刷新 `ReliefWinRecoveryOriginalStart`，再设置 `Start=4`。
撤销先恢复并刷新原值，再删除保存值。中途失败时保存值保留，允许继续或撤销。
已由其他操作更改启动类型的驱动不会被覆盖。
`Start=0`、`ErrorControl>=2` 和存在 `StartOverride` 的驱动禁止禁用。
`Services` 下不含 `Type` 的配置键不作为驱动枚举。

BCD 备份复制完整文件。恢复时先暂存、校验并刷新所选备份，
随后将当前配置保存为 `BCD.ReliefWin.before-restore`，最后替换 BCD。
因此也可以选择上一次生成的 `.ReliefWin.before-restore` 文件进行恢复。
无效备份、相同源与目标、文件占用和保存当前配置失败都会中止替换。
BCD 校验在独立临时副本上进行，采用备份／还原权限加载受保护的 hive，
关闭后卸载并恢复原权限状态，不通过加载原文件来校验。
恢复也支持当前 BCD 已损坏或文件已丢失的情况。
本版本不重写分区表、不格式化磁盘、不自动修改启动配置。

日志仅以只读方式打开，最多显示最后 256 KiB。
支持带 BOM 的 UTF-8、UTF-16LE，以及 UTF-8／系统代码页文本。
应用注册表关闭后可能在后台卸载，后续操作最多等待文件释放两秒，超时报告失败。

## 契约来源

注册表、文件与服务常量采用 Microsoft 公开 Win32 文档：

- [RegLoadAppKeyW](https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regloadappkeyw)
- [RegLoadKeyW](https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regloadkeyw)
- [RegUnLoadKeyW](https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regunloadkeyw)
- [AdjustTokenPrivileges](https://learn.microsoft.com/windows/win32/api/securitybaseapi/nf-securitybaseapi-adjusttokenprivileges)
- [RegFlushKey](https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regflushkey)
- [RegQueryValueExW](https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regqueryvalueexw)
- [RegSetValueExW](https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regsetvalueexw)
- [CreateServiceW：启动类型与错误级别](https://learn.microsoft.com/windows/win32/api/winsvc/nf-winsvc-createservicew)
- [Services 注册表树](https://learn.microsoft.com/windows-hardware/drivers/install/hklm-system-currentcontrolset-services-registry-tree)
- [CopyFileW](https://learn.microsoft.com/windows/win32/api/winbase/nf-winbase-copyfilew)
- [MoveFileExW](https://learn.microsoft.com/windows/win32/api/winbase/nf-winbase-movefileexw)
- [FlushFileBuffers](https://learn.microsoft.com/windows/win32/api/fileapi/nf-fileapi-flushfilebuffers)
- [GetFileInformationByHandle](https://learn.microsoft.com/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle)
- [BCD 对象类型](https://learn.microsoft.com/previous-versions/windows/desktop/bcd/bcdobject)
- [BCD 引导管理器元素](https://learn.microsoft.com/previous-versions/windows/desktop/bcd/bcdbootmgrelementtypes)

安装器的 BCD 内存盘设备序列化沿用 ReliefWin 已有消费者
`base/setup/reliefwin/updbcd.c::BuildBcdRamdiskDevice`，
与 `image/bcd/CMakeLists.txt::bootloader_ramdisk_device` 的生成格式对应。
这些私有布局的证据是现有 ReliefWin 消费者，不宣称经过 Windows 11 布局验证。

## 定向验证

`base/system/recovery/tests.c` 与生产程序链接相同的 `operations.c`，
使用临时注册表和文件验证保护规则、持久化、撤销、重复操作、冲突、
控制集选择、当前系统拒绝、BCD 文件内容、无效备份、回退备份恢复以及日志截断。
执行 `recovery_test.exe`，退出码 0 表示全部检查通过。
可追加实际 BCD 文件路径，使用同一测试程序校验原生镜像生成的配置。
测试需要备份权限以创建临时 hive，不修改真实系统的驱动启动类型。
`python3 scripts/test_recovery_image.py --build output-Clang-amd64-debug`
检查生成的 BCD 恢复项、正常默认项、WIM 父设备及实际磁盘签名解析函数的边界。

构建目标：

```sh
CCACHE_DISABLE=1 ninja -C output-Clang-amd64-debug recoverycenter recovery_test setuplib bootcd reliefwinimg
CCACHE_DISABLE=1 ninja -C output-Clang-i386-debug recoverycenter recovery_test setuplib bootloaderstandardbcd bootloaderstandardpcatbcd
CCACHE_DISABLE=1 ninja -C output-Clang-arm64-debug recoverycenter recovery_test setuplib bootloaderstandardbcd
```

当前 amd64 QEMU 中，携带实际 UEFI／BIOS 初始 BCD 的测试共 253 项，全部通过。
缺少驱动 `Type` 的配置键回归测试在修复前失败、修复后通过。
镜像测试另验证启用 HTTP 启动时仍保留恢复入口。
amd64、i386、ARM64 的恢复中心、测试程序、安装库和磁盘签名工具均已定向编译。
amd64 QEMU（2 GiB 内存、双核）中，系统盘与安装 ISO 的恢复入口均已实际启动，
并读取离线系统日志、枚举驱动及启动分区 BCD。

提交前使用 amd64 构建的编译数据库和目标宏检查五个改动的 C 文件。
cppcheck 2.7 无法解析 `shellapi.h` 的枚举，`main.c` 的静态分析因此不完整。
它对 `bootsup.c` 的重复释放报告经源码核对为误报：工具配置将
`RtlFreeHeap` 的第一个参数视为释放对象，实际释放对象是第三个参数，
两次调用分别释放独立分配的 `Entries` 和 `Header`。

Windows 11 基线、真实硬件、i386／ARM64 运行验证需要对应测试环境；
完整安装流程也尚未运行，编译通过不代表这些环境中的行为已经验证。
