# Phase3a Hotfix 1 — 2026-11-10

软件由 Yuzi_Lj 制作 / Software by Yuzi_Lj

## 修复范围

用户报告编辑器提示 Windows media error 0xc00d3702。代码审查确认预览未将 FFprobe 时长限制到 MFPlay 实际时长；裁剪后 seek/get-position 也未转换片段相对坐标。这些缺陷已修复，尚无用户原始视频，不能声称已复现或确认该截图的唯一原因。

- 从 IMFPMediaItem 读取实际时长，预览出点限制到此范围；不修改工程和导出入出点。
- 滑条与时间轴 seek 使用片段相对时间，位置限制在有效区间，避免 seek 到结束点之外。
- 播放头将相对时间还原为源视频时间。
- 异步打开尚未完成时忽略播放/seek 操作。
- 同步媒体错误记录具体 API 步骤和预览范围到 `%LOCALAPPDATA%\LiteReplay\2.0\editor-preview.log`，随后释放播放器。

## 更新和验证

退出旧 LiteReplay（包括托盘），完整解压新包到新目录，运行 bin/windows-x64/LiteReplay.exe。原始视频无需移动或修改。停止回放，再导入之前报错的视频测试播放；然后裁剪成非零入点，拖动到中间及片尾。

若仍报错，提供弹窗和上述日志，并说明是在导入、播放还是拖动进度条时发生。日志仅在同步媒体错误发生时生成；不要把没有日志当作已验证成功。

自动测试覆盖 Windows 时长短于 FFprobe、20 秒入点的相对 seek、片尾/前端钳位、极短片段、NaN/无穷/空范围。Windows UI、MFPlay 实机和用户视频复测仍待完成。没有新的性能测量。

Microsoft API reference:
https://learn.microsoft.com/en-us/previous-versions/windows/desktop/api/mfplay/nf-mfplay-imfpmediaitem-setstartstopposition
https://learn.microsoft.com/en-us/previous-versions/windows/desktop/api/mfplay/nf-mfplay-imfpmediaplayer-getposition
