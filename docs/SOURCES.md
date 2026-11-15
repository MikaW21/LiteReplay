# 官方接口与依赖参考

编写项目时查阅以下官方资料。网页可能更新；本项目未由这些厂商认证。

- Microsoft Desktop Duplication API：
  https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/desktop-dup-api
- IDXGIOutputDuplication::AcquireNextFrame / access lost：
  https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgioutputduplication-acquirenextframe
- IAudioCaptureClient::GetBuffer，特别是 QPC 的 100ns 单位：
  https://learn.microsoft.com/en-us/windows/win32/api/audioclient/nf-audioclient-iaudiocaptureclient-getbuffer
- D3D11 Video Processor output texture array view：
  https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_tex2d_array_vpov
- NVIDIA Video Codec SDK 编码指南，D3D11 外部资源：
  https://docs.nvidia.com/video-technologies/video-codec-sdk/13.0/nvenc-video-encoder-api-prog-guide/index.html
- FFmpeg NVENC 源码（D3D11 frame / hw_frames_ctx）：
  https://ffmpeg.org/doxygen/7.1/nvenc_8c_source.html
- 官方 vcpkg FFmpeg features：
  https://github.com/microsoft/vcpkg/blob/master/ports/ffmpeg/vcpkg.json
- 官方 vcpkg FFmpeg CMake 使用方式：
  https://github.com/microsoft/vcpkg/blob/master/ports/ffmpeg/usage

本次交付没有复制 NVENC SDK 实现，也不包含第三方二进制依赖。
