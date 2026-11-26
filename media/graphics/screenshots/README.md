# ReliefWin 实际运行截图

这些截图来自本次源码构建的 amd64 `ReliefWin.img` 或 `bootcd.iso`，通过无显示窗口的 QEMU 实际启动并用 QMP 转储屏幕，分辨率为 1280 × 900。PNG 是屏幕像素的无损转换，没有合成或修饰。默认界面为简体中文。

窗口外观的后续更新见[Windows 11 风格三键与统一底色的边缘](caption-buttons.png)及[关闭键悬停](caption-buttons-hover.png)：亮色内圈与黑色外圈均已改为对应窗口边框底色。它们来自部署新版默认主题后的已安装系统，ReliefWord 通过原生文档关联实际启动。下列较早截图与 ISO 录屏保留其原始拍摄状态。变更范围与验证限制见[窗口按钮验证记录](../../doc/RELIEFWIN_CAPTION_VALIDATION.md)。

![ReliefWin 桌面](desktop.png)

[完整开机到桌面录屏（MP4，55 秒）](boot-to-desktop.mp4)来自较早 `f425257c…` ISO 新装系统，使用恢复套件后的镜像和普通启动项进行正常冷启动。从虚拟 CPU 开始执行前采集，连续保留实际时间，没有裁剪等待或加速；桌面约在 29.6 秒出现。该历史录屏尚未包含后续启动画面 Logo 更新。当前桌面截图由同一新装系统的 `ReliefUser` 会话通过 QMP 重新取得。

改为大型居中图形标志后的[引导加载阶段](boot-loader.png)与[内核加载阶段](boot-progress.png)来自同次新版模块部署后的实际冷启动：两个阶段都只显示放大的 ReliefWin 标志，不带产品文字；顶部进度条和居中的加载组件文字继续工作。主机定向测试、三架构构建及实际运行范围见[启动 Logo 验证记录](../../doc/RELIEFWIN_BOOT_LOGO_VALIDATION.md)。

| 系统位置 | 截图 |
| --- | --- |
| 启动画面 | [查看](boot.png) |
| 桌面与任务栏 | [查看](desktop.png) |
| 开始菜单 | [查看](start.png) |
| 关于 ReliefWin | [查看](winver.png) |
| 设置中的系统信息 | [查看](settings.png) |
| 系统属性 | [查看](sysdm.png) |
| 更新设置 | [查看](updates.png) |
| 安装语言与键盘 | [查看](setup.png) |
| 仅保留三种语言后的实际语言列表 | [查看](supported-languages.png) |
| 最终 ISO 的 Live 桌面与中文安装快捷方式 | [查看](live-installer.png) |
| 最终 ISO 的全新离线安装完成 | [查看](offline-install-complete.png) |
| 首次配置中的账户页面 | [查看](oobe-account.png) |
| 可选组件 | [查看](oobe-components.png) |
| 命令提示符 | [查看](console.png) |
| ReliefOffice 开始菜单中的七个入口 | [查看](office-start-menu.png) |
| ReliefOffice 套件卸载入口 | [查看](office-uninstall-entry.png) |
| ReliefWord 原生文档 | [查看](office-word.png) |
| ReliefExcel 原生工作簿 | [查看](office-excel.png) |
| ReliefPPT 原生演示文稿 | [查看](office-ppt.png) |
| ReliefTodo 原生任务清单 | [查看](office-todo.png) |
| ReliefDiagram 原生图表 | [查看](office-diagram.png) |
| ReliefMath++ 原生几何文档 | [查看](office-mathpp.png) |
| ReliefMath 原生公式文档 | [查看](office-math.png) |

启动图中的 TianoCore 标志和灰色日志来自测试虚拟机的 OVMF 固件。关于对话框中的原作者版权仍按来源保留；它与当前 ReliefWin 产品名称、Logo 分开显示。命令提示符中的 `ver` 保留 NT10 兼容输出，随后实际查询注册表所得的 ProductName 为 ReliefWin。可选组件中标为上游软件包的项目保留真实来源，不是本项目的更新服务。

构建、镜像审计、测试结果及未验证范围见[验证记录](../../doc/RELIEFWIN_VALIDATION.md)。

三种界面语言清理后的截图来自本次资源部署并冷启动的已安装系统，真实首次配置程序的下拉框仅有简体中文、美式英语和日语。保留的兼容数据、原厂驱动例外与最终审计范围见[语言验证记录](../../doc/RELIEFWIN_LANGUAGES_VALIDATION.md)。

启动进度与 Office 移植的构建和实测记录见[补充验证记录](../../doc/RELIEFWIN_OFFICE_VALIDATION.md)。Office 截图通过已注册的原生文件关联打开来自 Unix 源码的格式样例。

Live 桌面和离线安装完成两图来自修正 RAM Unicode 文件名后的最终 ISO。“安装 ReliefWin”和“使用说明”显示正常，从安装图标启动安装器后，在无网络 VM 上清除旧 GPT、重新分区并格式化 NTFS，完成全新安装；没有预先运行手工注册助手。

原生菜单的后续更新见[菜单栏、弹出菜单与子菜单](modern-menus.png)和[系统桌面右键菜单](modern-context-menu.png)。它们来自本次菜单相关模块部署后的已安装系统，默认 96 DPI；前者使用真实原生菜单测试窗口，后者直接在桌面打开并展开“排列图标”。两张画面均由 QMP 原样取得。改前／改后同程序对照、三架构构建、字体测试修正及未验证范围见[菜单验证记录](../../doc/RELIEFWIN_MENUS_VALIDATION.md)。
