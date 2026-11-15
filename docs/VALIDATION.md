# 验证记录 — 2026-11-07

## 已实际执行

| 检查 | 实际结果 | 证明范围 |
|---|---|---|
| GNU g++ 13.3.0，C++17，-Wall -Wextra -Werror | 通过 | 可移植核心编译 |
| 核心 7 组测试 | 通过 | GOP 时间/容量淘汰、首关键帧、音频乱序、过大包、包数上限、PCM 缺口/覆盖、并发快照 |
| AddressSanitizer + UndefinedBehaviorSanitizer | 通过（关闭 LeakSanitizer） | 所执行核心测试内未报告越界/未定义行为 |
| Python 6 个检查器测试 | 通过 | 正常时间戳、坏关键帧、重复 DTS、空音轨、AV 端点偏移、NaN |
| CMake 4.4.4 Release 配置/编译/CTest | 2/2 测试入口通过 | Linux 构建入口、上述 C++ 与 Python 测试 |
| 3 秒合成视频检查器测试 | 通过；全量解码无报错 | 检查脚本能处理真实 MP4；素材由系统 FFmpeg 生成，**不是 LiteReplay 录制结果** |
| FFmpeg 7.1.1 最小 Windows x64 共享库交叉编译 | 通过 | 生成 avcodec-61 / avformat-61 / avutil-59 / swresample-5 DLL |
| 全部 Windows C++ 源文件编译 / 链接 | 通过，Zig 0.13.0 | 生成 GUI 主程序与控制台诊断程序 |
| 附带 build-cross.sh 重建 | 通过 | 整个构建脚本从源压缩包完成依赖和应用重建 |
| PE32+ x64 导入表检查 | 通过 | 所需四个应用 DLL 已放在同目录，其余静态导入为 Windows 系统库/API set |

LeakSanitizer 首次运行报当前执行环境/ptrace 不支持，随后只关闭泄漏检查运行 ASan/UBSan；不能据此声称长期无泄漏。

编译过程中修复了性能计数器所需 `pdhmsg.h` 包含。跨编译 FFmpeg 移除了生成配置中的 GNU ld 专用参数，并明确设置主程序 Windows GUI 子系统；这些是实际遇到并修复的构建问题。

## 尚未验证

- Windows 首次启动、原生窗口/控件/托盘、安装/卸载、自启、热键冲突行为。
- RTX 5060 上 D3D11 Video Processor 输出 NV12 texture 的实际兼容性。
- NVENC 实际创建、编码、H.264 / HEVC / AV1 的稳定输出及颜色准确性。
- WASAPI 实际设备格式、系统声音、麦克风混音、分轨及设备切换。
- LiteReplay 实际回放文件的可播放性、内容音画同步、长期时钟漂移。
- Alt+Tab、独占全屏、驱动重置、锁屏、休眠与恢复。
- 两小时或更长时间的内存/句柄稳定性。
- 12100F / RTX 5060 的 CPU、显存、编码占用、FPS / 1% low / 帧时间损失。
- Windows/MSVC/vcpkg 构建路径及 GitHub Actions（工作流只提供，未触发）。

没有 Windows 主机、NVIDIA GPU、Wine 或实际游戏可用于上述验收。
“交叉编译成功”不等于“已经可以稳定录制”。发行文件属于未签名开发预览。

## 实测命令

```bash
g++ -std=c++17 -O1 -Wall -Wextra -Werror -pthread \
  -fsanitize=address,undefined -g -I LiteReplay/src \
  LiteReplay/tests/core_tests.cpp -o /tmp/litereplay-core-tests
ASAN_OPTIONS=detect_leaks=0 /tmp/litereplay-core-tests
python3 -m unittest discover -s LiteReplay/tests -p 'test_*.py' -v
cmake -S LiteReplay -B /tmp/litereplay-cmake -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/litereplay-cmake --parallel 2
ctest --test-dir /tmp/litereplay-cmake --output-on-failure
```

完整交叉构建脚本为 `scripts/build-cross.sh`。本地合成测试使用 320×180、30fps 的 testsrc2 和 48kHz 正弦音，H.264 无 B 帧、AAC、`-avoid_negative_ts make_zero`；这只是测试检查器，应用视频编码仍仅允许 NVENC。
