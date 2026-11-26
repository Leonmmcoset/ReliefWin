# 启动画面大型 ReliefWin 标志

启动加载画面现在显示单独的蓝色 ReliefWin 图形标志，不带旁边的产品文字。标志水平、垂直居中，高度约为屏幕高度的四分之一，并保留原始长宽比。winload 与内核共用同一幅透明 PNG 导出的 BMP V4 资源和布局计算；内核交接时清屏，再画标志、顶部进度条及居中的加载组件文字。非 GOP VGA 启动使用独立 RLE4 图标资源，关机 Logo 沿用原资源。

标志来源是 `media/graphics/branding/reliefwin-mark.png`。其 BMP BGRA 字节与透明源图逐像素一致。格式使用本项目现有 `bitmap32` 生成器；BMP 文件头和 V4 位域依据 Microsoft [BITMAPFILEHEADER](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader) 与 [BITMAPV4HEADER](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv4header)。没有引入 Windows Logo ABI。

## 验证

| 检查 | 结果 |
| --- | --- |
| 修复前基线 | `python3 scripts/test_boot_logo.py --baseline 8e1b49b5699` 按预期失败，指出该版本仍嵌入小型文字 Logo。 |
| `python3 scripts/test_boot_logo.py` | 47 项交接检查、1,215 项资源与布局检查通过。测试调用实际 loader 和内核绘制函数；六种分辨率（1 × 1 至 3840 × 2160）输出逐像素相同，并核对资源与透明源图一致。 |
| `python3 scripts/test_boot_brand.py` | 内核进度绘制 15,091,427 项、loader 进度 2,105 项、字形映射 768 项通过。 |
| `python3 scripts/check_brand_assets.py` | 42 项生成资源与 3,000 张图片审查通过，包括源图、BGRA alpha、启动 VGA RLE4 与关机资源。 |
| 三架构目标构建 | `CCACHE_DISABLE=1 ninja -C output-Clang-{amd64,i386,arm64}-debug -j2 ntoskrnl bootloader` 均退出 0。最终分离 VGA 启动资源后再次构建，均通过。 |
| cppcheck | 使用 AMD64 构建数据库中的实际编译命令；在 SDK CRT 头 `stdio.h:90` 的 `_CRTDATA` 处解析中止，未计作通过。 |

AMD64 使用 OVMF / QEMU 实际冷启动，分辨率 1280 × 900，系统正常进入桌面。截图来自同一次启动的原始 QMP 帧：

- [引导加载阶段：Loading acpi.sys…](../graphics/screenshots/boot-loader.png)，进度白色长度 389 / 1280。
- [内核加载阶段：Loading cache manager…](../graphics/screenshots/boot-progress.png)，进度白色长度 819 / 1280。

截图无合成或修饰。两阶段 Logo 区域逐像素一致，只显示居中的图形标志；加载文字和顶部进度条均可见。随后添加了独立 VGA fallback 资源，GOP 资源未变；最终三架构目标重新构建后，把最终 AMD64 内核与引导程序部署到既有镜像，再次启动并进入桌面。两张加载阶段截图来自此前同一 GOP 实现的启动。VGA fallback 尚未在无 GOP 的 VM 中单独启动验证。

我尝试重建 AMD64 安装 ISO 时，Ninja 开始重建 4,521 个无关目标，在第 826 项中断以避免不必要的构建开销。因此现有 `bootcd.iso` 不包含最终的 VGA 资源拆分，不能作为该最终源码的安装介质交付。本次没有做全新安装、i386／ARM64 VM 启动或 Windows 11 基线测试。
