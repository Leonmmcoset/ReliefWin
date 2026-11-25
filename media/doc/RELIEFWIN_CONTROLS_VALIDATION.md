# 原生圆角控件验证

默认主题的按钮、输入框、可编辑及只读下拉框、复选框、单选框、滚动条、进度条、标签页、数值微调器、滑块、分组框、工具栏按钮及列表／树项目背景改用现代圆角。普通控件采用 96 DPI 下 4 px 圆角，单选框和滑块手柄为圆形；相接的分段边缘保持直线。

设计依据为 Microsoft 的 [Geometry](https://learn.microsoft.com/en-us/windows/apps/design/signature-experiences/geometry) 指引。这是 ReliefWin 自有主题的视觉设计，不是 Windows 11 Win32 控件逐像素对照结果。六个 12 px Fluent SVG 来自固定 Microsoft 上游修订，保留原始文件、原生尺寸及 MIT 许可证。来源、配色、状态和生成命令见 [素材说明](../themes/Win8/win8.msstyles/sources/MODERN_CONTROLS.md)。

NORMAL 和 ContentLight 共 78 张新位图使用 32-bit BI_RGB、straight alpha；uxtheme 在载入时执行预乘。圆角外不涂固定颜色。三个字体变体均绑定新素材，字体与系统度量没有变化。此前 258 张位图逐字节保持，包括已修复的标题栏按钮和无描边窗口边缘。

## 原生绘制

实际旧代码运行中，独立数值微调器的透明角显示默认底色，进度条跑马灯会持续把圆角外的背景染亮。数值微调器现在在透明箭头绘制之前恢复父窗口背景，再绘制 buddy 边框；进度条先完整恢复父背景及轨道，再绘制填充，条纹与 `GetThemeBackgroundRegion` 返回的填充区域相交。空内容矩形不进入条纹绘制，区域生成或剪裁失败时保留已绘制的填充，所有保存的 DC 状态均恢复。

父窗口背景绘制会同步调用应用程序。代码在回调后验证窗口仍持有原控件状态；进度条还验证主题句柄，改变时退出本次绘制。画刷清理由绘制前捕获的所有权决定，`EndPaint` 使用捕获的 HWND，避免回调销毁控件后的状态访问。主题区域辅助函数先销毁持有位图的 DC，再删除位图，覆盖成功及绘制失败出口。

上述 API 合同依据 Microsoft 的 [DrawThemeParentBackground](https://learn.microsoft.com/en-us/windows/win32/api/uxtheme/nf-uxtheme-drawthemeparentbackground)、[GetThemeBackgroundRegion](https://learn.microsoft.com/en-us/windows/win32/api/uxtheme/nf-uxtheme-getthemebackgroundregion)、[ExtSelectClipRgn](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-extselectcliprgn) 和 [DeleteObject](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-deleteobject)。控件私有状态的位置来自各自现有创建／销毁消费者，没有新增 Windows 结构字段、消息或接口。

## 父背景绘制范围

真实截图检查发现，属性页及控件展示中的部分内容会被后绘制的按钮擦掉。按钮类使用 `CS_PARENTDC`，其 DC 可以覆盖父窗口；新主题的透明圆角使按钮请求父背景，而 `DrawThemeParentBackground` 在 `prc=NULL` 时没有限制绘制范围，父页面的整页背景填充因此覆盖其他子控件。旧、新 Tab.Pane 都有实色中心，问题来自背景 API 的范围，不是标签页面板的颜色。

Microsoft 文档明确：空 `prc` 表示子控件占据的整个范围，显式矩形使用子窗口坐标。新回归入口 `ParentBackgroundClip` 使用偏移子窗口、非零 viewport、对比底色及父窗口位置标记，扫描整张 DIB；覆盖空／非零矩形与无／已有裁剪的四种组合，检查范围、父坐标映射和绘制后 clip／viewport 恢复。父窗口绘制消息的次数只记入日志，不将未公开的次数约定作为断言。

背景函数现在始终将请求范围与现有 DC 裁剪相交，父坐标的偏移取自子窗口原点，避免显式矩形的非零 left／top 被额外计入。DC 在回调前保存，成功及裁剪／偏移失败出口均恢复；保存失败则不进入绘制。原有父窗口递归保护、无父窗口的提前返回及空矩形的回调路径保持。

`ParentBackgroundSiblings` 动态调用实际 comctl32 v6 的 `PropertySheetW`，在页面中创建 static、edit 和两个按钮。分别取得控件自身绘制样本，再完整重绘属性页及最后一个按钮，核对此前三个子控件的样本保持。像素来自同一控件自身的绘制，不假定系统配色或 Windows 11 像素。

## 滑块圆心

实际截图中，灰色轨道仍按旧尖头手柄定位，默认圆形手柄与轨道相差 1.5 px。现在按主题报告的 TrueSize、UniformSizing 和 TS_DRAW 尺寸计算圆形手柄的实际绘制中心，并让轨道厚度与圆形直径保持相同奇偶性。默认轨道从 4 px 调为 3 px，使两者在像素网格上的中心完全相同。沿轴端点和手柄命中矩形保留；`TBM_GETCHANNELRECT` 与 custom draw 收到的矩形也反映实际轨道位置。创建、主题变化及固定手柄长度变化都会重新计算。

`TrackbarCenter` 在真实 comctl32 v6 上捕获 840 帧，覆盖深浅两套已验证不同的配色、横竖两轴、七种样式、普通／固定尺寸、最小／中间／最大值、调整大小、主题禁用及还原。检查真实像素中心、公开几何矩形、custom draw 矩形和重复绘制 GDI 数量。144／192 DPI 部分仅检查主题资源 API 尺寸。

在同一个 AMD64 程序 `c3bbf6d019aaf29d39756d41f67c1601353f8c4f51d681b8fe9894ddafd248a0`、同一主题及 uxtheme 下，旧 v6 DLL 的 7,762 项检查失败 792 项；修复后的同一入口失败 0、跳过 0、退出 0。默认案例的两倍中心值从 21／24 改为相等；其他固定尺寸的半像素偏移也被捕获。修复版另独立通过 `TrackbarBuddyLifetime` 的 61 项检查，覆盖 buddy 移动回调销毁滑块后停止布局，不在旧实现上故意触发已确认的释放后访问。

冷启动截图中，灰线实色范围 y=655…657，蓝色手柄核心 y=653…659，两者中心均为 y=656。[局部放大](../graphics/screenshots/rounded-controls-slider.png) 由真实 1280×900 截图裁切并以 nearest-neighbor 放大 3 倍。`DeleteObject(DC)` 改用 `DeleteDC` 依据 API 合同；当前 GDI 已转发前者，旧版 GDI 数量检查也通过，不声称测得旧版泄漏。

## 独立滚动条

真实展示检查还发现，独立 `ScrollBar` 仍使用原生方形绘制路径。现在普通 `SBS_HORZ`／`SBS_VERT` 的箭头、轨道和手柄在绘制、按下、拖动及 setter 重绘时使用主题；输入与滚动通知保留原生状态机。显式 `WM_CTLCOLORSCROLLBAR` 自定义画刷、主题禁用回退及 `SBS_SIZEGRIP`／`SBS_SIZEBOX` 的专门绘制路径保留。非客户区滚动条没有改动。

主题所有者保存在 uxtheme 私有的窗口数据项，锁、引用及 generation 保护创建、主题替换和清理。每次绘制另持有短期主题引用，且用既有 user API hook 引用保护模块；父背景／颜色回调销毁窗口或替换主题后停止旧帧，并恢复 DC。私有导出 `UXTHEME_OpenNcThemeData` 仅桥接本项目已有的内部 cdecl 函数，不是新增的 Windows API 或 Windows ABI；短期引用不发布或恢复窗口的 `ux_theme` 属性。setter 使用项目已有的 `SIF_THEMED`，不新增消息或 hook 结构字段。

## 素材及构建

`test_modern_controls.py` 的 7 组检查通过，覆盖实际位图头、状态、透明度、抗锯齿和 Fluent 覆盖，核对所有 INI／RC 引用，检查重复生成、损坏拒绝和 DPI 属性。既有 caption 的 5 组及 frame 的 4 组检查也通过。主机素材检查不等同于系统中的绘制或操作验证。

在现有 `output-Clang-{amd64,i386,arm64}-debug` 目录、`CCACHE_DISABLE=1` 下构建 `win8.msstyles`、`uxtheme`、`comctl32`、`comctl32_v6` 和 `uxtheme_apitest`，三个架构均退出 0；临时启用的 `ENABLE_ROSTESTS` 均已恢复为 `OFF`。

首轮样式对照的 AMD64 主题 SHA-256：`2a0ae4c070b0428069e9f16b7207881cd3ce3517fd2ae1e34c338b4f5688fbb9`。

首轮样式对照的 AMD64 测试程序 SHA-256：`f719a0d925c07fe3b9a56f3d326758ffb26ec621d2550093088c137d4b80d314`。

`RoundedControls` 使用作用域内的 activation context 加载实际 comctl32 v6；其独立 manifest 位于资源 2，不改变其他测试的默认上下文。矩阵覆盖 44 个部件、深浅两套配色和两种对比底色，可拉伸部件使用两种目标尺寸，TrueSize 部件使用原生尺寸；检查透明圆角、分段直边、半透明边缘、状态及目标区域之外的像素。列表／树普通与禁用状态另检查完全不绘背景。

真实原生控件检查包括按钮通知、复选／单选切换、编辑与焦点、两种下拉框的选择及展开、滚动范围、标签选择、滑块／数值微调器数值、列表／树选择与文字；进度条覆盖满格、归零、重填和跑马灯多帧。无 buddy 的微调器只核对位置，独立核对有 buddy 时的整数转换状态。

## 客体对照

本节及后续早期运行是历史证据。后来的配色审查确认，旧程序仅使用默认类和 `ContentLight`，在默认应用模式下可能两次都选中浅色；这些运行不能证明深色覆盖。当前测试显式选择 ForceDark／ForceLight，并要求实际文字颜色及绘制签名不同。最终运行结果另列于本文末尾。

开发宿主随后在 2026-11-25 15:30 UTC 重启，清空了 `/tmp`。以下历史阶段的临时日志及 JSON 已丢失；计数和校验值保留自当时实际工具输出及审阅记录，不能再打开旧临时路径复核。最终运行重新生成持久化的 [构建与测试结果](RELIEFWIN_CONTROLS_RESULTS.json)。源码、已安装系统镜像和截图均未丢失。

使用同一个首轮测试程序、原生 DLL 和安装盘：原 ISO 主题执行 9,005 项，失败 3,144、跳过 0；圆角主题执行相同 9,005 项，失败 80、跳过 0。圆角主题中的全部 44 部件矩阵、状态、抗锯齿与原生功能检查通过，80 个失败均来自独立微调器及进度条的角像素。输入框和两种下拉框原生检查通过，没有为假设问题修改它们的绘制代码。

随后补充区域资源和销毁回归，使用同一个 AMD64 程序 `d16c276a706d86827830f583e7ad6c04da83b130299d97f998d37a78a607ddaf`、同一个主题 `941c5de5c1f6b5395d74fe7fb8ce56d2231562ef3dd9ebe3fd174ef4a118e712` 比较原始及修复 DLL：

| 检查 | 项数 | 原始 DLL 失败 | 修复 DLL 失败 | 跳过 |
| --- | ---: | ---: | ---: | ---: |
| RoundedControls | 9,219 | 84 | 0 | 0 |
| RoundedControlLifetime | 85 | 未运行 | 0 | 0 |

原始 DLL 的 84 个失败为上述 80 个角像素和 4 个未触发父背景回调的微调器销毁检查。区域重复创建／删除的 GDI 数量检查在两边均通过；本客体没有测到原实现的资源增长，位图删除顺序的修正依据 API 合同。跑马灯还检查内部像素网格的帧哈希发生变化，避免把停止动画误判为圆角修复。

`RoundedControlLifetime` 在修复后的产品上独立运行，覆盖 `WM_PRINTCLIENT` 与实际 `WM_PAINT` 中父回调销毁进度条、回调切换主题及后续位置／圆角重画。这一入口没有在旧实现上故意触发已由源码确认的释放后访问，不声称完成该问题的旧版失败对照。

每轮都在关机后部署／收集，主盘为原有 `ReliefWin.img`、载荷为固定 FAT 镜像；仅更新对应位数的 v6 winsxs DLL 及 System32／SysWOW64 的 uxtheme，部署后逐字节校验。v5 DLL 校验值保持。监视器使用 `ROS_QEMU_DISPLAY=none --timeout 30 --stall 6`，QEMU 无网络。原生基线有一轮监视器包装进程返回 143，但测试及 done 文件已完整写出且客体正常关机；修复轮测试退出 0、监视器退出 0。

## DPI 回归

独立检查发现新素材绑定缺少旧主题已有的 `TrueSizeScalingType=Dpi` 与 `TrueSizeStretchMark=50`，使复选框、单选框及六种滑块手柄在高 DPI 下保持源图尺寸。仅恢复这 8 个部件、两套配色、三个字体变体的上述属性，保留 TrueSize 与 UniformSizing，不改变 78 张位图、源图大小及紧凑目标区域的适配。

新增独立入口 `RoundedControlDpi`，追加前的整个测试源码前缀逐字节等于上一版，不修改普通控件与生命周期用例。使用 AMD64 程序 `175571975c27ef917b84993e8fb24cb84ff7ac1d89c57e37cabc902a2cb64bf1`，原属性实际执行 146 项，失败 32、跳过 0，恰为两种配色、8 个部件在 144／192 DPI 下的绘制尺寸。96 DPI 与 TS_TRUE 源图尺寸均通过。主机属性检查也在原三套 INI 上报告 48 个预期缺失部件。

设计尺寸为复选框／单选框在 96、144、192 DPI 下依次绘制为 16、24、32；滑块手柄依次为 20、30、40。TS_TRUE 始终报告 16 或 20 的源图尺寸，实际原生控件的小目标矩形仍由 UniformSizing 适配。这是本主题的缩放合同，不是 Windows 11 的逐像素尺寸声称。

## 早期控件及 DPI 运行（历史记录）

恢复 DPI 属性后，AMD64 主题 SHA-256 为 `6f9e39436085fbda6724844e5ab9ffd187b3afce935d7004cd94f289e139655c`，测试程序为上述 `175571975c27ef917b84993e8fb24cb84ff7ac1d89c57e37cabc902a2cb64bf1`，使用已经通过原生对照、尚未加入父背景范围修复的 DLL。这一历史阶段冷启动后的三组结果如下：

| 入口 | 检查 | 失败 | 跳过 | 退出状态 |
| --- | ---: | ---: | ---: | ---: |
| RoundedControls | 9,219 | 0 | 0 | 0 |
| RoundedControlLifetime | 85 | 0 | 0 | 0 |
| RoundedControlDpi | 146 | 0 | 0 | 0 |
| 合计 | 9,450 | 0 | 0 | — |

批处理写入完成标记，客体正常关机，监视器退出 0。关机后重新读取并核对主题、两种位数的 v6 comctl32 及 uxtheme 校验值。这一历史阶段的日志 SHA-256 为 `e2702190e94eb455dc7f0e308b71d9b2f4f599f145e8f1a6649fdcabc7d5c545`。

## 父背景对照及六组运行（历史记录）

父背景回归使用同一个最终 AMD64 测试程序 `228a9220c43848c1d7f6569311250bb2665c9a655568c9a120a2b2d01b7435a7`，在相同主题及 v6 comctl32 下分别冷启动旧、新 uxtheme。旧版 AMD64 uxtheme SHA-256 为 `eeeeb6c662ddf1ee1ee568bdd0be4c809e1c0e217d9edaacd7efece35f31f10a`。两项新增回归的实际对照如下，均无跳过：

| 入口 | 检查 | 旧 DLL 失败／退出状态 | 修复 DLL 失败／退出状态 |
| --- | ---: | ---: | ---: |
| ParentBackgroundClip | 34 | 3／3 | 0／0 |
| ParentBackgroundSiblings | 153 | 54／54 | 0／0 |

旧版的三个 DIB 失败分别为：`prc=NULL` 且无已有裁剪时改写子区域外 18,432 个像素；显式非零矩形在无／已有裁剪下分别有 32／22 个内部像素映射错误。已有裁剪覆盖子区域的 `prc=NULL` 用例通过。属性页检查中，三个先前绘制的控件在整页及最后按钮重绘后共 54 个样本被父背景覆盖。相同最终二进制在修复 DLL 上均通过，确认修复的是这些实际失败。

随后在最终 DLL 上冷启动执行全部六组，结果如下：

| 入口 | 检查 | 失败 | 跳过 | 退出状态 |
| --- | ---: | ---: | ---: | ---: |
| RoundedControls | 9,219 | 0 | 0 | 0 |
| RoundedControlLifetime | 85 | 0 | 0 | 0 |
| RoundedControlDpi | 146 | 0 | 0 | 0 |
| ParentBackgroundClip | 34 | 0 | 0 | 0 |
| ParentBackgroundSiblings | 153 | 0 | 0 | 0 |
| DrawThemeParentBackground | 42 | 0 | 0 | 0 |
| 合计 | 9,679 | 0 | 0 | 各入口 0 |

批处理写入完成标记，客体正常关机，监视器退出 0。关机后重新读取并逐字节核对部署产物。最终运行及背景基线的 SHA-256 如下；i386 DLL 已部署并校验，但不代表执行了 i386 测试程序。

| 产物 | SHA-256 |
| --- | --- |
| 最终 AMD64 测试程序 | `228a9220c43848c1d7f6569311250bb2665c9a655568c9a120a2b2d01b7435a7` |
| AMD64 主题 | `6f9e39436085fbda6724844e5ab9ffd187b3afce935d7004cd94f289e139655c` |
| AMD64 uxtheme | `d6a50f4d8b6f15a68f9586eecb6b0c96d03dc61a8f969af83f0aa4c973060bc9` |
| i386 uxtheme | `f9905e83e5bb1f9d6250539886d0dd75c4e0fba7dd2bfa3865c6acef0a326ad9` |
| AMD64 v6 comctl32 | `fa516b70730a6e077a57b96bc4c0d4a573933543761e38d359c79a7e5ff6bf27` |
| i386 v6 comctl32 | `936210c4fcdfff911a48d5b997b57800fd27c7ceca797f08cc9b7dbcb7bf78a0` |
| 最终六组日志 | `63e1db8c4a35c318cd1989b480ab513db686b829a9ec803e4e196e026f8ee4c0` |
| 相同最终程序的旧 DLL 背景基线日志 | `f39d33eeb89014728375e71a6c86f254aa3df87de01404fa1a9e62a9756d5518` |

最终 `RoundedControls.c` SHA-256 为 `30cfcfc9ed0ef4d4caa8c785ac61709f632934923d915c4a5c2081342fc79222`，`draw.c` 为 `0184fb541e5f7d7d606e355075b837135841657aaf96b357f0488f834a0d8eb7`。精确构建和运行记录分别位于开发环境的 `/tmp/reliefwin-controls-build.json` 与 `/tmp/reliefwin-controls-validation.json`；固定工作日志为 `/tmp/reliefwin-controls.log`。

运行环境为简体中文 ReliefWin AMD64、ReliefUser、1280×900、96 DPI；QEMU q35、TCG、max CPU、2 vCPU、2048 MiB，无网卡。源码、三架构构建产物及现有安装系统镜像已更新；本次没有重新打包安装 ISO。

## 截图

最终构建的 AMD64 安装系统完成冷启动后，通过 QMP 取得以下桌面图。控件展示包含实际原生滑块与独立滚动条；画面中灰轨道与蓝色手柄中心对齐，滚动条的手柄、箭头及轨道均走主题绘制。滑块局部图源自真实 1280×900 帧，按最近邻方式放大，不移动或合成控件像素。

- [原生圆角控件展示](../graphics/screenshots/rounded-controls.png)
- [显示设置属性页](../graphics/screenshots/rounded-controls-settings.png)
- [滑块轨道与圆形手柄中心](../graphics/screenshots/rounded-controls-slider.png)

显示设置属性页图是此前拍摄的界面示例，不作为本次滑块修复的最终证据；居中结论以最终 `TrackbarCenter` 测试及本次冷启动控件展示为准。

## 证据范围

本环境没有可用的 Windows 11 参考虚拟机，不能声称 Windows 11 像素或 API 行为对照通过。i386／ARM64 测试程序仅构建，未作对应架构的客体或硬件运行。DPI 检查使用 `OpenThemeDataForDpi`／`GetThemePartSize` 验证尺寸合同，没有切换客体桌面缩放或验证高 DPI 屏幕上的真实控件像素。分组框和工具栏的素材与绑定经过主机检查，但不在 44 部件客体像素矩阵中；分组框的设置页截图尚待确认，工具栏没有独立原生运行断言。自行绘制控件的应用由其自身绘制代码决定外观。

cppcheck 使用构建目录之外的实际编译数据库、逐文件过滤及目标 defines。内置解析器在既有 `sdk/include/vcruntime/crtdefs.h:232` 的 `errno_t` 声明处中止；一次实际 Clang AST 尝试因 importer 丢失 `--target` 而无法分析 Windows SDK。两次均不计为静态分析通过，没有添加假 SDK 宏或抑制。
