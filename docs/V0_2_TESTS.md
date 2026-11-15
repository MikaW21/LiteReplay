# 0.2 更新验收

## 已执行与待执行的界限

已执行：C++ 选项映射与 GPU 引擎聚合测试、核心回归测试、Python 检查器测试、Windows x64 交叉编译/链接、包校验。
待执行：中文界面的实际排版、性能计数器读数、监视器显示/隐藏、自启、720p75/4K/高帧率 NVENC 实际录制与游戏开销。没有在开发环境里运行 Windows EXE。

## 设置与中文

- 从旧 0.1.1 升级，确认 1080p60、路径、热键、码率仍保留。
- 选择 720p 和 75 FPS，应用后退出并重开，确认仍为 1280×720 / 75 FPS。
- 保存短片，用播放器或 ffprobe 验证尺寸与时间戳。实际平均 FPS 可能因漏采下降；重复帧也计入编码帧率。
- 分别测试其他分辨率/帧率组合。驱动不支持时应给出错误，不能切换 CPU 软件视频编码。
- 检查主界面、托盘、气泡、保存/停止/重试提示和诊断脚本显示中文。底层错误详情保留技术原文。

## 性能监视器

1. 点击“打开性能监视器”，等待至少两个采样周期；观察全机 CPU、进程 CPU、GPU、内存。
2. 同时打开任务管理器，在相近时间尺度下比较大致趋势；采样时刻和聚合口径不同，不要求逐位相同。
3. GPU 全机指标：将相同物理引擎的各进程计数相加，再选择最忙引擎；程序 GPU 指标仅使用 LiteReplay 的实例。
4. 视频编码引擎单独显示；没有实例/数据无效时是“暂不可用”。不等于 0%，不代表硬件编码失败。
5. 全机内存是物理内存使用量；进程私有提交内存和驻留工作集含义不同，不要求两者一致。
6. 隐藏/最小化主窗口与监视器，采样应暂停；录制继续。重新打开后 1–2 秒刷新当前样本。
7. 只关闭主窗口时，如果性能窗口仍显示，采样继续；只关闭性能窗口不影响录制。托盘退出会一并结束。
8. 显示器/驱动重置后检查 GPU 计数恢复。若长期不可用，记录日志与系统版本；不以虚构数字填补。

## 采样开销对比

在同一段游戏 benchmark 对比监视器显示和全部窗口隐藏时的游戏 FPS、帧时间及 LiteReplay CPU。此版没有监视器开销的实测百分比。

## 官方参考

- GPU 最忙引擎统计： https://devblogs.microsoft.com/directx/gpus-in-the-task-manager/
- GetSystemTimes 的 kernel 时间包含 idle： https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getsystemtimes
- 全机物理内存： https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
