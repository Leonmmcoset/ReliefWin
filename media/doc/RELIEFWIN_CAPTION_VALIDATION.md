# 窗口按钮及边框验证

默认主题的常规最小化、最大化／还原、关闭按钮改为 Windows 11 风格：96 DPI 下每键 46 × 32，三键紧邻排列，完整的 16 像素 Fluent 图标居中，关闭键仅在悬停／按下时显示红底。主题标题高度同步调整，避免旧高度裁掉按钮下缘。46 × 32 是本主题的设计尺寸，不是 Windows API 的固定合同。

常规、最小化和工具窗口 frame 的白色／亮灰内圈与黑色／暗色外圈均已移除，深浅两套、活动／非活动状态均覆盖。12 张 frame 位图仅改变 1,696 个原描边像素，替换为各自原主体色；尺寸及其余像素保持不变。18 张 frame 的 36 个状态均为对应主体纯色。最大化和 sizing 模板原本没有描边，6 张对应位图逐字节未变。窗口缩放命中区域没有更改。

产品修改仅涉及默认主题资源及 INI，未改动窗口消息处理、DWM 或内核代码。Fluent SVG 来自固定 Microsoft 上游修订，保留原始路径和尺寸。来源、生成命令和颜色见 [caption 素材说明](../themes/Win8/win8.msstyles/sources/CAPTIONBUTTONS.md)及 [frame 素材说明](../themes/Win8/win8.msstyles/sources/FRAMES.md)。

## 构建及产物

在现有 `output-Clang-{amd64,i386,arm64}-debug` 中以 `CCACHE_DISABLE=1` 构建 `win8.msstyles` 和 `uxtheme_apitest`，三个架构均退出 0。为构建测试临时打开的 `ENABLE_ROSTESTS` 均已恢复为原来的 `OFF`。

最终 AMD64 主题 SHA-256：`6a5d38b82cb8d44d58548e0425272cd922705d042bb16ff741049505cdd0b81d`。

最终 AMD64 测试程序 SHA-256：`5800c6635334fed09db73251667eae564f213b2f710d202bc2eabf45ffac3880`。

停机后将主题部署到现有 GPT 安装盘的 `/Windows/Resources/Themes/Aero/aero.msstyles`，用 `ntfscat` 逐字节核对，再解除 loop 映射后冷启动。更新已有用户的持久化指标时，通过“显示属性”重新应用默认主题。没有复制磁盘镜像，也没有在运行的 VM 上修改磁盘或载荷。

此轮未重打包此前交付的 `bootcd.iso`。新截图来自已安装系统中的最终主题；较早截图和启动录屏保留各自拍摄时的内容。

## 素材检查

`test_captionbuttons_fluent.py` 的 5 组检查通过：读取实际 8 张 BMP 的头和全部 64 个状态，独立核对原始 SVG 覆盖、颜色、尺寸、透明度及重复生成结果，并拒绝损坏／缺失素材。旧按钮位图不满足新尺寸与像素检查。

`test_frames.py` 的 4 组检查通过：逐像素核对实际 18 张 frame 的两个状态、原几何和 BMP 头，覆盖三份 INI 中 72 个消费者，检查重复生成及损坏／缺失拒绝。原始黑色加亮色描边和中间仅保留暗线的两组 12 张位图均在同一纯色检查中失败。

## 客体回归

使用上述同一个最终测试程序，先部署原 ISO 中的旧主题，再停机替换为最终主题；两个阶段均冷启动、使用相同已安装系统指标和用户权限。旧主题 SHA-256 为 `442d89a662c91e8ff4b15f1d129ae38bdd8f4e3a16cbd35442781a3c62f9104d`。结果逐项比较如下：

| 测试 | 检查数 | 旧主题失败 | 最终主题失败 | 跳过 |
| --- | ---: | ---: | ---: | ---: |
| CaptionGlyphs | 67 | 19 | 0 | 0 |
| NonClientHitTest | 6 | 0 | 0 | 0 |
| NonClientBorder | 25 | 0 | 0 | 0 |
| DwmCaptionButton | 19 | 0 | 0 | 0 |
| NonClientCaption（既有测试） | 11 | 2 | 2 | 0 |

`CaptionGlyphs` 检查实际 `WM_GETTITLEBARINFOEX` 的 46 × 32 尺寸和连续排列、窗口自身 backing 中的无描边背景、屏幕字形尺寸与居中、灰色／红色悬停，并通过真实 `SendInput` 点击最小化、最大化、还原、关闭，核对窗口状态、还原前后矩形与关闭后的生命周期。实测 min 字形为 10 × 2，max 为 12 × 12，close／restore 为 10 × 10。测试窗使用空标题，防止文字光晕落到背景参照点；顶部参照取标题内部，避免把窗口外缘当成按钮底色。没有为通过检查调高原有颜色容差。

既有 `NonClientCaption` 的两个失败在同一程序、相同指标的旧／新主题上相同：`g` 到第 35 行、`o` 到第 34 行，以及第 35 行触及 36 行标题带底缘。该测试把任意 RGB 差异当作字形边界，而当前标题光晕也会改变这些像素；这不是可据以断言实际字形裁切或 Windows 11 对齐的证据。本次未修改标题文字绘制，也未删除、放宽或计通过这两个检查。

工作日志为 `/tmp/reliefwin-caption-baseline.log` 和 `/tmp/reliefwin-caption-final.log`，客体固定 FAT 载荷中的 `caption.exit` 为最终 `0`。构建记录为 `/tmp/reliefwin-caption-test-build.json`，最终源码的 cppcheck 记录为 `/tmp/reliefwin-caption-glyphs-cppcheck.json`。

## 证据范围

实际运行环境为 AMD64 ReliefWin、简体中文 `ReliefUser` 会话、无网络／无显示窗口 QEMU，1280 × 900、96 DPI，使用现有 `ReliefWin.img` 与固定 FAT 测试载荷。监视器参数为 `--timeout 30 --stall 6`。

普通状态与关闭键悬停的 [ReliefWord 截图](../graphics/screenshots/caption-buttons.png)和[悬停截图](../graphics/screenshots/caption-buttons-hover.png)均为 QMP 实际屏幕转储，没有合成、裁剪或修饰。应用通过已注册的 `.rword` 文件关联打开原有格式样例。

实际拖动 ReliefWord 右边缘从 `(1225,500)` 到 `(1245,500)` 后，原窗口外部点 `(1235,500)` 从桌面色 `RGB(82,168,220)` 变成窗口滚动条灰色 `RGB(111,111,111)`；反向拖动恢复后重新变回原桌面色。恢复尺寸后重新取得两张最终截图，没有改动文档。该检查实际验证了取消描边后仍可从原边缘拖动缩放，记录为 `/tmp/reliefwin-caption-resize.json`。

本环境没有可用的 Windows 11 参考 VM；这些结果不能称为 Windows 11 像素或 API 行为对照通过。i386／ARM64 仅构建，未做客体或硬件运行。运行时高 DPI、浅色窗口、工具窗口仍未逐项截图验证，素材检查不等同于这些运行通过。

对修改的 `CaptionGlyphs.c` 使用构建目录外的实际编译数据库、逐文件过滤及 AMD64 目标 defines 运行 cppcheck 2.22。分析在既有 `sdk/include/vcruntime/crtdefs.h:232` 的 `errno_t` 声明解析中止，退出 2，未计为静态分析通过，也未增加假宏或抑制。
