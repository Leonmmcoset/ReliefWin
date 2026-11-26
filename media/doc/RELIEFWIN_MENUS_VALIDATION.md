# 原生菜单现代化验证

系统普通右键菜单、弹出菜单及菜单栏改用现代几何。96 DPI 下，弹出面板圆角为 8 px、外侧留白为 6 px、悬停背景圆角为 4 px；普通行至少 28 px，并保留字体上下各 6 px。分隔行高 9 px，中间只有一条 1 px 细线。勾选、单选标记及四向箭头使用未经修改的原版 12 px Microsoft Fluent UI System Icons。

这些尺寸是 ReliefWin 的产品设计，依据 Microsoft [Geometry](https://learn.microsoft.com/en-us/windows/apps/design/signature-experiences/geometry) 指引，不是实测 Windows 11 像素常量。[素材说明](../themes/Win8/win8.msstyles/sources/MODERN_MENUS.md) 记录固定上游修订、MIT 许可证、位图及共享字形掩码的生成方式。

## 绘制与坐标

实际系统菜单由 win32k 直接绘制，因此修改实际原生菜单渲染器，同时为公共主题 API 消费者更新 MENU 部件资源。内核从系统菜单颜色生成背景、悬停和细边，不调用用户态 uxtheme；两者共享几何值及相同原生字形覆盖。应用程序自行绘制菜单、显式背景画刷、回调位图及分栏菜单仍遵守应用程序的绘制合同。

普通主题弹出窗口取消旧非客户区边框。合成开启时通过已有圆角属性由 DWM 裁切；无合成时使用窗口圆角区域。硬区域只用于无合成路径，避免损伤 DWM 抗锯齿。每次打开创建自己的窗口，关闭时清理；菜单已打开期间切换合成状态未作专门验证。

菜单行的绘制与命中共用滚动偏移，滚动箭头和面板留白不命中隐藏项目。公开的 `GetMenuItemRect` 按实际客户区原点、滚动及 RTL 映射返回屏幕坐标，`MenuItemFromPoint` 使用对应逆变换。弹出菜单的 `GetMenuBarInfo(OBJID_CLIENT, 0)` 按 ReliefWin 产品约束从整个客户区计算，而不再从首项外推。重新打开菜单会复位滚动位置；子菜单按父项对齐，方向包含从所有者继承的 RTL 布局。

`MenuItemFromPoint` 在 DPI-unaware 调用者入口将逻辑屏幕坐标转为物理坐标，再执行命中；因此公开矩形与公开命中可以往返。公开 `MenuItemFromPoint(HWND, HMENU, POINT)` 由 user32 的 C 包装明确拆成底层 X、Y 参数，修正 64 位聚合传参；导出元数据使用两个指针和八字节整数，避免 relay 将 POINT 当作浮点值。未命中返回文档规定的 `-1`，不再泄漏内部 `0xffff` 选择标记。

定位按原始锚点选择显示器，锚点位于工作区内时使用工作区，`TPM_WORKAREA` 强制使用工作区，其他情况使用完整屏幕。子菜单使用位于父菜单内部的物理点选择边界，避免父菜单右缘恰在工作区的半开边界上时错误覆盖任务栏。内部结束跟踪标记改用与公开 TPM 标志不重叠的私有高位，消除与公开 `TPM_WORKAREA` 的位冲突。长菜单的最终客户区高度和滚动范围同时扣除真实边框并限制在可用区域。

RTL 单项和整菜单重绘都在保存的 DC 范围内规范布局，避免高 DPI 物理绘制后的映射不一致。主题菜单滚动重新绘制实际面板，不把物理像素矩形传给采用逻辑单位的滚动接口。极小的最大高度会限制箭头区域并裁剪绘制。

新绘制路径运行在既有 USER 锁和菜单对象生命周期内；共享字形数据只读，DC 锁仅用于读取布局并立即释放，不跨调用持有。现代绘制路径拒绝所有者绘制与回调位图，不新增应用绘制回调。保存 DC 失败即停止该次绘制，成功后的字体、画刷、布局及剪裁均恢复。无合成区域经现有窗口接口复制，调用方释放原区域，复制品由窗口销毁路径回收。没有增加 Windows 结构字段或架构分支。

物理／逻辑 DPI 绘制辅助函数设置缩放时保留 DC 的 RTL 负横向 extent。依据现有 GDI 坐标消费者，横向比例为 viewport extent / window extent；用正值覆盖会丢失镜像方向。改动仅修正该方向不变量，纵向及 LTR 缩放保持原合同。

API 定义及合同来自 Microsoft 公开 SDK `vsstyle.h`、`vssym32.h`、[winuser.h](https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/um/WinUser.h)、[windef.h](https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/shared/windef.h)，以及 [x64 参数传递](https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention)与 [ARM64 ABI](https://learn.microsoft.com/en-us/cpp/build/arm64-windows-abi-conventions)文档。菜单合同参考：

- [TrackPopupMenuEx](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-trackpopupmenuex)
- [CalculatePopupWindowPosition](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-calculatepopupwindowposition)，含工作区锚点规则及 `TPM_WORKAREA` 对 TrackPopupMenu／Ex 的支持说明
- [GetMenuItemRect](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmenuitemrect)
- [MenuItemFromPoint](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-menuitemfrompoint)
- [GetMenuBarInfo](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmenubarinfo)
- [SetMenuInfo](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setmenuinfo)
- [WM_PRINTCLIENT](https://learn.microsoft.com/en-us/windows/win32/gdi/wm-printclient)
- [CreateRoundRectRgn](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createroundrectrgn)
- [SetWindowRgn](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowrgn)
- [SetThreadDpiAwarenessContext](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setthreaddpiawarenesscontext)

## 验证方法

`ModernMenu.c` 使用真实 `TrackPopupMenuEx` 的嵌套消息循环。计时器观察实际菜单窗口并输入键盘，不用自绘菜单模拟系统菜单。原生内容检查捕获实际 `WM_PRINTCLIENT`，覆盖深浅系统菜单配色、留白、行高、分隔线、字形及公共坐标。公共主题部件的圆角由主题 DIB 测试验证；原生窗口的区域查询在 DWM 未提供应用区域时仅记录说明，不能作为 DWM 外角的独立断言。交互检查覆盖返回命令、禁用项目、勾选状态、子菜单开关、取消、屏幕边缘及重复清理；长菜单检查滚动后的绘制、命中和重新打开。

交互测试通过公开 `TPM_NOANIMATION` 关闭该次展开动画，并在既有 24 ticks 总界内等待真实屏幕采样与独立 `WM_PRINTCLIENT` DIB 的对应像素一致，然后推进按键。选中阶段还要求公开状态含 `MFS_HILITE`、参考像素与选中前不同；实际画面不更新会超时失败。DIB 绘制不会替实际窗口补画。系统正常动画路径仍保留。菜单跟踪保持 DPI-unaware；读取真实屏幕像素时短暂进入 PMv2、重新获取物理项目矩形并读取屏幕 DC，随后恢复原上下文。RTL 客户区使用 `MapWindowPoints` 的双点矩形转换，禁用状态按创建时实际掩码检查是否保持。动态背景回归在 `WM_INITMENUPOPUP` 内执行有／无自定义画刷的两个反向切换，验证最终边框、客户区和命中同步。

客体字形检查比较实际原生菜单与独立公共主题绘制结果的非背景轮廓；它不证明 RTL 左箭头的逐级 alpha 覆盖。原版左右箭头各自的原始 alpha、固定 SVG 校验值及共享头的一致性由主机素材测试验证。

原有 `MenuTheme` 文本回归改为与相同主题字体（未定义时为 DC 当前字体）、背景、格式及指定 RGB 的独立 GDI `DrawTextW` DIB 逐像素比较，同时要求参考确实含文本并保持绘制面积上限。依据 [DrawThemeTextEx 字体合同](https://learn.microsoft.com/en-us/windows/win32/api/uxtheme/nf-uxtheme-drawthemetextex)及 [LOGFONTW 的抗锯齿质量](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfontw)，公开 API 不保证笔画中存在完全覆盖的纯白像素。实际探针在两个部件均观察到 195 个文字像素，最大 RGB 样本为 `fefcf9`，主题字体未定义而使用 DC 字体；原来的纯白像素断言误报。32 位 BI_RGB 的高字节未使用，参考比较只读取低 24 位 RGB。临时探针已移除。

复用现有三个架构构建目录，编译时设置 `CCACHE_DISABLE=1`。AMD64 客体使用既有 `ReliefWin.img` 及固定 FAT 载荷镜像；仅在正常关机后更新磁盘并收集最终日志；保留失败客体时只读日志用于诊断。监视器使用 `ROS_QEMU_DISPLAY=none --timeout 30 --stall 6`。环境为简体中文 ReliefWin、ReliefUser 会话、1280×900、96／144 DPI，QEMU q35 / TCG / max CPU / 2 vCPU / 2048 MiB，无网卡。

## 证据范围

本环境没有 Windows 11 参考镜像，`INFO_NEEDED`：相同测试程序的 Windows 11 公共 API 行为基线尚未取得，不能声称 Windows 11 逐像素或 API 对照通过。i386、ARM64 的构建不代表已完成对应客体或硬件运行。144 DPI 的公开交互、实际屏幕选中像素及菜单定位已单独验证；该缩放下未运行固定 96 px 的资源外观断言。关闭 DWM 的硬窗口区域分支未单独运行，菜单打开期间切换 DWM 也需要额外环境验证。每应用深浅模式不等于全局内核菜单配色；原生菜单的深浅覆盖以实际系统颜色切换为依据。

`MenuItemFromPoint` 网页对弹出菜单使用客户区坐标的描述，与官方 SDK 的 `ptScreen` 参数名及当前 Wine 的屏幕坐标行为存在矛盾。本次使用屏幕坐标与 `GetMenuItemRect` 往返验证 ReliefWin；Windows 11 的裁决仍属 `INFO_NEEDED`。`rcBar` 精确等于弹出客户区及各项像素尺寸均为 ReliefWin 产品约束，未将其宣称为 Windows 11 公共合同。

## 实际结果

菜单相关的 `win32k`、`user32`、默认主题及测试程序均在既有 amd64、i386、ARM64 目录编译成功，`CCACHE_DISABLE=1`；三个目录的 `ENABLE_ROSTESTS` 已恢复 OFF。这里是相关目标构建，不是全系统重新编译或重制安装 ISO。运行验证为 AMD64 客体，其他架构只完成构建。

旧产品由本地提交 `8193276317f886ac1019916f38759bf259cba833` 的原始菜单、DPI、user32 及主题源文件在同一构建目录重建；构建后立即恢复本次源文件。临时基线部署时，从已安装最终产品读取字节到内存，部署基线后将这些相同字节恢复至既有构建输出，未创建另一份可执行文件缓存。两次基线运行后恢复同一最终产品。所有下表运行使用同一 AMD64 测试程序 SHA-256：

`5ec124df78361f2b9886a2dd5e82d100a8bd54572e21b4af2d4b6177de699073`

| 环境 | 入口 | 检查数 | 失败 | 跳过 |
| --- | --- | ---: | ---: | ---: |
| 改前 96 DPI | MenuTheme | 50 | 0 | 0 |
| 改前 96 DPI | MenuAppearance | 194 | 37 | 0 |
| 改前 96 DPI | NativeMenuAppearance | 414 | 64 | 0 |
| 改前 96 DPI | MenuInteraction | 2014 | 169 | 0 |
| 改前 96 DPI | NativeMenuScroll | 5329 | 127 | 0 |
| 改后 96 DPI | MenuTheme | 50 | 0 | 0 |
| 改后 96 DPI | MenuAppearance | 194 | 0 | 0 |
| 改后 96 DPI | NativeMenuAppearance | 430 | 0 | 0 |
| 改后 96 DPI | MenuInteraction | 2086 | 0 | 0 |
| 改后 96 DPI | NativeMenuScroll | 5329 | 0 | 0 |
| 改前 144 DPI | MenuInteraction | 2019 | 131 | 0 |
| 改后 144 DPI | MenuInteraction | 2091 | 0 | 0 |

计时器重试与分支会影响执行的检查总数；上表保留每次真实结果，不把两个环境的总数当作逐项等价证明。两侧使用同一 ReliefUser 会话配置和 AMD64 原生程序；基线只替换相关 AMD64 产品文件，未运行 WoW64 程序。144 DPI 日志的初始字体注册表值为 `0x90`，96 DPI 为 `0x60`；DPI-unaware 进程输出的 `MENU_DPI system/window/gdi=96` 是虚拟化结果，不是客体未切换。

主机素材检查已通过：20 个位图一致性、5 组菜单素材检查、7 组原有控件回归。所有构建源码、产物、部署文件及运行日志摘要见[机器可读证据](RELIEFWIN_MENUS_RESULTS.json)。

已按实际编译数据库、目标定义及每文件过滤器对全部更改的 C 文件尝试 cppcheck 2.7。解析被已有的 `stdio.h:90` `_CRTDATA` 与 `shellapi.h:895` `SIID_WORLD` 问题阻断；返回码 0 不能当作完整静态分析通过。未伪造 SDK 宏或添加压制。源代码经过针对生命周期、DC 状态、坐标单位及分配失败路径的人工与独立审查；这不替代尚未完成的静态分析。

实际截图：

- [原生菜单与展开的子菜单](../graphics/screenshots/modern-menus.png)
- [系统桌面右键菜单](../graphics/screenshots/modern-context-menu.png)

截图来自最终产品在默认 96 DPI 下的正常冷启动，通过 QMP 无损取得原始画面，未合成或修饰。
