# 启动 Logo 交接修复

基于 `ff070cb6873ab3181347a393a6565b86485e399f`，修复引导程序与内核交接后出现两个 ReliefWin Logo 的问题。

原来的 `BmUiBeginBoot` 在屏幕高度四分之一处绘制独立图标；`InbvGopHandleBootBitmap` 接管后在顶部绘制另一份带字 Logo，却没有清理引导程序留下的图像。

现在两个阶段都使用 `ntos/inbv/resources/reliefwin_logo.bmp`，共用 `sdk/include/reliefwin/bootlogo.h` 的布局计算，水平居中、距顶端约屏幕高度的 8%。内核在持有现有 INBV 显示锁、确认显示所有权后，先清除旧画面，再绘制唯一 Logo，随后初始化原有顶部进度条和加载文字。未增加延时或修改进度计算。

布局及 Logo 是 ReliefWin 自有设计，不引入 Windows ABI 定义。引导程序解码的格式由已有 `scripts/generate_brand_assets.py` 的 `bitmap32` 生成；回归测试固定检查 BMP V4、32 位 BGRA 位域、透明度和数据长度。格式依据为 Microsoft [BITMAPFILEHEADER](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader) 和 [BITMAPV4HEADER](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv4header)。

## 定向验证

| 检查 | 结果 |
| --- | --- |
| `python3 scripts/test_boot_logo.py` | 交接 48 项、资源和像素布局 569 项通过；调用实际内核接管与绘制函数、实际引导 Logo 绘制函数。五种屏幕尺寸从 1 × 1 到 3840 × 2160，默认缩放下整幅 Logo 帧缓冲逐像素相同。另覆盖无 GOP、查询失败、空尺寸、资源失败、失去显示所有权、重复交接和 DPI 范围。 |
| 修复前的回归 | 改动产品代码前运行交接测试，在原图标位置必须清黑的断言处失败；修复后同一断言通过。 |
| `python3 scripts/test_boot_brand.py` | 内核进度 15,091,427 项、引导进度 2,105 项、字形映射 768 项通过。 |
| 三架构增量构建 | `CCACHE_DISABLE=1 ninja -C output-Clang-{amd64,i386,arm64}-debug -j2 ntoskrnl bootloader` 均退出 0。复用已有构建目录。 |
| AMD64 安装介质 | 正常关机后执行 `CCACHE_DISABLE=1 ninja -C output-Clang-amd64-debug -j2 bootcd`，退出 0；复用原有 `bootcd.iso`，未重复安装验证。 |
| cppcheck | 使用 AMD64 构建数据库中两个变更 C 文件的三个实际编译单元；在已有 CRT 头 `stdio.h:90` 的 `_CRTDATA` 宏处中止，未计作通过。 |

## 实际启动

停机后仅更新既有 AMD64 `ReliefWin.img` 的内核、两种 winload、PCAT bootmgr 和两处 EFI bootmgr，逐个读回确认与构建产物一致。通过无网络、无显示窗口的 QEMU / OVMF 正常冷启动，使用固定既有测试载荷；监视参数为 `--timeout 30 --stall 6 --keep --continue-after-observation`。

同次启动的原始 QMP 截图：

- [引导程序：Loading SYSTEM…](../graphics/screenshots/boot-loader.png)，约 6.04 秒，进度条白色部分 128 / 1280 像素。
- [内核：Loading registry…](../graphics/screenshots/boot-progress.png)，约 7.89 秒，进度条白色部分 793 / 1280 像素。

两图均为 1280 × 900，无合成或修饰。排除顶部进度条后，从 y=10 到 y=649 的整个画面逐像素一致；原中部独立图标所在矩形完全为黑色。系统随后正常进入简体中文桌面，并完成正常关机。

实际启动的 AMD64 内核 SHA-256：`e856c9efc71c3cc1e1d1d8eddb84c68b2474a57ff107e9fdc65494a8127a7bff`；winload.efi：`08a67211f211d723bbf38b0bbc8ca1ef2178d768488965c3ab775570ea1944d2`；bootmgfw.efi：`3a1b561adc1b851a7f485dab084c721ab004832e97a4d505d78f0ba33a6572ed`。

随后封装的 ISO 为 557,629,440 字节，SHA-256 为 `a70084ab26cf5bb0460af7f840751f97488a421d77ca9e3dd4a16817010a612a`。封装过程更新其余模块及内核中的完整性目录，以上截图对应先前定向部署的内核；未将 ISO 封装后的二进制计作已启动实测。

本轮未重做系统全量语言审计、Office 测试或全新安装。i386、ARM64 仅完成构建，未运行客体。实际截图为默认 96 DPI；内核保留已有的自定义 DPI 缩放，引导早期使用默认 DPI。未执行 Windows 11 基线，本次自有品牌画面不声称 Windows 绘制实现或像素一致性。
