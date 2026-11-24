ReliefWin
=========

ReliefWin 是基于 LiberNT 和 ReactOS 的自由 NT 操作系统，目标是兼容 Windows 11，并支持现代硬件。

本系统默认使用简体中文，其他语言资源和英语回退仍然保留。

系统使用维护者提供的新 Logo；名称、安装程序、启动界面、桌面和系统信息均使用 ReliefWin 品牌。

目前尚未配置 ReliefWin 官网、公开下载仓库或更新服务。默认关闭自动系统更新；仅在维护者配置服务器后才能检查更新。

安装程序为 C:\<架构>\reliefwin.exe。安装可能格式化磁盘或删除分区，请先备份重要数据。

原作者和第三方组件的版权、许可证以及必要的兼容接口保持原样。LiberNT、ReactOS、Wine 和其他上游项目的名称只用于来源说明，不表示其认可或支持本发行版。

源代码与构建
------------

在源码根目录执行 ./configure.sh，然后在对应构建目录中运行：

    CCACHE_DISABLE=1 ninja reliefwinimg

输出镜像名称为 ReliefWin.img。安装光盘目标为 bootcd，输出为 bootcd.iso。

有关源代码许可和作者名单，请查阅源码中的 COPYING、CREDITS.md 和 TRADEMARK.md。
