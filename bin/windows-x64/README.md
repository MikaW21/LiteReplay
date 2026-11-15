# LiteReplay 2.0 · UI Preview 1

**软件由 Yuzi_Lj 制作 / Software by Yuzi_Lj**

本版在 Phase3a HF2 基础上升级桌面 UI：深色卡片分区、明确的主操作按钮、导航选中状态、首页真实负载数值，以及更清楚的剪辑工作区。

完整解压到新目录，退出旧版托盘进程，运行 `bin/windows-x64/LiteReplay.exe`。窗口标题显示 **UI Preview 1**。不要单独复制 EXE。

使用说明与限制：[docs/UI_PREVIEW_1.md](docs/UI_PREVIEW_1.md)。基础剪辑：[docs/PHASE3.md](docs/PHASE3.md)。截图与剪贴板、AMD/Intel 回放兼容逻辑保持原样。

**Windows 交叉构建和可移植测试通过；Windows 实机 UI、DPI、录制和导出尚待复测。未测量性能。**

剪辑预览/导出前需要停止回放。此版未增加 BGM、字幕、变速、多轨或代理预览。系统对话框、部分原生控件、音频内部面板和 HUD 未全面重做。

English: A native Win32 UI update on top of Phase3a HF2. The dashboard, navigation, grouped settings and editor layout were refreshed. Recording and export logic remain unchanged. Build/portable checks passed; Windows UI, DPI and hardware runtime validation is pending. Stop replay before editor preview/export.
