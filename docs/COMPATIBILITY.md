# AMD / Intel / 旧笔记本兼容更新

2026-11-09。本包在 Phase 2 媒体库版本上修改捕获与编码路径，是待实机验收的兼容性构建，不是完整 2.0 正式版。

## 本次修复

1. 取消 `VendorId != NVIDIA` 的显示输出过滤。Desktop Duplication 现在枚举所有非软件适配器上已连接的 AMD / Intel / NVIDIA 屏幕，在该屏幕所属适配器上创建 D3D11 设备。
2. 捕获和编码选择分开。自动模式首先尝试捕获适配器的 NVENC 或 AMF，然后在 H.264 配置下尝试 Windows MF 硬件编码。显式选项也可以分别指定后端。
3. 新增 AMD AMF：H.264 / HEVC / AV1 是否支持取决于实际显卡及驱动。初始化失败记录具体错误，不把“库里存在编码器”当作硬件支持。
4. 新增 Windows Media Foundation H.264 硬件路径。由系统枚举已注册的硬件编码 MFT；可以用于具备相应驱动的 Intel/AMD 等设备。这不是专门实现的 FFmpeg QSV 后端，也不承诺选择某个特定物理编码器。
5. 新增 H.264 CPU 兼容路径。默认不开启；用户可显式选择 CPU，或勾选“允许硬件初始化失败后尝试 CPU”。不偷偷把 HEVC/AV1 改成 H.264。
6. 录制状态和日志显示实际成功初始化的后端、硬件/软件属性、捕获适配器和此前失败原因。MF 路径明确标注 GPU→CPU NV12 数据传输；不能宣传该路径是零拷贝。
7. 部分 MF 编码器延迟返回视频头信息；现在等待这些参数有效后才发布可保存的会话，避免保存线程读到正在修改的参数。五秒仍无参数会报错重试。
8. 保留 Phase 2 的媒体库与预览功能。系统音、麦克风、缓存和异步保存流程继续复用；新增后端的同步/关键帧兼容仍需实测。

## R5 M320 笔记本怎么试

先退出旧版并完整解压，不能只替换 EXE：本包的四个 FFmpeg DLL 也已更新。

1. 进入“即时回放”，滚动到底部，点“旧笔记本兼容预设”。它填入 720p、30 FPS、H.264、6 Mbps、60 秒、128 MiB 编码包缓存、屏幕 0、自动硬件和允许 CPU 备用。
2. 点击“应用设置”或“开启回放”。预设不会直接开始录制，也不会默认设置开机启动。
3. 查看状态和日志中的实际编码器。若为 `h264_mf [SOFTWARE]`，说明使用 CPU；这不是 R5 M320 硬件编码成功。
4. 若自动模式失败，分别尝试“Windows MF 硬件”与“CPU 兼容”。强制 CPU 也失败时，用下述诊断脚本获取日志。精简 Windows 可能缺少 Media Foundation，软件不能凭空提供缺失的系统组件。
5. 先保存短片，检查画面、声音、同步，再逐步增加规格。此处的 720p30 是起始测试配置，不是性能保证。CPU 编码仍可能影响游戏流畅度。

R5 M320 这一型号本次没有实机验证，不能承诺其驱动提供 AMF 硬件编码。混合显卡笔记本的内屏可能接 Intel 核显，此时按屏幕归属捕获是预期行为。现有代码没有实现任意 GPU 之间的纹理共享/复制编码；不要为了强行调用 AMD 而在错误适配器上 DuplicateOutput。

## 后端选择

| 设置 | 尝试路径 | CPU 是否自动启用 |
|---|---|---|
| 自动硬件，未勾 CPU | NVIDIA 输出尝试 NVENC；AMD 输出尝试 AMF；H.264 再尝试 MF 硬件 | 否 |
| 自动硬件，勾选 CPU | 上述硬件初始化都失败后，H.264 才尝试 MF 软件 | 是，已由用户勾选授权 |
| NVIDIA NVENC | 指定 NVENC；勾选时可追加 H.264 CPU 备用 | 取决于勾选 |
| AMD AMF | 指定 AMF；勾选时可追加 H.264 CPU 备用 | 取决于勾选 |
| Windows MF 硬件 | 只请求系统硬件 MFT；本版限 H.264 | 取决于勾选 |
| CPU 兼容 | 只请求同步软件 H.264 MFT | 用户已明确选择 |

初始化成功不等于完整录制成功：驱动也可能在首帧或运行时失败。当前的 CPU 备用发生在编码器初始化阶段；运行中错误仍沿用会话重建，不会声称无缝切换且不丢缓存。驱动重置、全屏、旋转/HDR限制仍需分别验收。

## 诊断

- `LiteReplayProbe.exe`：列出所有适配器连接的屏幕和编译包含的编码器；不启动捕获，不证明硬件支持。
- `TestReplay-Compat.cmd`：实际尝试约五秒 720p30 系统声音录制，允许 CPU 备用。结果写 `compat-probe-log.txt`，视频在 LiteReplayTest。
- `TestReplay-CPU.cmd`：明确只测 CPU 兼容路径。
- 自定义：`LiteReplayProbe.exe --capture --backend=amf --monitor=0`，或 `--backend=mf_hw` / `--backend=mf_sw`。日志能区分驱动初始化、捕获、视频参数和保存失败。

录制日志在 `%LOCALAPPDATA%\LiteReplay\2.0\LiteReplay.log`。请自行播放/试听输出；诊断成功不能代替游戏性能测试。

## 资源与限制

NVENC/AMF 在可用时保留 D3D11 硬件帧输入。MF 路径将已经由 GPU 缩放/转换的 NV12 帧传回 CPU，使用有界 8 帧 CPU 缓冲池；仍有原先的 16 帧 GPU 池。编码包缓存上限不是整个进程的 RAM/显存上限。

`thread_count` 只是传给编码接口的请求，不保证系统 MFT 按该数限制工作线程。没有 CPU/GPU/内存或游戏帧率损失实测数据。AMF 需要实际驱动运行时；FFmpeg 使用 AMF 1.4.33 头文件编译，旧驱动仍可能不兼容，此时需尝试 MF 或显式 CPU。

不使用 GDI CPU 截屏。即使 CPU 编码也仍依赖可用的 Desktop Duplication、D3D11 视频处理及 NV12 支持；不支持这些接口的驱动不能保证运行。此更新没有实现 QSV 专用后端、跨 GPU 编码互操作、WGC/HDR 或主动缩略图生成。

## 验证状态

本次验证：新 FFmpeg 中 NVENC、AMF、h264_mf、AAC 编译启用；Windows x64 应用与诊断程序交叉构建；六组 C++ 可移植测试及七项 Python 用例（13 个构建/运行命令）。具体结果见 validation。

未验证：AMD/R5 M320/Intel/NVIDIA 真实硬件录制、Windows MFT 选择、音画同步、MP4/MKV 实际播放、设备恢复、GPU→CPU 传输性能、长稳以及游戏并行录制。不能把此构建标注为“所有 A 卡已实测支持”。

## 构建与参考

- `scripts/build-compat-ffmpeg.sh`：重建本次依赖。输入原始 FFmpeg 7.1.1、nv-codec-headers 12.2.72.0、AMF headers 1.4.33。`AMF_INCLUDE` 目录下需存在 `AMF/core/Version.h`。
- `scripts/build-mingw.sh`：随后重建主程序和 Probe。旧 `build-cross.sh` 仍是旧 NVENC-only 依赖配方，不能用于复现本兼容包；MSVC/vcpkg 路线也必须明确启用所需编码器并通过 Probe 的编译能力检查。
- AMD AMF 官方源：https://github.com/GPUOpen-LibrariesAndSDKs/AMF
- Microsoft H.264 编码器：https://learn.microsoft.com/en-us/windows/win32/medfound/h-264-video-encoder
- FFmpeg mfenc.c / mf_utils.c：本包随附匹配版本的源码归档。
