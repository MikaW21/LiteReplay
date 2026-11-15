# LiteReplay 1.4 轻量音频面板

这是一个原生 Windows 音量控制器 + Equalizer APO 配置前端。不是完整 Sonar，也不创建虚拟 Game/Chat 输出设备。不依赖赛睿 GG。

## 分应用音量（无需额外安装）

1. 从主窗口或托盘打开“音频面板 / 耳机 EQ”。
2. 选择实际播放设备。例如 SteelSeries 耳机对应的实体输出。
3. 让游戏、QQ、Discord 或音乐软件先播放声音，点击“刷新应用列表”。
4. 选中一行，调节 0–100% 音量或静音；“耳机总音量”作用于该设备。

列表以 Windows 音频会话为单位。同一软件可能有多个会话，浏览器也可能按进程出现多行。不要仅凭一行名字判断软件全部声音都被调节。Windows 可能保留会话音量；LiteReplay 不在软件重启后强行重设。
设备下拉框只改变控制对象，不改变系统默认输出，不迁移正在播放的软件。没有发声过的程序可能不出现；软件关闭/重新打开或连接新耳机后手动刷新。设备不可用时显示错误并停止轮询，刷新后恢复。
面板显示时每秒读取现有音量；最小化暂停读取；关闭后释放音频 COM 对象并停止定时器。新会话需要手动刷新。

## 耳机 EQ（需要一次性安装 Equalizer APO）

Equalizer APO 官方页面： https://sourceforge.net/projects/equalizerapo/
官方说明： https://sourceforge.net/p/equalizerapo/wiki/Documentation/

1. 从官方页面安装 Equalizer APO，使用其设备选择器绑定目标耳机，按安装程序要求重启。不要盲目勾选麦克风或所有输出。
2. 在 LiteReplay 音频面板选择同一耳机。点击“选择 APO 配置”，选择 EqualizerAPO 的 `config/config.txt`。标准位置通常是 `C:\Program Files\EqualizerAPO\config\config.txt`；程序也会尝试从 APO 注册表项识别路径。
3. 选择预设或调整 31/62/125/250/500/1k/2k/4k/8k/16k Hz，范围 -12 至 +12 dB。预设只是通用起点，没有按特定耳机或游戏校准。
4. 点击“应用 EQ”。面板按设备 GUID 限定作用对象，为正增益自动加入负前级余量；其他已存在的 EQ 仍可能叠加，这不是对绝不削波的保证。
5. 通过“应用 EQ”和“旁路 EQ”比较试听，确认声音变化。状态“配置已写入”仅证明文件写入成功，不能证明 APO 驱动链实际启用。
6. 如无声音变化，检查耳机是否在 APO 设备选择器中启用、是否完成重启、Windows 音频增强是否关闭、播放器是否使用独占/ASIO 或其他绕过系统处理的路径。

预设：平直、低频增强、人声清晰、脚步参考、影音、柔和高频。拖动只修改编辑值，点击应用后才写入 APO。退出 LiteReplay 后，已接入的 EQ 仍由 APO 处理；使用旁路或移除接入关闭。
当前只维护一个 LiteReplay EQ 文件和一个目标设备；更换耳机并应用会替换原来的 LiteReplay 目标，不是多设备独立配置库。

## 配置修改与权限

- 不自动下载/安装 APO，不自动提权，不改系统默认音频设备。
- 在选择的 config.txt 末尾增加标识清晰的 Include 块，滤波参数放入同目录 `LiteReplay-EQ.txt`。
- 修改主配置前创建 `config.txt.LiteReplay-时间标识.bak`；保留原来的过滤器，不覆盖整个原始配置。并非一键将原有 EQ 清空。
- 更新采用同目录临时文件 + 原子替换，拒绝重复/被外部改写的管理块、UTF-16/二进制主配置和超过 1 MiB 的配置。
- “旁路 EQ”写入不含滤波器的 LiteReplay 子配置；不禁用其他软件的 EQ。“移除接入”仅移除本程序管理块并备份主文件，保留子文件和其他配置。
- 目录无写权限时会报错，不假装成功；可用“导出 EQ”将文件保存到其他可写目录，然后按 APO 官方说明接入。也可退出 LiteReplay 后以管理员身份完成配置写入，再恢复普通启动；运行中的实例不能被第二个管理员实例替代。
- 手动接入时，将导出的文件放入 APO config 目录，在 APO 配置编辑器末尾明确设置 `Device: all`、`Stage: post-mix`、`Channel: all`，再添加 Include；导出的子文件自身限定耳机 GUID。若原主配置有条件块，请在编辑器中确认 Include 位于条件块之外。此类人工接入不由 LiteReplay 的“移除接入”自动管理。
- 如果在应用时主配置写入失败而子配置已写入，程序会报错；已有 Include 的情况下，子文件变更可能已被 APO 读取，应试听确认。

## 与即时回放的关系

软件音量改动影响实际播放，也会影响系统声音回放录制。EQ 是否被 WASAPI loopback 录入取决于设备处理链，不保证录制的是 EQ 前或 EQ 后信号，需实机检查。
原有 NVENC 回放、提示音和 10 种悬浮窗保留。没有增加降噪、虚拟环绕、应用路由、游戏/聊天平衡、麦克风 EQ 或独立录音混音总线。

## 开发验证与实机检查

已执行：Windows x64 交叉编译/链接；核心、选项/GPU 聚合、EQ 配置生成/边界/幂等接入/安全移除与 Python 检查器/WAV 测试；ZIP 校验。
未执行：Windows 新界面排版、音量会话控制、耳机切换、Equalizer APO 安装与听感、系统声音录制相对 EQ 的位置、游戏性能与长时间资源占用实测。

实机验收：
1. 两个软件同时放声音，分别设为 20% 和 70%，只静音其中一个，核对 Windows 混音器和听感。
2. 同一应用有多个会话时逐个测试；退出重开、耳机插拔后刷新，确认不会调节旧设备/错会话。
3. 关闭面板后检查播放继续，音量不会恢复；托盘退出后无残留进程。
4. 先备份并检查已有 APO 配置，选择实体耳机，应用一个明显但适度的 EQ 后旁路对比；确认其他输出未受该 EQ 影响。
5. 重复应用不能重复追加 Include。移除接入后其他原有配置内容保留。无权限或被手工改写的管理块必须报错。
6. 重启 LiteReplay 后保留编辑值和目标，不自动重新应用。关闭回放和开着回放时都可打开音频面板。
7. 游戏固定场景对比面板关闭/显示的 CPU、内存和帧时间，未测量前不承诺开销比例。

API：
https://learn.microsoft.com/en-us/windows/win32/api/audiopolicy/nf-audiopolicy-iaudiosessionmanager2-getsessionenumerator
https://learn.microsoft.com/en-us/windows/win32/api/audioclient/nf-audioclient-isimpleaudiovolume-setmastervolume
https://sourceforge.net/p/equalizerapo/wiki/Configuration%20reference/
