**当前包：Phase3a Hotfix 2（2026-11-10）** — 绕开仍报错的预览范围 API，改用完整文件播放和绝对定位；窗口标识 P3a HF2。Windows 实机待复测。见 `docs/HOTFIX2.md`。

# LiteReplay 2.0 Phase3a — 基础剪辑开发版

**软件由 Yuzi_Lj 制作 / Software by Yuzi_Lj**

即时回放、截图/剪贴板、媒体库与基础剪辑整合在一个原生 Windows 程序内。保留 AMD/Intel 回放兼容修改。

完整解压，退出旧版，运行 `bin/windows-x64/LiteReplay.exe`。不要仅复制 EXE：DLL、languages、editor-tools 都要保留。

## 开始剪辑

先停止回放，点击左侧“视频剪辑”，或从媒体库选择视频后“进入视频剪辑”。导入 → 选片段 → 设置入点/出点 → 应用裁剪 → 分割/排序 → 导出。

已实现：单轨顺序时间轴、播放头/定位与缩放、片段裁剪/分割/复制/删除/排序、音量、撤销/重做、工程保存/打开、自动保存恢复、按需预览、常规 MP4 导出与选中片段 stream-copy 快速裁剪。导出成功自动入库。

常规导出会重新编码。GPU 提供 H.264/HEVC NVENC；CPU 兼容为 MPEG-4 Part 2（不是 H.264）。快速裁剪不重编码但切点受关键帧影响，忽略分辨率/FPS设置。原文件不会被覆盖。

回放运行时阻止剪辑预览/导出；开启回放会取消编辑任务以优先录制。更细的并行调度仍待实现。

**P3 尚未全部完成：BGM、多音轨、字幕、变速、代理预览、暂停导出以及 AMD/Intel 编辑硬编导出待后续版本。** 编辑器中的 NVENC/CPU 选项与回放编码选项不同。

完整使用与测试：`docs/PHASE3.md`。截图默认 Ctrl+Shift+F11/F8，可自行设置；截图同时保存 PNG 和复制到剪贴板。见 `docs/SCREENSHOTS.md`。R5 M320 回放诊断见 `docs/COMPATIBILITY.md`。

## 验证

Windows x64 交叉构建通过。可移植逻辑测试通过。使用实际 C++ 参数在 Linux FFmpeg 上实测裁剪/缩放/混合有声无声素材拼接/音量/stream copy/完整解码和文件保护通过。

Windows UI、MFPlay、NVENC 导出、录制与编辑调度及性能仍待实机验证。用户此前反馈截图可用，不等于完整套件通过验收。详见 validation/RESULTS.md。

## English

One native LiteReplay application; the Video Editor is embedded, not a separate app. Stop replay before editor preview/export. Import, set In/Out, Apply trim, split/reorder and export. Project save/open, bounded undo/redo and autosave recovery are available. Normal export re-encodes (NVENC H.264/HEVC or explicitly selected CPU MPEG-4). Stream-copy exports only the selected clip with keyframe-dependent boundaries. BGM, titles, speed changes, proxies, multi-track editing and pause/resume remain unimplemented. Windows runtime validation is pending.
